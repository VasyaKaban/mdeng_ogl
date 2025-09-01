#include "WFObject.h"
#include <fstream>
#include <format>
#include <cstdio>
#include <optional>
#include <ranges>
#include <array>
#include <unordered_map>
#include <cassert>
#include <functional>

template<typename T>
requires(std::integral<T> || std::floating_point<T>)
static std::pair<bool, std::string_view::const_iterator> parse_value(std::string_view str,
                                                                     T& out) noexcept
{
    auto [ptr, res] = std::from_chars(str.data(), str.data() + str.size(), out);
    if(res != std::errc(0))
        return {false, str.end()};

    if(ptr != str.data() + str.size())
        return {false, str.end()};

    return {true, str.begin() + std::distance(str.data(), ptr)};
}

struct ParseCallbackDesc
{
    std::string_view prefix;
    std::function<void(std::string_view)> callback;
    bool allow_empty;
};

static void parse(const std::filesystem::path& path, std::span<const ParseCallbackDesc> callbacks)
{
    std::ifstream ifs(path);
    if(!ifs.is_open())
        throw std::runtime_error(std::format("Failed to open file: {}", path.string()));

    std::string line;
    line.reserve(1024);

    std::size_t line_number = 1;

    while(true)
    {
        std::getline(ifs, line);

        if(ifs.eof())
            break;

        bool found = false;
        for(const auto& cback_desc: callbacks)
        {
            if(line.starts_with(cback_desc.prefix))
            {
                found = true;

                std::string_view view(line.begin() + cback_desc.prefix.size(), line.end());
                if(view.empty() && !cback_desc.allow_empty)
                    throw std::runtime_error(
                        std::format("Bad line: {} -> #{}: {}", path.string(), line_number, line));

                try
                {
                    cback_desc.callback(view);
                }
                catch(const std::exception& ex)
                {
                    throw std::runtime_error(std::format("Bad line; {}: {} -> #{}: {}",
                                                         ex.what(),
                                                         path.string(),
                                                         line_number,
                                                         line));
                }
                catch(...)
                {
                    throw std::runtime_error(
                        std::format("Bad line: {} -> #{}: {}", path.string(), line_number, line));
                }
            }
        }

        if(!found)
            throw std::runtime_error(std::format("Bad line; Unrecognized prefix: {} -> #{}: {}",
                                                 path.string(),
                                                 line_number,
                                                 line));

        line_number++;
    }
}

WFObject::WFObject(const std::filesystem::path& path,
                   const std::filesystem::path& material_lib_root)
{
    vertices.reserve(1024);
    texture_coordinates.reserve(1024);
    normals.reserve(1024);
    material_groups.reserve(4);

    std::int32_t tmp_faces_pitch = -1;
    std::optional<WFObjectFaceType> tmp_face_type;

    auto parse_face_index = [this](std::string_view value, int component_index)
    {
        std::uint32_t index = 0;
        auto [parsed, ptr] = parse_value(value, index);
        if(!parsed || value.end() != ptr)
            throw std::runtime_error("Bad face value");

        if(component_index == 0)
            material_groups.back().faces.push_back(hrs::math::glsl::vec3{});

        material_groups.back().faces.back()[component_index] = index;
    };

    std::array callbacks = {
        ParseCallbackDesc{.prefix = "",
                          .callback =
                              [](std::string_view view)
                          {
                              //noop
                          },
                          .allow_empty = true},
        ParseCallbackDesc{.prefix = "#",
                          .callback =
                              [](std::string_view view)
                          {
                              //noop
                          },
                          .allow_empty = true},
        ParseCallbackDesc{
            .prefix = "mtllib ",
            .callback =
                [this, &material_lib_root](std::string_view view)
            {
                //mtllib str

                if(view.empty())
                    throw std::runtime_error("Bad material library value");

                Material* current_mtl = nullptr;

                std::array callbacks = {
                    ParseCallbackDesc{.prefix = "",
                                      .callback =
                                          [](std::string_view view)
                                      {
                                          //noop
                                      },
                                      .allow_empty = true},
                    ParseCallbackDesc{.prefix = "#",
                                      .callback =
                                          [](std::string_view view)
                                      {
                                          //noop
                                      },
                                      .allow_empty = true},
                    ParseCallbackDesc{
                        .prefix = "newmtl ",
                        .callback =
                            [&current_mtl, this](std::string_view view)
                        {
                            current_mtl =
                                &materials.insert({std::string(view), Material{}}).first->second;
                        },
                        .allow_empty = false},
                    ParseCallbackDesc{.prefix = "map_Kd ",
                                      .callback =
                                          [&current_mtl, this](std::string_view view)
                                      {
                                          if(current_mtl == nullptr)
                                              throw std::runtime_error(
                                                  "No previous material defined");

                                          current_mtl->diffuse_map = view;
                                      },
                                      .allow_empty = false},
                };

                parse(material_lib_root / view, callbacks);
            },
            .allow_empty = false},
        ParseCallbackDesc{.prefix = "v ",
                          .callback =
                              [this](std::string_view view)
                          {
                              //v %f %f %f [%f]

                              hrs::math::glsl::vec4 v;
                              auto spaces = std::ranges::count(view, ' ');
                              if(!(spaces == 2 || spaces == 3))
                                  throw std::runtime_error("Bad vertex format");

                              int i = 0;
                              for(auto value: std::ranges::split_view(view, ' '))
                              {
                                  std::string_view value_view(value.begin(), value.end());
                                  auto [parsed, ptr] = parse_value(value_view, v[i]);
                                  if(!parsed || value_view.end() != ptr)
                                      throw std::runtime_error(std::format("Bad vertex value"));

                                  i++;
                              }

                              vertices.push_back({v[0], v[1], v[2]});
                          },
                          .allow_empty = false},
        ParseCallbackDesc{.prefix = "vt ",
                          .callback =
                              [this](std::string_view view)
                          {
                              //vt %f [%f] [%f]

                              hrs::math::glsl::vec3 vt;
                              auto spaces = std::ranges::count(view, ' ');
                              if(spaces > 2)
                                  throw std::runtime_error("Bad texture coordinate format");

                              int i = 0;
                              for(auto value: std::ranges::split_view(view, ' '))
                              {
                                  std::string_view value_view(value.begin(), value.end());
                                  auto [parsed, ptr] = parse_value(value_view, vt[i]);
                                  if(!parsed || value_view.end() != ptr)
                                      throw std::runtime_error("Bad texture coordinate value");

                                  i++;
                              }

                              texture_coordinates.push_back({vt[0], vt[1]});
                          },
                          .allow_empty = false},
        ParseCallbackDesc{.prefix = "vn ",
                          .callback =
                              [this](std::string_view view)
                          {
                              //vn %f %f %f
                              hrs::math::glsl::vec3 vn;
                              auto spaces = std::ranges::count(view, ' ');
                              if(spaces != 2)
                                  throw std::runtime_error("Bad normal format");

                              int i = 0;
                              for(auto value: std::ranges::split_view(view, ' '))
                              {
                                  std::string_view value_view(value.begin(), value.end());
                                  auto [parsed, ptr] = parse_value(value_view, vn[i]);
                                  if(!parsed || value_view.end() != ptr)
                                      throw std::runtime_error("Bad normal value");

                                  i++;
                              }

                              normals.push_back({vn[0], vn[1], vn[2]});
                          },
                          .allow_empty = false},
        ParseCallbackDesc{
            .prefix = "usemtl ",
            .callback =
                [this](std::string_view view)
            {
                //usemtl str -> new group

                if(view.empty())
                    throw std::runtime_error("Bad metarial group value");

                auto mtl = materials.find(view);
                if(mtl == materials.end())
                    throw std::runtime_error(std::format("Undefined material: {}", view));

                material_groups.push_back(MaterialGroup{.name = {view.begin(), view.end()},
                                                        .mtl = &mtl->second,
                                                        .faces = {}});
                material_groups.back().faces.reserve(1024);
            },
            .allow_empty = false},
        ParseCallbackDesc{
            .prefix = "f ",
            .callback =
                [this, &tmp_faces_pitch, &tmp_face_type, &parse_face_index](std::string_view view)
            {
                //f v1 v2 v3 ...
                //f v1/vt1 v2/vt2/v3/vt3 ...
                //f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
                //f v1//vn1 v2//vn2 v3//vn3 ...
                if(material_groups.empty())
                    throw std::runtime_error("No previous usemtl");

                if(tmp_faces_pitch == -1)
                {
                    auto spaces = std::ranges::count(view, ' ');
                    if(spaces < 2)
                        throw std::runtime_error("Bad face format");

                    tmp_faces_pitch = spaces + 1;

                    faces_pitch = tmp_faces_pitch;
                }

                if(!tmp_face_type)
                {
                    auto index = view.find(' ');
                    std::string_view value_view(view.begin(), view.begin() + index);

                    auto slash_count = std::ranges::count(value_view, '/');
                    if(slash_count > 2)
                        throw std::runtime_error("Bad face format");

                    switch(slash_count)
                    {
                        case 0:
                            tmp_face_type = WFObjectFaceType::Vertex;
                            break;
                        case 1:
                            tmp_face_type = WFObjectFaceType::Vertex_TextureCoordinate;
                            break;
                        default:
                        {
                            auto slash_index = value_view.find('/');
                            if(value_view[slash_index + 1] == '/')
                                tmp_face_type = WFObjectFaceType::Vertex_Normal;
                            else
                                tmp_face_type = WFObjectFaceType::Vertex_TextureCoordinate_Normal;
                        }
                        break;
                    }

                    face_type = *tmp_face_type;
                }

                for(auto value: std::ranges::split_view(view, ' '))
                {
                    switch(*tmp_face_type)
                    {
                        case WFObjectFaceType::Vertex:
                        {
                            parse_face_index(std::string_view{value.begin(), value.end()}, 0);
                        }
                        break;
                        case WFObjectFaceType::Vertex_Normal:
                        {
                            int i = 0;
                            for(auto val: std::ranges::split_view(value, "//"))
                            {
                                parse_face_index(std::string_view{val.begin(), val.end()}, i);
                                i = 2;
                            }
                        }
                        break;
                        case WFObjectFaceType::Vertex_TextureCoordinate:
                        case WFObjectFaceType::Vertex_TextureCoordinate_Normal:
                        {
                            int i = 0;
                            for(auto val: std::ranges::split_view(value, '/'))
                            {
                                parse_face_index(std::string_view{val.begin(), val.end()}, i);
                                i++;
                            }
                        }
                        break;
                    }
                }
            },
            .allow_empty = false}};

    parse(path, callbacks);

    int indices_per_vertex = 0;
    switch(face_type)
    {
        case WFObjectFaceType::Vertex:
            indices_per_vertex = 1;
            break;
        case WFObjectFaceType::Vertex_TextureCoordinate_Normal:
            indices_per_vertex = 3;
            break;
        case WFObjectFaceType::Vertex_TextureCoordinate:
        case WFObjectFaceType::Vertex_Normal:
            indices_per_vertex = 2;
            break;
    }

    //trinagulation
    if(faces_pitch != 3)
    {
        for(auto& mtl_group: material_groups)
        {
            std::vector<hrs::math::glsl::uvec3> new_faces;
            new_faces.reserve(3 * mtl_group.faces.size() -
                              (6 * mtl_group.faces.size()) / faces_pitch);

            for(std::size_t i = 0; i < mtl_group.faces.size(); i += faces_pitch)
            {
                for(std::size_t j = i + 1; j < i + 1 + faces_pitch - 2; j++)
                {
                    new_faces.push_back(mtl_group.faces[j]);
                    new_faces.push_back(mtl_group.faces[j + 1]);
                    new_faces.push_back(mtl_group.faces[i]);
                }
            }

            mtl_group.faces = std::move(new_faces);
        }
    }
}

std::vector<Mesh> WFObject::GetMeshes()
{
    std::vector<Mesh> meshes;
    meshes.reserve(material_groups.size());

    auto hasher = [this](const hrs::math::glsl::uvec3& v) -> std::size_t
    {
        std::size_t hash = 0;
        switch(face_type)
        {
            case WFObjectFaceType::Vertex:
                hash = v[0];
                break;
            case WFObjectFaceType::Vertex_TextureCoordinate_Normal:
                hash = v[0] + v[1] + v[2];
                break;
            case WFObjectFaceType::Vertex_TextureCoordinate:
                hash = v[0] + v[1];
                break;
            case WFObjectFaceType::Vertex_Normal:
                hash = v[0] + v[2];
                break;
        }

        return hash;
    };

    auto comparator = [this](const hrs::math::glsl::uvec3& v1,
                             const hrs::math::glsl::uvec3& v2) -> bool
    {
        bool same = false;
        switch(face_type)
        {
            case WFObjectFaceType::Vertex:
                same = (v1[0] == v2[0]);
                break;
            case WFObjectFaceType::Vertex_TextureCoordinate_Normal:
                same = (v1[0] == v2[0]) && (v1[1] == v2[1]) && (v1[2] == v2[2]);
                break;
            case WFObjectFaceType::Vertex_TextureCoordinate:
                same = (v1[0] == v2[0]) && (v1[1] == v2[1]);
                break;
            case WFObjectFaceType::Vertex_Normal:
                same = (v1[0] == v2[0]) && (v1[2] == v2[2]);
                break;
        }

        return same;
    };

    for(std::size_t mtl_group_index = 0; mtl_group_index < material_groups.size();
        mtl_group_index++)
    {
        MaterialGroup& mtl_group = material_groups[mtl_group_index];
        std::uint32_t index = 0;
        std::unordered_map<hrs::math::glsl::uvec3,
                           std::uint32_t,
                           decltype(hasher),
                           decltype(comparator)>
            faces_set(mtl_group.faces.size(), hasher, comparator);
        for(const auto& face: mtl_group.faces)
        {
            auto [_, inserted] = faces_set.insert({face, index});
            if(inserted)
                index++;
        }

        int vertex_size = 0;
        switch(face_type)
        {
            case WFObjectFaceType::Vertex:
                vertex_size = 3;
                break;
            case WFObjectFaceType::Vertex_TextureCoordinate_Normal:
                vertex_size = 3 + 2 + 3;
                break;
            case WFObjectFaceType::Vertex_TextureCoordinate:
                vertex_size = 3 + 2;
                break;
            case WFObjectFaceType::Vertex_Normal:
                vertex_size = 3 + 3;
                break;
        }

        std::vector<float> vertex_buffer;
        vertex_buffer.resize(vertex_size * faces_set.size());
        for(auto& sface: faces_set)
        {
            auto& face = sface.first;
            std::size_t vertex_index = vertex_size * sface.second;

            int vertex_data_offset = 0;
            //push vertex
            const auto& vert = vertices[face[0] - 1];
            for(int i = 0; i < 3; i++, vertex_data_offset++)
                vertex_buffer[vertex_index + vertex_data_offset] = vert[i];

            if(face_type == WFObjectFaceType::Vertex_TextureCoordinate ||
               face_type == WFObjectFaceType::Vertex_TextureCoordinate_Normal)
            {
                const auto& text_coord = texture_coordinates[face[1] - 1];
                for(int i = 0; i < 2; i++, vertex_data_offset++)
                    vertex_buffer[vertex_index + vertex_data_offset] = text_coord[i];
            }

            if(face_type == WFObjectFaceType::Vertex_Normal ||
               face_type == WFObjectFaceType::Vertex_TextureCoordinate_Normal)
            {
                const auto& normal = normals[face[2] - 1];
                for(int i = 0; i < 3; i++, vertex_data_offset++)
                    vertex_buffer[vertex_index + vertex_data_offset] = normal[i];
            }
        }

        std::vector<std::uint32_t> index_buffer;
        index_buffer.reserve(mtl_group.faces.size());

        for(const auto& face: mtl_group.faces)
        {
            auto it = faces_set.find(face);
            assert(it != faces_set.end());

            index_buffer.push_back(it->second);
        }

        meshes.push_back(Mesh{.mtl_group_index = mtl_group_index,
                              .vertex_data = std::move(vertex_buffer),
                              .index_data = std::move(index_buffer)});
    }

    return meshes;
}

const std::vector<MaterialGroup>& WFObject::GetMaterialGroups() const noexcept
{
    return material_groups;
}

WFObjectFaceType WFObject::GetFaceType() const noexcept
{
    return face_type;
}
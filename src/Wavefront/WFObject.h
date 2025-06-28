#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include "../hrs/math/vector.hpp"

struct MaterialGroup
{
    std::string name;
    std::vector<hrs::math::glsl::uvec3> faces;
};

enum class WFObjectFaceType
{
    Vertex,
    Vertex_TextureCoordinate,
    Vertex_Normal,
    Vertex_TextureCoordinate_Normal
};

struct Mesh
{
    std::string material_name;
    std::vector<float> vertex_data;
    std::vector<std::uint32_t> index_data;
};

class WFObject
{
public:
    WFObject(const std::filesystem::path& path);
    ~WFObject() = default;
    WFObject(const WFObject&) = default;
    WFObject(WFObject&&) = default;
    WFObject& operator=(const WFObject&) = default;
    WFObject& operator=(WFObject&&) = default;

    std::vector<Mesh> GetMeshes();
    WFObjectFaceType GetFaceType() const noexcept;
private:
    std::vector<hrs::math::glsl::vec3> vertices;
    std::vector<hrs::math::glsl::vec3> texture_coordinates;
    std::vector<hrs::math::glsl::vec3> normals;

    std::uint32_t faces_pitch;
    std::vector<MaterialGroup> material_groups;

    WFObjectFaceType face_type;
};

//# -> skip
//v %f %f %f [%f]
//vt %f [%f] [%f]
//vn %f %f %f
//usemtl str -> new group
//f v1 v2 v3 ...
//f v1/vt1 v2/vt2/v3/vt3 ...
//f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
//f v1//vn1 v2//vn2 v3//vn3 ...
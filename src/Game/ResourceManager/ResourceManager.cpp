#include "ResourceManager.h"
#include <fstream>
#include "../RenderEngine/RenderEngine.h"
#include "Core/DDS/DDS.h"
#include "../RenderEngine/TransferQueue/TransferChannel.h"
#include "hrs/scoped_call.hpp"

struct BinaryData
{
    std::uint8_t* ptr;
    std::size_t size;
};

static BinaryData read_file(const std::filesystem::path& path)
{
    std::ifstream ifs;
    ifs.open(path, std::ios::binary | std::ios::in);
    if(!ifs.is_open())
        throw std::runtime_error(std::format("Failed to open file: {}", path.string()));

    ifs.seekg(0, std::ios::end);
    auto size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    BinaryData data;
    data.ptr = new std::uint8_t[size];
    data.size = size;

    ifs.read(reinterpret_cast<char*>(data.ptr), size);

    return data;
}

ResourceManager::ShaderEntry::ShaderEntry(Shader&& _shader) noexcept
    : shader(std::move(_shader))
{}

Shader* ResourceManager::ShaderEntry::operator->() noexcept
{
    return &shader;
}

Shader* ResourceManager::ShaderEntry::Get() noexcept
{
    return &shader;
}

ResourceManager::ImageEntry::ImageEntry(Image&& _image) noexcept
    : image(std::move(_image)),
      state(ImageEntryState::NotReady)
{}

Image* ResourceManager::ImageEntry::operator->() noexcept
{
    return &image;
}

Image* ResourceManager::ImageEntry::Get() noexcept
{
    return &image;
}

ResourceManager::ImageEntryState ResourceManager::ImageEntry::GetState() const noexcept
{
    return state;
}

void ResourceManager::ImageEntry::SetState(ImageEntryState _state) noexcept
{
    state = _state;
}

ResourceManager::ResourceManager(const ResourceManagerInfo& info)
    : prefixes{.shader_path_prefix = info.shader_path_prefix,
               .image_path_prefix = info.image_path_prefix}
{
    for(const auto& sh_ext_desc: info.shader_resource_descs)
        ext_mappings.shader_ext_mapping.insert(std::pair{sh_ext_desc.ext, sh_ext_desc.desc});
}

hrs::rc_ptr<ResourceManager::ShaderEntry> ResourceManager::CreateShader(std::string_view path)
{
    auto it = resources.shaders.find(path);
    if(it != resources.shaders.end())
        return it->second;

    auto res_path = prefixes.shader_path_prefix / path;
    auto ext = res_path.extension().string();
    auto ext_it = ext_mappings.shader_ext_mapping.find(ext);
    if(ext_it == ext_mappings.shader_ext_mapping.end())
        throw std::runtime_error(std::format("Undefined shader extension: {}", ext));

    auto data = read_file(res_path);
    hrs::scoped_call cleanup(
        [&data]()
        {
            delete[] data.ptr;
        });

    const ShaderInfo info = {.stage = ext_it->second.stage,
                             .code =
                                 std::span{reinterpret_cast<const GLchar*>(data.ptr), data.size}};

    Shader shader(RenderEngine::GetInstance()->GetContext(), info);

    auto ins_it = resources.shaders.insert(
        std::pair{std::string(path), hrs::rc_ptr<ShaderEntry>(new ShaderEntry(std::move(shader)))});

    return ins_it.first->second;
}

hrs::rc_ptr<ResourceManager::ImageEntry> ResourceManager::CreateImage(std::string_view path,
                                                                      bool prefer_image_host_copy)
{
    auto it = resources.images.find(path);
    if(it != resources.images.end())
        return it->second;

    auto res_path = prefixes.image_path_prefix / path;
    auto data = read_file(res_path);

    auto dds_result = DDS::Parse({data.ptr, data.size});
    auto dds_resolve = DDS::Resolve(dds_result);

    Image image(RenderEngine::GetInstance()->GetContext(), dds_resolve.image_info);

    /*ImageViewType view_type;
    if(dds_resolve.is_cubemap)
    {
        if(dds_resolve.image_info.array_layers == 1)
            view_type = ImageViewType::ImageViewCubeMap;
        else
            view_type = ImageViewType::ImageViewCubeMapArray;
    }
    else
    {
        switch(dds_resolve.image_info.image_type)
        {
            case ImageType::Image1D:
                if(dds_resolve.image_info.array_layers == 1)
                    view_type = ImageViewType::ImageView1D;
                else
                    view_type = ImageViewType::ImageView1DArray;
                break;
            case ImageType::Image2D:
                if(dds_resolve.image_info.array_layers == 1)
                    view_type = ImageViewType::ImageView2D;
                else
                    view_type = ImageViewType::ImageView2DArray;
                break;
            case ImageType::Image3D:
                view_type = ImageViewType::ImageView3D;
                break;
        }
    }

    const ImageViewInfo view_info = {
        .image = &image,
        .view_type = view_type,
        .format = dds_resolve.image_info.format,
        .components = ComponentMapping{.r = ComponentSwizzle::SwizzleIdentity,
                                       .g = ComponentSwizzle::SwizzleIdentity,
                                       .b = ComponentSwizzle::SwizzleIdentity,
                                       .a = ComponentSwizzle::SwizzleIdentity},
        .subresource_range = ImageSubresourceRange{
            .min_mip_level = 0,
            .mip_level_count = static_cast<GLuint>(dds_resolve.image_info.mip_levels),
            .min_layer = 0,
            .layer_count = static_cast<GLuint>(dds_resolve.image_info.array_layers)}};

    ImageView image_view(RenderEngine::GetInstance()->GetContext(), view_info);*/

    auto ins_it = resources.images.insert(
        std::pair{std::string(path), hrs::rc_ptr<ImageEntry>(new ImageEntry(std::move(image)))});

    TransferChannel* transfer_channel = RenderEngine::GetInstance()->GetTransferChannel();
    transfer_channel->Reserve(dds_resolve.regions.size() + 1); //image regions itself + callback
    for(const auto& reg: dds_resolve.regions)
    {
        const TransferRegion transfer_reg = {
            .data = TransferImageRegion{.data = reg.data,
                                        .buffer_row_length = reg.copy_region.buffer_row_length,
                                        .buffer_image_height = reg.copy_region.buffer_image_height,
                                        .subresource_layers = reg.copy_region.subresource_layers,
                                        .offset = reg.copy_region.offset,
                                        .extent = reg.copy_region.extent,
                                        .data_format = reg.copy_region.data_format,
                                        .data_type = reg.copy_region.data_type,
                                        .image = ins_it.first->second->Get(),
                                        .prefer_image_host_copy = prefer_image_host_copy}};

        RenderEngine::GetInstance()->GetTransferChannel()->Transfer({&transfer_reg, 1});
    }

    const TransferRegion cback_reg = {
        .data = TransferCallback{[img_entry = ins_it.first->second, data = std::move(data)]()
                                 {
                                     delete[] data.ptr;
                                     img_entry->SetState(ImageEntryState::Ready);
                                 }}};
    RenderEngine::GetInstance()->GetTransferChannel()->Transfer({&cback_reg, 1});

    return ins_it.first->second;
}

hrs::rc_ptr<ResourceManager::ShaderEntry> ResourceManager::FindShader(std::string_view path)
{
    auto it = resources.shaders.find(path);
    if(it == resources.shaders.end())
        return {};

    return it->second;
}

hrs::rc_ptr<ResourceManager::ImageEntry> ResourceManager::FindImage(std::string_view path)
{
    auto it = resources.images.find(path);
    if(it == resources.images.end())
        return {};

    return it->second;
}

void ResourceManager::ClearUnused() noexcept
{
    //shaders
    {
        auto it = resources.shaders.begin();
        while(it != resources.shaders.end())
        {
            auto next = std::next(it);
            if(it->second.get_refs() <= 1)
                resources.shaders.erase(it);

            it = next;
        }
    }

    //images
    {
        auto it = resources.images.begin();
        while(it != resources.images.end())
        {
            auto next = std::next(it);
            if(it->second.get_refs() <= 1 && it->second->GetState() == ImageEntryState::Ready)
                resources.images.erase(it);

            it = next;
        }
    }
}
#include "ResourceManager.h"
#include <fstream>
#include "../RenderEngine/RenderEngine.h"
#include "../Engine.h"
#include "Core/Render/Context.h"
#include "Core/DDS/DDS.h"
#include "../RenderEngine/TransferQueue/TransferChannel.h"
#include "hrs/scoped_call.hpp"

using BinaryData = std::vector<std::uint8_t>;

static BinaryData read_file(const std::filesystem::path& path)
{
    std::ifstream ifs;
    ifs.open(path, std::ios::binary | std::ios::in | std::ios::ate);
    if(!ifs.is_open())
        throw std::runtime_error(std::format("Failed to open file: {}", path.string()));

    auto size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    BinaryData data;
    data.resize(size);

    ifs.read(reinterpret_cast<char*>(data.data()), size);

    return data;
}

ResourceManager::ResourceManager(const ResourceManagerInfo& info)
    : prefixes{.shaders_path_prefix = info.shaders_path_prefix,
               .images_path_prefix = info.images_path_prefix},
      transfer_storage(Engine::GetInstance()->GetRenderEngine())
{
    transfer_channel = new Task<TransferChannel>(
        static_cast<Task<TransferQueue>*>(
            Engine::GetInstance()->GetRenderEngine()->Find(std::string_view("TransferQueue"))),
        TaskBase::TaskKey{.priority = 0, .name = std::string_view("TransferChannel")},
        info.transfer_channel_info);

    Events::Connect<TaskEraseEvent>(transfer_channel,
                                    [this](const TaskEraseEvent&)
                                    {
                                        transfer_channel = nullptr;

                                        return Events::HandlerAction::Erase;
                                    });

    for(const auto& sh_ext_desc: info.shader_resource_descs)
        ext_mappings.shader_ext_mapping.insert(std::pair{sh_ext_desc.ext, sh_ext_desc.desc});
}

hrs::rc_ptr<ResourceManager::ShaderResource> ResourceManager::CreateShader(std::string_view path)
{
    auto res_path = prefixes.shaders_path_prefix / path;

    auto it = resources.shaders.find(res_path.string());
    if(it != resources.shaders.end())
        return it->second;

    if(!std::filesystem::is_regular_file(res_path))
        throw std::runtime_error(std::format("{} is not a file", res_path.string()));

    auto ext = res_path.extension().string();
    auto ext_it = ext_mappings.shader_ext_mapping.find(ext);
    if(ext_it == ext_mappings.shader_ext_mapping.end())
        throw std::runtime_error(std::format("Undefined shader extension: {}", ext));

    auto data = read_file(res_path);
    const Render::ShaderInfo info = {
        .stage = ext_it->second.stage,
        .code = std::span{reinterpret_cast<const char*>(data.data()), data.size()}};

    Render::Shader* shader_handle =
        Engine::GetInstance()->GetRenderEngine()->GetContext()->CreateShader(info);
    hrs::scoped_call cleanup = [shader_handle]()
    {
        delete shader_handle;
    };

    hrs::rc_ptr<ShaderResource> shader_ptr(new ShaderResource(shader_handle));
    cleanup.drop();

    auto ins_it = resources.shaders.insert(std::pair{std::string(path), shader_ptr});

    return ins_it.first->second;
}

hrs::rc_ptr<ResourceManager::ImageResource> ResourceManager::CreateImage(std::string_view path,
                                                                         TransferMode mode)
{
    auto res_path = prefixes.images_path_prefix / path;

    auto it = resources.images.find(res_path.string());
    if(it != resources.images.end())
        return it->second;

    if(!std::filesystem::is_regular_file(res_path))
        throw std::runtime_error(std::format("{} is not a file", res_path.string()));

    auto data = read_file(res_path);

    auto dds_result_exp = DDS::Parse({data.data(), data.size()});
    if(!dds_result_exp)
        throw dds_result_exp.error();

    auto& dds_result = *dds_result_exp;

    auto dds_resolve_exp = DDS::Resolve(dds_result);
    if(!dds_resolve_exp)
        throw dds_resolve_exp.error();

    auto& dds_resolve = *dds_resolve_exp;

    Render::ImageViewType view_type;
    if(dds_resolve.is_cubemap)
    {
        if(dds_resolve.image_info.array_layers == 1)
            view_type = Render::ImageViewType::ImageViewCubeMap;
        else
            view_type = Render::ImageViewType::ImageViewCubeMapArray;
    }
    else
    {
        switch(dds_resolve.image_info.image_type)
        {
            case Render::ImageType::Image1D:
                if(dds_resolve.image_info.array_layers == 1)
                    view_type = Render::ImageViewType::ImageView1D;
                else
                    view_type = Render::ImageViewType::ImageView1DArray;
                break;
            case Render::ImageType::Image2D:
                if(dds_resolve.image_info.array_layers == 1)
                    view_type = Render::ImageViewType::ImageView2D;
                else
                    view_type = Render::ImageViewType::ImageView2DArray;
                break;
            case Render::ImageType::Image3D:
                view_type = Render::ImageViewType::ImageView3D;
                break;
        }
    }

    Render::Image* image_handle = nullptr;
    Render::ImageView* image_view_handle = nullptr;

    hrs::scoped_call cleanup = [&image_handle, &image_view_handle]()
    {
        delete image_view_handle;
        delete image_handle;
    };

    image_handle =
        Engine::GetInstance()->GetRenderEngine()->GetContext()->CreateImage(dds_resolve.image_info);

    const Render::ImageViewInfo view_info = {
        .image = image_handle,
        .view_type = view_type,
        .format = dds_resolve.image_info.format,
        .components = Render::ComponentMapping{.r = Render::ComponentSwizzle::SwizzleIdentity,
                                               .g = Render::ComponentSwizzle::SwizzleIdentity,
                                               .b = Render::ComponentSwizzle::SwizzleIdentity,
                                               .a = Render::ComponentSwizzle::SwizzleIdentity},
        .subresource_range =
            Render::ImageSubresourceRange{.min_mip_level = 0,
                                          .mip_level_count = dds_resolve.image_info.mip_levels,
                                          .min_layer = 0,
                                          .layer_count = dds_resolve.image_info.array_layers}};

    image_view_handle =
        Engine::GetInstance()->GetRenderEngine()->GetContext()->CreateImageView(view_info);

    hrs::rc_ptr<ImageResource> image(new ImageResource(image_handle, image_view_handle));
    cleanup.drop();

    auto ins_it = resources.images.insert(std::pair{std::string(path), image});

    if(mode == TransferMode::Channel && transfer_channel)
    {
        transfer_channel->Reserve(dds_resolve.regions.size() + 1); //image regions itself + callback
        transfer_channel->Transfer(
            TransferImageOperation{.image = ins_it.first->second->GetImage(),
                                   .regions = std::move(dds_resolve.regions)});

        transfer_channel->Transfer(TransferCallbackOperation{
            .cback = [img_entry = ins_it.first->second, _ = std::move(data)]()
            {
                img_entry->SetReady();
            }});
    }
    else //use storage or just fallback
    {
        transfer_storage.Reserve(dds_resolve.regions.size() + 1); //image regions itself + callback
        transfer_storage.Transfer(
            TransferImageOperation{.image = ins_it.first->second->GetImage(),
                                   .regions = std::move(dds_resolve.regions)});

        transfer_storage.Transfer(TransferCallbackOperation{
            .cback = [img_entry = ins_it.first->second, _ = std::move(data)]()
            {
                img_entry->SetReady();
            }});
    }

    return ins_it.first->second;
}

hrs::rc_ptr<ResourceManager::ShaderResource> ResourceManager::FindShader(std::string_view path)
{
    auto it = resources.shaders.find(path);
    if(it == resources.shaders.end())
        return {};

    return it->second;
}

hrs::rc_ptr<ResourceManager::ImageResource> ResourceManager::FindImage(std::string_view path)
{
    auto it = resources.images.find(path);
    if(it == resources.images.end())
        return {};

    return it->second;
}

void ResourceManager::ClearUnused() noexcept
{
    //shaders
    std::erase_if(resources.shaders,
                  [](const auto& kv)
                  {
                      const auto& [k, v] = kv;
                      return v.get_refs() <= 1;
                  });

    //images
    std::erase_if(resources.images,
                  [](const auto& kv)
                  {
                      const auto& [k, v] = kv;
                      return v.get_refs() <= 1 && v->GetState() == ResourceState::Ready;
                  });
}

TransferStorage& ResourceManager::GetTransferStorage() noexcept
{
    return transfer_storage;
}

TransferChannel* ResourceManager::GetTransferChannel() const noexcept
{
    return transfer_channel;
}
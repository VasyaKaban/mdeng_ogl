#include "ResourceManager.h"
#include <fstream>
#include "../RenderEngine/RenderEngine.h"
#include "../Engine.h"
#include "Core/Render/Context.h"
#include "Core/DDS/DDS.h"
#include "hrs/scoped_call.hpp"

namespace Engine
{
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
        : queue_family_index(info.queue_family_index),
          shaders_path_prefix(info.shaders_path_prefix),
          images_path_prefix(info.images_path_prefix),
          transfer_storage(info.render_engine, info.transfer_storage_info)
    {
        for(const auto& sh_ext_desc: info.shader_resource_descs)
            shader_ext_mapping.insert(std::pair{sh_ext_desc.ext, sh_ext_desc.desc});
    }

    hrs::rc_ptr<ResourceManager::ShaderResource>
    ResourceManager::CreateShader(std::string_view path)
    {
        auto res_path = (shaders_path_prefix / path).lexically_normal();
        auto res_path_string = res_path.string();

        auto it = shaders.find(res_path_string);
        if(it != shaders.end())
            return it->second;

        if(!std::filesystem::is_regular_file(res_path))
            throw std::runtime_error(std::format("{} is not a file", res_path_string));

        auto ext = res_path.extension().string();
        auto ext_it = shader_ext_mapping.find(ext);
        if(ext_it == shader_ext_mapping.end())
            throw std::runtime_error(std::format("Undefined shader extension: {}", ext));

        auto data = read_file(res_path);
        const Render::ShaderInfo info = {
            .syntax = Render::ShaderSyntaxFlagBits::GLSL,
            .stage = ext_it->second.stage,
            .code = std::span{reinterpret_cast<const char*>(data.data()), data.size()}};

        Render::Shader* shader_handle =
            Engine::GameEngine::GetInstance()->GetRenderEngine()->GetContext()->CreateShader(info);
        hrs::scoped_call cleanup = [shader_handle]()
        {
            delete shader_handle;
        };

        hrs::rc_ptr<ShaderResource> shader_ptr(new ShaderResource(shader_handle));
        cleanup.drop();

        auto ins_it = shaders.insert(std::pair{res_path_string, shader_ptr});

        return ins_it.first->second;
    }

    hrs::rc_ptr<ResourceManager::ImageResource> ResourceManager::CreateImage(std::string_view path)
    {
        auto res_path = (images_path_prefix / path).lexically_normal();
        auto res_path_string = res_path.string();

        auto it = images.find(res_path_string);
        if(it != images.end())
            return it->second;

        if(!std::filesystem::is_regular_file(res_path))
            throw std::runtime_error(std::format("{} is not a file", res_path_string));

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
            Engine::GameEngine::GetInstance()->GetRenderEngine()->GetContext()->CreateImage(
                dds_resolve.image_info);

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
            Engine::GameEngine::GetInstance()->GetRenderEngine()->GetContext()->CreateImageView(
                view_info);

        hrs::rc_ptr<ImageResource> image(new ImageResource(image_handle, image_view_handle));
        cleanup.drop();

        auto ins_it = images.insert(std::pair{res_path_string, image});

        const Render::ImageMemoryBarrier pre_image_barrier = {
            .src_access = 0,
            .dst_access = transfer_storage.GetTransferAccessFlags(),
            .old_layout = Render::ImageLayout::Undefined,
            .new_layout = transfer_storage.GetTransferImageLayout(),
            .src_queue_family_index = Render::QUEUE_FAMILY_IGNORED,
            .dst_queue_family_index = transfer_storage.GetQueueFamilyIndex(),
            .image = image_handle,
            .subresource_range =
                Render::ImageSubresourceRange{.min_mip_level = 0,
                                              .mip_level_count = dds_resolve.image_info.mip_levels,
                                              .min_layer = 0,
                                              .layer_count = dds_resolve.image_info.array_layers}};

        const Render::PipelineBarrier pre_barrier = {
            .src_stages = Render::PipelineStageFlagBits::TopOfPipePipelineStageBit,
            .dst_stages = Render::PipelineStageFlagBits::TransferPipelineStageBit,
            .dependency = {},
            .buffer_barriers = {},
            .image_barriers = {&pre_image_barrier, 1}};

        transfer_storage.GetCommandBuffer()->SetPipelineBarrier(pre_barrier);

        transfer_storage.Transfer(TransferImageOperation{.image = ins_it.first->second->GetImage(),
                                                         .regions = dds_resolve.regions});

        bool is_depth_stencil_format = Render::IsDepthStencilFormat(dds_resolve.image_info.format);

        const Render::ImageMemoryBarrier post_image_barrier = {
            .src_access = transfer_storage.GetTransferAccessFlags(),
            .dst_access = Render::AccessFlagBits::AccessMemoryReadBit,
            .old_layout = transfer_storage.GetTransferImageLayout(),
            .new_layout = Render::ImageLayout::ShaderReadOnlyOptimal,
            .src_queue_family_index = transfer_storage.GetQueueFamilyIndex(),
            .dst_queue_family_index = queue_family_index,
            .image = image_handle,
            .subresource_range =
                Render::ImageSubresourceRange{.min_mip_level = 0,
                                              .mip_level_count = dds_resolve.image_info.mip_levels,
                                              .min_layer = 0,
                                              .layer_count = dds_resolve.image_info.array_layers}};

        const Render::PipelineBarrier post_barrier = {
            .src_stages = Render::PipelineStageFlagBits::TransferPipelineStageBit,
            .dst_stages = Render::PipelineStageFlagBits::AllGraphicsPipelineStageBit,
            .dependency = {},
            .buffer_barriers = {},
            .image_barriers = {&post_image_barrier, 1}};

        transfer_storage.GetCommandBuffer()->SetPipelineBarrier(post_barrier);

        return ins_it.first->second;
    }

    hrs::rc_ptr<ResourceManager::ShaderResource> ResourceManager::FindShader(std::string_view path)
    {
        auto it = shaders.find(path);
        if(it == shaders.end())
            return {};

        return it->second;
    }

    hrs::rc_ptr<ResourceManager::ImageResource> ResourceManager::FindImage(std::string_view path)
    {
        auto it = images.find(path);
        if(it == images.end())
            return {};

        return it->second;
    }

    void ResourceManager::ClearUnused() noexcept
    {
        //shaders
        std::erase_if(shaders,
                      [](const auto& kv)
                      {
                          const auto& [k, v] = kv;
                          return v.get_refs() <= 1;
                      });

        //images
        std::erase_if(images,
                      [](const auto& kv)
                      {
                          const auto& [k, v] = kv;
                          return v.get_refs() <= 1;
                      });
    }

    TransferStorage& ResourceManager::GetTransferStorage() noexcept
    {
        return transfer_storage;
    }
};
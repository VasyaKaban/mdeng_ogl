#pragma once

#include <span>
#include <filesystem>
#include <unordered_map>
#include "hrs/string_transparent.hpp"
#include "hrs/non_creatable.hpp"
#include "hrs/rc.hpp"
#include "Core/Render/Objects/ImageView.h"
#include "Core/Render/Objects/Shader.h"
#include "Core/Render/Objects/Image.h"
#include "TransferStorage.h"

template<typename T>
struct ResourceExtensionDesc
{
    std::string_view ext;
    T desc;
};

struct ShaderResourceDesc
{
    Render::ShaderStage stage;
};

struct ResourceManagerInfo
{
    std::filesystem::path shaders_path_prefix;
    std::filesystem::path images_path_prefix;

    std::span<const ResourceExtensionDesc<ShaderResourceDesc>> shader_resource_descs;

    TransferStorageInfo transfer_storage_info;
};

class ResourceManager : hrs::non_copyable, hrs::non_movable
{
public:
    enum class ResourceState
    {
        NotReady,
        Ready
    };

    class ShaderResource : public hrs::rc
    {
    public:
        ShaderResource(Render::Shader* _shader) noexcept
            : shader(_shader)
        {}

        ~ShaderResource()
        {
            delete shader;
        }

        Render::Shader* GetShader() const noexcept
        {
            return shader;
        }
    private:
        Render::Shader* shader;
    };

    class ImageResource : public hrs::rc
    {
    public:
        ImageResource(Render::Image* _image, Render::ImageView* _image_view) noexcept
            : image(_image),
              image_view(_image_view),
              state(ResourceState::NotReady)
        {}

        ~ImageResource()
        {
            delete image_view;
            delete image;
        }

        Render::Image* GetImage() const noexcept
        {
            return image;
        }

        Render::ImageView* GetImageView() const noexcept
        {
            return image_view;
        }

        void SetReady() noexcept
        {
            state = ResourceState::Ready;
        }

        ResourceState GetState() const noexcept
        {
            return state;
        }
    private:
        Render::Image* image;
        Render::ImageView* image_view;
        ResourceState state;
    };

    ResourceManager(const ResourceManagerInfo& info);
    ~ResourceManager() = default;

    hrs::rc_ptr<ShaderResource> CreateShader(std::string_view path);
    hrs::rc_ptr<ImageResource> CreateImage(std::string_view path);

    hrs::rc_ptr<ShaderResource> FindShader(std::string_view path);
    hrs::rc_ptr<ImageResource> FindImage(std::string_view path);

    void ClearUnused() noexcept;

    TransferStorage& GetTransferStorage() noexcept;
private:
    struct Prefixes
    {
        std::filesystem::path shaders_path_prefix;
        std::filesystem::path images_path_prefix;
    } prefixes;

    struct ResourceExtensionMappings
    {
        std::unordered_map<std::string,
                           ShaderResourceDesc,
                           hrs::transparent_string_hasher<std::string>,
                           hrs::transparent_string_equal_comparator<std::string>>
            shader_ext_mapping;
    } ext_mappings;

    struct Resources
    {
        template<typename T>
        using resource_map =
            std::unordered_map<std::string,
                               hrs::rc_ptr<T>,
                               hrs::transparent_string_hasher<std::string>,
                               hrs::transparent_string_equal_comparator<std::string>>;

        resource_map<ShaderResource> shaders;
        resource_map<ImageResource> images;
    } resources;

    TransferStorage transfer_storage;

    //shaders[Shader -> handle]
    //textures[Texture -> handle + TextureView handle]
    //objects[WFObject parsed -> only meshes]
    //
};
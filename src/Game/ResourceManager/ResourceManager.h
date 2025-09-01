#pragma once

#include <span>
#include <filesystem>
#include <unordered_map>
#include "hrs/string_transparent.hpp"
#include "hrs/non_creatable.hpp"
#include "hrs/rc.hpp"
#include "Core/Render/Objects/Shader/Shader.h"
#include "Core/Render/Objects/Image/Image.h"

template<typename T>
struct ResourceExtensionDesc
{
    std::string_view ext;
    T desc;
};

struct ShaderResourceDesc
{
    ShaderStage stage;
};

struct ResourceManagerInfo
{
    std::filesystem::path shader_path_prefix;
    std::filesystem::path image_path_prefix;

    std::span<const ResourceExtensionDesc<ShaderResourceDesc>> shader_resource_descs;
};

class ResourceManager : hrs::non_copyable, hrs::non_movable
{
public:
    class ShaderEntry : public hrs::rc
    {
    public:
        ShaderEntry(Shader&& _shader) noexcept;
        ~ShaderEntry() = default;

        Shader* operator->() noexcept;
        Shader* Get() noexcept;
    private:
        Shader shader;
    };

    enum class ImageEntryState
    {
        NotReady,
        Ready
    };

    class ImageEntry : public hrs::rc
    {
    public:
        ImageEntry(Image&& _image) noexcept;
        ~ImageEntry() = default;

        Image* operator->() noexcept;
        Image* Get() noexcept;

        ImageEntryState GetState() const noexcept;
        void SetState(ImageEntryState _state) noexcept;
    private:
        Image image;
        ImageEntryState state;
    };

    ResourceManager(const ResourceManagerInfo& info);
    ~ResourceManager() = default;

    hrs::rc_ptr<ShaderEntry> CreateShader(std::string_view path);
    hrs::rc_ptr<ImageEntry> CreateImage(std::string_view path, bool prefer_image_host_copy);

    hrs::rc_ptr<ShaderEntry> FindShader(std::string_view path);
    hrs::rc_ptr<ImageEntry> FindImage(std::string_view path);

    void ClearUnused() noexcept;
private:
    struct Prefixes
    {
        std::filesystem::path shader_path_prefix;
        std::filesystem::path image_path_prefix;
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

        resource_map<ShaderEntry> shaders;
        resource_map<ImageEntry> images;
    } resources;

    //shaders[Shader -> handle]
    //textures[Texture -> handle + TextureView handle]
    //objects[WFObject parsed -> only meshes]
    //
};
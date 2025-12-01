#include "Config.h"
#include <stdexcept>
#include <format>
#include "Core/Window/RenderBackend.h"

namespace JSON
{
    template<>
    Core::RenderBackendType Parse(const Doc& doc)
    {
        if(!doc.is_string())
            throw std::runtime_error("Failed to parse string");

        const auto& str = doc.get<Doc::string_t>();
        if(str == "opengl")
            return Core::RenderBackendType::OpenGL;
        else
            throw std::runtime_error("Unknown render backend type. Possible values: opengl");
    }
};

#define JSON_DEF
#include "Core/JSON/MetaBegin.h"
#include "Config_Gen.h"
#include "Core/JSON/MetaEnd.h"

#undef JSON_DEF
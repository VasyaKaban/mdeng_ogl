#include "Meta.h"
#include <stdexcept>

namespace JSON
{
    template<>
    std::string Parse(const Doc& doc)
    {
        if(!doc.is_string())
            throw std::runtime_error("Failed to parse string");

        return doc.get<Doc::string_t>();
    }

    template<>
    bool Parse(const Doc& doc)
    {
        if(!doc.is_boolean())
            throw std::runtime_error("Failed to parse bool");

        return doc.get<Doc::boolean_t>();
    }

    template<>
    Doc Parse(const Doc& doc)
    {
        return doc;
    }
};
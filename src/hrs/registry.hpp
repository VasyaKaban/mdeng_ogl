#pragma once

#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <format>
#include "string_transparent.hpp"

namespace hrs
{
    template<typename T, typename I>
    class registry
    {
    public:
        registry() = default;
        ~registry() = default;
        registry(const registry&) = default;
        registry(registry&&) = default;
        registry& operator=(const registry&) = default;
        registry& operator=(registry&&) = default;

        void register_class(const std::string& name, const std::function<T(const I&)>& factory)
        {
            auto [it, inserted] = factories.insert({name, factory});
            if(!inserted)
                throw std::runtime_error(std::format(
                    "Failed to register class because name: {} has been already registered",
                    name));
        }

        void register_class(const std::string& name, std::function<T(const I&)>&& factory)
        {
            auto [it, inserted] = factories.insert({name, std::move(factory)});
            if(!inserted)
                throw std::runtime_error(std::format(
                    "Failed to register class because name: {} has been already registered",
                    name));
        }

        T create_instance(std::string_view name, const I& info)
        {
            auto it = factories.find(name);
            if(it == factories.end())
                throw std::runtime_error(std::format("Class: {} not found in registry", name));

            return it->second(info);
        }

        bool is_registered(std::string_view name) const noexcept
        {
            auto it = factories.find(name);
            return it != factories.end();
        }

        void unregister(std::string_view name) noexcept
        {
            auto it = factories.find(name);
            if(it == factories.end())
                return;

            factories.erase(it);
        }
    private:
        std::unordered_map<std::string,
                           std::function<T(const I&)>,
                           hrs::transparent_string_hasher<std::string>,
                           hrs::transparent_string_equal_comparator<std::string>>
            factories;
    };
};
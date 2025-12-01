#ifdef JSON_DECL
#    define JSON_BEGIN(NAME) \
        struct NAME \
        {
#    define JSON_VALUE(NAME, TYPE, ...) TYPE NAME;

#    define JSON_ARRAY(NAME, TYPE, ...) std::vector<TYPE> NAME;

#    define JSON_MAP(NAME, TYPE, ...) \
        std::unordered_map<std::string, \
                           TYPE, \
                           hrs::transparent_string_hasher<std::string>, \
                           hrs::transparent_string_equal_comparator<std::string>> \
            NAME;

#    define JSON_RAW(NAME, ...) JSON::Doc NAME;

#    define JSON_OPTIONAL_VALUE(NAME, TYPE, ...) std::optional<TYPE> NAME;

#    define JSON_OPTIONAL_ARRAY(NAME, TYPE, ...) std::optional<std::vector<TYPE>> NAME;

#    define JSON_OPTIONAL_MAP(NAME, TYPE, ...) \
        std::optional<std::unordered_map<std::string, \
                                         TYPE, \
                                         hrs::transparent_string_hasher<std::string>, \
                                         hrs::transparent_string_equal_comparator<std::string>>> \
            NAME;

#    define JSON_END(NAME) \
        } \
        ;
#elif defined JSON_DEF
#    define JSON_BEGIN(NAME) \
        namespace JSON \
        { \
            template<> \
            NAME Parse(const Doc& doc) \
            { \
                NAME out;

#    define JSON_VALUE(NAME, TYPE, ...) \
        out.NAME = ParseKey<TYPE>(doc, #NAME); \
        __VA_OPT__(__VA_ARGS__(std::as_const(out.NAME), std::as_const(out));)

#    define JSON_ARRAY(NAME, TYPE, ...) \
        { \
            __VA_OPT__(auto __check = __VA_ARGS__;) \
            auto __jarr = doc[#NAME]; \
            if(!__jarr.is_array()) \
                throw std::runtime_error(#NAME " is not an array"); \
\
            std::size_t __jarr_size = __jarr.size(); \
            out.NAME.reserve(__jarr_size); \
            for(const auto& __jelem: __jarr) \
            { \
                out.NAME.push_back(Parse<TYPE>(__jelem)); \
            } \
            __VA_OPT__(__check(std::as_const(out.NAME), std::as_const(out));) \
        }

#    define JSON_MAP(NAME, TYPE, ...) \
        { \
            __VA_OPT__(auto __check = __VA_ARGS__;) \
            auto __jobj = doc[#NAME]; \
            if(!__jobj.is_object()) \
                throw std::runtime_error(#NAME " is not an object"); \
\
            for(const auto& [k, v]: __jobj.items()) \
            { \
                [[maybe_unused]] auto [it, inserted] = out.NAME.insert({k, Parse<TYPE>(v)}); \
            } \
            __VA_OPT__(__check(std::as_const(out.NAME), std::as_const(out));) \
        }

#    define JSON_RAW(NAME, ...) \
        out.NAME = doc[#NAME]; \
        __VA_OPT__(__VA_ARGS__(std::as_const(out.NAME), std::as_const(out));)

#    define JSON_OPTIONAL_VALUE(NAME, TYPE, ...) \
        out.NAME = ParseOptionalKey<TYPE>(doc, #NAME); \
        __VA_OPT__(__VA_ARGS__(std::as_const(out.NAME), std::as_const(out));)

#    define JSON_OPTIONAL_ARRAY(NAME, TYPE, ...) \
        { \
            __VA_OPT__(auto __check = __VA_ARGS__;) \
            auto __jarr = doc[#NAME]; \
            if(__jarr.is_null()) \
                out.NAME = std::nullopt; \
            else \
            { \
                if(!__jarr.is_array()) \
                    throw std::runtime_error(#NAME " is not an array"); \
\
                out.NAME = decltype(out.NAME){}; \
                std::size_t __jarr_size = __jarr.size(); \
                out.NAME->reserve(__jarr_size); \
                for(const auto& __jelem: __jarr) \
                { \
                    out.NAME->push_back(Parse<TYPE>(__jelem)); \
                } \
            } \
            __VA_OPT__(__check(std::as_const(out.NAME), std::as_const(out));) \
        }

#    define JSON_OPTIONAL_MAP(NAME, TYPE, ...) \
        { \
            __VA_OPT__(auto __check = __VA_ARGS__;) \
            auto __jobj = doc[#NAME]; \
            if(__jobj.is_null()) \
                out.NAME = std::nullopt; \
            else \
            { \
                if(!__jobj.is_object()) \
                    throw std::runtime_error(#NAME " is not an object"); \
\
                out.NAME = decltype(out.NAME){}; \
                for(const auto& [k, v]: __jobj.items()) \
                { \
                    [[maybe_unused]] auto [it, inserted] = out.NAME->insert({k, Parse<TYPE>(v)}); \
                } \
            } \
            __VA_OPT__(__check(std::as_const(out.NAME), std::as_const(out));) \
        }

#    define JSON_END(NAME) \
        return out; \
        } \
        } \
        ;
#else
#    pragma message("Neither JSON_DECL nor JSON_DEF defined!")
#endif
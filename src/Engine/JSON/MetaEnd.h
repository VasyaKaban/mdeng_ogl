#if defined JSON_DECL || defined JSON_DEF
#    undef JSON_BEGIN
#    undef JSON_VALUE
#    undef JSON_ARRAY
#    undef JSON_MAP
#    undef JSON_RAW
#    undef JSON_OPTIONAL_VALUE
#    undef JSON_OPTIONAL_ARRAY
#    undef JSON_OPTIONAL_MAP
#    undef JSON_END
#else
#    pragma message("Neither JSON_DECL nor JSON_DEF defined!")
#endif
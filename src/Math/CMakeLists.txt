
add_library(Math
    GenAlgoOptions.cxx
    MinimizerOptions.cxx
    )

target_include_directories(
    Math
    PRIVATE
    ${PROJECT_SOURCE_DIR}/inc
    )

target_compile_definitions(
    Math
    PRIVATE
    MATH_NO_PLUGIN_MANAGER
    )


install(TARGETS Math
        EXPORT MINUIT2Config
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include)

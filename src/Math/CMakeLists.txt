
add_library(Math
    GenAlgoOptions.cxx
    MinimizerOptions.cxx
    )

target_include_directories(
    Math
    PUBLIC
    ${PROJECT_SOURCE_DIR}/inc
    )

target_compile_definitions(
    Math
    PRIVATE
    MATH_NO_PLUGIN_MANAGER
    )

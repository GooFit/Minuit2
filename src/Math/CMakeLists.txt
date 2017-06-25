
add_library(Math
    GenAlgoOptions.cxx
    MinimizerOptions.cxx
    )

target_include_directories(
    Math
    PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/inc>  
    $<INSTALL_INTERFACE:inc>
    )

target_compile_definitions(
    Math
    PRIVATE
    MATH_NO_PLUGIN_MANAGER
    )

install(TARGETS Math DESTINATION lib EXPORT Minuit2-targets)

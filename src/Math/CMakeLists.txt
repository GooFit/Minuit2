
add_library(Math
    GenAlgoOptions.cxx
    MinimizerOptions.cxx
    )

target_include_directories(
    Math
    PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:include/Minuit2>
    )

target_compile_definitions(
    Math
    PRIVATE
    MATH_NO_PLUGIN_MANAGER
    )

install(TARGETS Math DESTINATION lib EXPORT Minuit2-targets)

target_compile_features(Math PUBLIC cxx_auto_type cxx_static_assert)
set_target_properties(Math PROPERTIES CXX_EXTENSIONS OFF)


install(TARGETS Math
        EXPORT Minuit2Config
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include)


add_library(Math
    GenAlgoOptions.cxx
    MinimizerOptions.cxx
    )

target_include_directories(
    Math
    PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/inc> $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
    )

target_compile_definitions(
    Math
    PRIVATE
    MATH_NO_PLUGIN_MANAGER
    )

#Require c++11
#target_compile_features(Math PUBLIC cxx_constexpr)

install(TARGETS Math
        EXPORT MINUIT2Config
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include)

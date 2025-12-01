set(target shaderModule)
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "*.cpp" "*.h")

add_executable(${target} ${sources})

target_link_libraries(${target} PRIVATE Vulkan::Vulkan glfw)

file(GLOB SHADER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.spv)

FILE(COPY ${SHADER_SOURCES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shaders/)
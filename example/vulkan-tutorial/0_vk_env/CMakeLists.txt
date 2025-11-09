set(target 0_vk_env)
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "*.cpp" "*.h")

add_executable(${target} ${sources})

target_link_libraries(${target} PRIVATE Vulkan::Vulkan glfw)

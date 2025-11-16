PROJECT(VulkanTutorial)
FIND_PACKAGE(Vulkan REQUIRED)

add_subdirectory(00_vk_env)
add_subdirectory(00_base)
add_subdirectory(01_VkInstance)
add_subdirectory(03_physicalDevice)
add_subdirectory(04_logicDevice)
add_subdirectory(05_windowSurface)

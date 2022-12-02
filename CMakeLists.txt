cmake_minimum_required(VERSION 3.15)
project(VulkanTriagle)

SET(CMAKE_CXX_STANDARD 17)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

# add_subdirectory(src)
add_subdirectory(example)

# FILE(GLOB headers ${CMAKE_SOURCE_DIR}/*.h)
# FILE(COPY ${headers} DESTINATION ${CMAKE_SOURCE_DIR}/include)
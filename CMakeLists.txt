cmake_minimum_required(VERSION 3.15)
project(VulkanTriagle)

SET(CMAKE_CXX_STANDARD 17)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# add_subdirectory(src)
add_subdirectory(example)

# FILE(GLOB headers ${CMAKE_SOURCE_DIR}/*.h)
# FILE(COPY ${headers} DESTINATION ${CMAKE_SOURCE_DIR}/include)

set(target_ c01)

FILE(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

find_package(Vulkan REQUIRED)

add_executable(${target_} ${SOURCE_FILES})

target_include_directories(${target_} PRIVATE
        ${CMAKE_SOURCE_DIR}/src/VkBase
)

target_link_libraries(${target_} PRIVATE VKBase Vulkan::Vulkan)

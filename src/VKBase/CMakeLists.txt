set(target_ VKBase)

FILE(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
FILE(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(Stb REQUIRED)

add_library(${target_} STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${target_} PUBLIC glm::glm glfw)
target_include_directories(${target_} PUBLIC ${Stb_INCLUDE_DIR} ${GLM_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIR})


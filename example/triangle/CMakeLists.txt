SET(target_ triangle)

FILE(GLOB sourcefile ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

ADD_EXECUTABLE(${target_}
    ${sourcefile}
)

target_include_directories(${target_}
    PRIVATE
)

TARGET_LINK_DIRECTORIES(${target_}
    PRIVATE
    ${includeDIR}
)

TARGET_LINK_LIBRARIES(${target_} PRIVATE glfw glm::glm Vulkan::Vulkan)

FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/sources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(target_ triangle)

set(includeDIR ${CMAKE_SOURCE_DIR}/include)

FILE(GLOB courcefiles ${CMAKE_CURRENT_SOURCE_DIR}/*.*)

set_target_properties()

add_executable(${target_}
                ${courcefiles}
)

target_include_directories(${target_}
    PRIVATE
    ${includeDIR}
)

target_link_libraries(${target_} PRIVATE glfw glm::glm Vulkan::Vulkan)

FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/sources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


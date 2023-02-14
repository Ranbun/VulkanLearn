SET(target_ triangle)
SET(includeDIR ${CMAKE_SOURCE_DIR}/include)
FILE(GLOB sourcefile ${CMAKE_CURRENT_SOURCE_DIR}/*.*)

ADD_EXECUTABLE(${target_}
        ${sourcefile}
)

TARGET_LINK_DIRECTORIES(${target_}
    PRIVATE
    ${includeDIR}
)

TARGET_LINK_LIBRARIES(${target_} PRIVATE glfw glm::glm Vulkan::Vulkan)

FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/sources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


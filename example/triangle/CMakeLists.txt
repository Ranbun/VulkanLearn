set(target_ triangle)

set(includeDIR ${CMAKE_SOURCE_DIR}/include)

add_executable(${target_}
    main.cpp
    vulkanTriangle.cpp
    vulkanTriangle.h)

target_include_directories(${target_} 
    PRIVATE 
    includeDIR
)

target_link_libraries(${target_} PRIVATE glfw glm::glm Vulkan::Vulkan)

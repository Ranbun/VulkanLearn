set(target windowSurface)
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "*.cpp" "*.h")

add_executable(${target} ${sources})

target_link_libraries(${target} PRIVATE Vulkan::Vulkan glfw)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Configuring for Linux - Linking X11 and XCB")
    find_package(X11 REQUIRED)
    target_link_libraries(${target} PRIVATE
        X11
        X11-xcb
        xcb)
endif()


add_subdirectory(glad)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_subdirectory(glfw)
endif()

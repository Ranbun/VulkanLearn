#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_INCLUDE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

int main()
{
    glfwInit();
    auto window = glfwCreateWindow(800, 600, "Vulkan Wndow", nullptr, nullptr);
    


    std::cout << "Hello World" << std::endl;
    return 0;
}

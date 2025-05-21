#include <iostream>
#include <vector>

#include "VulkanApplication.h"

int main(int argc, char **args)
{
    VulkanApplication app("c05_renderloop", 1000, 800);

    if (!app.getInitialized())
    {
        std::cout << "Failed top initialized VulkanApplication!" << std::endl;
        return 1;
    }

 
    while (!app.shouldClose())
    {
        glfwPollEvents();
    }


    return 0;
}

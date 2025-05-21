#include <iostream>
#include <vector>

#include "VulkanApplication.h"

int main(int argc, char **args)
{
    VulkanApplication app("c05_render loop", 1024, 768);

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

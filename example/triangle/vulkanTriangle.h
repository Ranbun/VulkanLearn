#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

class HelloTriangleApplication
{
public:
    void run();

private:
    void initVulkan();

    void mainLoop();

    void cleanup();



};


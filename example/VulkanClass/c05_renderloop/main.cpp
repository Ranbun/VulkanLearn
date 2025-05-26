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

    VkImageViewCreateInfo imageViewCreateInfo{
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        nullptr, 0,
            nullptr, VK_IMAGE_VIEW_TYPE_2D,
        app.getSurfaceFormat(),
            {
                VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY ,
                VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
    };

    const size_t imageSize = app.getImage().size();
    std::vector<VkImageView> imageView(imageSize);
    for (size_t i = 0; i < imageSize; i++)
    {
        imageViewCreateInfo.image = app.getImage()[i];
        auto res = vkCreateImageView(app.getLogicDevice(), &imageViewCreateInfo,nullptr, &imageView[i]);
        if (res != VK_SUCCESS)
        {
            throw std::runtime_error("vkCreateImageView Failed!");
        }
    }


    while (!app.shouldClose())
    {
        glfwPollEvents();
    }


    return 0;
}

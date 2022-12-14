#ifndef GLFW2VULKANTOOLFUNCTIONSETS_H_
#define GLFW2VULKANTOOLFUNCTIONSETS_H_

#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>
#include <vulkan/vulkan.h>

class GLFW2VulKan
{
public:
    static auto matchGlfwExtensionsInVulKanExtensions(
        std::unordered_set<std::string>& glfwExtensions,
        const std::vector<VkExtensionProperties>& vkExtensions) -> std::vector<std::string>
    {
        std::cout << "GLFW available extension: " << std::endl;
        std::vector<std::string> GLFWSupportExtensionsInVk;
        for (auto & extension : glfwExtensions)
        {
            std::cout << "\t" << extension << std::endl;
        }

        std::cout << "VulKan available extension: " << std::endl;
        for (const auto& it : vkExtensions)
        {
            std::cout << "\t" << it.extensionName << std::endl;

            if (glfwExtensions.find(it.extensionName) != glfwExtensions.end())
            {
                GLFWSupportExtensionsInVk.emplace_back(it.extensionName);
            }
        }

        return std::move(GLFWSupportExtensionsInVk);
    }

};


#endif 
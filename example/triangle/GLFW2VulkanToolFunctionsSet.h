#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>

class GLFW2Vulkan
{
public:
    static std::vector<std::string> matchGLFWExtensionsInVulKanExtensions(
        std::unordered_set<std::string>& GLFWExtensions,
        std::vector<VkExtensionProperties>& vkExtensions)
    {
        std::cout << "GLFW available extension: " << std::endl;
        std::vector<std::string> GLFWSupportExtensionsInVk;
        for (auto & extension : GLFWExtensions)
        {
            std::cout << "\t" << extension << std::endl;
        }

        std::cout << "VulKan available extension: " << std::endl;
        for (const auto& it : vkExtensions)
        {
            std::cout << "\t" << it.extensionName << std::endl;

            if (GLFWExtensions.find(it.extensionName) != GLFWExtensions.end())
            {
                GLFWSupportExtensionsInVk.emplace_back(it.extensionName);
            }
        }

        return std::move(GLFWSupportExtensionsInVk);
    }

};

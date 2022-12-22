#ifndef GLFW2VULKANTOOLFUNCTIONSETS_H_
#define GLFW2VULKANTOOLFUNCTIONSETS_H_

#include <fstream>
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


    static auto readFile(const std::string& filename) -> std::vector<char>
    {
        /// std::ios::ate 从文件尾部开始读
        /// binary 以二进制方式读取文件
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if(!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }

        /// 获取文件长度 
        const auto fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize); 

        /// 读取文件
        file.seekg(0);  ///< 跳转到文件起始位置
        file.read(buffer.data(), static_cast<std::streamsize>(fileSize)); ///< 读取文件 

        /// 关闭文件
        file.close();
        return { buffer };
    }

};


#endif 
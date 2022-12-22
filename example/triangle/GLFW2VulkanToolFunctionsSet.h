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
        /// std::ios::ate ���ļ�β����ʼ��
        /// binary �Զ����Ʒ�ʽ��ȡ�ļ�
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if(!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }

        /// ��ȡ�ļ����� 
        const auto fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize); 

        /// ��ȡ�ļ�
        file.seekg(0);  ///< ��ת���ļ���ʼλ��
        file.read(buffer.data(), static_cast<std::streamsize>(fileSize)); ///< ��ȡ�ļ� 

        /// �ر��ļ�
        file.close();
        return { buffer };
    }

};


#endif 
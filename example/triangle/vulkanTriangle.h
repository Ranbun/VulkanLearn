#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

#define GLFW_INCLUDE_VULKAN

const int WIDTH = 800;
const int HEIGHT = 600;

const std::vector<const char*> validationLayers = {"VK_LAYER_LUNAGR_standard_validation"};

/// 是否使用检验层 
#ifdef NODEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif 

class GLFWwindow;

class HelloTriangleApplication
{
public:
    void run();

private:

    void initWindow();

    void initVulKan();

    void mainLoop() const;

    void cleanup();

    void createInstance();

    bool checkValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> avaliableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, avaliableLayers.data());


        /// 检测所有的检验层都能从列表中找到
        for(auto layerName: validationLayers)
        {
            bool layerFound = false;
            for(const auto& layerProperties: avaliableLayers )
            {
                if(strcmp(layerName,layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }

        }

        return true;
    }


private:

    GLFWwindow* m_window{nullptr};


    /// VulKan
    VkInstance m_vkInstance = nullptr;



};


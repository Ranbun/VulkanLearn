#include "VulkanApplication.h"

#include <iostream>
#include <vector>

VulkanApplication::VulkanApplication(const char *appName)
    : _initiallized(false)
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW." << std::endl;
        return;
    }

    if (!glfwVulkanSupported())
    {
        std::cout << "Vulkan is not supported." << std::endl;
        return;
    }
    _initiallized = createInstance(appName);

    auto selectResult = obtainPhysicalDevice();
    if (selectResult != VK_SUCCESS)
    {
        std::cout << "Can not select Physical device." << std::endl;
    }

    auto createResult = createLogicDevice();
    if (createResult != VK_SUCCESS)
    {
        std::cout << "Can not create logic device." << std::endl;
    }
}

VulkanApplication::~VulkanApplication()
{
    if(_logicDevice)
    {
        vkDestroyDevice(_logicDevice, nullptr);
    }

    if (_initiallized && _instance)
    {
        vkDestroyInstance(_instance, nullptr);
    }

    glfwTerminate();
}

bool VulkanApplication::createInstance(const char *appName)
{
    /// create application
    VkApplicationInfo appinfo{
            VK_STRUCTURE_TYPE_APPLICATION_INFO,
            nullptr,
            "c01_instance",
            VK_MAKE_VERSION(1, 0, 0),
            "VukanCalss",
            VK_MAKE_VERSION(1, 0, 0),
            VK_API_VERSION_1_2};

    /// check support extensions
    uint32_t glfwExtCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);
    std::cout << "GLFW initialized. It requires the following etensions." << std::endl;

    for (auto i = 0; i < glfwExtCount; i++)
    {
        // std::cout << glfwExtensions[i] << std::endl;
    }

    // const char *layersName[] = {"VK_LAYER_KHRONOS_validation"};

    VkInstanceCreateInfo vkInstanceCreateInfo{
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            nullptr, 0,
            &appinfo,
            0, nullptr,
            glfwExtCount,
            glfwExtensions};

    VkResult result = vkCreateInstance(&vkInstanceCreateInfo, nullptr, &_instance);
    if (result != VK_SUCCESS)
    {
        std::cout << "Vukan instance create failed!" << std::endl;
        return false;
    }
    std::cout << "Vukan instance create success!" << std::endl;

    return true;
}

bool VulkanApplication::obtainPhysicalDevice()
{
    /// select physical deveice
    uint32_t deviceCount = 0;
    VkResult reslut = vkEnumeratePhysicalDevices(getInstance(), &deviceCount, nullptr);
    if (reslut != VK_SUCCESS || !deviceCount)
    {
        std::cout << "Failed to find physical devices." << std::endl;
        return reslut;
    }
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(getInstance(), &deviceCount, physicalDevices.data());

    for (auto &device: physicalDevices)
    {
        VkPhysicalDeviceProperties proper;
        vkGetPhysicalDeviceProperties(device, &proper);

        if (proper.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            std::cout << "Device: " << proper.deviceName << "(type = Discrate)" << std::endl;
        }

        if (proper.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            std::cout << "Device: " << proper.deviceName << "(type = Integated)" << std::endl;
        }
    }

    _physicalDevice = physicalDevices[0];

    return VK_SUCCESS;
}

bool VulkanApplication::createLogicDevice()
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(getPhysicalDevice(), &queueFamilyCount, nullptr);
    if (!queueFamilyCount)
    {
        std::cout << "Failed to find physical devices's queueFamily." << std::endl;
        return false;
    }

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(getPhysicalDevice(), &queueFamilyCount, queueFamilies.data());
    for (auto &queueFamily: queueFamilies)
    {
        std::cout << "master physical device supports: " << queueFamily.queueFlags << std::endl;
        std::cout << "  Graphics supports: " << bool(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) << std::endl;
        std::cout << "  Compute supports: " << bool(queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) << std::endl;
    }

    /// requirement phyiscal device  feature
    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    vkGetPhysicalDeviceFeatures(getPhysicalDevice(), &physicalDeviceFeatures);
    std::cout << "Geomtry: " << bool(physicalDeviceFeatures.geometryShader) << std::endl;
    std::cout << "Tessellation: " << bool(physicalDeviceFeatures.tessellationShader) << std::endl;

    ///  create logical device
    ///  创建操作队列
    float proper = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            nullptr, 0,
            0, 1,/// queueFamily, queueCount
            &proper};

    VkDeviceCreateInfo logicDeviceCreateInfo{
            VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            nullptr, 0,
            1,
            &queueCreateInfo,
            0, nullptr,            ///< 开启的层
            0, nullptr,            ///< 启用的扩展
            &physicalDeviceFeatures///< 物理设备特性
    };

    auto reslut = vkCreateDevice(getPhysicalDevice(), &logicDeviceCreateInfo, nullptr, &_logicDevice);
    if (reslut != VK_SUCCESS)
    {
        std::cout << "Failed to create logic Device" << std::endl;
        return reslut;
    }

    return VK_SUCCESS;
}

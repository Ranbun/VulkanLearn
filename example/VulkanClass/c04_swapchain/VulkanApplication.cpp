#include "VulkanApplication.h"

#include <iostream>
#include <vector>


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageServerity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                    void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

const std::vector validationLayers = { "VK_LAYER_KHRONOS_validation" };    /// 验证层扩展
const std::vector deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

VulkanApplication::VulkanApplication(const char *appName, int width, int height)
    : _initialized(false)
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
    _initialized = createInstance(appName);

    if(_initialized)
    {
        _initialized = obtainPhysicalDevice();
    }

    if(_initialized)
    {
        _initialized = createLogicDevice();
    }

    if(_initialized)
    {
        _initialized = createWindowSurface(appName, width, height);
    }
}

VulkanApplication::~VulkanApplication()
{
    if(_surface)
    {
        vkDestroySurfaceKHR(_instance, _surface, nullptr);
    }

    if(_window)
    {
        glfwDestroyWindow(_window);
    }

    if(_logicDevice)
    {
        vkDestroyDevice(_logicDevice, nullptr);
    }

    if (_initialized && _instance)
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
            appName,
            VK_MAKE_VERSION(1, 0, 0),
            "VukanCalss",
            VK_MAKE_VERSION(1, 0, 0),
            VK_API_VERSION_1_2};

    /// check support extensions
    uint32_t glfwExtCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);
    std::cout << "GLFW initialized. It requires the following extensions." << std::endl;

    for (auto i = 0; i < glfwExtCount; i++)
    {
        std::cout << glfwExtensions[i] << std::endl;
    }

    std::vector<char *> layersName{"VK_LAYER_KHRONOS_validation"};

    VkInstanceCreateInfo vkInstanceCreateInfo{
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            nullptr, 0,
            &appinfo,
            0, nullptr,
            //1, layersName,
            glfwExtCount,
            glfwExtensions};

    vkInstanceCreateInfo.enabledLayerCount = 1;
    vkInstanceCreateInfo.ppEnabledLayerNames = layersName.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = debugCallback;

    vkInstanceCreateInfo.pNext = &debugCreateInfo;

    VkResult result = vkCreateInstance(&vkInstanceCreateInfo, nullptr, &_instance);
    if (result != VK_SUCCESS)
    {
        std::cout << "Vulkan instance create failed!" << std::endl;
        return false;
    }
    std::cout << "Vulkan instance create success!" << std::endl;

    return true;
}

bool VulkanApplication::obtainPhysicalDevice()
{
    /// select physical device
    uint32_t deviceCount = 0;
    const VkResult result = vkEnumeratePhysicalDevices(getInstance(), &deviceCount, nullptr);
    if (result != VK_SUCCESS || !deviceCount)
    {
        std::cout << "Failed to find physical devices." << std::endl;
        return false;
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

    return true;
}

bool VulkanApplication::createLogicDevice()
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(getPhysicalDevice(), &queueFamilyCount, nullptr);
    if (!queueFamilyCount)
    {
        std::cout << "Failed to find physical device's queueFamily." << std::endl;
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

    /// requirement physical device  feature
    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    vkGetPhysicalDeviceFeatures(getPhysicalDevice(), &physicalDeviceFeatures);
    std::cout << "Geometry: " << bool(physicalDeviceFeatures.geometryShader) << std::endl;
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

    logicDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    logicDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    logicDeviceCreateInfo.enabledLayerCount = validationLayers.size();
    logicDeviceCreateInfo.ppEnabledLayerNames = validationLayers.data();

    auto result = vkCreateDevice(getPhysicalDevice(), &logicDeviceCreateInfo, nullptr, &_logicDevice);
    if (result != VK_SUCCESS)
    {
        std::cout << "Failed to create logic Device" << std::endl;
        return result;
    }

    return true;
}
bool VulkanApplication::createWindowSurface(const char *name, int width, int height)
{
    /// create surface
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    _window = glfwCreateWindow(width, height, name, nullptr, nullptr);

    _surface = nullptr;
    VkResult result = glfwCreateWindowSurface(getInstance(), _window, nullptr, &_surface);
    if(result != VK_SUCCESS)
    {
        std::cout << "Failed to initialize vulkan surface." << std::endl;
        return false;
    }

    glfwSetWindowUserPointer(_window, this);

    glfwSetMouseButtonCallback(_window, [](GLFWwindow * window, int button, int action, int mods){
           auto pointer = glfwGetWindowUserPointer(window);
           auto app = reinterpret_cast<VulkanApplication*>(pointer);
           app->mouseButtonCallBack(window, button, action, mods);
    });
    glfwSetKeyCallback(_window, [](GLFWwindow * window, int key, int scancode, int action, int mods){
           auto pointer = glfwGetWindowUserPointer(window);
           auto app = reinterpret_cast<VulkanApplication*>(pointer);
           app->keyPressCallBack(window, key, scancode, action, mods);

    });
    glfwSetWindowSizeCallback(_window, [](GLFWwindow * window, int w, int h){
          auto pointer = glfwGetWindowUserPointer(window);
          auto app = reinterpret_cast<VulkanApplication*>(pointer);
          app->resizeCallBack(window, w,h);
    });

    return true;
}

void VulkanApplication::keyPressCallBack(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void)this;

    std::cout << "Key button: " << key << " " << action << std::endl;
    if(key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, 1);
    }
}

void VulkanApplication::resizeCallBack(GLFWwindow *window, int w, int h)
{
    (void)this;

    std::cout << "new resize: " << "w = " << w << "h = " << h << std::endl;
}

void VulkanApplication::mouseButtonCallBack(GLFWwindow *window, int button, int action, int mods)
{
    (void)this;

    std::cout << "Mouse button: " << button << " " << action << std::endl;
}

#include "VulkanApplication.h"

#include <fstream>
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

VulkanApplication::VulkanApplication(const char *appName, const int width, const int height)
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

    if (_initialized)
    {
        _initialized = createSwapChain();
    }

    if (_initialized)
    {
        _initialized = createSwapChainImageView();
    }

    if (_initialized)
    {
        _initialized = createCommandBuffer();
    }

    if (_initialized)
    {
        _initialized = createRenderPass();
    }

//    if (_initialized)
//    {
//        _initialized = setUpGraphicsPipeline(std::string(), std::string(), <#initializer#>, <#initializer#>);
//    }

    if (_initialized)
    {
        _initialized = createFramebuffer(width, height);
    }
}

VulkanApplication::~VulkanApplication()
{
    if (_graphics_pipeline)
    {
        vkDestroyPipeline(_logicDevice, _graphics_pipeline, nullptr);
    }
    /// delete pipeline layout object
    if (_pipelineLayout)
    {
        vkDestroyPipelineLayout(getLogicDevice(), _pipelineLayout, nullptr);
    }

    /// delete render pass
    if (_renderPass)
    {
        /// 之前必须保证所有命令都被提交
        vkDestroyRenderPass(getLogicDevice(), _renderPass, nullptr);
    }

    if (_commandPool)
    {
        vkDestroyCommandPool(getLogicDevice(), _commandPool, nullptr);
    }

    cleanUpSwapChain();

#if 0
    for (auto &fence: _fences | std::views::values)
    {
        vkDestroyFence(getLogicDevice(), fence, nullptr);
    }

    for (auto & semaphore: _semaphores | std::views::values)
    {
        vkDestroySemaphore(getLogicDevice(), semaphore, nullptr);
    }
#else
    for (auto & [name, fence]: _fences)
    {
        vkDestroyFence(getLogicDevice(), fence, nullptr);
    }

    for (auto & [name, semaphore]: _semaphores)
    {
        vkDestroySemaphore(getLogicDevice(), semaphore, nullptr);
    }
#endif


    if (_swapChain)
    {
        vkDestroySwapchainKHR(_logicDevice, _swapChain, nullptr);
    }

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
            "VulkanClass",
            VK_MAKE_VERSION(1, 0, 0),
            VK_API_VERSION_1_2};

    /// check support extensions
    uint32_t glfwExtCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);
    std::cout << "GLFW initialized. It requires the following extensions." << std::endl;

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtCount);

    /// 启用debug支持必须显示指定使用此扩展
    extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    for (const auto &extension: extensions)
    {
        std::cout << extension << std::endl;
    }

    const std::vector<const char *> layersName{"VK_LAYER_KHRONOS_validation"};

    VkInstanceCreateInfo vkInstanceCreateInfo{
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            nullptr, 0,
            &appinfo,
            0, nullptr,
            //1, layersName,
            static_cast<uint32_t>(extensions.size()),
            extensions.data()};

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

    const auto result = vkCreateDevice(getPhysicalDevice(), &logicDeviceCreateInfo, nullptr, &_logicDevice);
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

    _surface = VK_NULL_HANDLE;
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

bool VulkanApplication::createSwapChain()
{
    /// create swap chain
    /// 获取物理设备熟悉
    VkSurfaceCapabilitiesKHR caps;
    VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getPhysicalDevice(), getSurface(), &caps);

    if (result != VK_SUCCESS)
    {
        std::cout << "Failed to get capabilities." << std::endl;
        return false;
    }

    /// 获取物理设备的图像格式 & 图像的呈现方式
    uint32_t formatCount = 0, presentModeCount = 0;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(getPhysicalDevice(), getSurface(), &formatCount, nullptr);

    if (result != VK_SUCCESS || formatCount == 0)
    {
        std::cout << "Failed to get surface formats." << std::endl;
        return false;
    }
    std::vector<VkSurfaceFormatKHR> imageFormat(formatCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(getPhysicalDevice(), getSurface(), &formatCount, imageFormat.data());

    result = vkGetPhysicalDeviceSurfacePresentModesKHR(getPhysicalDevice(), getSurface(), &presentModeCount, nullptr);

    if (result != VK_SUCCESS || presentModeCount == 0)
    {
        std::cout << "Failed to get surface present mode." << std::endl;
        return false;
    }
    std::vector<VkPresentModeKHR> imagePresentMode(presentModeCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(getPhysicalDevice(), getSurface(), &presentModeCount, imagePresentMode.data());

    /// TODO: 需要使用 Mail Box 作为呈现模式
    _presentMode = imagePresentMode[0];
    _imageFormat = imageFormat[0];

    for (auto mode: imagePresentMode)
    {
        if (mode == VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR)
        {
            _presentMode = mode;
            break;
        }
    }

    const VkSwapchainCreateInfoKHR createInfo{
            VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            nullptr, 0,
            getSurface(),
            caps.minImageCount,
            _imageFormat.format, imageFormat[0].colorSpace,
            caps.maxImageExtent,
            1,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_SHARING_MODE_EXCLUSIVE,
            0, nullptr,
            caps.currentTransform,
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            _presentMode,
            VK_TRUE, VK_NULL_HANDLE};

    std::cout << getLogicDevice() << std::endl;

    result = vkCreateSwapchainKHR(getLogicDevice(), &createInfo, nullptr, &_swapChain);
    if (result != VK_SUCCESS)
    {
        std::cout << "Failed  to create swapChain." << std::endl;
        return false;
    }

    /// 记录当前窗口的大小
    _extent = caps.maxImageExtent;

    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(getLogicDevice(), _swapChain, &imageCount, nullptr);

    _swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(getLogicDevice(), _swapChain, &imageCount, _swapChainImages.data());

    return true;
}


bool VulkanApplication::createSwapChainImageView()
{
    /// create swapchain image view
    VkImageViewCreateInfo imageViewCreateInfo{
            VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            nullptr,
            0,
            VK_NULL_HANDLE,
            VK_IMAGE_VIEW_TYPE_2D,
            getSurfaceFormat().format,
            {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
             VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

    const size_t imageSize = getSwapChainImage().size();
    _swapChainImageViews.resize(imageSize);

    for (size_t i = 0; i < imageSize; i++)
    {
        imageViewCreateInfo.image = getSwapChainImage()[i];
        if (const auto res = vkCreateImageView(getLogicDevice(), &imageViewCreateInfo, nullptr, &_swapChainImageViews[i]);
            res != VK_SUCCESS)
        {
            throw std::runtime_error("vkCreateImageView Failed!");
        }
    }

    return true;
}

bool VulkanApplication::createCommandBuffer()
{
    /// create command pool
    VkCommandPoolCreateInfo poolInfo{
            VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, nullptr,
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            0// 队列族的号
    };

    /// 创建指令池
    auto res = vkCreateCommandPool(getLogicDevice(), &poolInfo, nullptr, &_commandPool);
    if (res != VK_SUCCESS)
    {
        std::cout << "Failed create command pool." << std::endl;
        return false;
    }

    // 创建指令缓存  -- 缓存发送的指令
    VkCommandBufferAllocateInfo allocInfo{
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr,
            _commandPool,
            VK_COMMAND_BUFFER_LEVEL_PRIMARY,// 主指令缓存
            1                               // 分配一个指令缓存
    };


    // 从指令池中获取一个指令缓冲
    res = vkAllocateCommandBuffers(getLogicDevice(), &allocInfo, &_commandBuffer);
    if (res != VK_SUCCESS)
    {
        std::cout << "Failed allocate command buffer." << std::endl;
        return false;
    }

    return true;
}

bool VulkanApplication::createRenderPass()
{
    /// 指定使用的颜色和深度缓冲
    /// VkAttachmentDescription 描述的一张缓冲？ 然后将这个东西指定给具体的subpass 作为使用的缓冲对象
    // create render pass
    /// 代表着交換鏈圖像
    VkAttachmentDescription colorAttachment{
            0, getSurfaceFormat().format,
            VK_SAMPLE_COUNT_1_BIT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,  /// 渲染后不關心 緩沖區的顏色
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_IMAGE_LAYOUT_UNDEFINED,   /// 图像之前的内容不能保证是会被保留的
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR   /// 使得渲染后的图像可以被呈现
    };

    /// VkAttachmentReference的attachment成员用来在VkAttachmentDescription结构体数组中找值，这个找到的值会被子流程引用
    VkAttachmentReference colorAttachmentRef{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

    /// 一個渲染流程可以有多個子流程 後一個子流程依賴前一個子流程的輸出(處理后的結果)
    /// 多個子流程組成一個渲染流程
    /// 每個子流程可以引用一個或多個附着 通過 VkAttachmentReference 指定

    /// 創建子流程
    VkSubpassDescription subpass{
            0, VK_PIPELINE_BIND_POINT_GRAPHICS,   /// 指定当前的这个子流程是图形管线的子流程
            0, nullptr,
            1, &colorAttachmentRef,   /// 引用的颜色附着
            nullptr, nullptr,
            0, nullptr};

    VkRenderPassCreateInfo renderPassInfo{
            VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            nullptr, 0,
            1, &colorAttachment,
            1, &subpass,
            0, nullptr};

    const auto res = vkCreateRenderPass(getLogicDevice(), &renderPassInfo, nullptr, &_renderPass);
    if (res != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create render pass!");
        return false;
    }
    return true;
}

bool VulkanApplication::createFramebuffer(int w, int h)
{
    _framebuffers.resize(getSwapChainImageView().size());
    for (auto i = 0; i < _framebuffers.size(); i++)
    {
        VkImageView attachments[] = {getSwapChainImageView()[i]};
        VkFramebufferCreateInfo info{
                VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                nullptr, 0,
                getRenderPass(),
                1, attachments,
                static_cast<uint32_t>(w), static_cast<uint32_t>(h), 1

        };
        const auto res = vkCreateFramebuffer(getLogicDevice(), &info, nullptr, &_framebuffers[i]);
        if (res != VK_SUCCESS)
        {
            return false;
        }
    }

    return true;
}

void VulkanApplication::cleanUpSwapChain() const
{
    for (auto &_framebuffer: _framebuffers)
    {
        vkDestroyFramebuffer(getLogicDevice(), _framebuffer, nullptr);
    }

    for (const auto _swapChainImageView: _swapChainImageViews)
    {
        vkDestroyImageView(getLogicDevice(), _swapChainImageView, nullptr);
    }

    if (_swapChain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(getLogicDevice(), _swapChain, nullptr);
    }
}

bool VulkanApplication::setUpGraphicsPipeline(std::string vertexShader_path, std::string fragmentShader_path,
                                            VkPipelineVertexInputStateCreateInfo &vertexInput,
                                            VkPipelineInputAssemblyStateCreateInfo &vertexInputAssembly)
{
#if 0
    auto vertexShaderModel = createShaderModule("./shaders/sample_vert.spv");
    auto fragShaderModel = createShaderModule("./shaders/sample_frag.spv");

    /// 可编成阶段配置
    /// 创建着色器阶段
    VkPipelineShaderStageCreateInfo shaderStages[] = {
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            nullptr, 0,
            VK_SHADER_STAGE_VERTEX_BIT, vertexShaderModel,
            "main", nullptr  // 通过指定常量优化效率
        },
    {
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        nullptr, 0,
        VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderModel,
        "main", nullptr
        }
    };
#endif

    auto vertexShaderModel = createShaderModule(vertexShader_path);
    auto fragShaderModel = createShaderModule(fragmentShader_path);

    /// 可编成阶段配置
    /// 创建着色器阶段
    VkPipelineShaderStageCreateInfo shaderStages[] = {
            {
                    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    nullptr, 0,
                    VK_SHADER_STAGE_VERTEX_BIT, vertexShaderModel,
                    "main", nullptr  // 通过指定常量优化效率
            },
            {
                    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    nullptr, 0,
                    VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderModel,
                    "main", nullptr
            }
    };



#if 0
    /// 固定功能阶段
    /// 1. 顶点输入
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;

    /// 输入装配
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
#endif

    /// 视口&裁减
    // _swapChainImageViews[0]
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = _extent.width;
    viewport.height = _extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    /// 裁减矩形
    VkRect2D scissor = {};
    scissor.offset = {0,0};
    scissor.extent = _extent;

    /// bind viewport & scissor 需要组合在一起
    std::vector<VkViewport> viewportStates;
    viewportStates.emplace_back(std::move(viewport));

    std::vector<VkRect2D> scissorStates;
    scissorStates.emplace_back(std::move(scissor));

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = viewportStates.data();
    viewportState.scissorCount = 1;
    viewportState.pScissors = scissorStates.data();

    /// 光栅化
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;  // true 表示都不能通过光栅化
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; ///背面剔除
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;    /// 指定顶点顺序 -> 顺时针 or 逆时针
    rasterizer.depthBiasEnable = VK_FALSE;             /// 有点类似于深度偏移
    rasterizer.depthBiasConstantFactor  = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    /// 多重采样 -- 当前的实例中禁用多重采样
    VkPipelineMultisampleStateCreateInfo multisampleState{};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.sampleShadingEnable = VK_FALSE;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleState.minSampleShading = 1.0f;
    multisampleState.pSampleMask = nullptr;
    multisampleState.alphaToCoverageEnable = VK_FALSE;
    multisampleState.alphaToOneEnable = VK_FALSE;

    /// 深度和模板测试
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates;
    colorBlendAttachmentStates.emplace_back(std::move(colorBlendAttachment));

    /// 颜色混合
    VkPipelineColorBlendStateCreateInfo colorBlendingCreateInfo{};
    colorBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendingCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendingCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendingCreateInfo.attachmentCount = 1;
    colorBlendingCreateInfo.pAttachments = colorBlendAttachmentStates.data();
    colorBlendingCreateInfo.blendConstants[0] = 0.0f;
    colorBlendingCreateInfo.blendConstants[1] = 0.0f;
    colorBlendingCreateInfo.blendConstants[2] = 0.0f;
    colorBlendingCreateInfo.blendConstants[3] = 0.0f;
    /// 不设置会覆盖原有颜色
    /// dynamic bind 只有極少數的狀配置可以動態修改
    VkDynamicState dynamicState [] = {
        VK_DYNAMIC_STATE_VIEWPORT,   /// 視口大小
        // VK_DYNAMIC_STATE_LINE_WIDTH, ///
        VK_DYNAMIC_STATE_SCISSOR ///
    };
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = 2;
    dynamicStateCreateInfo.pDynamicStates = dynamicState;

    /// 管线布局
    /// set uniform varianle
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    // pipelineLayoutCreateInfo.pSetLayouts = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    // pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
    if (VK_SUCCESS != vkCreatePipelineLayout(getLogicDevice(), &pipelineLayoutCreateInfo,nullptr, &_pipelineLayout))
    {
        throw std::runtime_error("Failed to Create VkPipelineLayout!");
    }

    /// 创建图形管线
    VkGraphicsPipelineCreateInfo  pipeline_create_info  = {};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.stageCount = 2;
    pipeline_create_info.pStages = shaderStages;
    pipeline_create_info.pVertexInputState = &vertexInput;
    pipeline_create_info.pInputAssemblyState = &vertexInputAssembly;
    pipeline_create_info.pViewportState = &viewportState;
    pipeline_create_info.pRasterizationState = &rasterizer;
    pipeline_create_info.pMultisampleState = &multisampleState;
    pipeline_create_info.pDepthStencilState = nullptr;
    pipeline_create_info.pColorBlendState = &colorBlendingCreateInfo;
    pipeline_create_info.pDynamicState = &dynamicStateCreateInfo;
    pipeline_create_info.layout = _pipelineLayout;
    pipeline_create_info.renderPass = _renderPass;
    pipeline_create_info.subpass = 0;   /// 指定0号子pass
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_create_info.basePipelineIndex = -1;

    auto res = vkCreateGraphicsPipelines(getLogicDevice(),VK_NULL_HANDLE,1, &pipeline_create_info, nullptr, &_graphics_pipeline);
    if (res != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    /// remove shadermodule
    vkDestroyShaderModule(getLogicDevice(), vertexShaderModel, nullptr);
    vkDestroyShaderModule(getLogicDevice(), fragShaderModel, nullptr);

    /// create pipeline success
    return true;
}

VkShaderModule VulkanApplication::createShaderModule(const std::string &name) const
{
    std::ifstream fin(name, std::ios::in | std::ios::binary);

    if (!fin.is_open())
    {
        throw std::runtime_error("Failed to load file!");
    }

    std::string buffer((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());

    const VkShaderModuleCreateInfo createInfo {
        VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        nullptr, 0,
        buffer.size(), reinterpret_cast<const uint32_t *>(buffer.data()),
    };

    VkShaderModule shaderModule = nullptr;
    VkResult result = vkCreateShaderModule(getLogicDevice(), &createInfo, nullptr, &shaderModule);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shader module!");
    }

    return shaderModule;
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

VkFence VulkanApplication::getOrCreateFence(const std::string &name)
{
    if (_fences.count(name))
    {
        return _fences.at(name);
    }

    VkFenceCreateInfo fenceCreateInfo = {
            VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr,
            VK_FENCE_CREATE_SIGNALED_BIT                ///< 创建时指定第一次创建的结果是SIGNALED

    };

    VkFence fence = VK_NULL_HANDLE;

    /// 栅栏 用于CPU & GPU之间的同步
    auto res = vkCreateFence(getLogicDevice(), &fenceCreateInfo, nullptr, &fence);
    if (res != VK_SUCCESS)
    {
        throw std::runtime_error("vkCreateFence Failed!");
        return VK_NULL_HANDLE;
    }

    _fences[name] = fence;

    return fence;
}

VkSemaphore VulkanApplication::getOrCreateSemaphore(const std::string &name)
{
    if (_semaphores.count(name))
    {
        return _semaphores.at(name);
    }

    VkSemaphoreCreateInfo semaphoreCreateInfo = {
            VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, nullptr, 0};

    VkSemaphore semaphore = VK_NULL_HANDLE;

    // 信号量用于GPU之间
    auto res = vkCreateSemaphore(getLogicDevice(), &semaphoreCreateInfo, nullptr, &semaphore);
    if (res != VK_SUCCESS)
    {
        throw std::runtime_error("vkCreateSemaphore Failed!");
        return VK_NULL_HANDLE;
    }

    _semaphores[name] = semaphore;

    return semaphore;
}
void VulkanApplication::submitAndPresent(VkSemaphore waitImage, VkSemaphore waitSubmission, VkFence fenceSubmission, uint32_t imagIndex)
{
    // 提交之前判断下一帧图像是否准备好？ 当前的渲染流水线什么时候等待
    VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSwapchainKHR swapChains[] = {getSwapChain()};

    /// TODO: move to create  device
    VkQueue queue = nullptr;
    vkGetDeviceQueue(getLogicDevice(), 0, 0, &queue);

    /// 提交指令到GPU - VULKAN
    VkSubmitInfo submitInfo{
            VK_STRUCTURE_TYPE_SUBMIT_INFO, nullptr,
            1,
            &waitImage,// 等待信号量  -- 照片是不是执行获取完成
            &waitStageMask,
            1, &_commandBuffer,
            1, &waitSubmission// 提交的指令缓存  执行完成之后通知这个信号量  告诉等待这个信号量的地方开始执行
    };

    vkQueueSubmit(queue, 1, &submitInfo, fenceSubmission);

    VkPresentInfoKHR presentInfo{
            VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            nullptr,
            1, &waitSubmission,
            1, swapChains,
            &imagIndex,
            nullptr};

    vkQueuePresentKHR(queue, &presentInfo);

    /// 等待上次的指令结束执行
    vkQueueWaitIdle(queue);
}


uint32_t VulkanApplication::reCreateSwapChain(int &w, int &h, VkSemaphore &waitImage)
{
    cleanUpSwapChain();
    // 等待设备空闲得时候重新创建交换链
    auto  waitRes = vkDeviceWaitIdle(_logicDevice);
    if (waitRes!=VK_SUCCESS)
    {
        return 0;
    }

    glfwGetWindowSize(_window, &w, &h);
    auto created = createSwapChain();
    if (created) created = createSwapChainImageView();
    if (created) created = createFramebuffer(w,h);

    uint32_t imageIndex = 0;
    if (created)
    {
        /// 重新请求下一张图
        waitRes = vkAcquireNextImageKHR(getLogicDevice(), _swapChain, UINT64_MAX, waitImage,VK_NULL_HANDLE, &imageIndex);
        if (waitRes!=VK_SUCCESS)
        {
            std::cout<<"Failed recreating swapChain!"<<std::endl;
            return 0;
        }
    }

    _extent.width = w;
    _extent.height = h;

    return imageIndex;
}

void VulkanApplication::mouseButtonCallBack(GLFWwindow *window, int button, int action, int mods)
{
    (void)this;

    std::cout << "Mouse button: " << button << " " << action << std::endl;
}

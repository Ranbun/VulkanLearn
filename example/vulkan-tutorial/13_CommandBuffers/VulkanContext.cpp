#include "VulkanContext.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <filesystem>

#include "Application.h"
#include "VulkanFeatureManager.h"
#include "VulkanUtils.h"

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


VulkanContext::VulkanContext(const VulkanFeatureManager &feature) : m_instance(VK_NULL_HANDLE),
                                                                    m_featureManager(feature) {
    init();
}

VulkanContext::~VulkanContext() {
    cleanup();
}

void VulkanContext::cleanup() {
    m_vulkanDebugger.reset();

    if (m_commmandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(m_logicDevice, m_commmandPool, nullptr);
    }

    for (const auto &framebuffer: m_swapChainFramebuffers) {
        if (framebuffer != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(m_logicDevice, framebuffer, nullptr);
        }
    }

    if (m_graphicsPipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_logicDevice, m_graphicsPipeline, nullptr);
    }

    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_logicDevice, m_pipelineLayout, nullptr);
    }

    if (m_renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(m_logicDevice, m_renderPass, nullptr);
    }

    for (auto &imageView: m_swapChainImageViews) {
        if (imageView != VK_NULL_HANDLE) {
            vkDestroyImageView(m_logicDevice, imageView, nullptr);
        }
    }

    if (m_swapChain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(m_logicDevice, m_swapChain, nullptr);
    }

    if (m_surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    }

    if (m_logicDevice != VK_NULL_HANDLE) {
        vkDestroyDevice(m_logicDevice, nullptr);
    }

    if (m_instance != VK_NULL_HANDLE) {
        vkDestroyInstance(m_instance, nullptr);
    }
};

void VulkanContext::init() {
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createCommandBuffers();
};

void VulkanContext::createInstance() {
    /// 检测验证层
    auto &validationLayers = m_featureManager.getValidationLayers();
    bool enablevalidationLayers = !validationLayers.empty();

    if (enablevalidationLayers && !m_featureManager.checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo app_info{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "Hello Vulkan",
        .pEngineName = "No Engine",
        .apiVersion = VK_API_VERSION_1_3,
    };

    VkInstanceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &app_info,

        /// 启用哪些全局验证层
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,

        /// 启用哪些实例扩展
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = nullptr,
    };

    if (!m_featureManager.checkInstanceExtensionSupport()) {
        throw std::runtime_error("some extensions requested, but not available!");
    }

    const auto &instanceExtensions = m_featureManager.getEnabledInstanceExtensions();
    createInfo.enabledExtensionCount = instanceExtensions.size();
    createInfo.ppEnabledExtensionNames = instanceExtensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    if (enablevalidationLayers) {
        const auto &layers = m_featureManager.getValidationLayers();
        createInfo.enabledLayerCount = layers.size();
        createInfo.ppEnabledLayerNames = layers.data();

        VulkanDebugger::populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    }

    if (auto res = vkCreateInstance(&createInfo, nullptr, &m_instance); res != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }

    std::cout << "Create VkInstance Success!" << std::endl;
}

void VulkanContext::setupDebugMessenger() {
    if (const bool enableValidationLayer = m_featureManager.getValidationLayers().empty(); enableValidationLayer) {
        m_vulkanDebugger = std::make_unique<VulkanDebugger>(m_instance);
    }
}

void VulkanContext::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("Failed to found Gpus with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, physicalDevices.data());

    auto findQueueFamilies = [&](const VkPhysicalDevice &device) -> QueueFamilyIndices {
        QueueFamilyIndices indices;

        /// 获取队列族
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        if (queueFamilyCount == 0) {
            throw std::runtime_error("Can not found QueueFamily!.");
        }

        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (auto i = 0; i < queueFamilyCount; i++) {
            auto &queueFamily = queueFamilies.at(i);
            VkBool32 presentSupport = false;

            /// 检测当前设备是否可以在特定的surface上渲染显示结果 -- 查找可以用于呈现的队列族索引
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
            if (presentSupport) {
                indices.presentFamily = i;
            }

            /// 如果支持图形队列 -- 查找可以用于渲染的队列族索引
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            /// others check.....

            if (indices.isComplete()) {
                break;
            }
        }

        return indices;
    };

    auto querySwapChainSupportDetails = [&](const VkPhysicalDevice &device)-> SwapChainSupportDetails {
        SwapChainSupportDetails details;

        ///查询 某个设备对于特定suface的能力
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount,
                                                      details.presentModes.data());
        }

        return details;
    };

    auto isDeviceSuitable = [&findQueueFamilies, querySwapChainSupportDetails, this](const VkPhysicalDevice &device) {
        /// 获取属性
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        std::cout << "find physical device: " << deviceProperties.deviceName << std::endl;

        const auto queueFamily = findQueueFamilies(device);

        const auto extensionSupported = m_featureManager.validateDeviceSupport(device);; ///< 是否支持请求的那些扩展

        bool swapChainAdequate = false;
        if (extensionSupported) {
            const auto swapChainDetails = querySwapChainSupportDetails(device);
            swapChainAdequate = !swapChainDetails.formats.empty() && !swapChainDetails.presentModes.empty();
        }

        return queueFamily.isComplete() && extensionSupported && swapChainAdequate;
    };

    for (auto &device: physicalDevices) {
        if (isDeviceSuitable(device)) /// select the first avaliable device
        {
            m_physicalDevice = device;
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }

    findQueueFamiliesFunc = std::move(findQueueFamilies);
    querySwapChainSupportDetailsFunc = std::move(querySwapChainSupportDetails);
};

void VulkanContext::createLogicDevice() {
    VkDeviceCreateInfo deviceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = 0,
        .pQueueCreateInfos = nullptr,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = nullptr,
        .pEnabledFeatures = nullptr
    };

    /// find queue family : create queue
    auto queueFamily = findQueueFamiliesFunc(m_physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> queueFamilies{queueFamily.graphicsFamily.value(), queueFamily.presentFamily.value()};

    float queuePriorities = 1.0f; /// 设置队列的优先级 [0.0f - 1.0f]
    for (auto queueFamily: queueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = queueFamily,
            .queueCount = 1,
            .pQueuePriorities = &queuePriorities
        };
        queueCreateInfos.emplace_back(queueCreateInfo);
    }


    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();


    /// 指定设备启用的特性
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    /// 设备扩展
    auto requiredExtensions = m_featureManager.getEnabledDeviceExtensions();
    deviceCreateInfo.enabledExtensionCount = requiredExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();

    if (auto res = vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_logicDevice); res != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logic device.");
    }

    vkGetDeviceQueue(m_logicDevice, queueFamily.presentFamily.value(), 0, &m_presentQueue);

    /// 之前的Vulkan分为实例扩展和设备扩展，如果在版本更老Vulkan版本开发，请在创建Device的时候指定验证层和消息传递的扩展

    std::cout << "Create VkDevice Success!" << std::endl;
};

void VulkanContext::createSurface() {
    auto &app = Application::Instance();
    auto *window = static_cast<GLFWwindow *>(app.RenderWindow()->getNativeWindow());

    if (auto res = glfwCreateWindowSurface(m_instance, window, nullptr, &m_surface); res != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }

    if (m_surface == VK_NULL_HANDLE) {
        throw std::runtime_error("must create VkSurface!");
    }

    std::cout << "Create VkSurface Success!" << std::endl;
}

void VulkanContext::createSwapChain() {
    SwapChainSupportDetails details = querySwapChainSupportDetailsFunc(m_physicalDevice);

    using namespace VulkanUtils;
    auto extent = chooseSwapChainExtent(details.capabilities);
    auto surfaceFormat = chooseSwapChainSurfaceFormat(details.formats);
    auto presentMode = chooseSwapChainPresentMode(details.presentModes);

    /// 交换链需要多少张图像
    /// 仅仅满足这个最低要求意味着我们有时可能需要等待驱动程序完成内部操作才能获取另一张用于渲染的图像。因此，建议请求的图像数量至少比最低要求多一张
    auto imageCount = details.capabilities.minImageCount + 1;
    /// 图像的数量不能超过最大图像数量
    if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
        imageCount = details.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapChainCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = m_surface,
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE, /// 不在队列之间共享
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .preTransform = details.capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    auto queueFamily = findQueueFamiliesFunc(m_physicalDevice);
    std::vector<uint32_t> queueFamilyIndices{queueFamily.graphicsFamily.value(), queueFamily.presentFamily.value()};
    std::set<uint32_t> indices{queueFamily.graphicsFamily.value(), queueFamily.presentFamily.value()};
    if (indices.size() == 2) {
        /// 表示两个队列 不相同 需要在两个队列之间共享
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = queueFamilyIndices.size();
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }

    if (queueFamilyIndices[0] != queueFamilyIndices[1]) /// 渲染队列和显示队列不是同一个
    {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = queueFamilyIndices.size();
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    } else {
        /// imageSharingMode 表示交换链图像是否需要在不同的队列共享 -- 这个由渲染队列和表示队列是否是一个来决定
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapChainCreateInfo.queueFamilyIndexCount = 0;
        swapChainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    if (auto res = vkCreateSwapchainKHR(m_logicDevice, &swapChainCreateInfo, nullptr, &m_swapChain);
        res != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swapChain!");
    }

    std::cout << "Create SwapChain Success!" << std::endl;

    /// get swapchain image
    uint32_t swapChainImageCount;
    vkGetSwapchainImagesKHR(m_logicDevice, m_swapChain, &swapChainImageCount, nullptr);
    m_swapChainImages.resize(swapChainImageCount);
    vkGetSwapchainImagesKHR(m_logicDevice, m_swapChain, &swapChainImageCount, m_swapChainImages.data());

    /// restore format & extent
    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;
}

void VulkanContext::createImageViews() {
    m_swapChainImageViews.resize(m_swapChainImages.size());
    for (auto i = 0; i < m_swapChainImages.size(); i++) {
        VkImageSubresourceRange sourceRange{
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        };

        VkImageViewCreateInfo imageViewCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = m_swapChainImages.at(i),
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = m_swapChainImageFormat,
            .components = {
                VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY
            },
            .subresourceRange = sourceRange
        };

        auto res = vkCreateImageView(m_logicDevice, &imageViewCreateInfo, nullptr, &m_swapChainImageViews[i]);
        if (res != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image view!");
        }
    }
}

void VulkanContext::createGraphicsPipeline() {
    const auto vertexShaderCode = VulkanUtils::readFile("./shaders/vert.spv");
    const auto fragmentShaderCode = VulkanUtils::readFile("./shaders/frag.spv");

    /// create shader module
    const auto vertexShaderModule = createShaderModule(vertexShaderCode);
    const auto fragmentShaderModule = createShaderModule(fragmentShaderCode);

    VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertexShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr
    };

    VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragmentShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr
    };

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages{
        vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo
    };

    /// 动态状态 - 需要动态修改的状态
    std::vector<VkDynamicState> dynamicStates{
        VK_DYNAMIC_STATE_VIEWPORT, /// 视口
        VK_DYNAMIC_STATE_SCISSOR /// 裁剪
    };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
    };

    /// 顶点输入  - vertex input
    VkPipelineVertexInputStateCreateInfo vertexInputStageCreatInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr
    };

    /// 输入装配
    VkPipelineInputAssemblyStateCreateInfo assemblyStateCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    /// 视口和裁剪
    VkViewport viewport
    {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(m_swapChainExtent.width),
        .height = static_cast<float>(m_swapChainExtent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor{
        .offset = {0, 0},
        .extent = m_swapChainExtent
    };

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor
    };

    /// 光栅化器
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr, .flags = 0,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL, /// 填充
        .cullMode = VK_CULL_MODE_BACK_BIT, /// 背面剔除
        .frontFace = VK_FRONT_FACE_CLOCKWISE, /// 正面顶点逆时针

        .depthBiasEnable = VK_FALSE, /// 深度偏移
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,

        .lineWidth = 1.0f
    };

    /// 多重采样
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE
    };

    /// 深度和模板测试
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_GREATER
    };
    /// 目前不配置这个结构体 直接传一个 nullptr

    /// color blend

    /// 控制全局的颜色混合参数
    /// 争对单个附件
    VkPipelineColorBlendAttachmentState ColorBlendAttachmentState{
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                          VK_COLOR_COMPONENT_A_BIT
    };

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &ColorBlendAttachmentState,
        .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
    };

    /// 管道布局
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };

    if (vkCreatePipelineLayout(m_logicDevice, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stageCount = 2,
        .pStages = shaderStages.data(),
        .pVertexInputState = &vertexInputStageCreatInfo,
        .pInputAssemblyState = &assemblyStateCreateInfo,
        .pTessellationState = nullptr,
        .pViewportState = &viewportStateCreateInfo,
        .pRasterizationState = &rasterizationStateCreateInfo,
        .pMultisampleState = &multisampleStateCreateInfo,
        .pDepthStencilState = nullptr,
        .pColorBlendState = &colorBlendStateCreateInfo,
        .pDynamicState = &dynamicStateCreateInfo,
        .layout = m_pipelineLayout,
        .renderPass = m_renderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0
    };

    if (vkCreateGraphicsPipelines(m_logicDevice, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr,
                                  &m_graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(m_logicDevice, vertexShaderModule, nullptr);
    vkDestroyShaderModule(m_logicDevice, fragmentShaderModule, nullptr);
}

VkShaderModule VulkanContext::createShaderModule(const std::vector<char> &code) const {
    VkShaderModuleCreateInfo createInfo
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = code.size(),
        .pCode = reinterpret_cast<const uint32_t *>(code.data())
    };

    VkShaderModule shaderModule{VK_NULL_HANDLE};
    if (vkCreateShaderModule(m_logicDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module!");
    }
    return shaderModule;
}

void VulkanContext::createRenderPass() {
    VkAttachmentDescription colorAttachmentDescription{
        .flags = 0,
        .format = m_swapChainImageFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    /// 子通道
    /// 子通道引用的colorAttachment
    VkAttachmentReference colorAttachmentReference{
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpassDescription{
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentReference,
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr
    };

    VkRenderPassCreateInfo renderPassInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .attachmentCount = 1,
        .pAttachments = &colorAttachmentDescription,
        .subpassCount = 1,
        .pSubpasses = &subpassDescription,
        .dependencyCount = 0,
        .pDependencies = nullptr,
    };

    if (vkCreateRenderPass(m_logicDevice, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void VulkanContext::createFramebuffers() {
    const size_t bufferSize = m_swapChainFramebuffers.size();
    m_swapChainFramebuffers.resize(bufferSize);
    for (size_t i = 0; i < bufferSize; ++i) {
        std::vector<VkImageView> attachmentView{m_swapChainImageViews[i]};

        VkFramebufferCreateInfo framebufferCreateInfo{
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = m_renderPass,
            .attachmentCount = static_cast<uint32_t>(attachmentView.size()),
            .pAttachments = attachmentView.data(),
            .width = m_swapChainExtent.width,
            .height = m_swapChainExtent.height,
            .layers = 1
        };

        if (vkCreateFramebuffer(m_logicDevice, &framebufferCreateInfo, nullptr, &m_swapChainFramebuffers.at(i)) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer!");
        }
    }
}

void VulkanContext::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamiliesFunc(m_physicalDevice);
    VkCommandPoolCreateInfo commandPoolCreateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value()
    };

    if (vkCreateCommandPool(m_logicDevice, &commandPoolCreateInfo, nullptr, &m_commmandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");
    }
}

void VulkanContext::createCommandBuffers() {
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = m_commmandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };

    if (vkAllocateCommandBuffers(m_logicDevice, &commandBufferAllocateInfo, &m_commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers!");
    }
}

void VulkanContext::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pInheritanceInfo = nullptr
    };

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    VkClearValue clearColor = {{0.0f, 0.0f, 0.0f, 1.0f}};
    VkRenderPassBeginInfo renderPassBeginInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = m_renderPass,
        .framebuffer = m_swapChainFramebuffers.at(imageIndex),
        .renderArea = {0, 0, m_swapChainExtent},
        .clearValueCount = 1,
        .pClearValues = &clearColor
    };

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);

    VkViewport viewport = {
        .x = 0,
        .y = 0,
        .width = static_cast<float>(m_swapChainExtent.width),
        .height = static_cast<float>(m_swapChainExtent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    VkRect2D scissor = {
        .offset = {0,0},
        .extent = m_swapChainExtent
    };

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }

}

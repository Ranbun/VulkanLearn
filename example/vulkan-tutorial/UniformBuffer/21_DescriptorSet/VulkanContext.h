#ifndef VULKANCONTEXT_H
#define VULKANCONTEXT_H

#include <functional>
#include <memory>

#include "Vertex.h"
#include "VulkanDebugger.h"
#include "VulkanFeatureManager.h"

struct QueueFamilyIndices;
struct SwapChainSupportDetails;

/// 可以同时处理的帧数
constexpr int MAX_FRAMES_IN_FLIGHT = 2;

using WindowFunc = std::function<void *()>;

class VulkanContext
{
public:
    explicit VulkanContext(const VulkanFeatureManager &feature, WindowFunc getWindows = nullptr);
    ~VulkanContext();
    VulkanContext(const VulkanContext &) = delete;
    VulkanContext &operator=(const VulkanContext &) = delete;

    void drawFrame();

    void OnResize(uint32_t width, uint32_t height);

private:
    /**
     * @brief init
     *
     */
    void init();

    /**
     * @brief cleanup resource
     *
     */
    void cleanup();

    /**
     * @brief Create an Instance object
     *
     */
    void createInstance();

    /**
     * @brief select physical device
     *
     */
    void pickPhysicalDevice();


    /**
     * @brief Create a Logic Device object
     *
     */
    void createLogicDevice();

    /**
     * @brief Create a Surface object
     *
     */
    void createSurface();

    void createSwapChain();

    void createImageViews();

    void createGraphicsPipeline();

    [[nodiscard]] VkShaderModule createShaderModule(const std::vector<char> &code) const;

    void createRenderPass();

    void createFramebuffers();

    void createCommandPool();

    void createCommandBuffers();

    void recordCommandBuffer(const VkCommandBuffer &commandBuffer, uint32_t imageIndex) const;

    void createSyncObjects();

    void recreateSwapChain();

    void cleanSwapChain() const;

    /// 创建 vertex buffer & index buffer
    void createVertexBuffers();
    void createIndexBuffers();

    /// 创建描述符布局
    void createDescriptorSetLayout();
    void createUniformBuffers();

    /**
     * @brief update unform buffer data
     *
     */
    void updateUniformBuffer(uint32_t frame) const;

    void createDescriptorPool();

    void createDescriptorSet();

    /**
     * @brief setup messenger
     *
     */
    void setupDebugMessenger();

private:
    const VulkanFeatureManager &m_featureManager;
    std::unique_ptr<VulkanDebugger> m_vulkanDebugger{nullptr};

    VkInstance m_instance{VK_NULL_HANDLE};
    VkPhysicalDevice m_physicalDevice{VK_NULL_HANDLE};
    VkDevice m_logicDevice{VK_NULL_HANDLE};
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
    VkQueue m_presentQueue{VK_NULL_HANDLE};
    VkQueue m_graphicsQueue{VK_NULL_HANDLE};
    VkPipelineLayout m_pipelineLayout{VK_NULL_HANDLE};
    VkRenderPass m_renderPass{VK_NULL_HANDLE};
    VkPipeline m_graphicsPipeline{VK_NULL_HANDLE};
    VkCommandPool m_commandPool{VK_NULL_HANDLE};

    std::vector<VkCommandBuffer> m_commandBuffers{VK_NULL_HANDLE};
    std::vector<VkFramebuffer> m_swapChainFramebuffers;

    VkSwapchainKHR m_swapChain{VK_NULL_HANDLE};
    std::vector<VkImage> m_swapChainImages{VK_NULL_HANDLE};
    VkFormat m_swapChainImageFormat{VK_FORMAT_UNDEFINED};
    VkExtent2D m_swapChainExtent{0, 0};
    std::vector<VkImageView> m_swapChainImageViews{VK_NULL_HANDLE};

    /// buffer
    std::vector<Vertex> m_vertices;
    VkBuffer m_vertexBuffer{VK_NULL_HANDLE};
    VkDeviceMemory m_vertexBufferMemory{VK_NULL_HANDLE};
    std::vector<uint16_t> m_indices;
    VkBuffer m_indexBuffer{VK_NULL_HANDLE};
    VkDeviceMemory m_indexBufferMemory{VK_NULL_HANDLE};

    /// 同步
    std::vector<VkSemaphore> m_imageAvailableSemaphores{VK_NULL_HANDLE};
    std::vector<VkSemaphore> m_renderFinishedSemaphores{VK_NULL_HANDLE};
    std::vector<VkFence> m_inFlightFences{VK_NULL_HANDLE};

    /// DescriptorSet
    VkDescriptorSetLayout m_descriptorSetLayout{VK_NULL_HANDLE};
    VkDescriptorPool m_descriptorPool{VK_NULL_HANDLE};
    std::vector<VkDescriptorSet> m_descriptorsets;

    /// uniform buffer
    std::vector<VkBuffer> m_uniformBuffers;
    std::vector<VkDeviceMemory> m_uniformBuffersMemory;
    std::vector<void *> m_uniformBuffersMapped;

    uint32_t m_currentFrame = 0;

    bool framebufferResized = false;

    std::function<QueueFamilyIndices(VkPhysicalDevice &)> findQueueFamiliesFunc;
    std::function<SwapChainSupportDetails(VkPhysicalDevice &)> querySwapChainSupportDetailsFunc;
    std::function<void *()> m_getWindowFunc;
    std::function<void(const VkBuffer srcBuffer, const VkBuffer dstBuffer, const VkDeviceSize bufferSize)>
            m_copyBufferFunc;
};

#endif

#ifndef VULKANCONTEXT_H
#define VULKANCONTEXT_H

#include <functional>
#include <memory>
#include <vulkan/vulkan.h>

#include "VulkanDebugger.h"
#include "VulkanFeatureManager.h"

struct QueueFamilyIndices;
struct SwapChainSupportDetails;

class VulkanContext
{
public:
    explicit VulkanContext(const VulkanFeatureManager &feature);
    ~VulkanContext();
    VulkanContext(const VulkanContext &) = delete;
    VulkanContext &operator=(const VulkanContext &) = delete;

    void drawFrame();

    VkInstance getInstance() const { return m_instance; }
    VkDevice getDevice() const { return m_logicDevice; }

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

    VkShaderModule createShaderModule(const std::vector<char> &code) const;

    void createRenderPass();

    void createFramebuffers();

    void createCommandPool();

    void createCommandBuffers();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) const;

    void createSyncObjects();

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
    VkCommandPool m_commmandPool{VK_NULL_HANDLE};
    VkCommandBuffer m_commandBuffer{VK_NULL_HANDLE};

    std::vector<VkFramebuffer> m_swapChainFramebuffers;

    VkSwapchainKHR m_swapChain{VK_NULL_HANDLE};
    std::vector<VkImage> m_swapChainImages{VK_NULL_HANDLE};
    VkFormat m_swapChainImageFormat{VK_FORMAT_UNDEFINED};
    VkExtent2D m_swapChainExtent{0, 0};
    std::vector<VkImageView> m_swapChainImageViews{VK_NULL_HANDLE};

    /// 同步
    VkSemaphore m_imageAvailableSemaphore{VK_NULL_HANDLE};
    VkSemaphore m_renderFinishedSemaphore{VK_NULL_HANDLE};
    VkFence m_inFlightFence{VK_NULL_HANDLE};

    std::function<QueueFamilyIndices(VkPhysicalDevice &)> findQueueFamiliesFunc;
    std::function<SwapChainSupportDetails(VkPhysicalDevice &)> querySwapChainSupportDetailsFunc;
};

#endif

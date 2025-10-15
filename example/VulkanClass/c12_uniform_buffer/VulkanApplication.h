#ifndef VULKANAPPLICATION_H_
#define VULKANAPPLICATION_H_

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <map>

/**
 * @brief Vulkan Application
 *
 */
class VulkanApplication
{
public:
    explicit VulkanApplication(const char *app_name, int width = 1000, int height = 800);
    VulkanApplication(const VulkanApplication &) = delete;
    VulkanApplication(VulkanApplication &&) = delete;
    VulkanApplication& operator=(const VulkanApplication &) = delete;
    VulkanApplication &operator=(VulkanApplication &&) = delete;
    ~VulkanApplication();

    [[nodiscard]] VkInstance GetInstance() const { return instance; }
    [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
    [[nodiscard]] VkSurfaceKHR getSurface() const { return surface; }
    [[nodiscard]] VkDevice getLogicDevice() const { return logicDevice; }

    [[nodiscard]] VkSurfaceFormatKHR getSurfaceFormat() const { return surfaceImageFormat; }
    [[nodiscard]] std::vector<VkImage> getSwapChainImage() const { return swapChainImages; }
    [[nodiscard]] VkPresentModeKHR getPresentMode() const { return presentMode; }
    [[nodiscard]] VkSwapchainKHR getSwapChain() const { return swapChain; }
    [[nodiscard]] VkCommandBuffer getCommandBuffer() const { return commandBuffer; }
    std::vector<VkImageView> &getSwapChainImageView() { return swapChainImageViews; }
    VkRenderPass &getRenderPass() { return renderPass; }
    std::vector<VkFramebuffer> &getFrameBuffer() { return frameBuffers; }
    VkPipeline &getGraphicsPipeline() { return graphicsPipeline; }
    VkPipelineLayout & GetPipelineLayout() {return pipelineLayout;}

    VkFence getOrCreateFence(const std::string &name);
    VkSemaphore getOrCreateSemaphore(const std::string &name);

    void submitAndPresent(VkSemaphore waitImage, VkSemaphore waitSubmission, VkFence fenceSubmission, uint32_t imagIndex);

    [[nodiscard]] bool getInitialized() const { return initialized; }
    [[nodiscard]] bool shouldClose() const { return glfwWindowShouldClose(window); }
    [[maybe_unused]] void setClose() const { glfwSetWindowShouldClose(window, 1); }
    uint32_t reCreateSwapChain(int &w, int &h, VkSemaphore &wait_image);

    [[nodiscard]] VkShaderModule createShaderModule(const std::string &name) const;

    bool setUpGraphicsPipeline(const std::string &vertex_shader_path, const std::string &fragment_shader_path,
                               VkPipelineVertexInputStateCreateInfo &vertex_input,
                               VkPipelineInputAssemblyStateCreateInfo &vertex_input_assembly, VkDescriptorSetLayout desc_set);

    void mouseButtonCallBack(GLFWwindow *window, int button, int action, int mods);
    void keyPressCallBack(GLFWwindow *window, int key, int scancode, int action, int mods);
    void resizeCallBack(GLFWwindow *window, int w, int h);

private:
    bool createInstance(const char *app_name);
    bool obtainPhysicalDevice();
    bool createLogicDevice();
    bool createWindowSurface(const char *name, int width, int height);
    bool createSwapChain();
    bool createSwapChainImageView();

    bool createCommandBuffer();
    bool createRenderPass();
    bool createFrameBuffer(int w, int h);
    void cleanUpSwapChain() const;


    VkInstance instance{};
    VkPhysicalDevice physicalDevice{};
    VkDevice logicDevice{};
    VkSurfaceKHR surface{};
    GLFWwindow *window{};

    VkCommandPool commandPool{};
    VkCommandBuffer commandBuffer{};

    std::vector<VkFramebuffer> frameBuffers;
    VkRenderPass renderPass{};

    /// swap chain
    VkSwapchainKHR swapChain{};
    std::vector<VkImage> swapChainImages{};
    std::vector<VkImageView> swapChainImageViews{};
    VkPresentModeKHR presentMode{};
    VkSurfaceFormatKHR surfaceImageFormat{};

    VkPipelineLayout pipelineLayout{};
    VkPipeline graphicsPipeline{};

    std::map<std::string, VkFence> fences;
    std::map<std::string, VkSemaphore> semaphores;

    VkExtent2D extent{};

    bool initialized{false};
};
#endif// VULKANAPPLICATION_H_

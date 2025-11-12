#ifndef APPLICATION_H
#define APPLICATION_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <vulkan/vulkan.h>

extern const std::vector<const char *> validationLayers;

#ifdef NDEBUG
constexpr bool enableValidationLayers() { return false; };
#else
constexpr bool enableValidationLayers() { return true; }
#endif

class Application
{
public:
    Application();
    ~Application();

    /**
     * @brief Run the application main loop
     */
    void run();

private:
    /**
     * @brief Initialize the window
     *
     */
    void initWindow();
    /**
     * @brief Initialize Vulkan components
     */
    void initVulkan();

    /**
     * @brief Main application loop
     */
    void mainLoop();

    /**
     * @brief Cleanup Vulkan resources
     */
    void cleanup();

    /**
     * @brief Check system requirements
     *
     */
    bool checkRequirementsExtensionSupport(std::vector<const char *> &requiredExtensions);

    /**
     * @brief Get the Requirement Extensions object
     *
     * @return std::vector<const char *>
     */
    std::vector<const char *> getRequirementExtensions();

    /**
     * @brief Check if validation layers are supported
     *
     * @return true
     * @return false
     */
    bool checkValidationLayerSupport();

    /**
     * @brief Create a Instance object
     *
     */
    void createInstance();

    /**
     * @brief setup debug messenger
     *
     */
    void setupDebugMessenger();

protected:
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks *pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                                "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator,
                                          VkDebugUtilsMessengerEXT *debugMessenger)
    {
        /// load debug messenger function
        auto func =
                (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func)
        {
            return func(instance, pCreateInfo, pAllocator, debugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }


private:
    GLFWwindow *window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
};

#endif

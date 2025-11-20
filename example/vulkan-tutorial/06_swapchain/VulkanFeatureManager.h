#ifndef VULKANLEARN_VULKANFEATUREMANAGER_H
#define VULKANLEARN_VULKANFEATUREMANAGER_H

#include <algorithm>
#include <iostream>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

enum class EngineFeature
{
    Compute,
    SwapChain,
    RayTracing,
    MeshShader,
    DebugUtils,
    Bindless
};

class VulkanFeatureManager
{
public:
    VulkanFeatureManager();

    void requestFeature(EngineFeature feature);

    void requestInstanceExtension(const char *extName);
    void requestDeviceExtension(const char *extName);

    const std::vector<const char *> &getEnabledInstanceExtensions() const;
    const std::vector<const char *> &getEnabledDeviceExtensions() const;
    const std::vector<const char *> &getValidationLayers() const;

    void enableValidationLayers(bool enable);

    bool validateDeviceSupport(VkPhysicalDevice device) const;
    bool checkInstanceExtensionSupport() const;
    bool checkValidationLayerSupport() const;

private:
    std::vector<const char *> m_instanceExtensions;
    std::vector<const char *> m_deviceExtensions;
    std::vector<const char *> m_validationLayers;

    std::set<std::string> m_uniqueInstanceExtensions;
    std::set<std::string> m_uniqueDeviceExtensions;

    bool m_validationEnabled = false;
};


#endif // VULKANLEARN_VULKANFEATUREMANAGER_H

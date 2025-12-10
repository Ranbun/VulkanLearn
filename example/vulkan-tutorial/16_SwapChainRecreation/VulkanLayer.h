#ifndef VULKANLEARN_VULKANLAYER_H
#define VULKANLEARN_VULKANLAYER_H

#include "Layer.h"
#include "VulkanFeatureManager.h"

class VulkanContext;

using WindowFunc = std::function<void *()>;

class VulkanLayer : public Layer {
public:
    VulkanLayer(const std::string &name, const VulkanFeatureManager &feature_manager, WindowFunc func);

    ~VulkanLayer() override;

    void OnAttach() override;

    void OnDetach() override;

    void OnUpdate(float ts) override;

    void OnEvent(Event &event) override;

private:
    std::unique_ptr<VulkanContext> m_vkContext;
    VulkanFeatureManager featureManager;
};


#endif //VULKANLEARN_VULKANLAYER_H

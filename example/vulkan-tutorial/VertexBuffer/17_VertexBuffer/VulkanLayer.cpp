#include "VulkanLayer.h"
#include "VulkanContext.h"

VulkanLayer::VulkanLayer(const std::string &name, const VulkanFeatureManager &feature_manager, WindowFunc func)
    : Layer(name)
    , featureManager(feature_manager)
    , m_vkContext(std::make_unique<VulkanContext>(feature_manager, func))
{
}

VulkanLayer::~VulkanLayer() {
    m_vkContext.reset();
}

void VulkanLayer::OnAttach() {
    Layer::OnAttach();
}

void VulkanLayer::OnDetach() {
    m_vkContext.reset();
}

void VulkanLayer::OnUpdate(float ts) {
    m_vkContext->drawFrame();
}

void VulkanLayer::OnEvent(Event &event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizeEvent>([this](const WindowResizeEvent &event)-> bool {
        m_vkContext->OnResize(event.GetWidth(), event.GetHeight());
        return false;
    });
}

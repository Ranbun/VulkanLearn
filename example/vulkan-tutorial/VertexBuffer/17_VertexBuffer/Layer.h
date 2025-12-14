#ifndef LAYER_H_
#define LAYER_H_

#include "Event.h"
#include <string>

class Layer
{
public:
    Layer(const std::string& name = "Layer") : m_DebugName(name)
    {
    }

    virtual ~Layer() = default;

    virtual void OnAttach()
    {
    } // 当层被添加到应用时
    virtual void OnDetach()
    {
    } // 当层被移除时
    virtual void OnUpdate(float ts)
    {
    } // 每一帧更新逻辑
    virtual void OnEvent(Event& event)
    {
    } // 接收并处理事件

    const std::string& GetName() const { return m_DebugName; }

protected:
    std::string m_DebugName;
};

#endif

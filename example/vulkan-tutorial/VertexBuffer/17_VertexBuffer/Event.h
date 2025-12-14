#ifndef EVENT_H
#define EVENT_H

#include <functional>
#include <string>

enum class EventType {
    None = 0,
    WindowClose,
    WindowResize,
    KeyPressed,
    KeyReleased,
};

class Event {
public:
    virtual ~Event() = default;

    virtual EventType GetEventType() const = 0;

    virtual const char *GetName() const = 0;

    bool Handled = false; // 如果一层处理了事件，设为true，阻止向下传递
};

// 具体的窗口调整大小事件
class WindowResizeEvent : public Event {
public:
    WindowResizeEvent(unsigned int width, unsigned int height)
        : m_Width(width), m_Height(height) {
    }

    unsigned int GetWidth() const { return m_Width; }
    unsigned int GetHeight() const { return m_Height; }

    EventType GetEventType() const override { return EventType::WindowResize; }
    static EventType GetStaticType() { return EventType::WindowResize; }

    const char *GetName() const override { return "WindowResize"; }

private:
    unsigned int m_Width, m_Height;
};

class AppCloseEvent : public Event {
public:
    AppCloseEvent(bool close)
        :m_close(close)
    {
    }
    bool GetClose() const { return m_close; }
    EventType GetEventType() const override { return EventType::WindowClose; }
    static EventType GetStaticType() { return EventType::WindowClose; }
    const char *GetName() const override { return "WindowClose"; }

private :
    bool m_close{false};
};


class EventDispatcher {
public:
    EventDispatcher(Event &event)
        : m_Event(event) {
    }

    template<typename T, typename F>
    bool Dispatch(const F &func) {
        if (m_Event.GetEventType() == T::GetStaticType()) {
            // T::GetStaticType 需要在具体事件类中定义静态方法，或者简化为直接比较
            // 这里为了演示简便，假设匹配成功
            // 实际工程中建议在具体Event类加 static EventType GetStaticType()
            if (dynamic_cast<T *>(&m_Event)) {
                m_Event.Handled = func(*(T *) &m_Event);
                return true;
            }
        }
        return false;
    }

private:
    Event &m_Event;
};


#endif //！ EVENT_H

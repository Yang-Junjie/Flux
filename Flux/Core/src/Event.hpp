// Copyright 2026 Beisent
// Event system for Flux framework

#ifndef FLUX_CORE_SRC_EVENT_HPP_
#define FLUX_CORE_SRC_EVENT_HPP_

#include <cstdint>
#include <functional>
#include <string>

namespace flux
{

  enum class EventType
  {
    None = 0,
    WindowClose,
    WindowResize,
    WindowFocus,
    WindowLostFocus,
    WindowMoved,
    KeyPressed,
    KeyReleased,
    KeyTyped,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled
  };

  enum EventCategory
  {
    None = 0,
    EventCategoryApplication = 1 << 0,
    EventCategoryInput = 1 << 1,
    EventCategoryKeyboard = 1 << 2,
    EventCategoryMouse = 1 << 3,
    EventCategoryMouseButton = 1 << 4
  };

#define EVENT_CLASS_TYPE(type)                                                 \
  static EventType GetStaticType() { return EventType::type; }                 \
  virtual EventType GetEventType() const override { return GetStaticType(); }  \
  virtual const char *GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category)                                         \
  virtual int GetCategoryFlags() const override { return category; }

  class Event
  {
  public:
    virtual ~Event() = default;

    bool handled = false;

    virtual EventType GetEventType() const = 0;
    virtual const char *GetName() const = 0;
    virtual int GetCategoryFlags() const = 0;
    virtual std::string ToString() const { return GetName(); }

    bool IsInCategory(EventCategory category) const
    {
      return GetCategoryFlags() & category;
    }
  };

  class EventDispatcher
  {
  public:
    explicit EventDispatcher(Event &event) : event_(event) {}

    template <typename T, typename F> bool Dispatch(const F &func)
    {
      if (event_.GetEventType() == T::GetStaticType())
      {
        event_.handled |= func(static_cast<T &>(event_));
        return true;
      }
      return false;
    }

  private:
    Event &event_;
  };

  // Window Events
  class WindowResizeEvent : public Event
  {
  public:
    WindowResizeEvent(uint32_t width, uint32_t height)
        : width_(width), height_(height)
    {
    }

    uint32_t GetWidth() const { return width_; }
    uint32_t GetHeight() const { return height_; }

    std::string ToString() const override
    {
      return "WindowResizeEvent: " + std::to_string(width_) + ", " +
             std::to_string(height_);
    }

    EVENT_CLASS_TYPE(WindowResize)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)

  private:
    uint32_t width_, height_;
  };

  class WindowCloseEvent : public Event
  {
  public:
    WindowCloseEvent() = default;

    EVENT_CLASS_TYPE(WindowClose)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
  };

  // Key Events
  class KeyEvent : public Event
  {
  public:
    int GetKeyCode() const { return key_code_; }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

  protected:
    explicit KeyEvent(int keycode) : key_code_(keycode) {}

    int key_code_;
  };

  class KeyPressedEvent : public KeyEvent
  {
  public:
    KeyPressedEvent(int keycode, int repeat_count)
        : KeyEvent(keycode), repeat_count_(repeat_count)
    {
    }

    int GetRepeatCount() const { return repeat_count_; }

    std::string ToString() const override
    {
      return "KeyPressedEvent: " + std::to_string(key_code_) +
             " (" + std::to_string(repeat_count_) + " repeats)";
    }

    EVENT_CLASS_TYPE(KeyPressed)

  private:
    int repeat_count_;
  };

  class KeyReleasedEvent : public KeyEvent
  {
  public:
    explicit KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}

    std::string ToString() const override
    {
      return "KeyReleasedEvent: " + std::to_string(key_code_);
    }

    EVENT_CLASS_TYPE(KeyReleased)
  };

  class KeyTypedEvent : public KeyEvent
  {
  public:
    explicit KeyTypedEvent(int keycode) : KeyEvent(keycode) {}

    std::string ToString() const override
    {
      return "KeyTypedEvent: " + std::to_string(key_code_);
    }

    EVENT_CLASS_TYPE(KeyTyped)
  };

  // Mouse Events
  class MouseMovedEvent : public Event
  {
  public:
    MouseMovedEvent(float x, float y) : mouse_x_(x), mouse_y_(y) {}

    float GetX() const { return mouse_x_; }
    float GetY() const { return mouse_y_; }

    std::string ToString() const override
    {
      return "MouseMovedEvent: " + std::to_string(mouse_x_) + ", " +
             std::to_string(mouse_y_);
    }

    EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

  private:
    float mouse_x_, mouse_y_;
  };

  class MouseScrolledEvent : public Event
  {
  public:
    MouseScrolledEvent(float x_offset, float y_offset)
        : x_offset_(x_offset), y_offset_(y_offset)
    {
    }

    float GetXOffset() const { return x_offset_; }
    float GetYOffset() const { return y_offset_; }

    std::string ToString() const override
    {
      return "MouseScrolledEvent: " + std::to_string(x_offset_) + ", " +
             std::to_string(y_offset_);
    }

    EVENT_CLASS_TYPE(MouseScrolled)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

  private:
    float x_offset_, y_offset_;
  };

  class MouseButtonEvent : public Event
  {
  public:
    int GetMouseButton() const { return button_; }

    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput |
                         EventCategoryMouseButton)

  protected:
    explicit MouseButtonEvent(int button) : button_(button) {}

    int button_;
  };

  class MouseButtonPressedEvent : public MouseButtonEvent
  {
  public:
    explicit MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

    std::string ToString() const override
    {
      return "MouseButtonPressedEvent: " + std::to_string(button_);
    }

    EVENT_CLASS_TYPE(MouseButtonPressed)
  };

  class MouseButtonReleasedEvent : public MouseButtonEvent
  {
  public:
    explicit MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

    std::string ToString() const override
    {
      return "MouseButtonReleasedEvent: " + std::to_string(button_);
    }

    EVENT_CLASS_TYPE(MouseButtonReleased)
  };

} // namespace flux

#endif // FLUX_CORE_SRC_EVENT_HPP_

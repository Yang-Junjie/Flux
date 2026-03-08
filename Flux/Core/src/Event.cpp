// Copyright 2026 Beisent

#include "Event.hpp"

namespace flux {

std::string WindowResizeEvent::ToString() const
{
    return "WindowResizeEvent: " + std::to_string(width_) + ", " + std::to_string(height_);
}

std::string KeyPressedEvent::ToString() const
{
    return "KeyPressedEvent: " + std::to_string(key_code_) + " (" +
           std::to_string(repeat_count_) + " repeats)";
}

std::string KeyReleasedEvent::ToString() const
{
    return "KeyReleasedEvent: " + std::to_string(key_code_);
}

std::string KeyTypedEvent::ToString() const
{
    return "KeyTypedEvent: " + std::to_string(key_code_);
}

std::string MouseMovedEvent::ToString() const
{
    return "MouseMovedEvent: " + std::to_string(mouse_x_) + ", " + std::to_string(mouse_y_);
}

std::string MouseScrolledEvent::ToString() const
{
    return "MouseScrolledEvent: " + std::to_string(x_offset_) + ", " +
           std::to_string(y_offset_);
}

std::string MouseButtonPressedEvent::ToString() const
{
    return "MouseButtonPressedEvent: " + std::to_string(button_);
}

std::string MouseButtonReleasedEvent::ToString() const
{
    return "MouseButtonReleasedEvent: " + std::to_string(button_);
}

} // namespace flux

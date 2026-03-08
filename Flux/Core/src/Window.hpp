// Copyright 2026 Beisent
// Window abstraction for Flux framework

#ifndef FLUX_CORE_SRC_WINDOW_HPP_
#define FLUX_CORE_SRC_WINDOW_HPP_

#include "Event.hpp"

#include <cstdint>

#include <functional>
#include <string>

struct GLFWwindow;

namespace flux {

struct WindowSpecification {
    std::string title = "App";
    uint32_t width = 800;
    uint32_t height = 600;
    bool fullscreen = false;
    bool resizable = true;
    bool decorated = true;
    bool maximized = false;
    bool vsync = true;
    int msaa_samples = 0;
};

class Window {
public:
    using EventCallbackFn = std::function<void(Event&)>;

    explicit Window(const WindowSpecification& spec);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void SwapBuffers();
    void PollEvents();

    [[nodiscard]] uint32_t GetWidth() const;
    [[nodiscard]] uint32_t GetHeight() const;
    void SetVSync(bool enabled);
    [[nodiscard]] bool IsVSync() const;
    [[nodiscard]] void* GetNativeWindow() const;
    [[nodiscard]] float GetTime() const;

    void SetEventCallback(const EventCallbackFn& callback);

private:
    void Init();
    void Shutdown();
    void SetupCallbacks();

    struct WindowData {
        std::string title;
        uint32_t width = 0;
        uint32_t height = 0;
        bool vsync = true;
        EventCallbackFn event_callback;
    };

    GLFWwindow* window_ = nullptr;
    WindowData data_;
    WindowSpecification spec_;
};

} // namespace flux

#endif // FLUX_CORE_SRC_WINDOW_HPP_

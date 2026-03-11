// Copyright 2026 Beisent
// Window abstraction implementation

#include "Window.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace flux {

Window::Window(const WindowSpecification& spec)
    : spec_(spec)
{
    data_.title = spec.title;
    data_.width = spec.width;
    data_.height = spec.height;
    data_.vsync = spec.vsync;

    Init();
}

Window::~Window()
{
    Shutdown();
}

void Window::Init()
{
    if (!glfwInit()) {
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, spec_.resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, spec_.decorated ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, spec_.maximized ? GLFW_TRUE : GLFW_FALSE);

    if (spec_.msaa_samples > 0) {
        glfwWindowHint(GLFW_SAMPLES, spec_.msaa_samples);
    }

    GLFWmonitor* monitor = spec_.fullscreen ? glfwGetPrimaryMonitor() : nullptr;

    window_ = glfwCreateWindow(spec_.width, spec_.height, spec_.title.c_str(), monitor, nullptr);

    if (!window_) {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(spec_.vsync ? 1 : 0);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        return;
    }

    if (spec_.msaa_samples > 0) {
        glEnable(GL_MULTISAMPLE);
    }

    glfwSetWindowUserPointer(window_, &data_);
    SetupCallbacks();
}

void Window::Shutdown()
{
    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    glfwTerminate();
}

void Window::SetupCallbacks()
{
    glfwSetWindowCloseCallback(window_, [](GLFWwindow* window) {
        auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        WindowCloseEvent event;
        data.event_callback(event);
    });

    glfwSetWindowSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
        auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        data.width = width;
        data.height = height;
        WindowResizeEvent event(width, height);
        data.event_callback(event);
    });

    glfwSetKeyCallback(window_,
                       [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                           auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
                           switch (action) {
                               case GLFW_PRESS: {
                                   KeyPressedEvent event(key, 0);
                                   data.event_callback(event);
                                   break;
                               }
                               case GLFW_RELEASE: {
                                   KeyReleasedEvent event(key);
                                   data.event_callback(event);
                                   break;
                               }
                               case GLFW_REPEAT: {
                                   KeyPressedEvent event(key, 1);
                                   data.event_callback(event);
                                   break;
                               }
                           }
                       });

    glfwSetCharCallback(window_, [](GLFWwindow* window, unsigned int keycode) {
        auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        KeyTypedEvent event(keycode);
        data.event_callback(event);
    });

    glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods) {
        auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        switch (action) {
            case GLFW_PRESS: {
                MouseButtonPressedEvent event(button);
                data.event_callback(event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event(button);
                data.event_callback(event);
                break;
            }
        }
    });

    glfwSetScrollCallback(window_, [](GLFWwindow* window, double xOffset, double yOffset) {
        auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
        data.event_callback(event);
    });

    glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double xPos, double yPos) {
        auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
        data.event_callback(event);
    });
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(window_);
}

void Window::PollEvents()
{
    glfwPollEvents();
}

uint32_t Window::GetWidth() const
{
    return data_.width;
}

uint32_t Window::GetHeight() const
{
    return data_.height;
}

void Window::SetVSync(bool enabled)
{
    glfwSwapInterval(enabled ? 1 : 0);
    data_.vsync = enabled;
}

bool Window::IsVSync() const
{
    return data_.vsync;
}

void* Window::GetNativeWindow() const
{
    return window_;
}

float Window::GetTime() const
{
    return static_cast<float>(glfwGetTime());
}

void Window::SetEventCallback(const EventCallbackFn& callback)
{
    data_.event_callback = callback;
}

} // namespace flux

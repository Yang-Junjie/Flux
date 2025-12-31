#pragma once
#include "Layer.hpp"

#include <imgui.h>
#include <memory>
#include <vector>
#include <functional>
#include <string>

#ifdef ANDROID
#include <GLES3/gl3.h>
#include <android_native_app_glue.h>
#include <EGL/egl.h>
#else
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif

namespace Flux {

    struct ApplicationSpecification {
        std::string Name = "App";
        uint32_t Width = 800;
        uint32_t Height = 600;
#ifdef ANDROID
        struct android_app* AndroidApp = nullptr;
#endif
    };

    class Application {
    public:
        explicit Application(const ApplicationSpecification &spec = ApplicationSpecification());

        virtual ~Application() = default;

        void Run();

        void PushLayer(std::unique_ptr<Layer> layer);

        void SetMenubarCallback(std::function<void()> callback);

#ifdef ANDROID
        ANativeWindow* GetWindowHandle() const { return m_AndroidWindow; }
#else
        GLFWwindow* GetWindowHandle() const { return m_WindowHandle; }
#endif

        float GetTime() const;

        void Close();

    private:
        void Init();
        void Shutdown();

#ifdef ANDROID
        void PlatformInitEGL();
#endif

    private:
        ApplicationSpecification m_Specification;
        bool m_Running = false;

#ifndef ANDROID
        GLFWwindow* m_WindowHandle = nullptr;
#else
        ANativeWindow* m_AndroidWindow = nullptr;
        EGLDisplay m_Display = EGL_NO_DISPLAY;
        EGLSurface m_Surface = EGL_NO_SURFACE;
        EGLContext m_Context = EGL_NO_CONTEXT;
#endif

        std::vector<std::unique_ptr<Layer>> m_LayerStack;
        std::function<void()> m_MenubarCallback;

        float m_TimeStep = 0.0f;
        float m_FrameTime = 0.0f;
        float m_LastFrameTime = 0.0f;
    };

    // 工厂函数
    std::unique_ptr<Application> CreateApplication();

} // namespace Flux

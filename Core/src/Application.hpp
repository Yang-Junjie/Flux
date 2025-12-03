#pragma once
#include "Layer.hpp"

#include <imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <functional>
namespace Flux
{
    class Application
    {
    public:
        Application();
        virtual ~Application() = default;
        void Run();
        void SetMenubarCallback(std::function<void()> callback);

        float GetTime() const;

        void PushLayer(std::unique_ptr<Layer> layer);
        void Close();

    private:
        void Init();
        void Shutdown();

    private:
        bool m_Running = false;
        GLFWwindow *m_WindowHandle = nullptr;
        std::vector<std::unique_ptr<Layer>> m_LayerStack;
        std::function<void()> m_MenubarCallback;

        float m_TimeStep = 0.0f;
        float m_FrameTime = 0.0f;
        float m_LastFrameTime = 0.0f;
    };
    std::unique_ptr<Application> CreateApplication();
}
#pragma once
#include "Layer.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
namespace Flux
{
    class Application
    {
    public:
        Application();
        virtual ~Application() = default;
        void Run();
        void pushLayer(std::unique_ptr<Layer> layer);
        void Close();

    private:
        void Init();
        void Shutdown();

    private:
        bool m_Running = false;
        GLFWwindow *m_WindowHandle = nullptr;
        std::vector<std::unique_ptr<Layer>> m_LayerStack;
    };
    std::unique_ptr<Application> CreateApplication();
}
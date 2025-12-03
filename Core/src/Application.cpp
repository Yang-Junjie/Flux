#include "Application.hpp"

namespace Flux
{
    Application::Application()
    {
        Init();
    }

    void Application::Run()
    {
        m_Running = true;
        while (m_Running)
        {
            if (glfwWindowShouldClose(m_WindowHandle))
                m_Running = false;
            for (auto &layer : m_LayerStack)
            {
                layer->OnUpdate(0.016f);
            }
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glfwSwapBuffers(m_WindowHandle);

            glfwPollEvents();
        }
        Shutdown();
    }

    void Application::pushLayer(std::unique_ptr<Layer> layer)
    {
        layer->OnAttach();
        m_LayerStack.push_back(std::move(layer));
    }

    void Application::Close()
    {
        m_Running = false;
    }

    void Application::Init()
    {
        if (!glfwInit())
        {
            return;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        m_WindowHandle = glfwCreateWindow(800, 600, "Flux Application", nullptr, nullptr);
        if (!m_WindowHandle)
        {
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(m_WindowHandle);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            return;
        }
    }

    void Application::Shutdown()
    {
        for (auto &layer : m_LayerStack)
        {
            layer->OnDetach();
        }
        m_LayerStack.clear();
        glfwDestroyWindow(m_WindowHandle);
        glfwTerminate();
    }
}
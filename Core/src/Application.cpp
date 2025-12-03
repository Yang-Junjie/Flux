#include "Application.hpp"
#include <glm/glm.hpp>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
namespace Flux
{
    Application::Application()
    {
        Init();
    }

    void Application::Run()
    {
        m_Running = true;
        while (!glfwWindowShouldClose(m_WindowHandle) && m_Running)
        {

            float time = GetTime();
            m_FrameTime = time - m_LastFrameTime;
            m_TimeStep = glm::min<float>(m_FrameTime, 0.0333f);
            m_LastFrameTime = time;

            glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            for (auto &layer : m_LayerStack)
            {
                layer->OnUpdate(m_TimeStep);
            }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            {
                static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

                // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
                // because it would be confusing to have two docking targets within each others.
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
                if (m_MenubarCallback)
                    window_flags |= ImGuiWindowFlags_MenuBar;

                const ImGuiViewport *viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

                // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
                // and handle the pass-thru hole, so we ask Begin() to not render a background.
                if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                    window_flags |= ImGuiWindowFlags_NoBackground;

                // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
                // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
                // all active windows docked into it will lose their parent and become undocked.
                // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
                // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                ImGui::Begin("DockSpace Demo", nullptr, window_flags);
                ImGui::PopStyleVar();

                ImGui::PopStyleVar(2);

                // Submit the DockSpace
                ImGuiIO &io = ImGui::GetIO();
                if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
                {
                    ImGuiID dockspace_id = ImGui::GetID("VulkanAppDockspace");
                    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
                }

                if (m_MenubarCallback)
                {
                    if (ImGui::BeginMenuBar())
                    {
                        m_MenubarCallback();
                        ImGui::EndMenuBar();
                    }
                }

                for (auto &layer : m_LayerStack)
                    layer->OnRenderUI();

                ImGui::End();
            }
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            ImGuiIO &io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow *backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }

            glfwSwapBuffers(m_WindowHandle);

            glfwPollEvents();
        }
        Shutdown();
    }

    void Application::SetMenubarCallback(std::function<void()> callback)
    {
        m_MenubarCallback = callback;
    }

    float Application::GetTime() const
    {
        return static_cast<float>(glfwGetTime());
    }

    void Application::PushLayer(std::unique_ptr<Layer> layer)
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
        glfwSetFramebufferSizeCallback(m_WindowHandle, [](GLFWwindow *window, int width, int height)
                                       { glViewport(0, 0, width, height); });

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

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(m_WindowHandle, true);
        ImGui_ImplOpenGL3_Init("#version 430");
    }

    void Application::Shutdown()
    {
        for (auto &layer : m_LayerStack)
        {
            layer->OnDetach();
        }
        m_LayerStack.clear();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(m_WindowHandle);
        glfwTerminate();
    }
}

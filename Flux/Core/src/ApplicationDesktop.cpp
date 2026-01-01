#include "Application.hpp"

#include <glm/glm.hpp>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Flux {

    struct Application::PlatformState {
        GLFWwindow* WindowHandle = nullptr;
    };

    Application::Application(const ApplicationSpecification& spec)
        : m_Specification(spec) {
        if (m_Specification.ImGuiUIScale > 0.0f)
            m_UIScale = m_Specification.ImGuiUIScale;
        Init();
    }

    Application::~Application() = default;

    void Application::Init() {
        m_Platform = std::make_unique<PlatformState>();

        if (!glfwInit())
            return;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_Platform->WindowHandle = glfwCreateWindow(
                m_Specification.Width,
                m_Specification.Height,
                m_Specification.Name.c_str(),
                nullptr,
                nullptr);

        if (!m_Platform->WindowHandle) {
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(m_Platform->WindowHandle);
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
            return;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(m_Platform->WindowHandle, true);
        ImGui_ImplOpenGL3_Init("#version 430");
    }

    void Application::Run() {
        if (!m_Platform || !m_Platform->WindowHandle)
            return;

        m_Running = true;
        while (m_Running) {
            float time = GetTime();
            m_FrameTime = time - m_LastFrameTime;
            m_TimeStep = glm::min<float>(m_FrameTime, 0.0333f);
            m_LastFrameTime = time;

            glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            for (auto& layer : m_LayerStack)
                layer->OnUpdate(m_TimeStep);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
            if (m_MenubarCallback)
                window_flags |= ImGuiWindowFlags_MenuBar;

            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar |
                            ImGuiWindowFlags_NoCollapse |
                            ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoBringToFrontOnFocus |
                            ImGuiWindowFlags_NoNavFocus;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace Demo", nullptr, window_flags);
            ImGui::PopStyleVar(1);

            ImGuiID dockspace_id = ImGui::GetID("DockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockspace_flags);

            if (m_MenubarCallback) {
                if (ImGui::BeginMenuBar()) {
                    m_MenubarCallback();
                    ImGui::EndMenuBar();
                }
            }

            for (auto& layer : m_LayerStack)
                layer->OnRenderUI();

            ImGui::End();
            ImGui::PopStyleVar(2);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }

            glfwSwapBuffers(m_Platform->WindowHandle);
            glfwPollEvents();

            if (glfwWindowShouldClose(m_Platform->WindowHandle))
                m_Running = false;
        }

        Shutdown();
    }

    void Application::Shutdown() {
        for (auto& layer : m_LayerStack)
            layer->OnDetach();
        m_LayerStack.clear();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        if (m_Platform && m_Platform->WindowHandle) {
            glfwDestroyWindow(m_Platform->WindowHandle);
            m_Platform->WindowHandle = nullptr;
        }

        glfwTerminate();
        m_Platform.reset();
    }

    void* Application::GetNativeWindow() const {
        if (!m_Platform)
            return nullptr;
        return m_Platform->WindowHandle;
    }

    float Application::GetTime() const {
        return static_cast<float>(glfwGetTime());
    }

} // namespace Flux

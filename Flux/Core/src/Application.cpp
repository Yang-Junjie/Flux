#include "Application.hpp"
#include <glm/glm.hpp>

#ifdef ANDROID
#include <backends/imgui_impl_android.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLES3/gl3.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <time.h>
#else
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#endif

#ifdef ANDROID
extern struct android_app* g_AndroidApp;
#endif

namespace Flux {

Application::Application(const ApplicationSpecification &spec)
    : m_Specification(spec) {
    Init();
}

void Application::Init() {
#ifdef ANDROID
    g_AndroidApp = m_Specification.AndroidApp;

    // EGL 初始化
    PlatformInitEGL();

    // ImGui 初始化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // 保留 Dock
    io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable; // 禁用多窗口
    ImGui::StyleColorsDark();
    ImGui_ImplAndroid_Init(g_AndroidApp->window);
    ImGui_ImplOpenGL3_Init("#version 300 es");

#else
    if (!glfwInit()) return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_WindowHandle = glfwCreateWindow(m_Specification.Width,
                                      m_Specification.Height,
                                      m_Specification.Name.c_str(),
                                      nullptr, nullptr);
    if (!m_WindowHandle) {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_WindowHandle);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_WindowHandle, true);
    ImGui_ImplOpenGL3_Init("#version 430");
#endif
}

float Application::GetTime() const {
#ifdef ANDROID
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + now.tv_nsec * 1e-9f;
#else
    return static_cast<float>(glfwGetTime());
#endif
}

void Application::Run() {
    m_Running = true;
    while (m_Running) {
#ifdef ANDROID
        // Android 事件循环
        int events;
        struct android_poll_source* source;
        int pollResult;
        do {
            pollResult = ALooper_pollOnce(0, nullptr, &events, (void**)&source);
            if (source) source->process(g_AndroidApp, source);
        } while (pollResult >= 0);

        if (g_AndroidApp && g_AndroidApp->destroyRequested)
            m_Running = false;
#endif

        float time = GetTime();
        m_FrameTime = time - m_LastFrameTime;
        m_TimeStep = glm::min<float>(m_FrameTime, 0.0333f);
        m_LastFrameTime = time;

        glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (auto &layer: m_LayerStack)
            layer->OnUpdate(m_TimeStep);

        ImGui_ImplOpenGL3_NewFrame();
#ifdef ANDROID
        ImGui_ImplAndroid_NewFrame();
#else
        ImGui_ImplGlfw_NewFrame();
#endif
        ImGui::NewFrame();

        // DockSpace UI
#ifdef ANDROID
        // 单窗口 DockSpace 模拟
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                        ImGuiWindowFlags_NoCollapse |
                                        ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove |
                                        ImGuiWindowFlags_NoDocking;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar(2);

        ImGuiID dockspace_id = ImGui::GetID("DockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_None);

        for (auto &layer : m_LayerStack)
            layer->OnRenderUI();

        ImGui::End();

#else
        // 桌面 DockSpace 保留
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
        if (m_MenubarCallback) window_flags |= ImGuiWindowFlags_MenuBar;

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

        for (auto &layer: m_LayerStack)
            layer->OnRenderUI();

        ImGui::End();
        ImGui::PopStyleVar(2);
#endif

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#ifndef ANDROID
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(m_WindowHandle);
        glfwPollEvents();
        if (glfwWindowShouldClose(m_WindowHandle))
            m_Running = false;
#else
        eglSwapBuffers(m_Display, m_Surface);
#endif
    }

    Shutdown();
}

void Application::Shutdown() {
    for (auto &layer: m_LayerStack)
        layer->OnDetach();
    m_LayerStack.clear();

#ifdef ANDROID
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();

    eglMakeCurrent(m_Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(m_Display, m_Context);
    eglDestroySurface(m_Display, m_Surface);
    eglTerminate(m_Display);
#else
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_WindowHandle);
    glfwTerminate();
#endif
}

void Application::PushLayer(std::unique_ptr<Layer> layer) {
    layer->OnAttach();
    m_LayerStack.push_back(std::move(layer));
}

void Application::SetMenubarCallback(std::function<void()> callback) {
    m_MenubarCallback = callback;
}

void Application::Close() { m_Running = false; }

#ifdef ANDROID
void Application::PlatformInitEGL() {
    if (!g_AndroidApp || !g_AndroidApp->window)
        return;

    m_AndroidWindow = g_AndroidApp->window;

    m_Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_Display == EGL_NO_DISPLAY) {
        __android_log_print(ANDROID_LOG_ERROR, "FluxCore", "eglGetDisplay failed");
        return;
    }

    if (eglInitialize(m_Display, nullptr, nullptr) != EGL_TRUE) {
        __android_log_print(ANDROID_LOG_ERROR, "FluxCore", "eglInitialize failed");
        return;
    }

    const EGLint egl_attributes[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_NONE
    };

    EGLint num_configs;
    if (eglChooseConfig(m_Display, egl_attributes, nullptr, 0, &num_configs) != EGL_TRUE || num_configs <= 0) {
        __android_log_print(ANDROID_LOG_ERROR, "FluxCore", "eglChooseConfig failed");
        return;
    }

    EGLConfig egl_config;
    eglChooseConfig(m_Display, egl_attributes, &egl_config, 1, &num_configs);

    EGLint egl_format;
    eglGetConfigAttrib(m_Display, egl_config, EGL_NATIVE_VISUAL_ID, &egl_format);
    ANativeWindow_setBuffersGeometry(m_AndroidWindow, 0, 0, egl_format);

    const EGLint context_attributes[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
    };

    m_Context = eglCreateContext(m_Display, egl_config, EGL_NO_CONTEXT, context_attributes);
    if (m_Context == EGL_NO_CONTEXT) {
        __android_log_print(ANDROID_LOG_ERROR, "FluxCore", "eglCreateContext failed");
        return;
    }

    m_Surface = eglCreateWindowSurface(m_Display, egl_config, m_AndroidWindow, nullptr);
    if (m_Surface == EGL_NO_SURFACE) {
        __android_log_print(ANDROID_LOG_ERROR, "FluxCore", "eglCreateWindowSurface failed");
        return;
    }

    if (eglMakeCurrent(m_Display, m_Surface, m_Surface, m_Context) != EGL_TRUE) {
        __android_log_print(ANDROID_LOG_ERROR, "FluxCore", "eglMakeCurrent failed");
        return;
    }

    eglSwapInterval(m_Display, 1);
}
#endif

} // namespace Flux

#include "Application.hpp"

#include <glm/glm.hpp>

#include <android/log.h>
#include <android_native_app_glue.h>
#include <backends/imgui_impl_android.h>
#include <backends/imgui_impl_opengl3.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <imgui.h>
#include <jni.h>
#include <time.h>

namespace Flux {

    struct Application::PlatformState {
        android_app* App = nullptr;
        ANativeWindow* Window = nullptr;
        EGLDisplay Display = EGL_NO_DISPLAY;
        EGLSurface Surface = EGL_NO_SURFACE;
        EGLContext Context = EGL_NO_CONTEXT;
        bool SoftKeyboardVisible = false;
    };

    namespace {
        bool InitEGL(Application::PlatformState& state) {
            if (!state.Window)
                return false;

            state.Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            if (state.Display == EGL_NO_DISPLAY) {
                __android_log_print(ANDROID_LOG_ERROR, "FluxCore", "eglGetDisplay failed");
                return false;
            }

            if (eglInitialize(state.Display, nullptr, nullptr) != EGL_TRUE) {
                __android_log_print(ANDROID_LOG_ERROR, "FluxCore", "eglInitialize failed");
                return false;
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
            if (eglChooseConfig(state.Display, egl_attributes, nullptr, 0, &num_configs) != EGL_TRUE || num_configs <= 0) {
                __android_log_print(ANDROID_LOG_ERROR, "FluxCore", "eglChooseConfig failed");
                return false;
            }

            EGLConfig egl_config;
            eglChooseConfig(state.Display, egl_attributes, &egl_config, 1, &num_configs);

            EGLint egl_format;
            eglGetConfigAttrib(state.Display, egl_config, EGL_NATIVE_VISUAL_ID, &egl_format);
            ANativeWindow_setBuffersGeometry(state.Window, 0, 0, egl_format);

            const EGLint context_attributes[] = {
                    EGL_CONTEXT_CLIENT_VERSION, 3,
                    EGL_NONE
            };

            state.Context = eglCreateContext(state.Display, egl_config, EGL_NO_CONTEXT, context_attributes);
            if (state.Context == EGL_NO_CONTEXT) {
                __android_log_print(ANDROID_LOG_ERROR, "FluxCore", "eglCreateContext failed");
                return false;
            }

            state.Surface = eglCreateWindowSurface(state.Display, egl_config, state.Window, nullptr);
            if (state.Surface == EGL_NO_SURFACE) {
                __android_log_print(ANDROID_LOG_ERROR, "FluxCore", "eglCreateWindowSurface failed");
                return false;
            }

            if (eglMakeCurrent(state.Display, state.Surface, state.Surface, state.Context) != EGL_TRUE) {
                __android_log_print(ANDROID_LOG_ERROR, "FluxCore", "eglMakeCurrent failed");
                return false;
            }

            eglSwapInterval(state.Display, 1);
            return true;
        }

        struct JniEnvScope
        {
            JNIEnv* Env = nullptr;
            JavaVM* Vm = nullptr;
            bool Attached = false;
        };

        bool AcquireJniEnv(android_app* app, JniEnvScope& scope)
        {
            if (!app || !app->activity || !app->activity->vm)
                return false;

            scope.Vm = app->activity->vm;
            jint status = scope.Vm->GetEnv(reinterpret_cast<void**>(&scope.Env), JNI_VERSION_1_6);
            if (status == JNI_EDETACHED)
            {
                if (scope.Vm->AttachCurrentThread(&scope.Env, nullptr) != JNI_OK)
                    return false;
                scope.Attached = true;
            }
            else if (status != JNI_OK)
            {
                return false;
            }
            return scope.Env != nullptr;
        }

        void ReleaseJniEnv(JniEnvScope& scope)
        {
            if (scope.Attached && scope.Vm)
                scope.Vm->DetachCurrentThread();
            scope.Env = nullptr;
            scope.Vm = nullptr;
            scope.Attached = false;
        }

        void ToggleSoftKeyboard(android_app* app, bool show)
        {
            if (!app || !app->activity || !app->activity->clazz)
                return;

            JniEnvScope scope;
            if (!AcquireJniEnv(app, scope))
                return;

            jclass activityClass = scope.Env->GetObjectClass(app->activity->clazz);
            if (activityClass)
            {
                const char* methodName = show ? "showSoftInput" : "hideSoftInput";
                jmethodID methodId = scope.Env->GetMethodID(activityClass, methodName, "()V");
                if (methodId)
                    scope.Env->CallVoidMethod(app->activity->clazz, methodId);
                scope.Env->DeleteLocalRef(activityClass);
            }

            if (scope.Env && scope.Env->ExceptionCheck())
                scope.Env->ExceptionClear();

            ReleaseJniEnv(scope);
        }

        void PollUnicodeCharacters(android_app* app)
        {
            if (!app || !app->activity || !app->activity->clazz)
                return;

            JniEnvScope scope;
            if (!AcquireJniEnv(app, scope))
                return;

            jclass activityClass = scope.Env->GetObjectClass(app->activity->clazz);
            if (!activityClass)
            {
                ReleaseJniEnv(scope);
                return;
            }

            jmethodID pollMethod = scope.Env->GetMethodID(activityClass, "pollUnicodeChar", "()I");
            if (!pollMethod)
            {
                scope.Env->DeleteLocalRef(activityClass);
                ReleaseJniEnv(scope);
                return;
            }

            ImGuiIO& io = ImGui::GetIO();
            jint unicodeCharacter = 0;
            while ((unicodeCharacter = scope.Env->CallIntMethod(app->activity->clazz, pollMethod)) != 0)
                io.AddInputCharacter(static_cast<unsigned int>(unicodeCharacter));

            if (scope.Env->ExceptionCheck())
                scope.Env->ExceptionClear();

            scope.Env->DeleteLocalRef(activityClass);
            ReleaseJniEnv(scope);
        }
    } // namespace

    Application::Application(const ApplicationSpecification& spec)
        : m_Specification(spec) {
        if (m_Specification.ImGuiUIScale > 0.0f)
            m_UIScale = m_Specification.ImGuiUIScale;
        Init();
    }

    Application::~Application() = default;

    void Application::Init() {
        m_Platform = std::make_unique<PlatformState>();
        m_Platform->App = static_cast<android_app*>(m_Specification.PlatformContext);

        if (!m_Platform->App) {
            __android_log_print(ANDROID_LOG_ERROR, "FluxCore", "android_app is null");
            return;
        }

        m_Platform->Window = m_Platform->App->window;
        if (!InitEGL(*m_Platform))
            return;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
        ImGui::StyleColorsDark();

        if (m_Specification.ImGuiUIScale > 0.0f)
            m_UIScale = m_Specification.ImGuiUIScale;
        else
            m_UIScale = 2.0f;

        io.FontGlobalScale = m_UIScale;

        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(m_UIScale);
        style.TouchExtraPadding = ImVec2(6, 6);

        ImGui_ImplAndroid_Init(m_Platform->Window);
        ImGui_ImplOpenGL3_Init("#version 300 es");
    }

    void Application::Run() {
        if (!m_Platform || !m_Platform->App)
            return;

        m_Running = true;
        while (m_Running) {
            int events;
            android_poll_source* source;
            int pollResult;
            do {
                pollResult = ALooper_pollOnce(0, nullptr, &events, reinterpret_cast<void**>(&source));
                if (source)
                    source->process(m_Platform->App, source);
            } while (pollResult >= 0);

            if (m_Platform->App->destroyRequested)
                m_Running = false;

            float time = GetTime();
            m_FrameTime = time - m_LastFrameTime;
            m_TimeStep = glm::min<float>(m_FrameTime, 0.0333f);
            m_LastFrameTime = time;

            glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            for (auto& layer : m_LayerStack)
                layer->OnUpdate(m_TimeStep);

            PollUnicodeCharacters(m_Platform->App);

            ImGuiIO& io = ImGui::GetIO();
            if (io.WantTextInput && !m_Platform->SoftKeyboardVisible)
            {
                ToggleSoftKeyboard(m_Platform->App, true);
                m_Platform->SoftKeyboardVisible = true;
            }
            else if (!io.WantTextInput && m_Platform->SoftKeyboardVisible)
            {
                ToggleSoftKeyboard(m_Platform->App, false);
                m_Platform->SoftKeyboardVisible = false;
            }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplAndroid_NewFrame();
            ImGui::NewFrame();

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                            ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoDocking |
                                            ImGuiWindowFlags_NoBringToFrontOnFocus |
                                            ImGuiWindowFlags_NoNavFocus;

            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::Begin("DockSpace", nullptr, window_flags);
            ImGui::PopStyleVar(2);

            ImGuiID dockspace_id = ImGui::GetID("DockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

            for (auto& layer : m_LayerStack)
                layer->OnRenderUI();

            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            eglSwapBuffers(m_Platform->Display, m_Platform->Surface);
        }

        Shutdown();
    }

    void Application::Shutdown() {
        for (auto& layer : m_LayerStack)
            layer->OnDetach();
        m_LayerStack.clear();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplAndroid_Shutdown();
        ImGui::DestroyContext();

        if (m_Platform) {
            if (m_Platform->Display != EGL_NO_DISPLAY) {
                eglMakeCurrent(m_Platform->Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

                if (m_Platform->Context != EGL_NO_CONTEXT)
                    eglDestroyContext(m_Platform->Display, m_Platform->Context);
                if (m_Platform->Surface != EGL_NO_SURFACE)
                    eglDestroySurface(m_Platform->Display, m_Platform->Surface);

                eglTerminate(m_Platform->Display);
            }
        }

        m_Platform.reset();
    }

    void* Application::GetNativeWindow() const {
        if (!m_Platform)
            return nullptr;
        return m_Platform->Window;
    }

    float Application::GetTime() const {
        timespec now{};
        clock_gettime(CLOCK_MONOTONIC, &now);
        return now.tv_sec + now.tv_nsec * 1e-9f;
    }

} // namespace Flux

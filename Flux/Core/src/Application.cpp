// Copyright 2026 Beisent
// Application class implementation

#include "Application.hpp"

#include <algorithm>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <utility>

namespace flux {

Application::Application(const ApplicationSpecification& spec)
    : specification_(spec)
{
    if (specification_.imgui_ui_scale > 0.0f) {
        ui_scale_ = specification_.imgui_ui_scale;
    }
    Init();
}

Application::~Application() = default;

void Application::Init()
{
    window_ = std::make_unique<Window>(specification_.window);

    if (!window_->GetNativeWindow()) {
        return;
    }

    window_->SetEventCallback([this](Event& e) {
        OnEvent(e);
    });

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    if (specification_.imgui_docking_enabled) {
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }

    if (specification_.imgui_viewports_enabled) {
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    }

    ImGui::StyleColorsDark();

    ImFontConfig font_config;
    font_config.OversampleH = 1;
    font_config.OversampleV = 1;
    font_config.PixelSnapH = true;

    if (!specification_.imgui_font_path.empty()) {
        io.Fonts->AddFontFromFileTTF(
            specification_.imgui_font_path.c_str(), specification_.imgui_font_size, &font_config);
    }

    if (specification_.imgui_enable_merge_font && !specification_.imgui_merge_font_path.empty()) {
        font_config.MergeMode = true;
        font_config.GlyphMinAdvanceX = specification_.imgui_merge_font_size;

        io.Fonts->AddFontFromFileTTF(specification_.imgui_merge_font_path.c_str(),
                                     specification_.imgui_merge_font_size,
                                     &font_config,
                                     io.Fonts->GetGlyphRangesChineseFull());
    }

    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window_->GetNativeWindow()), true);
    ImGui_ImplOpenGL3_Init("#version 430");
}

void Application::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& event) {
        return OnWindowClose(event);
    });
    dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) {
        return OnWindowResize(event);
    });

    for (auto it = layer_stack_.rbegin(); it != layer_stack_.rend(); ++it) {
        if (e.handled) {
            break;
        }
        (*it)->OnEvent(e);
    }
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
    running_ = false;
    return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e)
{
    if (e.GetWidth() == 0 || e.GetHeight() == 0) {
        minimized_ = true;
        return false;
    }

    minimized_ = false;
    glViewport(0, 0, e.GetWidth(), e.GetHeight());
    return false;
}

void Application::Run()
{
    if (!window_ || !window_->GetNativeWindow()) {
        return;
    }

    running_ = true;
    while (running_) {
        float time = GetTime();
        frame_time_ = time - last_frame_time_;
        time_step_ = std::clamp(frame_time_, 0.0f, 0.0333f);
        last_frame_time_ = time;

        TimeStep timestep(time_step_);

        if (!minimized_) {
            glClearColor(specification_.clear_color[0],
                         specification_.clear_color[1],
                         specification_.clear_color[2],
                         specification_.clear_color[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (auto& layer : layer_stack_) {
                layer->OnUpdate(timestep);
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (specification_.imgui_docking_enabled) {
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
            if (menubar_callback_) {
                window_flags |= ImGuiWindowFlags_MenuBar;
            }

            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace", nullptr, window_flags);
            ImGui::PopStyleVar(1);

            ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockspace_flags);

            if (menubar_callback_) {
                if (ImGui::BeginMenuBar()) {
                    menubar_callback_();
                    ImGui::EndMenuBar();
                }
            }

            for (auto& layer : layer_stack_) {
                layer->OnRenderUI();
            }

            ImGui::End();
            ImGui::PopStyleVar(2);
        } else {
            for (auto& layer : layer_stack_) {
                layer->OnRenderUI();
            }
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        window_->SwapBuffers();
        window_->PollEvents();
    }

    Shutdown();
}

void Application::Shutdown()
{
    for (auto& layer : layer_stack_) {
        layer->OnDetach();
    }
    layer_stack_.clear();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    window_.reset();
}

void* Application::GetNativeWindow() const
{
    if (!window_) {
        return nullptr;
    }
    return window_->GetNativeWindow();
}

float Application::GetTime() const
{
    if (!window_) {
        return 0.0f;
    }
    return window_->GetTime();
}

void Application::PushLayer(std::unique_ptr<Layer> layer)
{
    if (!layer) {
        return;
    }
    layer->OnAttach();
    layer_stack_.emplace(layer_stack_.begin() + layer_insert_index_, std::move(layer));
    layer_insert_index_++;
}

void Application::PushOverlay(std::unique_ptr<Layer> overlay)
{
    if (!overlay) {
        return;
    }
    overlay->OnAttach();
    layer_stack_.emplace_back(std::move(overlay));
}

void Application::PopLayer(Layer* layer)
{
    auto it = std::find_if(layer_stack_.begin(),
                           layer_stack_.begin() + layer_insert_index_,
                           [layer](const std::unique_ptr<Layer>& l) {
                               return l.get() == layer;
                           });

    if (it != layer_stack_.begin() + layer_insert_index_) {
        (*it)->OnDetach();
        layer_stack_.erase(it);
        layer_insert_index_--;
    }
}

Layer* Application::GetLayer(size_t index)
{
    if (index >= layer_stack_.size()) {
        return nullptr;
    }
    return layer_stack_[index].get();
}

Layer* Application::GetLayerByName(std::string_view name)
{
    for (auto& layer : layer_stack_) {
        if (layer->GetName() == name) {
            return layer.get();
        }
    }
    return nullptr;
}

void Application::SetMenubarCallback(std::function<void()> callback)
{
    menubar_callback_ = std::move(callback);
}

void Application::Close()
{
    running_ = false;
}

} // namespace flux

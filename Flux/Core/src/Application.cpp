// Copyright 2026 Beisent
// Application class implementation

#include "Application.hpp"

#include <utility>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace flux
{

  struct Application::PlatformState
  {
    GLFWwindow *window_handle = nullptr;
  };

  Application::Application(const ApplicationSpecification &spec)
      : specification_(spec)
  {
    if (specification_.imgui_ui_scale > 0.0f)
    {
      ui_scale_ = specification_.imgui_ui_scale;
    }
    Init();
  }

  Application::~Application() = default;

  void Application::Init()
  {
    platform_ = std::make_unique<PlatformState>();

    if (!glfwInit())
    {
      return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    platform_->window_handle =
        glfwCreateWindow(specification_.width, specification_.height,
                         specification_.name.c_str(), nullptr, nullptr);

    if (!platform_->window_handle)
    {
      glfwTerminate();
      return;
    }

    glfwMakeContextCurrent(platform_->window_handle);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
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

    ImGui_ImplGlfw_InitForOpenGL(platform_->window_handle, true);
    ImGui_ImplOpenGL3_Init("#version 430");
  }

  void Application::Run()
  {
    if (!platform_ || !platform_->window_handle)
    {
      return;
    }

    running_ = true;
    while (running_)
    {
      float time = GetTime();
      frame_time_ = time - last_frame_time_;
      time_step_ = glm::min<float>(frame_time_, 0.0333f);
      last_frame_time_ = time;

      glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      for (auto &layer : layer_stack_)
      {
        layer->OnUpdate(time_step_);
      }

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
      if (menubar_callback_)
      {
        window_flags |= ImGuiWindowFlags_MenuBar;
      }

      const ImGuiViewport *viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos(viewport->WorkPos);
      ImGui::SetNextWindowSize(viewport->WorkSize);
      ImGui::SetNextWindowViewport(viewport->ID);

      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
      window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                      ImGuiWindowFlags_NoBringToFrontOnFocus |
                      ImGuiWindowFlags_NoNavFocus;

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      ImGui::Begin("DockSpace Demo", nullptr, window_flags);
      ImGui::PopStyleVar(1);

      ImGuiID dockspace_id = ImGui::GetID("DockSpace");
      ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockspace_flags);

      if (menubar_callback_)
      {
        if (ImGui::BeginMenuBar())
        {
          menubar_callback_();
          ImGui::EndMenuBar();
        }
      }

      for (auto &layer : layer_stack_)
      {
        layer->OnRenderUI();
      }

      ImGui::End();
      ImGui::PopStyleVar(2);

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

      glfwSwapBuffers(platform_->window_handle);
      glfwPollEvents();

      if (glfwWindowShouldClose(platform_->window_handle))
      {
        running_ = false;
      }
    }

    Shutdown();
  }

  void Application::Shutdown()
  {
    for (auto &layer : layer_stack_)
    {
      layer->OnDetach();
    }
    layer_stack_.clear();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (platform_ && platform_->window_handle)
    {
      glfwDestroyWindow(platform_->window_handle);
      platform_->window_handle = nullptr;
    }

    glfwTerminate();
    platform_.reset();
  }

  void *Application::GetNativeWindow() const
  {
    if (!platform_)
    {
      return nullptr;
    }
    return platform_->window_handle;
  }

  float Application::GetTime() const
  {
    return static_cast<float>(glfwGetTime());
  }

  void Application::PushLayer(std::unique_ptr<Layer> layer)
  {
    if (!layer)
    {
      return;
    }
    layer->OnAttach();
    layer_stack_.push_back(std::move(layer));
  }

  void Application::SetMenubarCallback(std::function<void()> callback)
  {
    menubar_callback_ = std::move(callback);
  }

  void Application::Close() { running_ = false; }

} // namespace flux

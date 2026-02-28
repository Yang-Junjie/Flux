// Copyright 2026 Beisent
// Application class implementation

#include "Application.hpp"

#include <algorithm>
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

  // Static callback wrapper
  static Application *s_application_instance = nullptr;

  Application::Application(const ApplicationSpecification &spec)
      : specification_(spec)
  {
    s_application_instance = this;

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
    glfwWindowHint(GLFW_RESIZABLE, specification_.resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, specification_.decorated ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, specification_.maximized ? GLFW_TRUE : GLFW_FALSE);

    if (specification_.msaa_samples > 0)
    {
      glfwWindowHint(GLFW_SAMPLES, specification_.msaa_samples);
    }

    GLFWmonitor *monitor = specification_.fullscreen ? glfwGetPrimaryMonitor() : nullptr;

    platform_->window_handle =
        glfwCreateWindow(specification_.width, specification_.height,
                         specification_.name.c_str(), monitor, nullptr);

    if (!platform_->window_handle)
    {
      glfwTerminate();
      return;
    }

    glfwMakeContextCurrent(platform_->window_handle);
    glfwSwapInterval(specification_.vsync ? 1 : 0);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
      return;
    }

    if (specification_.msaa_samples > 0)
    {
      glEnable(GL_MULTISAMPLE);
    }

    SetupEventCallbacks();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    if (specification_.imgui_docking_enabled)
    {
      io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }

    if (specification_.imgui_viewports_enabled)
    {
      io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    }

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(platform_->window_handle, true);
    ImGui_ImplOpenGL3_Init("#version 430");
  }

  void Application::SetupEventCallbacks()
  {
    glfwSetWindowCloseCallback(platform_->window_handle,
                               [](GLFWwindow *window)
                               {
                                 WindowCloseEvent event;
                                 s_application_instance->OnEvent(event);
                               });

    glfwSetWindowSizeCallback(platform_->window_handle,
                              [](GLFWwindow *window, int width, int height)
                              {
                                WindowResizeEvent event(width, height);
                                s_application_instance->OnEvent(event);
                              });

    glfwSetKeyCallback(platform_->window_handle,
                       [](GLFWwindow *window, int key, int scancode, int action,
                          int mods)
                       {
                         switch (action)
                         {
                         case GLFW_PRESS:
                         {
                           KeyPressedEvent event(key, 0);
                           s_application_instance->OnEvent(event);
                           break;
                         }
                         case GLFW_RELEASE:
                         {
                           KeyReleasedEvent event(key);
                           s_application_instance->OnEvent(event);
                           break;
                         }
                         case GLFW_REPEAT:
                         {
                           KeyPressedEvent event(key, 1);
                           s_application_instance->OnEvent(event);
                           break;
                         }
                         }
                       });

    glfwSetCharCallback(platform_->window_handle,
                        [](GLFWwindow *window, unsigned int keycode)
                        {
                          KeyTypedEvent event(keycode);
                          s_application_instance->OnEvent(event);
                        });

    glfwSetMouseButtonCallback(platform_->window_handle,
                               [](GLFWwindow *window, int button, int action,
                                  int mods)
                               {
                                 switch (action)
                                 {
                                 case GLFW_PRESS:
                                 {
                                   MouseButtonPressedEvent event(button);
                                   s_application_instance->OnEvent(event);
                                   break;
                                 }
                                 case GLFW_RELEASE:
                                 {
                                   MouseButtonReleasedEvent event(button);
                                   s_application_instance->OnEvent(event);
                                   break;
                                 }
                                 }
                               });

    glfwSetScrollCallback(platform_->window_handle,
                          [](GLFWwindow *window, double xOffset, double yOffset)
                          {
                            MouseScrolledEvent event((float)xOffset, (float)yOffset);
                            s_application_instance->OnEvent(event);
                          });

    glfwSetCursorPosCallback(platform_->window_handle,
                             [](GLFWwindow *window, double xPos, double yPos)
                             {
                               MouseMovedEvent event((float)xPos, (float)yPos);
                               s_application_instance->OnEvent(event);
                             });
  }

  void Application::OnEvent(Event &e)
  {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(
        [this](WindowCloseEvent &event) { return OnWindowClose(event); });
    dispatcher.Dispatch<WindowResizeEvent>(
        [this](WindowResizeEvent &event) { return OnWindowResize(event); });

    // Propagate events to layers in reverse order (top to bottom)
    for (auto it = layer_stack_.rbegin(); it != layer_stack_.rend(); ++it)
    {
      if (e.handled)
        break;
      (*it)->OnEvent(e);
    }
  }

  bool Application::OnWindowClose(WindowCloseEvent &e)
  {
    running_ = false;
    return true;
  }

  bool Application::OnWindowResize(WindowResizeEvent &e)
  {
    if (e.GetWidth() == 0 || e.GetHeight() == 0)
    {
      minimized_ = true;
      return false;
    }

    minimized_ = false;
    glViewport(0, 0, e.GetWidth(), e.GetHeight());
    return false;
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

      TimeStep timestep(time_step_);

      if (!minimized_)
      {
        glClearColor(specification_.clear_color[0], specification_.clear_color[1],
                     specification_.clear_color[2], specification_.clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto &layer : layer_stack_)
        {
          layer->OnUpdate(timestep);
        }
      }

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      if (specification_.imgui_docking_enabled)
      {
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
        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar(1);

        ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
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
      }
      else
      {
        // No docking - just render layers directly
        for (auto &layer : layer_stack_)
        {
          layer->OnRenderUI();
        }
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

      glfwSwapBuffers(platform_->window_handle);
      glfwPollEvents();
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
    layer_stack_.emplace(layer_stack_.begin() + layer_insert_index_, std::move(layer));
    layer_insert_index_++;
  }

  void Application::PushOverlay(std::unique_ptr<Layer> overlay)
  {
    if (!overlay)
    {
      return;
    }
    overlay->OnAttach();
    layer_stack_.emplace_back(std::move(overlay));
  }

  void Application::PopLayer(Layer *layer)
  {
    auto it = std::find_if(layer_stack_.begin(),
                           layer_stack_.begin() + layer_insert_index_,
                           [layer](const std::unique_ptr<Layer> &l)
                           { return l.get() == layer; });

    if (it != layer_stack_.begin() + layer_insert_index_)
    {
      (*it)->OnDetach();
      layer_stack_.erase(it);
      layer_insert_index_--;
    }
  }

  Layer *Application::GetLayer(size_t index)
  {
    if (index >= layer_stack_.size())
    {
      return nullptr;
    }
    return layer_stack_[index].get();
  }

  Layer *Application::GetLayerByName(const std::string &name)
  {
    for (auto &layer : layer_stack_)
    {
      if (layer->GetName() == name)
      {
        return layer.get();
      }
    }
    return nullptr;
  }

  void Application::SetMenubarCallback(std::function<void()> callback)
  {
    menubar_callback_ = std::move(callback);
  }

  void Application::Close() { running_ = false; }

} // namespace flux

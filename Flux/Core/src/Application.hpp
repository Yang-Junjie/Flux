// Copyright 2026 Beisent
// Application class for Flux framework

#ifndef FLUX_CORE_SRC_APPLICATION_HPP_
#define FLUX_CORE_SRC_APPLICATION_HPP_

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Event.hpp"
#include "Layer.hpp"
#include "TimeStep.hpp"

namespace flux
{

  struct ApplicationSpecification
  {
    std::string name = "App";
    uint32_t width = 800;
    uint32_t height = 600;

    // Rendering configuration
    float clear_color[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    bool vsync = true;
    int msaa_samples = 0; // 0 = disabled, 2/4/8/16 = enabled

    // Window configuration
    bool fullscreen = false;
    bool resizable = true;
    bool decorated = true;
    bool maximized = false;

    // ImGui configuration
    float imgui_ui_scale = 0.0f;
    bool imgui_docking_enabled = true;
    bool imgui_viewports_enabled = true;

    // Platform-specific
    void *platform_context = nullptr;
  };

  class Application
  {
  public:
    explicit Application(
        const ApplicationSpecification &spec = ApplicationSpecification());
    virtual ~Application();

    // Disable copy and assign
    Application(const Application &) = delete;
    Application &operator=(const Application &) = delete;

    void Run();

    // Layer management with ownership control
    void PushLayer(std::unique_ptr<Layer> layer);
    void PushOverlay(std::unique_ptr<Layer> overlay);
    void PopLayer(Layer *layer);
    Layer *GetLayer(size_t index);
    Layer *GetLayerByName(const std::string &name);
    size_t GetLayerCount() const { return layer_stack_.size(); }

    void SetMenubarCallback(std::function<void()> callback);

    void *GetNativeWindow() const;
    float GetTime() const;
    void Close();

    const ApplicationSpecification &GetSpecification() const
    {
      return specification_;
    }

    struct PlatformState;

  private:
    void Init();
    void Shutdown();
    void OnEvent(Event &e);
    bool OnWindowClose(WindowCloseEvent &e);
    bool OnWindowResize(WindowResizeEvent &e);

    void SetupEventCallbacks();

    ApplicationSpecification specification_;
    bool running_ = false;
    bool minimized_ = false;

    std::vector<std::unique_ptr<Layer>> layer_stack_;
    size_t layer_insert_index_ = 0;
    std::function<void()> menubar_callback_;

    float time_step_ = 0.0f;
    float frame_time_ = 0.0f;
    float last_frame_time_ = 0.0f;
    float ui_scale_ = 1.0f;

    std::unique_ptr<PlatformState> platform_;
  };

  std::unique_ptr<Application> CreateApplication();

} // namespace flux

#endif // FLUX_CORE_SRC_APPLICATION_HPP_

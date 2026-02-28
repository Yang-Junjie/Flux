// Copyright 2026 Beisent
// Application class for Flux framework

#ifndef FLUX_CORE_SRC_APPLICATION_HPP_
#define FLUX_CORE_SRC_APPLICATION_HPP_

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Layer.hpp"

namespace flux
{

  struct ApplicationSpecification
  {
    std::string name = "App";
    uint32_t width = 800;
    uint32_t height = 600;
    void *platform_context = nullptr;
    float imgui_ui_scale = 0.0f;
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
    void PushLayer(std::unique_ptr<Layer> layer);
    void SetMenubarCallback(std::function<void()> callback);

    void *GetNativeWindow() const;
    float GetTime() const;
    void Close();

    struct PlatformState;

  private:
    void Init();
    void Shutdown();

    ApplicationSpecification specification_;
    bool running_ = false;

    std::vector<std::unique_ptr<Layer>> layer_stack_;
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

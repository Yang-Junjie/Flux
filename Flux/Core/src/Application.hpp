// Copyright 2026 Beisent
// Application class for Flux framework

#ifndef FLUX_CORE_SRC_APPLICATION_HPP_
#define FLUX_CORE_SRC_APPLICATION_HPP_

#include "Event.hpp"
#include "Layer.hpp"
#include "Window.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace flux {

struct ApplicationSpecification {
    WindowSpecification window;

    // Rendering configuration
    float clear_color[4] = {0.1f, 0.1f, 0.1f, 1.0f};

    float imgui_ui_scale = 0.0f;
    bool imgui_docking_enabled = true;
    bool imgui_viewports_enabled = true;

    std::string imgui_font_path;
    float imgui_font_size = 16.0f;

    std::string imgui_merge_font_path;
    float imgui_merge_font_size = 16.0f;

    bool imgui_enable_merge_font = false;
    void* platform_context = nullptr;
};

class Application {
public:
    explicit Application(const ApplicationSpecification& spec = ApplicationSpecification());
    virtual ~Application();

    // Disable copy and assign
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void Run();

    // Layer management with ownership control
    void PushLayer(std::unique_ptr<Layer> layer);
    void PushOverlay(std::unique_ptr<Layer> overlay);
    void PopLayer(Layer* layer);
    [[nodiscard]] Layer* GetLayer(size_t index);
    [[nodiscard]] Layer* GetLayerByName(std::string_view name);

    [[nodiscard]] size_t GetLayerCount() const
    {
        return layer_stack_.size();
    }

    void SetMenubarCallback(std::function<void()> callback);

    [[nodiscard]] void* GetNativeWindow() const;
    [[nodiscard]] float GetTime() const;
    void Close();

    [[nodiscard]] const ApplicationSpecification& GetSpecification() const
    {
        return specification_;
    }

private:
    void Init();
    void Shutdown();
    void OnEvent(Event& e);
    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);

    ApplicationSpecification specification_;
    bool running_ = false;
    bool minimized_ = false;

    std::unique_ptr<Window> window_;
    std::vector<std::unique_ptr<Layer>> layer_stack_;
    size_t layer_insert_index_ = 0;
    std::function<void()> menubar_callback_;

    float time_step_ = 0.0f;
    float frame_time_ = 0.0f;
    float last_frame_time_ = 0.0f;
    float ui_scale_ = 1.0f;
};

std::unique_ptr<Application> CreateApplication();

} // namespace flux

#endif // FLUX_CORE_SRC_APPLICATION_HPP_

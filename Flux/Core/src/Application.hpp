#pragma once

#include "Layer.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace Flux {

    struct ApplicationSpecification {
        std::string Name = "App";
        uint32_t Width = 800;
        uint32_t Height = 600;
        void* PlatformContext = nullptr;
        float ImGuiUIScale = 0.0f;
    };

    class Application {
    public:
        explicit Application(const ApplicationSpecification& spec = ApplicationSpecification());
        virtual ~Application();

        void Run();
        void PushLayer(std::unique_ptr<Layer> layer);
        void SetMenubarCallback(std::function<void()> callback);

        void* GetNativeWindow() const;
        float GetTime() const;
        void Close();

    private:
        void Init();
        void Shutdown();

        ApplicationSpecification m_Specification;
        bool m_Running = false;

        std::vector<std::unique_ptr<Layer>> m_LayerStack;
        std::function<void()> m_MenubarCallback;

        float m_TimeStep = 0.0f;
        float m_FrameTime = 0.0f;
        float m_LastFrameTime = 0.0f;
        float m_UIScale = 1.0f;

        struct PlatformState;
        std::unique_ptr<PlatformState> m_Platform;
    };

    std::unique_ptr<Application> CreateApplication();

} // namespace Flux

// Copyright 2026 Beisent
// 功能测试示例 - 展示所有新特性

#include <memory>
#include <imgui.h>
#include "Application.hpp"

// 测试事件处理的 Layer
class EventTestLayer : public flux::Layer {
public:
    EventTestLayer() : Layer("EventTest") {}

    void OnUpdate(flux::TimeStep ts) override {
        deltaTime_ = ts.GetSeconds();
        frameCount_++;
    }

    void OnRenderUI() override {
        ImGui::Begin("事件测试");
        ImGui::Text("帧时间: %.3f ms (%.1f FPS)", deltaTime_ * 1000.0f, 1.0f / deltaTime_);
        ImGui::Text("总帧数: %d", frameCount_);
        ImGui::Separator();
        ImGui::Text("最后按下的键: %d", lastKey_);
        ImGui::Text("鼠标位置: (%.1f, %.1f)", mouseX_, mouseY_);
        ImGui::Text("鼠标按钮: %d", lastMouseButton_);
        ImGui::End();
    }

    void OnEvent(flux::Event& event) override {
        flux::EventDispatcher dispatcher(event);

        dispatcher.Dispatch<flux::KeyPressedEvent>(
            [this](flux::KeyPressedEvent& e) {
                lastKey_ = e.GetKeyCode();
                return false;
            });

        dispatcher.Dispatch<flux::MouseMovedEvent>(
            [this](flux::MouseMovedEvent& e) {
                mouseX_ = e.GetX();
                mouseY_ = e.GetY();
                return false;
            });

        dispatcher.Dispatch<flux::MouseButtonPressedEvent>(
            [this](flux::MouseButtonPressedEvent& e) {
                lastMouseButton_ = e.GetMouseButton();
                return false;
            });
    }

private:
    float deltaTime_ = 0.0f;
    int frameCount_ = 0;
    int lastKey_ = -1;
    float mouseX_ = 0.0f;
    float mouseY_ = 0.0f;
    int lastMouseButton_ = -1;
};

// 测试 Layer 访问的示例
class ControlLayer : public flux::Layer {
public:
    ControlLayer() : Layer("Control") {}

    void OnRenderUI() override {
        ImGui::Begin("控制面板");
        ImGui::Text("这是一个 Overlay Layer");
        ImGui::Text("Layer 总数: %zu", layerCount_);

        if (ImGui::Button("打印所有 Layer 名称")) {
            printLayers_ = true;
        }

        ImGui::End();
    }

    void SetLayerCount(size_t count) { layerCount_ = count; }
    bool ShouldPrintLayers() {
        bool result = printLayers_;
        printLayers_ = false;
        return result;
    }

private:
    size_t layerCount_ = 0;
    bool printLayers_ = false;
};

// 主应用
class TestApp : public flux::Application {
public:
    explicit TestApp(const flux::ApplicationSpecification& spec)
        : flux::Application(spec) {

        // 添加普通 Layer
        PushLayer(std::make_unique<EventTestLayer>());

        // 添加 Overlay
        auto controlLayer = std::make_unique<ControlLayer>();
        controlLayerPtr_ = controlLayer.get();
        PushOverlay(std::move(controlLayer));

        // 设置菜单栏
        SetMenubarCallback([this]() {
            if (ImGui::BeginMenu("文件")) {
                if (ImGui::MenuItem("退出", "ESC")) {
                    Close();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("测试")) {
                if (ImGui::MenuItem("显示 Layer 信息")) {
                    ShowLayerInfo();
                }
                ImGui::EndMenu();
            }
        });
    }

    void ShowLayerInfo() {
        size_t count = GetLayerCount();
        printf("=== Layer 信息 ===\n");
        printf("总 Layer 数: %zu\n", count);

        for (size_t i = 0; i < count; ++i) {
            if (auto* layer = GetLayer(i)) {
                printf("Layer %zu: %s\n", i, layer->GetName().c_str());
            }
        }
        printf("==================\n");
    }

private:
    ControlLayer* controlLayerPtr_ = nullptr;
};

std::unique_ptr<flux::Application> flux::CreateApplication() {
    flux::ApplicationSpecification spec;
    spec.name = "Flux 功能测试";
    spec.width = 1280;
    spec.height = 720;

    // 测试自定义清屏颜色
    spec.clear_color[0] = 0.15f;
    spec.clear_color[1] = 0.15f;
    spec.clear_color[2] = 0.20f;
    spec.clear_color[3] = 1.0f;

    // 测试渲染配置
    spec.vsync = true;
    spec.msaa_samples = 4;

    // 测试窗口配置
    spec.resizable = true;
    spec.decorated = true;
    spec.maximized = false;

    return std::make_unique<TestApp>(spec);
}

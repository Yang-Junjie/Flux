// Copyright 2026 Beisent
// Example application

#include <memory>

#include <imgui.h>

#include "Application.hpp"

class MyLayer : public flux::Layer {
 public:
  MyLayer() : Layer("MyLayer") {}

  void OnAttach() override {}
  void OnDetach() override {}

  void OnUpdate(flux::TimeStep ts) override {
    // Update logic here
  }

  void OnRenderUI() override {
    ImGui::Begin("Hello, Flux!");
    ImGui::Text("This is a sample layer in MyApp.");
    ImGui::Text("Frame time: %.3f ms", ImGui::GetIO().Framerate);
    ImGui::End();
    ImGui::ShowDemoWindow();
  }

  void OnEvent(flux::Event& event) override {
    flux::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<flux::KeyPressedEvent>(
        [this](flux::KeyPressedEvent& e) {
          // Handle key press
          return false;
        });
  }
};

class MyApp : public flux::Application {
 public:
  explicit MyApp(const flux::ApplicationSpecification& spec)
      : flux::Application(spec) {
    PushLayer(std::make_unique<MyLayer>());
    SetMenubarCallback([this]() {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Exit")) {
          Close();
        }
        ImGui::EndMenu();
      }
    });
  }

  ~MyApp() override = default;
};

std::unique_ptr<flux::Application> flux::CreateApplication() {
  flux::ApplicationSpecification spec;
  spec.name = "MyApp";
  spec.width = 1280;
  spec.height = 720;
  spec.clear_color[0] = 0.1f;
  spec.clear_color[1] = 0.1f;
  spec.clear_color[2] = 0.15f;
  spec.clear_color[3] = 1.0f;
  spec.vsync = true;
  spec.msaa_samples = 4;
  spec.resizable = true;
  return std::make_unique<MyApp>(spec);
}
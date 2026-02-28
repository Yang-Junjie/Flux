// Copyright 2026 Beisent
// Example application

#include <memory>

#include <imgui.h>

#include "Application.hpp"

class MyLayer : public flux::Layer {
 public:
  void OnAttach() override {}
  void OnDetach() override {}
  void OnUpdate(float dt) override {}
  void OnRenderUI() override {
    ImGui::Begin("Hello, Flux!");
    ImGui::Text("This is a sample layer in MyApp.");
    ImGui::End();
    ImGui::ShowDemoWindow();
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
  return std::make_unique<MyApp>(spec);
}
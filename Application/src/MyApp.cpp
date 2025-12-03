#include "Application.hpp"
class MyLayer : public Flux::Layer
{
public:
    virtual void OnAttach() override {}
    virtual void OnDetach() override {}
    virtual void OnUpdate(float dt) override {}
    virtual void OnRenderUI() override {
        ImGui::Begin("Hello, Flux!");
        ImGui::Text("This is a sample layer in MyApp.");
        ImGui::End();
    }
};
class MyApp : public Flux::Application
{
public:
    MyApp()
    {
        pushLayer(std::make_unique<MyLayer>());
    }
    virtual ~MyApp() override = default;
};
std::unique_ptr<Flux::Application> Flux::CreateApplication()
{
    return std::make_unique<MyApp>();
}
#include "Application.hpp"
class MyLayer : public Flux::Layer
{
public:
    virtual void OnAttach() override {}
    virtual void OnDetach() override {}
    virtual void OnUpdate(float dt) override {}
    virtual void OnRenderUI() override
    {
        ImGui::Begin("Hello, Flux!");
        ImGui::Text("This is a sample layer in MyApp.");
        ImGui::End();
        ImGui::ShowDemoWindow();
    }
};
class MyApp : public Flux::Application
{
public:
    MyApp()
    {
        PushLayer(std::make_unique<MyLayer>());
        SetMenubarCallback([this]()
                           {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit"))
                    {
                        Close();          
                    }
                    ImGui::EndMenu();
            } });
    }

    virtual ~MyApp() override = default;
};
std::unique_ptr<Flux::Application> Flux::CreateApplication()
{
    return std::make_unique<MyApp>();
}
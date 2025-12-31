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
    MyApp(const Flux::ApplicationSpecification& spec): Flux::Application(spec)
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
    Flux::ApplicationSpecification spec;
    spec.Name = "MyApp";
    spec.Width = 1280;
    spec.Height = 720;
    return std::make_unique<MyApp>(spec);
}
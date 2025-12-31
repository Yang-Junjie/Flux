#include "PainterLayer.hpp"

void ExampleLayer::OnRenderUI() {
    if (m_ShowDemo)
        ImGui::ShowDemoWindow(&m_ShowDemo);

    ImGui::Begin("Painter");
    ImGui::Text("Hello from ExampleLayer!");
    if (ImGui::Button("Toggle Demo"))
        m_ShowDemo = !m_ShowDemo;
    ImGui::End();
}

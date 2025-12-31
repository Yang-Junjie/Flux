#pragma once

#include "Flux/Core/src/Layer.hpp"
#include <imgui.h>

class ExampleLayer final : public Flux::Layer {
public:
    void OnRenderUI() override;

private:
    bool m_ShowDemo = true;
};

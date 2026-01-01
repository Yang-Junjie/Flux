#include "Application.hpp"

#include <utility>

namespace Flux {

    void Application::PushLayer(std::unique_ptr<Layer> layer) {
        if (!layer)
            return;
        layer->OnAttach();
        m_LayerStack.push_back(std::move(layer));
    }

    void Application::SetMenubarCallback(std::function<void()> callback) {
        m_MenubarCallback = std::move(callback);
    }

    void Application::Close() {
        m_Running = false;
    }

} // namespace Flux

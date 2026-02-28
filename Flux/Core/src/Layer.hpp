// Copyright 2026 Beisent
// Layer interface for Flux application framework

#ifndef FLUX_CORE_SRC_LAYER_HPP_
#define FLUX_CORE_SRC_LAYER_HPP_

#include <string>

#include "Event.hpp"
#include "TimeStep.hpp"

namespace flux
{

  class Layer
  {
  public:
    explicit Layer(const std::string &name = "Layer") : debug_name_(name) {}
    virtual ~Layer() = default;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(TimeStep ts) {}
    virtual void OnRenderUI() {}
    virtual void OnEvent(Event &event) {}

    const std::string &GetName() const { return debug_name_; }

  protected:
    std::string debug_name_;
  };

} // namespace flux

#endif // FLUX_CORE_SRC_LAYER_HPP_
// Copyright 2026 Beisent
// Layer interface for Flux application framework

#ifndef FLUX_CORE_SRC_LAYER_HPP_
#define FLUX_CORE_SRC_LAYER_HPP_

namespace flux
{

  class Layer
  {
  public:
    virtual ~Layer() = default;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(float dt) {}
    virtual void OnRenderUI() {}
  };

} // namespace flux

#endif // FLUX_CORE_SRC_LAYER_HPP_
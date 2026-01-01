#include "ExampleLayer.hpp"
#include "Flux/Core/src/Application.hpp"

#ifdef ANDROID

#include <android_native_app_glue.h>

extern struct android_app *g_AndroidApp;
#endif

namespace Flux {

std::unique_ptr<Application> CreateApplication() {
  ApplicationSpecification spec;
  spec.Name = "ExampleApp";
#ifdef ANDROID
  spec.AndroidApp = g_AndroidApp;
#endif
  auto app = std::make_unique<Application>(spec);
  app->PushLayer(std::make_unique<ExampleLayer>());
  return app;
}

} // namespace Flux

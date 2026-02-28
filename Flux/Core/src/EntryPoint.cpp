// Copyright 2026 Beisent
// Entry point for Flux applications

#include "Application.hpp"

int main()
{
  std::unique_ptr<flux::Application> app = flux::CreateApplication();
  app->Run();
  return 0;
}
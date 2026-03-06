// Copyright 2026 Beisent
// Entry point for Flux applications

#include "Application.hpp"

int main()
{
    auto app = flux::CreateApplication();
    app->Run();
    return 0;
}

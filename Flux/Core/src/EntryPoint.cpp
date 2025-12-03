#include "Application.hpp"
int main()
{
    std::unique_ptr<Flux::Application> app = Flux::CreateApplication();
    app->Run();
    return 0;
}
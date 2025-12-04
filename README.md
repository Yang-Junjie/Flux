# Flux 应用程序模板

一个小型跨平台桌面应用程序模板，采用分层架构，将通用核心与业务逻辑解耦，适合快速搭建工具类或编辑器类应用。

## 环境要求

- 运行环境：Windows / macOS / Linux
- 编译器：支持 C++17 的编译器（MSVC / Clang / GCC 等）
- 构建工具：CMake（建议 3.15 及以上）

## 构建与运行

在项目根目录执行（以命令行为例）：

```bash
mkdir -p build
cd build
cmake ..          #如果需要编译样例程序：cmake .. -DFLUX_BUILD_EXAMPLES=ON
cmake --build .   
```

构建完成后，可执行文件会生成到 `bin/` 目录


## 使用方式与扩展

核心思想：你只需要在 `Application` 模块中编写自己的 Layer 和应用类，核心循环与 ImGui 初始化交给 `Core` 处理。

### 1. 编写自定义 Layer

继承 `Flux::Layer` 并重写你关心的几个虚函数：

```cpp
class MyLayer : public Flux::Layer
{
public:
    void OnAttach() override {}                // Layer 被加入时调用
    void OnDetach() override {}                // Layer 被移除时调用
    void OnUpdate(float dt) override {}        // 每帧逻辑更新
    void OnRenderUI() override                 // 每帧 ImGui UI 绘制
    {
        ImGui::Begin("Hello, Flux!");
        ImGui::Text("This is a sample layer in MyApp.");
        ImGui::End();
        ImGui::ShowDemoWindow();
    }
};
```

### 2. 定义你的 Application

在 `Application/src/MyApp.cpp` 中（或新建文件）继承 `Flux::Application`，并在构造函数里推入 Layer、设置菜单栏等：

```cpp
class MyApp : public Flux::Application
{
public:
    MyApp()
    {
        // 注册自定义 Layer
        PushLayer(std::make_unique<MyLayer>());

        // 设置菜单栏回调
        SetMenubarCallback([this]()
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit"))
                    Close();   // 关闭应用
                ImGui::EndMenu();
            }
        });
    }
};

std::unique_ptr<Flux::Application> Flux::CreateApplication()
{
    return std::make_unique<MyApp>();
}
```

注意：`Flux/Core/src/EntryPoint.cpp` 中的 `main` 函数会调用 `Flux::CreateApplication()`，因此你只需保证该函数返回你的应用实例即可。

### 3. Layer 生命周期

`Flux::Application` 在主循环中会按顺序调用每个 Layer：

- `OnAttach()`：Layer 被加入时调用一次
- `OnUpdate(float dt)`：每帧更新（`dt` 为时间步长，单位秒）
- `OnRenderUI()`：每帧 ImGui UI 绘制
- `OnDetach()`：应用退出或 Layer 被移除时调用

这让你可以按照“逻辑层”的概念拆分不同功能（如：场景编辑层、属性面板层、日志层等）。


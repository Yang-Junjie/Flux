# Flux 应用程序模板

一个基于 C++ / OpenGL / Dear ImGui 的小型桌面应用程序模板，采用分层架构，将通用核心与业务逻辑解耦，适合快速搭建工具类或编辑器类应用。

## 特性

- 分层架构：`Core` 提供窗口循环、渲染和 UI 基础能力，`Application` 只关心业务逻辑
- 集成 Dear ImGui：支持 Docking、多视口（Viewports）、菜单栏等现代工具界面
- 内置渲染循环：自动计算时间步长 `TimeStep`，调用每一层的 `OnUpdate` / `OnRenderUI`
- 零外部依赖：GLFW、GLAD、glm、stb_image、ImGui 等第三方库已作为源码包含在仓库中
- 统一输出目录：所有可执行文件默认输出到 `bin/`

## 目录结构

```text
Flux/
├─ Flux/                      # 框架核心代码与第三方库
│  ├─ Core/                   # 框架核心库（FluxCore）
│  │  ├─ src/
│  │  │  ├─ Application.hpp/.cpp # 窗口和主循环，管理 Layer 与 ImGui
│  │  │  ├─ EntryPoint.cpp       # 通用入口，调用 Flux::CreateApplication()
│  │  │  └─ Layer.hpp            # Layer 抽象接口
│  └─ external/               # 第三方库源码（GLFW、GLAD、glm、ImGui、stb_image 等）
├─ Application/               # 具体应用程序
│  └─ src/
│     └─ MyApp.cpp            # 示例应用：注册 Layer，设置菜单栏
├─ bin/                       # 编译输出目录（app 可执行文件）
├─ build/                     # 推荐的 CMake 构建目录（可手动创建）
└─ CMakeLists.txt             # 顶层 CMake 配置
```

## 环境要求

- 操作系统：以 Windows 为主（CMake 默认链接 `opengl32`），其他平台需要自行调整 CMake 配置
- 编译器：支持 C++17 的编译器（MSVC / Clang / GCC 等）
- 构建工具：CMake（建议 3.15 及以上）

## 构建与运行

在项目根目录执行（以命令行为例）：

```bash
mkdir -p build
cd build
cmake ..          # 生成工程（可指定 VS、Ninja 等生成器）
cmake --build .   # 编译
```

构建完成后，可执行文件会生成到 `bin/` 目录，例如：

- Windows: `bin/app.exe`
- 其他平台: `bin/app`

直接运行该可执行文件即可看到示例界面和 ImGui Demo 窗口。

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

## 第三方库与许可证

本项目中使用的第三方库（GLFW、glm、Dear ImGui、stb_image 等）的许可证文件位于 `Flux/external/` 对应目录下，请在实际项目中遵守相关开源协议。

当前仓库更侧重于学习和快速原型搭建，如需在生产环境中使用，建议根据自身需求调整目录结构、构建脚本以及平台相关配置。

set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Core)
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)

set(CORE_SOURCES
    ${CORE_DIR}/src/EntryPoint.cpp
    ${CORE_DIR}/src/Application.cpp
)

set(THIRD_PARTY_SOURCES
    ${EXTERNAL_DIR}/GLAD/src/glad.c

    ${EXTERNAL_DIR}/imgui/imgui.cpp
    ${EXTERNAL_DIR}/imgui/imgui_demo.cpp
    ${EXTERNAL_DIR}/imgui/imgui_draw.cpp
    ${EXTERNAL_DIR}/imgui/imgui_tables.cpp
    ${EXTERNAL_DIR}/imgui/imgui_widgets.cpp
    # ${IMGUI_PLATFORM_SOURCE}
    ${EXTERNAL_DIR}/imgui/backends/imgui_impl_opengl3.cpp
    ${EXTERNAL_DIR}/imgui/backends/imgui_impl_glfw.cpp

    ${EXTERNAL_DIR}/stb_image/stb_image.cpp

)

set(SOURCES
    ${CORE_SOURCES}
    ${THIRD_PARTY_SOURCES}
)

add_library(FluxCore STATIC ${SOURCES})

target_include_directories(FluxCore PUBLIC
    ${CORE_DIR}/src
    ${EXTERNAL_DIR}/glm
    ${EXTERNAL_DIR}/GLFW/include
    ${EXTERNAL_DIR}/imgui
    ${EXTERNAL_DIR}/imgui/backends
    ${EXTERNAL_DIR}/GLAD/include
)

target_link_libraries(FluxCore PRIVATE glfw)
target_link_libraries(FluxCore PRIVATE opengl32)

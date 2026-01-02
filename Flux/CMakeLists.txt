set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Core)
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)

# -------- Core Sources --------
set(CORE_SOURCES
        ${CORE_DIR}/src/EntryPoint.cpp
        ${CORE_DIR}/src/ApplicationCommon.cpp
        ${CORE_DIR}/src/GLWrappers.cpp
)

if(ANDROID)
    list(APPEND CORE_SOURCES
            ${CORE_DIR}/src/ApplicationAndroid.cpp
            ${CORE_DIR}/src/ImageAndroid.cpp
    )
else()
    list(APPEND CORE_SOURCES
            ${CORE_DIR}/src/ApplicationDesktop.cpp
            ${CORE_DIR}/src/ImageDesktop.cpp
    )
endif()

# -------- Third Party Sources --------
if(ANDROID)
    set(THIRD_PARTY_SOURCES
            ${EXTERNAL_DIR}/imgui/imgui.cpp
            ${EXTERNAL_DIR}/imgui/imgui_demo.cpp
            ${EXTERNAL_DIR}/imgui/imgui_draw.cpp
            ${EXTERNAL_DIR}/imgui/imgui_tables.cpp
            ${EXTERNAL_DIR}/imgui/imgui_widgets.cpp
            ${EXTERNAL_DIR}/imgui/backends/imgui_impl_android.cpp
            ${EXTERNAL_DIR}/imgui/backends/imgui_impl_opengl3.cpp
    )
else()
    set(THIRD_PARTY_SOURCES
            ${EXTERNAL_DIR}/imgui/imgui.cpp
            ${EXTERNAL_DIR}/imgui/imgui_demo.cpp
            ${EXTERNAL_DIR}/imgui/imgui_draw.cpp
            ${EXTERNAL_DIR}/imgui/imgui_tables.cpp
            ${EXTERNAL_DIR}/imgui/imgui_widgets.cpp
            ${EXTERNAL_DIR}/GLAD/src/glad.c
            ${EXTERNAL_DIR}/imgui/backends/imgui_impl_opengl3.cpp
            ${EXTERNAL_DIR}/imgui/backends/imgui_impl_glfw.cpp
            ${EXTERNAL_DIR}/stb_image/stb_image.cpp
    )
endif()

set(SOURCES
        ${CORE_SOURCES}
        ${THIRD_PARTY_SOURCES}
)

# -------- Build Library --------
if(ANDROID)
    add_library(FluxCore STATIC ${SOURCES})
else()
    add_library(FluxCore STATIC ${SOURCES})
endif()

# -------- Include Directories --------
target_include_directories(FluxCore PUBLIC
        ${CORE_DIR}/src
        ${EXTERNAL_DIR}/glm
        ${EXTERNAL_DIR}/imgui
        ${EXTERNAL_DIR}/imgui/backends
)

if(ANDROID)
    if(NOT DEFINED ANDROID_NDK)
        message(FATAL_ERROR "ANDROID_NDK not defined!")
    endif()
    target_include_directories(FluxCore PUBLIC
            ${ANDROID_NDK}/sources/android/native_app_glue
    )
endif()

if(NOT ANDROID)
    target_include_directories(FluxCore PUBLIC
            ${EXTERNAL_DIR}/GLFW/include
            ${EXTERNAL_DIR}/GLAD/include
    )
endif()

# -------- Platform Definitions & Linking --------
if(ANDROID)
    target_compile_definitions(FluxCore PRIVATE ANDROID)
    target_link_libraries(FluxCore PRIVATE GLESv3 EGL android log)
else()
    if(WIN32)
        target_link_libraries(FluxCore PRIVATE opengl32 glfw)
    elseif(APPLE)
        find_library(COCOA_LIBRARY Cocoa)
        find_library(IOKIT_LIBRARY IOKit)
        find_library(COREVIDEO_LIBRARY CoreVideo)
        find_library(OPENGL_LIBRARY OpenGL)
        target_link_libraries(FluxCore PRIVATE ${OPENGL_LIBRARY} glfw ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
    else()
        find_package(OpenGL REQUIRED)
        target_link_libraries(FluxCore PRIVATE ${OPENGL_LIBRARIES} glfw pthread X11 Xrandr Xi Xxf86vm Xcursor dl)
    endif()
endif()

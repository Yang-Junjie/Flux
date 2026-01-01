cmake_minimum_required(VERSION 3.22.1)
project("Example")

add_subdirectory(../Flux) #<----- FluxCore

set(PAINTER_SOURCES
        android_main.cpp
        ExampleApp.cpp
        ExampleLayer.cpp
)

add_library(${CMAKE_PROJECT_NAME} SHARED ${PAINTER_SOURCES})

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE FluxCore)

if(ANDROID)
        if(NOT DEFINED ANDROID_NDK)
                message(FATAL_ERROR "ANDROID_NDK not defined!")
        endif()
        target_sources(${CMAKE_PROJECT_NAME} PRIVATE
                ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
        )
        target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
                ${ANDROID_NDK}/sources/android/native_app_glue
        )
        target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE GLESv3 EGL android log)
endif()

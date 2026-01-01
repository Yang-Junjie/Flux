cmake_minimum_required(VERSION 3.15)
project(Flux)


if(ANDROID)
    add_subdirectory(Flux)
    add_subdirectory(Flux/external/glm)
else ()
    # default to not building examples
    option(FLUX_BUILD_EXAMPLES "Build Flux example applications" OFF)

    set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin CACHE PATH "Output directory for binaries")

    file(MAKE_DIRECTORY ${BIN_DIR})


    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BIN_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${BIN_DIR})


    add_subdirectory(Flux/external/glm)
    add_subdirectory(Flux/external/GLFW)

    add_subdirectory(Flux)

    if(FLUX_BUILD_EXAMPLES)
        add_subdirectory(example/DesktopApplication)
    endif()
endif ()


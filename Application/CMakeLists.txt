
add_executable(app src/MyApp.cpp)
set_target_properties(app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
)
target_link_libraries(app PRIVATE FluxCore)
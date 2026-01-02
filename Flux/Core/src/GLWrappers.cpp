#include "GLWrappers.hpp"

namespace Flux::GL {
namespace {

GLuint CompileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        glDeleteShader(shader);
        throw std::runtime_error(std::string("Shader compile failed: ") + infoLog);
    }
    return shader;
}

} // namespace

uint32_t CreateBuffer(uint32_t target, std::size_t size, const void* data, uint32_t usage) {
    GLuint id = 0;
    glGenBuffers(1, &id);
    glBindBuffer(target, id);
    glBufferData(target, static_cast<GLsizeiptr>(size), data, usage);
    return id;
}

void UpdateBufferData(uint32_t buffer, uint32_t target, std::size_t size, const void* data, uint32_t usage) {
    if (buffer == 0)
        return;
    glBindBuffer(target, buffer);
    glBufferData(target, static_cast<GLsizeiptr>(size), data, usage);
}

void DeleteBuffer(uint32_t buffer) {
    GLuint id = static_cast<GLuint>(buffer);
    if (id != 0)
        glDeleteBuffers(1, &id);
}

void BindBuffer(uint32_t target, uint32_t buffer) {
    glBindBuffer(target, buffer);
}

uint32_t CreateVertexArray() {
    GLuint id = 0;
    glGenVertexArrays(1, &id);
    return id;
}

void DeleteVertexArray(uint32_t vao) {
    GLuint id = static_cast<GLuint>(vao);
    if (id != 0)
        glDeleteVertexArrays(1, &id);
}

void BindVertexArray(uint32_t vao) {
    glBindVertexArray(vao);
}

void EnableVertexAttribArray(uint32_t index) {
    glEnableVertexAttribArray(index);
}

void VertexAttribPointer(uint32_t index, int size, uint32_t type, bool normalized, int stride, intptr_t offset) {
    glVertexAttribPointer(index, size, type, normalized ? GL_TRUE : GL_FALSE, stride, reinterpret_cast<const void*>(offset));
}

uint32_t CreateShaderProgram(const std::string& vertexSrc, const std::string& fragmentSrc) {
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        glDeleteProgram(program);
        throw std::runtime_error(std::string("Program link failed: ") + infoLog);
    }

    return program;
}

void DeleteShaderProgram(uint32_t program) {
    GLuint id = static_cast<GLuint>(program);
    if (id != 0)
        glDeleteProgram(id);
}

void UseProgram(uint32_t program) {
    glUseProgram(program);
}

void DrawElements(uint32_t mode, int count, uint32_t type, intptr_t offset) {
    glDrawElements(mode, count, type, reinterpret_cast<const void*>(offset));
}

bool SetUniformFloat(uint32_t program, const char* name, float value) {
    if (program == 0 || name == nullptr)
        return false;

    GLint previous = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &previous);
    if (static_cast<GLuint>(previous) != program)
        glUseProgram(program);

    GLint location = glGetUniformLocation(program, name);
    if (location < 0) {
        if (static_cast<GLuint>(previous) != program)
            glUseProgram(previous);
        return false;
    }

    glUniform1f(location, value);

    if (static_cast<GLuint>(previous) != program)
        glUseProgram(previous);
    return true;
}

void ClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void Clear(uint32_t mask) {
    glClear(mask);
}

void Viewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void BindFramebuffer(uint32_t target, uint32_t framebuffer) {
    glBindFramebuffer(target, framebuffer);
}

} // namespace Flux::GL

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <stdexcept>

#if defined(__ANDROID__)
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

namespace Flux::GL {

uint32_t CreateBuffer(uint32_t target, std::size_t size, const void* data, uint32_t usage);
void DeleteBuffer(uint32_t buffer);
void BindBuffer(uint32_t target, uint32_t buffer);

uint32_t CreateVertexArray();
void DeleteVertexArray(uint32_t vao);
void BindVertexArray(uint32_t vao);

void EnableVertexAttribArray(uint32_t index);
void VertexAttribPointer(uint32_t index, int size, uint32_t type, bool normalized, int stride, intptr_t offset);

uint32_t CreateShaderProgram(const std::string& vertexSrc, const std::string& fragmentSrc);
void DeleteShaderProgram(uint32_t program);
void UseProgram(uint32_t program);

void DrawElements(uint32_t mode, int count, uint32_t type, intptr_t offset);

bool SetUniformFloat(uint32_t program, const char* name, float value);

void ClearColor(float r, float g, float b, float a);
void Clear(uint32_t mask);
void Viewport(int x, int y, int width, int height);

void BindFramebuffer(uint32_t target, uint32_t framebuffer);

} // namespace Flux::GL

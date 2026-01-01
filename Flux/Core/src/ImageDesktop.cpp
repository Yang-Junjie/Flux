#include "Image.hpp"

#include <glad/glad.h>

#include <iostream>

namespace Flux {

    Image::Image(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height) {
        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(width), static_cast<GLsizei>(height),
                     0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenFramebuffers(1, &m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, m_TextureID, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Image FBO incomplete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Image::~Image() {
        if (m_FBO)
            glDeleteFramebuffers(1, &m_FBO);
        if (m_TextureID)
            glDeleteTextures(1, &m_TextureID);
    }

    void Image::SetData(const void* data) {
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        static_cast<GLsizei>(m_Width), static_cast<GLsizei>(m_Height),
                        GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Image::CopyFrom(Image& other) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, other.GetFramebuffer());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);

        glBlitFramebuffer(0, 0, static_cast<GLint>(other.GetWidth()), static_cast<GLint>(other.GetHeight()),
                          0, 0, static_cast<GLint>(m_Width), static_cast<GLint>(m_Height),
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

} // namespace Flux


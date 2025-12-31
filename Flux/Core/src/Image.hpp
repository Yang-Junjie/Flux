#pragma once
#include <cstdint>
#include <iostream>

#ifdef ANDROID
// Android / OpenGL ES
#include <GLES3/gl3.h>
#else
// Desktop OpenGL (glad)
#include <glad/glad.h>
#endif

namespace Flux
{
    class Image
    {
    public:
        Image(uint32_t width, uint32_t height)
            : m_Width(width), m_Height(height)
        {
            glGenTextures(1, &m_TextureID);
            glBindTexture(GL_TEXTURE_2D, m_TextureID);

#ifdef ANDROID
            // In GLES3 internal format GL_RGBA8 is available (core in ES3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(width), static_cast<GLsizei>(height),
                         0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
#else
            // Desktop OpenGL
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(width), static_cast<GLsizei>(height),
                         0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
#endif

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glGenFramebuffers(1, &m_FBO);
            glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D, m_TextureID, 0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "Image FBO incomplete!" << std::endl;

            // Unbind framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // Unbind texture
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        ~Image()
        {
            if (m_FBO) glDeleteFramebuffers(1, &m_FBO);
            if (m_TextureID) glDeleteTextures(1, &m_TextureID);
        }

        void SetData(const void *data)
        {
            glBindTexture(GL_TEXTURE_2D, m_TextureID);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                            static_cast<GLsizei>(m_Width), static_cast<GLsizei>(m_Height),
                            GL_RGBA, GL_UNSIGNED_BYTE, data);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        uint32_t GetFramebuffer() const { return m_FBO; }

        uint32_t GetColorAttachment() const { return m_TextureID; }

        void CopyFrom(Image &other)
        {
            // glBlitFramebuffer / GL_READ/GL_DRAW_FRAMEBUFFER are core in OpenGL ES 3.0
            glBindFramebuffer(GL_READ_FRAMEBUFFER, other.GetFramebuffer());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);

            // note: cast sizes to GLsizei
            glBlitFramebuffer(0, 0, static_cast<GLint>(other.GetWidth()), static_cast<GLint>(other.GetHeight()),
                              0, 0, static_cast<GLint>(m_Width), static_cast<GLint>(m_Height),
                              GL_COLOR_BUFFER_BIT, GL_NEAREST);

            // bind default framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

    private:
        uint32_t m_Width, m_Height;
        uint32_t m_FBO = 0;
        uint32_t m_TextureID = 0;
    };
}

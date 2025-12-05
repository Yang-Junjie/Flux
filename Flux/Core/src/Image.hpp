#pragma once
#include <glad/glad.h>
#include <cstdint>
#include <iostream>
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
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height,
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
        }

        ~Image()
        {
            glDeleteFramebuffers(1, &m_FBO);
            glDeleteTextures(1, &m_TextureID);
        }

        void SetData(const void *data)
        {
            glBindTexture(GL_TEXTURE_2D, m_TextureID);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                            m_Width, m_Height,
                            GL_RGBA, GL_UNSIGNED_BYTE, data);
        }

        uint32_t GetFramebuffer() const { return m_FBO; }

        uint32_t GetColorAttachment() const { return m_TextureID; }

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

    private:
        uint32_t m_Width, m_Height;
        uint32_t m_FBO = 0;
        uint32_t m_TextureID = 0;
    };
}
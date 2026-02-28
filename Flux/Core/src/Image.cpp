// Copyright 2026 Beisent
// Image class implementation

#include "Image.hpp"

#include <iostream>

#include <glad/glad.h>

namespace flux
{

    Image::Image(uint32_t width, uint32_t height)
        : width_(width), height_(height)
    {
        glGenTextures(1, &texture_id_);
        glBindTexture(GL_TEXTURE_2D, texture_id_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(width),
                     static_cast<GLsizei>(height), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenFramebuffers(1, &fbo_);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               texture_id_, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "Image FBO incomplete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Image::~Image()
    {
        if (fbo_)
        {
            glDeleteFramebuffers(1, &fbo_);
        }
        if (texture_id_)
        {
            glDeleteTextures(1, &texture_id_);
        }
    }

    void Image::SetData(const void *data)
    {
        glBindTexture(GL_TEXTURE_2D, texture_id_);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, static_cast<GLsizei>(width_),
                        static_cast<GLsizei>(height_), GL_RGBA, GL_UNSIGNED_BYTE,
                        data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Image::CopyFrom(Image &other)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, other.framebuffer());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);

        glBlitFramebuffer(0, 0, static_cast<GLint>(other.width()),
                          static_cast<GLint>(other.height()), 0, 0,
                          static_cast<GLint>(width_), static_cast<GLint>(height_),
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

} // namespace flux

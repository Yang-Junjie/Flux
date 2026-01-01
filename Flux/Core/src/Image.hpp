#pragma once

#include <cstdint>

namespace Flux {

    class Image {
    public:
        Image(uint32_t width, uint32_t height);
        ~Image();

        void SetData(const void* data);

        uint32_t GetFramebuffer() const { return m_FBO; }
        uint32_t GetColorAttachment() const { return m_TextureID; }

        void CopyFrom(Image& other);

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

    private:
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        uint32_t m_FBO = 0;
        uint32_t m_TextureID = 0;
    };

} // namespace Flux


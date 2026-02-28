// Copyright 2026 Beisent
// Image class for texture and framebuffer management

#ifndef FLUX_CORE_SRC_IMAGE_HPP_
#define FLUX_CORE_SRC_IMAGE_HPP_

#include <cstdint>

namespace flux
{

  class Image
  {
  public:
    Image(uint32_t width, uint32_t height);
    ~Image();

    // Disable copy and assign
    Image(const Image &) = delete;
    Image &operator=(const Image &) = delete;

    void SetData(const void *data);
    void CopyFrom(Image &other);

    uint32_t framebuffer() const { return fbo_; }
    uint32_t color_attachment() const { return texture_id_; }
    uint32_t width() const { return width_; }
    uint32_t height() const { return height_; }

  private:
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    uint32_t fbo_ = 0;
    uint32_t texture_id_ = 0;
  };

} // namespace flux

#endif // FLUX_CORE_SRC_IMAGE_HPP_

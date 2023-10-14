#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>

#include "../buffer/buffer.hpp"
#include "../command/command_buffer.hpp"
#include "../image/image.hpp"

#include <memory>

namespace NugieVulkan
{
  class Texture
  {
    public:
      Texture(Device* device, const char* textureFileName);
      ~Texture();

      VkDescriptorImageInfo getDescriptorInfo();

    private:
      Device* device;
      std::unique_ptr<Image> image;

      VkSampler sampler;
      uint32_t mipLevels;

      void createTextureImage(const char* textureFileName);
      void createTextureSampler();
  };
  
  
} // namespace NugieVulkan

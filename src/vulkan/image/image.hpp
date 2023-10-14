#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>

#include "../buffer/buffer.hpp"
#include "../command/command_buffer.hpp"

namespace NugieVulkan
{
  class Image
  {
    public:
      Image(Device* device, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, 
        VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, 
        VkImageAspectFlags aspectFlags);
      Image(Device* device, uint32_t width, uint32_t height, VkImage image, uint32_t mipLevels, VkFormat format, VkImageAspectFlags aspectFlags);
      ~Image();

      VkImage getImage() const { return this->image; }
      VkImageView getImageView() const { return this->imageView; }
      VkDeviceMemory getImageMemory() const { return this->imageMemory; }
      VkImageLayout getLayout() const { return this->layout; }
      
      VkImageAspectFlags getAspectFlag() const { return this->aspectFlags; }
      uint32_t getMipLevels() const { return this->mipLevels; }

      uint32_t getWidth() { return this->width; }
      uint32_t getHeight() { return this->height; }

      VkDescriptorImageInfo getDescriptorInfo(VkImageLayout desiredImageLayout);

      void copyBufferToImage(Buffer* srcBuffer, CommandBuffer* commandBuffer = nullptr);
      void copyImageToBuffer(Buffer* destBuffer, CommandBuffer* commandBuffer = nullptr);
      void copyImageFromOther(Image* srcImage, CommandBuffer* commandBuffer = nullptr);
      void copyImageToOther(Image* dstImage, CommandBuffer* commandBuffer = nullptr);

      void generateMipMap(CommandBuffer* commandBuffer = nullptr);

      void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, 
        VkAccessFlags srcAccess, VkAccessFlags dstAccess, uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, 
        uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, CommandBuffer* commandBuffer = nullptr, 
        Device* device = nullptr);

      static void transitionImageLayout(std::vector<Image*> images, VkImageLayout oldLayout, VkImageLayout newLayout, 
        VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, VkAccessFlags srcAccess, VkAccessFlags dstAccess,
        uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        CommandBuffer* commandBuffer = nullptr, Device* device = nullptr);

    private:
      Device* device;

      VkImage image;
      VkImageView imageView;
      VkDeviceMemory imageMemory;
      VkFormat format;
      VkImageAspectFlags aspectFlags;
      VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
      
      uint32_t width, height, mipLevels;
      bool isImageCreatedByUs = false;

      void createImage(VkSampleCountFlagBits numSamples, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
      void createImageView();
  };
  
  
} // namespace NugieVulkan

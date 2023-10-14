#pragma once

#include "../device/device.hpp"
#include "../image/image.hpp"
#include "../command/command_buffer.hpp"

#include <memory>
 
namespace NugieVulkan {
 
class Buffer {
 public:
  Buffer(
    Device* device,
    VkDeviceSize instanceSize,
    uint32_t instanceCount,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkDeviceSize minOffsetAlignment = 1);
  ~Buffer();

  VkBuffer getBuffer() const { return this->buffer; }
  void* getMappedMemory() const { return this->mapped; }
  uint32_t getInstanceCount() const { return this->instanceCount; }
  VkDeviceSize getInstanceSize() const { return this->instanceSize; }
  VkDeviceSize getAlignmentSize() const { return this->instanceSize; }
  VkDeviceSize getBufferSize() const { return this->bufferSize; }
  VkBufferUsageFlags getUsageFlags() const { return this->usageFlags; }
  VkMemoryPropertyFlags getMemoryPropertyFlags() const { return this->memoryPropertyFlags; }

  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

  void copyFromAnotherBuffer(Buffer* srcBuffer, CommandBuffer* commandBuffer = nullptr);
  void copyToAnotherBuffer(Buffer* destBuffer, CommandBuffer* commandBuffer = nullptr);
  void copyBufferToImage(Image* destImage, CommandBuffer* commandBuffer = nullptr);
  void copyImageToBuffer(Image* srcImage, CommandBuffer* commandBuffer = nullptr);
 
  VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  void unmap();
 
  void writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  void readFromBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
 
  void writeToIndex(void* data, int index);
  void readFromIndex(void* data, int index);
  VkResult flushIndex(int index);
  VkResult invalidateIndex(int index);
  VkDescriptorBufferInfo descriptorInfoForIndex(int index);

  void transitionBuffer(VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, 
    VkAccessFlags srcAccess, VkAccessFlags dstAccess, uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, 
    uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, CommandBuffer* commandBuffer = nullptr);

  static void transitionBuffer(std::vector<Buffer*> buffers, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, 
    VkAccessFlags srcAccess, VkAccessFlags dstAccess, uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, 
    uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, CommandBuffer* commandBuffer = nullptr, 
    Device* device = nullptr);
 
 private:
  static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);
 
  Device* device;

  void* mapped = nullptr;
  VkBuffer buffer = VK_NULL_HANDLE;
  VkDeviceMemory memory = VK_NULL_HANDLE;
 
  VkDeviceSize bufferSize;
  uint32_t instanceCount;
  VkDeviceSize instanceSize;
  VkDeviceSize alignmentSize;
  VkBufferUsageFlags usageFlags;
  VkMemoryPropertyFlags memoryPropertyFlags;
};
 
}  // namespace lve
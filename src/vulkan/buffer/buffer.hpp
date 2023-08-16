#pragma once

#include "../device/device.hpp"
#include "../command/command_buffer.hpp"

#include <memory>
 
namespace nugiEngine {
 
class EngineBuffer {
 public:
  EngineBuffer(
      EngineDevice& device,
      VkDeviceSize instanceSize,
      uint32_t instanceCount,
      VkBufferUsageFlags usageFlags,
      VkMemoryPropertyFlags memoryPropertyFlags,
      VkDeviceSize minOffsetAlignment = 1);
  ~EngineBuffer();

  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
  void copyBuffer(VkBuffer srcBuffer, VkDeviceSize size);
  void copyBufferToImage(VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);
 
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
    uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, std::shared_ptr<EngineCommandBuffer> commandBuffer = nullptr, 
    EngineDevice *appDevice = nullptr);
 
  VkBuffer getBuffer() const { return buffer; }
  void* getMappedMemory() const { return mapped; }
  uint32_t getInstanceCount() const { return instanceCount; }
  VkDeviceSize getInstanceSize() const { return instanceSize; }
  VkDeviceSize getAlignmentSize() const { return instanceSize; }
  VkBufferUsageFlags getUsageFlags() const { return usageFlags; }
  VkMemoryPropertyFlags getMemoryPropertyFlags() const { return memoryPropertyFlags; }
  VkDeviceSize getBufferSize() const { return bufferSize; }

  static void transitionBuffer(std::vector<std::shared_ptr<EngineBuffer>> buffers, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, 
    VkAccessFlags srcAccess, VkAccessFlags dstAccess, uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, 
    uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, std::shared_ptr<EngineCommandBuffer> commandBuffer = nullptr, 
    EngineDevice *appDevice = nullptr);
 
 private:
  static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);
 
  EngineDevice& engineDevice;

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
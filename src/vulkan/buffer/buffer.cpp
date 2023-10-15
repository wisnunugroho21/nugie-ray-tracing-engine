/*
 * Encapsulates a vulkan buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 */
 
#include "buffer.hpp"
 
// std
#include <cassert>
#include <cstring>
 
namespace NugieVulkan {
  /**
   * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
   *
   * @param instanceSize The size of an instance
   * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
   * minUniformBufferOffsetAlignment)
   *
   * @return VkResult of the buffer mapping call
   */
  VkDeviceSize Buffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
    if (minOffsetAlignment > 0) {
      return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
  }
  
  Buffer::Buffer(
      Device* device,
      VkDeviceSize instanceSize,
      uint32_t instanceCount,
      VkBufferUsageFlags usageFlags,
      VkMemoryPropertyFlags memoryPropertyFlags,
      VkDeviceSize minOffsetAlignment
    )
      : device{device},
        instanceSize{instanceSize},
        instanceCount{instanceCount},
        usageFlags{usageFlags},
        memoryPropertyFlags{memoryPropertyFlags} 
  {
    this->alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
    this->bufferSize = alignmentSize * instanceCount;

    this->createBuffer(bufferSize, usageFlags, memoryPropertyFlags);
  }
  
  Buffer::~Buffer() {
    this->unmap();
    vkDestroyBuffer(this->device->getLogicalDevice(), this->buffer, nullptr);
    vkFreeMemory(this->device->getLogicalDevice(), this->memory, nullptr);
  }
  
  /**
   * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
   *
   * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
   * buffer range.
   * @param offset (Optional) Byte offset from beginning
   *
   * @return VkResult of the buffer mapping call
   */
  VkResult Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
    assert(this->buffer && this->memory && "Called map on buffer before create");
    return vkMapMemory(this->device->getLogicalDevice(), this->memory, offset, size, 0, &this->mapped);
  }
  
  /**
   * Unmap a mapped memory range
   *
   * @note Does not return a result as vkUnmapMemory can't fail
   */
  void Buffer::unmap() {
    if (this->mapped) {
      vkUnmapMemory(this->device->getLogicalDevice(), this->memory);
      this->mapped = nullptr;
    }
  }
  
  /**
   * Copies the specified data to the mapped buffer. Default value writes whole buffer range
   *
   * @param data Pointer to the data to copy
   * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
   * range.
   * @param offset (Optional) Byte offset from beginning of mapped region
   *
   */
  void Buffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
    assert(this->mapped && "Cannot copy to unmapped buffer");
  
    if (size == VK_WHOLE_SIZE) {
      memcpy(this->mapped, data, this->bufferSize);
    } else {
      char *memOffset = (char *)this->mapped;
      memOffset += offset;
      memcpy(memOffset, data, size);
    }
  }

  /**
   * Copies the specified data from the mapped buffer. Default value writes whole buffer range
   *
   * @param data Pointer to the data to copy
   * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
   * range.
   * @param offset (Optional) Byte offset from beginning of mapped region
   *
   */
  void Buffer::readFromBuffer(void* data, VkDeviceSize size, VkDeviceSize offset) {
    assert(this->mapped && "Cannot copy from unmapped buffer");
  
    if (size == VK_WHOLE_SIZE) {
      memcpy(data, this->mapped, this->bufferSize);
    } else {
      char *memOffset = (char *)this->mapped;
      memOffset += offset;
      memcpy(data, memOffset, size);
    }
  }
  
  /**
   * Flush a memory range of the buffer to make it visible to the device
   *
   * @note Only required for non-coherent memory
   *
   * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
   * complete buffer range.
   * @param offset (Optional) Byte offset from beginning
   *
   * @return VkResult of the flush call
   */
  VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = this->memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkFlushMappedMemoryRanges(this->device->getLogicalDevice(), 1, &mappedRange);
  }
  
  /**
   * Invalidate a memory range of the buffer to make it visible to the host
   *
   * @note Only required for non-coherent memory
   *
   * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
   * the complete buffer range.
   * @param offset (Optional) Byte offset from beginning
   *
   * @return VkResult of the invalidate call
   */
  VkResult Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = this->memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkInvalidateMappedMemoryRanges(this->device->getLogicalDevice(), 1, &mappedRange);
  }
  
  /**
   * Create a buffer info descriptor
   *
   * @param size (Optional) Size of the memory range of the descriptor
   * @param offset (Optional) Byte offset from beginning
   *
   * @return VkDescriptorBufferInfo of specified offset and range
   */
  VkDescriptorBufferInfo Buffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
    return VkDescriptorBufferInfo{
        this->buffer,
        offset,
        size,
    };
  }
  
  /**
   * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
   *
   * @param data Pointer to the data to copy
   * @param index Used in offset calculation
   *
   */
  void Buffer::writeToIndex(void *data, int index) {
    this->writeToBuffer(data, this->instanceSize, index * this->alignmentSize);
  }

  /**
   * Copies "instanceSize" bytes of data from the mapped buffer at an offset of index * alignmentSize
   *
   * @param data Pointer to the data to copy
   * @param index Used in offset calculation
   *
   */
  void Buffer::readFromIndex(void* data, int index) {
    this->readFromBuffer(data, this->instanceSize, index * this->alignmentSize);
  }
  
  /**
   *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
   *
   * @param index Used in offset calculation
   *
   */
  VkResult Buffer::flushIndex(int index) { 
    return this->flush(this->alignmentSize, index * this->alignmentSize); 
  }
  
  /**
   * Create a buffer info descriptor
   *
   * @param index Specifies the region given by index * alignmentSize
   *
   * @return VkDescriptorBufferInfo for instance at index
   */
  VkDescriptorBufferInfo Buffer::descriptorInfoForIndex(int index) {
    return this->descriptorInfo(this->alignmentSize, index * this->alignmentSize);
  }
  
  /**
   * Invalidate a memory range of the buffer to make it visible to the host
   *
   * @note Only required for non-coherent memory
   *
   * @param index Specifies the region to invalidate: index * alignmentSize
   *
   * @return VkResult of the invalidate call
   */
  VkResult Buffer::invalidateIndex(int index) {
    return this->invalidate(this->alignmentSize, index * this->alignmentSize);
  }

  void Buffer::transitionBuffer(VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, 
    VkAccessFlags srcAccess, VkAccessFlags dstAccess, uint32_t srcQueueFamilyIndex, 
    uint32_t dstQueueFamilyIndex, CommandBuffer* commandBuffer)
  {
    bool isCommandBufferCreatedHere = false;
    
    if (commandBuffer == nullptr) {
      commandBuffer = new CommandBuffer(this->device);
      commandBuffer->beginSingleTimeCommand();

      isCommandBufferCreatedHere = true;
    }

    VkBufferMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;

    barrier.srcQueueFamilyIndex = srcQueueFamilyIndex;
    barrier.dstQueueFamilyIndex = dstQueueFamilyIndex;

    barrier.buffer = this->buffer;
    barrier.size = this->bufferSize;
    barrier.offset = 0;
    barrier.srcAccessMask = srcAccess;
    barrier.dstAccessMask = dstAccess;

    vkCmdPipelineBarrier(
      commandBuffer->getCommandBuffer(), 
      srcStage, 
      dstStage,
      0,
      0, 
      nullptr,
      1, 
      &barrier,
      0, 
      nullptr
    );

    if (isCommandBufferCreatedHere) {
      commandBuffer->endCommand();
      commandBuffer->submitCommand(this->device->getTransferQueue(0));

      delete commandBuffer;
    }
  }

  void Buffer::transitionBuffer(std::vector<Buffer*> buffers, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, 
    VkAccessFlags srcAccess, VkAccessFlags dstAccess, uint32_t srcQueueFamilyIndex, uint32_t dstQueueFamilyIndex, 
    CommandBuffer* commandBuffer, Device* device) 
  {
    bool isCommandBufferCreatedHere = false;
    
    if (commandBuffer == nullptr) {
      commandBuffer = new CommandBuffer(device);
      commandBuffer->beginSingleTimeCommand();

      isCommandBufferCreatedHere = true;
    }

    std::vector<VkBufferMemoryBarrier> barriers{};

    for (auto &&buffer : buffers) {
      VkBufferMemoryBarrier barrier{};
      barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;

      barrier.srcQueueFamilyIndex = srcQueueFamilyIndex;
      barrier.dstQueueFamilyIndex = dstQueueFamilyIndex;

      barrier.buffer = buffer->getBuffer();
      barrier.size = buffer->getBufferSize();
      barrier.offset = 0;
      barrier.srcAccessMask = srcAccess;
      barrier.dstAccessMask = dstAccess;

      barriers.emplace_back(barrier);
    }
    

    vkCmdPipelineBarrier(
      commandBuffer->getCommandBuffer(), 
      srcStage, 
      dstStage,
      0,
      0, 
      nullptr,
      static_cast<uint32_t>(barriers.size()), 
      barriers.data(),
      0, 
      nullptr
    );

    if (isCommandBufferCreatedHere) {
      commandBuffer->endCommand();
      commandBuffer->submitCommand(device->getTransferQueue(0));

      delete commandBuffer;
    }
  }

  void Buffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(this->device->getLogicalDevice(), &bufferInfo, nullptr, &this->buffer) != VK_SUCCESS) {
      throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(this->device->getLogicalDevice(), this->buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = this->device->findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(this->device->getLogicalDevice(), &allocInfo, nullptr, &this->memory) != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(this->device->getLogicalDevice(), this->buffer, this->memory, 0);
  }

  void Buffer::copyFromAnotherBuffer(Buffer* srcBuffer, CommandBuffer* commandBuffer) {
    bool isCommandBufferCreatedHere = false;
    
    if (commandBuffer == nullptr) {
      commandBuffer = new CommandBuffer(this->device);
      commandBuffer->beginSingleTimeCommand();

      isCommandBufferCreatedHere = true;
    }

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;  // Optional
    copyRegion.dstOffset = 0;  // Optional
    copyRegion.size = srcBuffer->getBufferSize();
    vkCmdCopyBuffer(commandBuffer->getCommandBuffer(), srcBuffer->getBuffer(), this->buffer, 1, &copyRegion);

    if (isCommandBufferCreatedHere) {
      commandBuffer->endCommand();
      commandBuffer->submitCommand(device->getTransferQueue(0));

      delete commandBuffer;
    }
  }

  void Buffer::copyToAnotherBuffer(Buffer* destBuffer, CommandBuffer* commandBuffer) {
    bool isCommandBufferCreatedHere = false;
    
    if (commandBuffer == nullptr) {
      commandBuffer = new CommandBuffer(this->device);
      commandBuffer->beginSingleTimeCommand();

      isCommandBufferCreatedHere = true;
    }

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;  // Optional
    copyRegion.dstOffset = 0;  // Optional
    copyRegion.size = this->bufferSize;
    vkCmdCopyBuffer(commandBuffer->getCommandBuffer(), this->buffer, destBuffer->getBuffer(), 1, &copyRegion);

    if (isCommandBufferCreatedHere) {
      commandBuffer->endCommand();
      commandBuffer->submitCommand(device->getTransferQueue(0));

      delete commandBuffer;
    }
  }

  void Buffer::copyBufferToImage(Image* destImage, CommandBuffer* commandBuffer) {
    bool isCommandBufferCreatedHere = false;
    
    if (commandBuffer == nullptr) {
      commandBuffer = new CommandBuffer(this->device);
      commandBuffer->beginSingleTimeCommand();

      isCommandBufferCreatedHere = true;
    }

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1u;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {destImage->getWidth(), destImage->getHeight(), 1};

    vkCmdCopyBufferToImage(
      commandBuffer->getCommandBuffer(),
      this->buffer,
      destImage->getImage(),
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      1,
      &region
    );

    if (isCommandBufferCreatedHere) {
      commandBuffer->endCommand();
      commandBuffer->submitCommand(device->getTransferQueue(0));

      delete commandBuffer;
    }
  }

  void Buffer::copyImageToBuffer(Image* srcImage, CommandBuffer* commandBuffer) {
    bool isCommandBufferCreatedHere = false;
    
    if (commandBuffer == nullptr) {
      commandBuffer = new CommandBuffer(this->device);
      commandBuffer->beginSingleTimeCommand();

      isCommandBufferCreatedHere = true;
    }

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1u;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {srcImage->getWidth(), srcImage->getHeight(), 1};

    vkCmdCopyImageToBuffer(
      commandBuffer->getCommandBuffer(),
      srcImage->getImage(),
      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      this->buffer,
      1,
      &region
    );

    if (isCommandBufferCreatedHere) {
      commandBuffer->endCommand();
      commandBuffer->submitCommand(device->getTransferQueue(0));

      delete commandBuffer;
    }
  }
  
 
}  // namespace lve
#pragma once

#include "../../../vulkan/device/device.hpp"
#include "../../../vulkan/buffer/buffer.hpp"
#include "../../../vulkan/command/command_buffer.hpp"
#include "../../utils/bvh/bvh.hpp"
#include "../../ray_ubo.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace NugieApp {
	class PrimitiveModel {
    public:
      PrimitiveModel(NugieVulkan::Device* device);

      VkDescriptorBufferInfo getPrimitiveInfo() { return this->primitiveBuffer->descriptorInfo();  }
      VkDescriptorBufferInfo getBvhInfo() { return this->bvhBuffer->descriptorInfo(); }

      uint32_t getPrimitiveSize() const { return static_cast<uint32_t>(this->primitives.size()); }
      uint32_t getBvhSize() const { return static_cast<uint32_t>(this->bvhNodes.size()); }

      void addPrimitive(std::vector<Primitive> primitives, std::vector<RayTraceVertex> vertices);
      void createBuffers(NugieVulkan::CommandBuffer *commandBuffer);

      // static std::vector<Primitive>> createPrimitivesFromFile(NugieVulkan::Device* device, NugieVulkan::CommandBuffer *commandBuffer, const std::string &filePath, uint32_t materialIndex);
      
    private:
      NugieVulkan::Device* device;

      std::vector<Primitive> primitives{};
      std::vector<BvhNode> bvhNodes{};
      
      std::shared_ptr<NugieVulkan::Buffer> primitiveBuffer;
      std::shared_ptr<NugieVulkan::Buffer> bvhBuffer;
      
      std::vector<BvhNode> createBvhData(std::vector<Primitive> primitives, std::vector<RayTraceVertex> vertices);
	};
} // namespace NugieApp

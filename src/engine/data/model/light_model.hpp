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
	class LightModel {
    public:
      LightModel(NugieVulkan::Device* device, std::vector<TriangleLight> triangleLights, std::vector<RayTraceVertex> vertices);

      VkDescriptorBufferInfo getLightInfo() { return this->lightBuffer->descriptorInfo(); }
      VkDescriptorBufferInfo getBvhInfo() { return this->bvhBuffer->descriptorInfo(); }
      
    private:
      NugieVulkan::Device* device;
      
      std::unique_ptr<NugieVulkan::Buffer> lightBuffer;
      std::unique_ptr<NugieVulkan::Buffer> bvhBuffer;

      void createBuffers(std::vector<TriangleLight> triangleLights, std::vector<BvhNode> bvhNodes);
	};
} // namespace NugieApp

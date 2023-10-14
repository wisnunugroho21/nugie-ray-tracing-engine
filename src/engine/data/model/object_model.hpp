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
	class ObjectModel {
    public:
      ObjectModel(NugieVulkan::Device* device, std::vector<Object> objects, std::vector<BoundBox*> boundBoxes);

      VkDescriptorBufferInfo getObjectInfo() { return this->objectBuffer->descriptorInfo();  }
      VkDescriptorBufferInfo getBvhInfo() { return this->bvhBuffer->descriptorInfo(); }

    private:
      NugieVulkan::Device* device;
      
      std::unique_ptr<NugieVulkan::Buffer> objectBuffer;
      std::unique_ptr<NugieVulkan::Buffer> bvhBuffer;

      void createBuffers(std::vector<Object> objects, std::vector<BvhNode> bvhNodes);
	};
} // namespace NugieApp

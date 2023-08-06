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

namespace nugiEngine {
	class EngineMasterModel {
    public:
      class Builder {
        public:
          Builder(EngineDevice &engineDevice);

          Builder addPrimitives(std::shared_ptr<std::vector<Primitive>> primitives, std::shared_ptr<std::vector<RayTraceVertex>> vertices);

          Builder setObjects(std::shared_ptr<std::vector<Object>> objects, std::vector<std::shared_ptr<BoundBox>> boundBoxes);
          Builder setVertices(std::shared_ptr<std::vector<RayTraceVertex>> vertices);
          Builder setLights(std::shared_ptr<std::vector<TriangleLight>> triangleLights, std::shared_ptr<std::vector<RayTraceVertex>> vertices);

          Builder setMaterials(std::shared_ptr<std::vector<Material>> materials);
          Builder setTransformations(std::vector<std::shared_ptr<TransformComponent>> transformationComponents);
          Builder setMortonPixels(std::shared_ptr<std::vector<Pixel>> mortonPixels);

          uint32_t getPrimitiveSize() const { return static_cast<uint32_t>(this->primitives->size()); }
          uint32_t gePrimitiveBvhSize() const { return static_cast<uint32_t>(this->primitiveBvhNodes->size()); }

          std::shared_ptr<EngineMasterModel> build();

        private:
          EngineDevice &engineDevice;

          std::shared_ptr<std::vector<Object>> objects;
          std::shared_ptr<std::vector<BvhNode>> objectBvhNodes;
          std::shared_ptr<std::vector<Primitive>> primitives;
          std::shared_ptr<std::vector<BvhNode>> primitiveBvhNodes;
          std::shared_ptr<std::vector<RayTraceVertex>> vertices;
          std::shared_ptr<std::vector<TriangleLight>> triangleLights;
          std::shared_ptr<std::vector<BvhNode>> triangleLightBvhNodes;
          std::shared_ptr<std::vector<Material>> materials;
          std::shared_ptr<std::vector<Transformation>> transformation;
          std::shared_ptr<std::vector<Pixel>> mortonPixels;
      };

      EngineMasterModel(EngineDevice &device, std::shared_ptr<std::vector<Object>> objects, std::shared_ptr<std::vector<BvhNode>> objectBvhNodes,
        std::shared_ptr<std::vector<Primitive>> primitives, std::shared_ptr<std::vector<BvhNode>> primitiveBvhNodes, 
        std::shared_ptr<std::vector<RayTraceVertex>> vertices, std::shared_ptr<std::vector<TriangleLight>> triangleLights, 
        std::shared_ptr<std::vector<BvhNode>> triangleLightBvhNodes, std::shared_ptr<std::vector<Material>> materials, 
        std::shared_ptr<std::vector<Transformation>> transformation, std::shared_ptr<std::vector<Pixel>> mortonPixels);

      VkDescriptorBufferInfo getObjectInfo() { return this->buffer->descriptorInfo(objectBufferSize, 0);  }
      VkDescriptorBufferInfo getObjectBvhInfo() { return this->buffer->descriptorInfo(objectBvhBufferSize, objectBufferSize); }
      VkDescriptorBufferInfo getPrimitiveInfo() { return this->buffer->descriptorInfo(primitiveBufferSize, objectBufferSize + objectBvhBufferSize);  }
      VkDescriptorBufferInfo getPrimitiveBvhInfo() { return this->buffer->descriptorInfo(primitiveBvhBufferSize, objectBufferSize + objectBvhBufferSize + 
        primitiveBufferSize); }
      VkDescriptorBufferInfo getVertexnfo() { return this->buffer->descriptorInfo(verticeBufferSize, objectBufferSize + objectBvhBufferSize + 
        primitiveBufferSize + primitiveBvhBufferSize); }
      VkDescriptorBufferInfo getTriangleLightInfo() { return this->buffer->descriptorInfo(triangleLightBufferSize, objectBufferSize + objectBvhBufferSize + 
        primitiveBufferSize + primitiveBvhBufferSize + verticeBufferSize); }
      VkDescriptorBufferInfo getTriangleLightBvhInfo() { return this->buffer->descriptorInfo(triangleLightBvhBufferSize, objectBufferSize + objectBvhBufferSize + 
        primitiveBufferSize + primitiveBvhBufferSize + verticeBufferSize + triangleLightBufferSize); }
      VkDescriptorBufferInfo getMaterialInfo() { return this->buffer->descriptorInfo(materialBufferSize, objectBufferSize + objectBvhBufferSize + 
        primitiveBufferSize + primitiveBvhBufferSize + verticeBufferSize + triangleLightBufferSize + triangleLightBvhBufferSize);  }
      VkDescriptorBufferInfo getTransformationInfo() { return this->buffer->descriptorInfo(transformationBufferSize, objectBufferSize + objectBvhBufferSize + 
        primitiveBufferSize + primitiveBvhBufferSize + verticeBufferSize + triangleLightBufferSize + triangleLightBvhBufferSize + materialBufferSize);  }
      VkDescriptorBufferInfo getMortonPixelInfo() { return this->buffer->descriptorInfo(transformationBufferSize, objectBufferSize + objectBvhBufferSize + 
        primitiveBufferSize + primitiveBvhBufferSize + verticeBufferSize + triangleLightBufferSize + triangleLightBvhBufferSize + materialBufferSize + 
        transformationBufferSize);  }
      
    private:
      VkDeviceSize objectBufferSize, objectBvhBufferSize;
      VkDeviceSize primitiveBufferSize, primitiveBvhBufferSize;
      VkDeviceSize verticeBufferSize;
      VkDeviceSize triangleLightBufferSize, triangleLightBvhBufferSize;
      VkDeviceSize materialBufferSize, transformationBufferSize, mortonPixelBufferSize;

      std::shared_ptr<EngineBuffer> buffer;
	};
} // namespace nugiEngine

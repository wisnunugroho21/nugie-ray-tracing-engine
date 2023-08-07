#include "master_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace nugiEngine {
  EngineMasterModel::Builder::Builder(EngineDevice &engineDevice) : engineDevice{engineDevice} {
		this->primitives = std::make_shared<std::vector<Primitive>>();
		this->primitiveBvhNodes = std::make_shared<std::vector<BvhNode>>();
	}

  EngineMasterModel::Builder EngineMasterModel::Builder::addPrimitives(std::shared_ptr<std::vector<Primitive>> curPrimitives, std::shared_ptr<std::vector<RayTraceVertex>> vertices) {
    std::vector<std::shared_ptr<BoundBox>> boundBoxes;
		for (uint32_t i = 0; i < primitives->size(); i++) {
			boundBoxes.push_back(std::make_shared<PrimitiveBoundBox>(PrimitiveBoundBox{ i + 1, (*primitives)[i], vertices }));
		}

		auto curBvhNodes = createBvh(boundBoxes);

		for (int i = 0; i < curBvhNodes->size(); i++) {
			this->primitiveBvhNodes->emplace_back((*curBvhNodes)[i]);
		}

		for (int i = 0; i < curPrimitives->size(); i++) {
			this->primitives->emplace_back((*curPrimitives)[i]);
		}

		return *this;
  }

	EngineMasterModel::Builder EngineMasterModel::Builder::setObjects(std::shared_ptr<std::vector<Object>> objects, std::vector<std::shared_ptr<BoundBox>> boundBoxes) {
		this->objects = objects;
		this->objectBvhNodes = createBvh(boundBoxes);

		return *this;
	}

  EngineMasterModel::Builder EngineMasterModel::Builder::setVertices(std::shared_ptr<std::vector<RayTraceVertex>> vertices) {
    this->vertices = vertices;
		return *this;
  }

	EngineMasterModel::Builder EngineMasterModel::Builder::setLights(std::shared_ptr<std::vector<TriangleLight>> triangleLights, std::shared_ptr<std::vector<RayTraceVertex>> vertices) {
		this->triangleLights = triangleLights;

		std::vector<std::shared_ptr<BoundBox>> boundBoxes;
		for (int i = 0; i < triangleLights->size(); i++) {
			boundBoxes.emplace_back(std::make_shared<TriangleLightBoundBox>(TriangleLightBoundBox{ i + 1, (*triangleLights)[i], vertices }));
		}

		this->triangleLightBvhNodes = createBvh(boundBoxes);
		return *this;
	}

	EngineMasterModel::Builder EngineMasterModel::Builder::setMaterials(std::shared_ptr<std::vector<Material>> materials) {
		this->materials = materials;
		return *this;
	}

	EngineMasterModel::Builder EngineMasterModel::Builder::setTransformations(std::vector<std::shared_ptr<TransformComponent>> transformationComponents) {
		auto newTransforms = std::make_shared<std::vector<Transformation>>();
		for (auto &&transform : transformationComponents) {
			newTransforms->emplace_back(Transformation{ 
				transform->getPointMatrix(),
				transform->getDirMatrix(), 
				transform->getPointInverseMatrix(), 
				transform->getDirInverseMatrix(), 
				transform->getNormalMatrix() 
			});
		}

		this->transformation = newTransforms;
		return *this;
	}

  EngineMasterModel::Builder EngineMasterModel::Builder::setMortonPixels(std::shared_ptr<std::vector<Pixel>> mortonPixels) {
		this->mortonPixels = mortonPixels;
		return *this;
	}

	std::shared_ptr<EngineMasterModel> EngineMasterModel::Builder::build() {
		return std::make_shared<EngineMasterModel>(this->engineDevice, this->objects, this->objectBvhNodes, 
			this->primitives, this->primitiveBvhNodes, this->vertices, this->triangleLights, 
			this->triangleLightBvhNodes, this->materials, this->transformation, this->mortonPixels);
	}

	EngineMasterModel::EngineMasterModel(EngineDevice &device, std::shared_ptr<std::vector<Object>> objects, std::shared_ptr<std::vector<BvhNode>> objectBvhNodes,
		std::shared_ptr<std::vector<Primitive>> primitives, std::shared_ptr<std::vector<BvhNode>> primitiveBvhNodes, 
		std::shared_ptr<std::vector<RayTraceVertex>> vertices, std::shared_ptr<std::vector<TriangleLight>> triangleLights, 
		std::shared_ptr<std::vector<BvhNode>> triangleLightBvhNodes, std::shared_ptr<std::vector<Material>> materials, 
		std::shared_ptr<std::vector<Transformation>> transformation, std::shared_ptr<std::vector<Pixel>> mortonPixels) 
	{
		auto minOffsetAlign = device.getProperties().limits.minStorageBufferOffsetAlignment;

		this->objectBufferSize = EngineBuffer::getSizeAfterAlignment(sizeof(Object) * objects->size(), minOffsetAlign);
		this->objectBvhBufferSize = EngineBuffer::getSizeAfterAlignment(sizeof(BvhNode) * objectBvhNodes->size(), minOffsetAlign);
		this->primitiveBufferSize = EngineBuffer::getSizeAfterAlignment(sizeof(Primitive) * primitives->size(), minOffsetAlign);
		this->primitiveBvhBufferSize = EngineBuffer::getSizeAfterAlignment(sizeof(BvhNode) * primitiveBvhNodes->size(), minOffsetAlign);
		this->verticeBufferSize = EngineBuffer::getSizeAfterAlignment(sizeof(RayTraceVertex) * vertices->size(), minOffsetAlign);
		this->triangleLightBufferSize = EngineBuffer::getSizeAfterAlignment(sizeof(TriangleLight) * triangleLights->size(), minOffsetAlign);
		this->triangleLightBvhBufferSize = EngineBuffer::getSizeAfterAlignment(sizeof(BvhNode) * triangleLightBvhNodes->size(), minOffsetAlign);
		this->materialBufferSize = EngineBuffer::getSizeAfterAlignment(sizeof(Material) * materials->size(), minOffsetAlign);
		this->transformationBufferSize = EngineBuffer::getSizeAfterAlignment(sizeof(Transformation) * transformation->size(), minOffsetAlign);
		this->mortonPixelBufferSize = EngineBuffer::getSizeAfterAlignment(sizeof(Pixel) * mortonPixels->size(), minOffsetAlign);

		auto totalSize = this->objectBufferSize + this->objectBvhBufferSize + this->primitiveBufferSize + this->primitiveBvhBufferSize + 
			this->verticeBufferSize + this->triangleLightBufferSize + this->triangleLightBvhBufferSize + this->materialBufferSize + 
			this->transformationBufferSize + this->mortonPixelBufferSize;
		
		EngineBuffer stagingBuffer {
			device,
			static_cast<VkDeviceSize>(totalSize),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			minOffsetAlign
		};

		stagingBuffer.map(this->objectBufferSize, 0);
		stagingBuffer.writeToBuffer(objects->data(), this->objectBufferSize, 0);
		stagingBuffer.unmap();

		stagingBuffer.map(this->objectBvhBufferSize, this->objectBufferSize);
		stagingBuffer.writeToBuffer(objectBvhNodes->data(), this->objectBvhBufferSize, this->objectBufferSize);
		stagingBuffer.unmap();

		stagingBuffer.map(this->primitiveBufferSize, this->objectBufferSize + this->objectBvhBufferSize);
		stagingBuffer.writeToBuffer(primitives->data(), this->primitiveBufferSize, this->objectBufferSize + this->objectBvhBufferSize);
		stagingBuffer.unmap();

		stagingBuffer.map(this->primitiveBvhBufferSize, this->objectBufferSize + this->objectBvhBufferSize + 
			this->primitiveBufferSize);
		stagingBuffer.writeToBuffer(primitiveBvhNodes->data(), this->primitiveBvhBufferSize, this->objectBufferSize + this->objectBvhBufferSize + 
			this->primitiveBufferSize);
		stagingBuffer.unmap();

		stagingBuffer.map(this->verticeBufferSize, this->objectBufferSize + this->objectBvhBufferSize + 
			this->primitiveBufferSize + this->primitiveBvhBufferSize);
		stagingBuffer.writeToBuffer(vertices->data(), this->verticeBufferSize, this->objectBufferSize + this->objectBvhBufferSize + 
			this->primitiveBufferSize + this->primitiveBvhBufferSize);
		stagingBuffer.unmap();

		stagingBuffer.map(this->triangleLightBufferSize, this->objectBufferSize + this->objectBvhBufferSize + 
			this->primitiveBufferSize + this->primitiveBvhBufferSize + this->verticeBufferSize);
		stagingBuffer.writeToBuffer(triangleLights->data(), this->triangleLightBufferSize, this->objectBufferSize + this->objectBvhBufferSize + 
			this->primitiveBufferSize + this->primitiveBvhBufferSize + this->verticeBufferSize);
		stagingBuffer.unmap();

		stagingBuffer.map(this->triangleLightBvhBufferSize, this->objectBufferSize + this->objectBvhBufferSize + 
			this->primitiveBufferSize + this->primitiveBvhBufferSize + this->verticeBufferSize + this->triangleLightBufferSize);
		stagingBuffer.writeToBuffer(triangleLightBvhNodes->data(), this->triangleLightBvhBufferSize, this->objectBufferSize + this->objectBvhBufferSize + 
			this->primitiveBufferSize + this->primitiveBvhBufferSize + this->verticeBufferSize + this->triangleLightBufferSize);
		stagingBuffer.unmap();

		stagingBuffer.map(this->materialBufferSize, this->objectBufferSize + this->objectBvhBufferSize + 
			this->primitiveBufferSize + this->primitiveBvhBufferSize + this->verticeBufferSize + this->triangleLightBufferSize + 
			this->triangleLightBvhBufferSize);
		stagingBuffer.writeToBuffer(materials->data(), this->materialBufferSize, this->objectBufferSize + this->objectBvhBufferSize + 
			this->primitiveBufferSize + this->primitiveBvhBufferSize + this->verticeBufferSize + this->triangleLightBufferSize + 
			this->triangleLightBvhBufferSize);
		stagingBuffer.unmap();

		stagingBuffer.map(this->transformationBufferSize, this->objectBufferSize + this->objectBvhBufferSize + 
			this->primitiveBufferSize + this->primitiveBvhBufferSize + this->verticeBufferSize + this->triangleLightBufferSize + 
			this->triangleLightBvhBufferSize + this->materialBufferSize);
		stagingBuffer.writeToBuffer(transformation->data(), this->transformationBufferSize, this->objectBufferSize + this->objectBvhBufferSize + 
			this->primitiveBufferSize + this->primitiveBvhBufferSize + this->verticeBufferSize + this->triangleLightBufferSize + 
			this->triangleLightBvhBufferSize + this->materialBufferSize);
		stagingBuffer.unmap();

		stagingBuffer.map(this->mortonPixelBufferSize, this->objectBufferSize + this->objectBvhBufferSize + 
			this->primitiveBufferSize + this->primitiveBvhBufferSize + this->verticeBufferSize + this->triangleLightBufferSize + 
			this->triangleLightBvhBufferSize + this->materialBufferSize + this->transformationBufferSize);
		stagingBuffer.writeToBuffer(mortonPixels->data(), this->mortonPixelBufferSize, this->objectBufferSize + this->objectBvhBufferSize + 
			this->primitiveBufferSize + this->primitiveBvhBufferSize + this->verticeBufferSize + this->triangleLightBufferSize + 
			this->triangleLightBvhBufferSize + this->materialBufferSize + this->transformationBufferSize);
		stagingBuffer.unmap();

		this->buffer = std::make_shared<EngineBuffer>(
			device,
			static_cast<VkDeviceSize>(totalSize),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			minOffsetAlign
		);

		this->buffer->copyBuffer(stagingBuffer.getBuffer(), static_cast<VkDeviceSize>(totalSize));
	}
} // namespace nugiEngine

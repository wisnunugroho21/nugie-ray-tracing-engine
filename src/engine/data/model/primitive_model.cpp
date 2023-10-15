#include "primitive_model.hpp"

#include <cstring>
#include <iostream>

namespace NugieApp {
	PrimitiveModel::PrimitiveModel(NugieVulkan::Device* device) : device{device} {
		this->primitives = std::vector<Primitive>();
		this->bvhNodes = std::vector<BvhNode>();
	}

	void PrimitiveModel::addPrimitive(std::vector<Primitive> curPrimitives, std::vector<RayTraceVertex> vertices) {
		auto curBvhNodes = this->createBvhData(curPrimitives, vertices);

		for (int i = 0; i < curBvhNodes.size(); i++) {
			this->bvhNodes.emplace_back(curBvhNodes.at(i));
		}

		for (int i = 0; i < curPrimitives.size(); i++) {
			this->primitives.emplace_back(curPrimitives.at(i));
		}
	}

	std::vector<BvhNode> PrimitiveModel::createBvhData(std::vector<Primitive> primitives, std::vector<RayTraceVertex> vertices) {
		std::vector<BoundBox*> boundBoxes;
		for (uint32_t i = 0; i < primitives.size(); i++) {
			boundBoxes.push_back(new PrimitiveBoundBox(i + 1, &primitives.at(i), vertices));
		}

		auto bvhNodes = createBvh(boundBoxes);

		for (auto &&boundBox : boundBoxes) {
			delete boundBox;
		}

		return bvhNodes;
	}

	void PrimitiveModel::createBuffers(NugieVulkan::CommandBuffer *commandBuffer) {
		auto bufferSize = static_cast<VkDeviceSize>(sizeof(Primitive));
		auto instanceCount = static_cast<uint32_t>(this->primitives.size());

		NugieVulkan::Buffer primitiveStagingBuffer {
			this->device,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		primitiveStagingBuffer.map();
		primitiveStagingBuffer.writeToBuffer(this->primitives.data());

		this->primitiveBuffer = std::make_shared<NugieVulkan::Buffer>(
			this->device,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->primitiveBuffer->copyFromAnotherBuffer(&primitiveStagingBuffer, commandBuffer);

		// -------------------------------------------------

		bufferSize = static_cast<VkDeviceSize>(sizeof(BvhNode));
		instanceCount = static_cast<uint32_t>(bvhNodes.size());

		NugieVulkan::Buffer bvhStagingBuffer {
			this->device,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		bvhStagingBuffer.map();
		bvhStagingBuffer.writeToBuffer(this->bvhNodes.data());

		this->bvhBuffer = std::make_shared<NugieVulkan::Buffer>(
			this->device,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->bvhBuffer->copyFromAnotherBuffer(&bvhStagingBuffer, commandBuffer);
	}

	/* std::vector<Primitive>> PrimitiveModel::createPrimitivesFromFile(NugieVulkan::Device* device, NugieVulkan::CommandBuffer *commandBuffer, const std::string &filePath, uint32_t materialIndex) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		auto primitives = std::make_shared<std::vector<Primitive>>();

		for (const auto &shape: shapes) {
			uint32_t numTriangle = static_cast<uint32_t>(shape.mesh.indices.size()) / 3;

			for (uint32_t i = 0; i < numTriangle; i++) {
				int vertexIndex0 = shape.mesh.indices[3 * i + 0].vertex_index;
				int vertexIndex1 = shape.mesh.indices[3 * i + 1].vertex_index;
				int vertexIndex2 = shape.mesh.indices[3 * i + 2].vertex_index;

				int textCoordIndex0 = shape.mesh.indices[3 * i + 0].texcoord_index;
				int textCoordIndex1 = shape.mesh.indices[3 * i + 1].texcoord_index;
				int textCoordIndex2 = shape.mesh.indices[3 * i + 2].texcoord_index;
				
				glm::vec3 point1 = {
					attrib.vertices[3 * vertexIndex0 + 0],
					attrib.vertices[3 * vertexIndex0 + 1],
					attrib.vertices[3 * vertexIndex0 + 2]
				};

				glm::vec3 point2 = {
					attrib.vertices[3 * vertexIndex1 + 0],
					attrib.vertices[3 * vertexIndex1 + 1],
					attrib.vertices[3 * vertexIndex1 + 2]
				};

				glm::vec3 point3 = {
					attrib.vertices[3 * vertexIndex2 + 0],
					attrib.vertices[3 * vertexIndex2 + 1],
					attrib.vertices[3 * vertexIndex2 + 2]
				};

				glm::vec3 texel0 = {
					attrib.texcoords[2 * textCoordIndex0 + 0],
					attrib.texcoords[2 * textCoordIndex1 + 0],
					attrib.texcoords[2 * textCoordIndex2 + 0]
				};

				glm::vec3 texel1 = {
					attrib.texcoords[2 * textCoordIndex0 + 1],
					attrib.texcoords[2 * textCoordIndex1 + 1],
					attrib.texcoords[2 * textCoordIndex2 + 1]
				};

				primitives->emplace_back(Primitive{ Triangle{point1, point2, point3}, TextureCoordinate{texel0, texel1}, materialIndex });
			}
		}

		return primitives;
	} */ 
} // namespace NugieApp


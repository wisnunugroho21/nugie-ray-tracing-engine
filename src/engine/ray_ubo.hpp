#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace nugiEngine {
  struct RayTraceVertex {
    glm::vec4 position;
    glm::vec4 textCoord;

    bool operator == (const RayTraceVertex &other) const {
			return this->position == other.position && this->textCoord == other.textCoord;
		}
  };

  struct Primitive {
    /* alignas(16) glm::uvec3 indices;
    uint32_t materialIndex; */
    glm::uvec4 primitiveMaterialIndex;
  };

  struct Object {
    /* uint32_t firstBvhIndex = 0u;
    uint32_t firstPrimitiveIndex = 0u;
    uint32_t transformIndex = 0u; */
    glm::uvec4 bvhPrimitiveTransformIndex;
  };

  struct BvhNode {
    /* uint32_t leftNode = 0u;
    uint32_t rightNode = 0u;
    uint32_t leftObjIndex = 0u;
    uint32_t rightObjIndex = 0u; 
    
    alignas(16) glm::vec3 maximum;
    alignas(16) glm::vec3 minimum;*/
    glm::uvec4 nodeObjIndex{0u};
    glm::vec4 maximum;
    glm::vec4 minimum;
  };

  struct Material {
    glm::vec4 baseColor;
    glm::vec4 baseNormal;

    /* float metallicness;
    float roughness;
    float fresnelReflect; */
    glm::vec4 params{0.0f};

    /* uint32_t colorTextureIndex;
    uint32_t normalTextureIndex; */
    glm::uvec4 textureIndex{0u};
  };

  struct Transformation {
    glm::mat4 pointMatrix{1.0f};
		glm::mat4 dirMatrix{1.0f};
    glm::mat4 pointInverseMatrix{1.0f};
    glm::mat4 dirInverseMatrix{1.0f};
    glm::mat4 normalMatrix{1.0f};
  };

  struct SunLight {
    alignas(16) glm::vec3 direction;
    alignas(16) glm::vec3 color;
  };

  struct TriangleLight {
    glm::uvec4 indices;
    glm::vec4 color;
  };

  struct Pixel {
    /* uint32_t xCoord;
    uint32_t yCoord; */
    glm::uvec4 coord;
  };

  struct RayTraceUbo {
    alignas(16) glm::vec3 origin;
    alignas(16) glm::vec3 horizontal;
    alignas(16) glm::vec3 vertical;
    alignas(16) glm::vec3 lowerLeftCorner;
    uint32_t numLights = 0u;
    SunLight sunLight;
  };

  struct RayTracePushConstant {
    uint32_t randomSeed = 0u;
  };
}
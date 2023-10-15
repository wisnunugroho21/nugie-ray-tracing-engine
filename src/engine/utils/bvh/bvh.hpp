#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../sort/sort.hpp"
#include "../transform/transform.hpp"
#include "../../ray_ubo.hpp"

#include <vector>
#include <memory>
#include <algorithm>
#include <stack>

#define SPLIT_NUMBER 12

namespace NugieApp {
  const glm::vec3 eps(0.01f);

  // Axis-aligned bounding box.
  struct Aabb {
    glm::vec3 min = glm::vec3{FLT_MAX};
    glm::vec3 max = glm::vec3{FLT_MIN};

    float area();

    uint32_t longestAxis();
    uint32_t randomAxis();
  };

  // Utility structure to keep track of the initial triangle index in the triangles array while sorting.
  struct BoundBox {
    uint32_t index;

    BoundBox(uint32_t i) : index{i} {}

    virtual Aabb boundingBox() = 0;

    virtual glm::vec3 getOriginalMin() { return glm::vec3(0.0f); }
    virtual glm::vec3 getOriginalMax() { return glm::vec3(0.0f); }
  };

  struct PrimitiveBoundBox : BoundBox {
    Primitive* primitive;
    std::vector<RayTraceVertex> vertices;

    PrimitiveBoundBox(uint32_t i, Primitive *p, std::vector<RayTraceVertex> v) : BoundBox(i), primitive{p}, vertices{v} {}

    Aabb boundingBox();
  };

  struct ObjectBoundBox : BoundBox {
    Object* object;
    TransformComponent* transformation;
    std::vector<Primitive> primitives;
    std::vector<RayTraceVertex> vertices;

    glm::vec3 originalMin{};
    glm::vec3 originalMax{};

    ObjectBoundBox(uint32_t i, Object *o, std::vector<Primitive> p, TransformComponent* t, std::vector<RayTraceVertex> v);

    glm::vec3 getOriginalMin() { return this->originalMin; }
    glm::vec3 getOriginalMax() { return this->originalMax; }
    
    Aabb boundingBox();

    private:
      float findMax(uint32_t index);
      float findMin(uint32_t index);
  };

  struct TriangleLightBoundBox : BoundBox {
    TriangleLight* light;
    std::vector<RayTraceVertex> vertices;

    TriangleLightBoundBox(int i, TriangleLight* l, std::vector<RayTraceVertex> v) : BoundBox(i), light{l}, vertices{v} {}

    Aabb boundingBox();
  };

  struct BvhBinSAH {
    Aabb box;
    uint32_t objectCount;
  };

  // Intermediate BvhNode structure needed for constructing Bvh.
  struct BvhItemBuild {
    Aabb box;
    uint32_t index = 0; // index refers to the index in the final array of nodes. Used for sorting a flattened Bvh.
    uint32_t leftNodeIndex = 0;
    uint32_t rightNodeIndex = 0;
    std::vector<BoundBox*> objects;

    BvhNode getGpuModel();
  };

  bool nodeCompare(BvhItemBuild& a, BvhItemBuild& b);
  Aabb surroundingBox(Aabb box0, Aabb box1);
  Aabb objectListBoundingBox(std::vector<BoundBox*> objects);
  bool boxCompare(BoundBox* a, BoundBox* b, int axis);
  bool boxXCompare(BoundBox* a, BoundBox* b);
  bool boxYCompare(BoundBox* a, BoundBox* b);
  bool boxZCompare(BoundBox* a, BoundBox* b);
  float findPrimitiveSplitPosition(BvhItemBuild node, int axis, float length);

  // Since GPU can't deal with tree structures we need to create a flattened BVH.
  // Stack is used instead of a tree.
  std::vector<BvhNode> createBvh(std::vector<BoundBox*> boundedBoxes);

}// namespace NugieApp 

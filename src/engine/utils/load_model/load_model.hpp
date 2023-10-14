#include "../../ray_ubo.hpp"

#include <string>
#include <memory>
#include <vector>

namespace NugieApp
{
  struct LoadedModel
  {
    std::vector<Primitive> primitives;
    std::vector<RayTraceVertex> vertices;
  };

  LoadedModel loadModelFromFile(const std::string &filePath, uint32_t materialIndex, uint32_t offsetIndex);
}
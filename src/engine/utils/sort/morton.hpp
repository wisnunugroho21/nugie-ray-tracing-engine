#pragma once

#include "../../ray_ubo.hpp"

#include <vector>
#include <memory>

namespace nugiEngine {
  uint32_t part1By1(uint32_t x);
  uint32_t part1By2(uint32_t x);

  uint32_t encodeMorton(uint32_t x, uint32_t y);
  uint32_t encodeMorton(uint32_t x, uint32_t y, uint32_t z);

  bool mortonComparator(Pixel a, Pixel b);

  std::shared_ptr<std::vector<Pixel>> sortPixelByMorton(uint32_t width, uint32_t height);
  
} // namespace name


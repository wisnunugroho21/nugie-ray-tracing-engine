#pragma once

#include <glm/glm.hpp>

namespace nugiEngine {
  struct CameraRay {
    alignas(16) glm::vec3 origin{0.0f};
    alignas(16) glm::vec3 horizontal{0.0f};
    alignas(16) glm::vec3 vertical{0.0f};
    alignas(16) glm::vec3 lowerLeftCorner{0.0f};
  };
  
  class EngineCamera {
    public:
      EngineCamera(uint32_t width, uint32_t height);

      void setViewDirection(glm::vec3 position, glm::vec3 direction, float vfov, glm::vec3 up = glm::vec3{0.0f, -1.0f, 0.0f});
      void setViewTarget(glm::vec3 position, glm::vec3 target, float vfov, glm::vec3 up = glm::vec3{0.0f, -1.0f, 0.0f});

      CameraRay getCameraRay() const { return this->cameraRay; }

    private:
      CameraRay cameraRay;
      uint32_t width, height;
  };
} // namespace nugiEngine


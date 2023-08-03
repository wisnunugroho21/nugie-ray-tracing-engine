#pragma once

#include "../../utils/camera/camera.hpp"
#include "../../../vulkan/window/window.hpp"

namespace nugiEngine {
  class EngineMouseController
  {
  public:
    struct KeyMappings {
      int leftButton = GLFW_MOUSE_BUTTON_LEFT;
      int rightButton = GLFW_MOUSE_BUTTON_RIGHT;
    };

    CameraRay rotateInPlaceXZ(GLFWwindow* window, float dt, CameraRay cameraRay, bool* isPressed);

    KeyMappings keymaps{};
    float lookSpeed{1.0f};

    double lastDragged_x = 0;
    double lastDragged_y = 0;
  };
  
} // namespace nugiEngine
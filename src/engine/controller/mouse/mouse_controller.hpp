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

    CameraTransformation rotateInPlaceXZ(GLFWwindow* window, float dt, CameraTransformation cameraTransformation, bool* isPressed);

    KeyMappings keymaps{};
    float lookSpeed{20.0f};

    double lastDragged_x = 0;
    double lastDragged_y = 0;
    bool isFirstPressed = true;
  };
  
} // namespace nugiEngine
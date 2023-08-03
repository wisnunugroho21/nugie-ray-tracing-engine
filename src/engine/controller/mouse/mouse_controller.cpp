#include "mouse_controller.hpp"

namespace nugiEngine {

  CameraRay EngineMouseController::rotateInPlaceXZ(GLFWwindow* window, float dt, CameraRay cameraRay, bool* isPressed) {
    if (glfwGetMouseButton(window, this->keymaps.rightButton) == GLFW_PRESS) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

      double curDragged_x = 0;
      double curDragged_y = 0;

      glfwGetCursorPos(window, &curDragged_x, &curDragged_y);
      glm::vec2 rotate{ (curDragged_y - this->lastDragged_y), ((curDragged_x - this->lastDragged_x) * -1) };

      this->lastDragged_x = curDragged_x;
      this->lastDragged_y = curDragged_y;

      if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
        glm::vec3 a = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::vec3 forwardDir = glm::normalize(cameraRay.direction);
        glm::vec3 rightDir = glm::normalize(glm::cross(forwardDir, a));
        glm::vec3 upDir = glm::cross(forwardDir, rightDir);

        float phi = glm::radians(curDragged_y - this->lastDragged_y);
        float theta = glm::radians(curDragged_x - this->lastDragged_x);

        float X = glm::sin(theta) * glm::cos(phi);
        float Y = glm::sin(theta) * glm::sin(phi);
        float Z = glm::cos(theta);

        cameraRay.direction = forwardDir * Z + upDir * Y + rightDir * X;
      }
    } else if (glfwGetMouseButton(window, this->keymaps.rightButton) == GLFW_RELEASE) {
      this->lastDragged_x = 0;
      this->lastDragged_y = 0;

      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    return cameraRay;
  }
} // namespace nugiEngine


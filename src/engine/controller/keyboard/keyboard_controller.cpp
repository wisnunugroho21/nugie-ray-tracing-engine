#include "keyboard_controller.hpp"

namespace nugiEngine {

  CameraRay EngineKeyboardController::moveInPlaceXZ(GLFWwindow* window, float dt, CameraRay cameraRay, bool* isPressed) {
    glm::vec3 a = glm::abs(glm::normalize(cameraRay.direction).x) > 0.9 ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);

    glm::vec3 forwardDir = glm::normalize(cameraRay.direction);
    glm::vec3 rightDir = glm::normalize(glm::cross(forwardDir, a));
    glm::vec3 upDir = glm::cross(forwardDir, rightDir);

    glm::vec3 moveDir{0.0f};
    *isPressed = false;

    if (glfwGetKey(window, keymaps.moveForward) == GLFW_PRESS) {
      moveDir += forwardDir;
      *isPressed = true;
    }

    if (glfwGetKey(window, keymaps.moveBackward) == GLFW_PRESS) {
      moveDir -= forwardDir;
      *isPressed = true;
    }

    if (glfwGetKey(window, keymaps.moveRight) == GLFW_PRESS) {
      moveDir += rightDir;
      *isPressed = true;
    }

    if (glfwGetKey(window, keymaps.moveLeft) == GLFW_PRESS) {
      moveDir -= rightDir;
      *isPressed = true;
    }

    if (glfwGetKey(window, keymaps.moveUp) == GLFW_PRESS) {
      moveDir += upDir;
      *isPressed = true;
    }

    if (glfwGetKey(window, keymaps.moveDown) == GLFW_PRESS) {
      moveDir -= upDir;
      *isPressed = true;
    }

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
      cameraRay.origin += moveSpeed * dt * glm::normalize(moveDir);
    }

    return cameraRay;
  }
  
} // namespace nugiEngin 


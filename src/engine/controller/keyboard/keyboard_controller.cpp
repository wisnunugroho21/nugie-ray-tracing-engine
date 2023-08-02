#include "keyboard_controller.hpp"

namespace nugiEngine {

  CameraRay EngineKeyboardController::moveInPlaceXZ(GLFWwindow* window, float dt, CameraRay cameraRay) {
    glm::vec3 forwardDir = cameraRay.direction;
    glm::vec3 rightDir = glm::vec3{forwardDir.z, 0.0f, -1.0f * forwardDir.x};
    glm::vec3 upDir = glm::vec3{0.0f, -1.0f * forwardDir.y, 0.0};

    glm::vec3 moveDir{0.0f};

    if (glfwGetKey(window, keymaps.moveForward) == GLFW_PRESS) moveDir += forwardDir;
    if (glfwGetKey(window, keymaps.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
    if (glfwGetKey(window, keymaps.moveRight) == GLFW_PRESS) moveDir += rightDir;
    if (glfwGetKey(window, keymaps.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
    if (glfwGetKey(window, keymaps.moveUp) == GLFW_PRESS) moveDir += upDir;
    if (glfwGetKey(window, keymaps.moveDown) == GLFW_PRESS) moveDir -= upDir;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
      cameraRay.origin += moveSpeed * dt * glm::normalize(moveDir);
    }

    return cameraRay;
  }
  
} // namespace nugiEngin 


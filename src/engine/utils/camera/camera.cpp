#include "camera.hpp"

namespace nugiEngine {
  EngineCamera::EngineCamera(uint32_t width, uint32_t height) : width{width}, height{height} {}

  void EngineCamera::setViewDirection(glm::vec3 position, glm::vec3 direction, float vfov, glm::vec3 vup) {
		float aspectRatio = static_cast<float>(this->width) / static_cast<float>(this->height);

		float theta = glm::radians(vfov);
		float h = glm::tan(theta / 2.0f);
		float viewportHeight = 2.0f * h;
		float viewportWidth = aspectRatio * viewportHeight;

		glm::vec3 w = glm::normalize(position - direction);
		glm::vec3 u = glm::normalize(glm::cross(vup, w));
		glm::vec3 v = glm::cross(w, u);

		this->cameraRay.origin = position;
		this->cameraRay.direction = direction;
		this->cameraRay.horizontal = glm::vec3(viewportWidth * u);
		this->cameraRay.vertical = glm::vec3(viewportHeight * v);
		this->cameraRay.lowerLeftCorner = glm::vec3(position - viewportWidth * u / 2.0f + viewportHeight * v / 2.0f - w);
  }

  void EngineCamera::setViewTarget(glm::vec3 position, glm::vec3 target, float vfov, glm::vec3 up) {
    this->setViewDirection(position, target - position, vfov, up);
  }

	void EngineCamera::updateCameraRay(CameraRay cameraRay) {
		this->cameraRay = cameraRay;
	}
} // namespace nugiEngine


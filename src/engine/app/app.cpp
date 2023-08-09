#include "app.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <string>
#include <chrono>
#include <iostream>
#include <cstdlib>

#include <thread>

namespace nugiEngine {
	EngineApp::EngineApp() {
		this->renderer = std::make_unique<EngineHybridRenderer>(this->window, this->device);
		this->keyboardController = std::make_shared<EngineKeyboardController>();
		this->mouseController = std::make_shared<EngineMouseController>();

		this->loadCornellBox();
		this->loadQuadModels();
		this->recreateSubRendererAndSubsystem();
	}

	EngineApp::~EngineApp() {}

	void EngineApp::renderLoop() {
		while (this->isRendering) {
			auto oldTime = std::chrono::high_resolution_clock::now();

			if (this->renderer->acquireFrame()) {
				uint32_t frameIndex = this->renderer->getFrameIndex();
				uint32_t imageIndex = this->renderer->getImageIndex();

				auto commandBuffer = this->renderer->beginCommand();
				this->rayTraceImage->prepareFrame(commandBuffer, frameIndex);

				this->traceRayRender->render(commandBuffer, this->rayTraceDescSet->getDescriptorSets(frameIndex), this->randomSeed);

				this->rayTraceImage->transferFrame(commandBuffer, frameIndex);
				this->accumulateImages->prepareFrame(commandBuffer, frameIndex);
				
				this->swapChainSubRenderer->beginRenderPass(commandBuffer, imageIndex);
				this->samplingRayRender->render(commandBuffer, this->samplingDescSet->getDescriptorSets(frameIndex), this->quadModels, this->randomSeed);
				this->swapChainSubRenderer->endRenderPass(commandBuffer);

				this->rayTraceImage->finishFrame(commandBuffer, frameIndex);
				this->accumulateImages->finishFrame(commandBuffer, frameIndex);

				this->renderer->endCommand(commandBuffer);
				this->renderer->submitCommand(commandBuffer);

				if (!this->renderer->presentFrame()) {
					this->recreateSubRendererAndSubsystem();
					this->randomSeed = 0;

					continue;
				}				

				if (frameIndex + 1 == EngineDevice::MAX_FRAMES_IN_FLIGHT) {
					if (this->isCameraMoved) {
						this->isCameraMoved = false;
						this->randomSeed = 0;

						for (uint32_t i = 0; i < EngineDevice::MAX_FRAMES_IN_FLIGHT; i++) {
							this->globalUniforms->writeGlobalData(i, this->globalUbo);
						}
					} else {
						this->randomSeed++;
					}
				}				
			}

			auto newTime = std::chrono::high_resolution_clock::now();
			this->frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - oldTime).count();
			oldTime = newTime;
		}
	}

	void EngineApp::run() {
		auto oldTime = std::chrono::high_resolution_clock::now();
		uint32_t t = 0;

		this->globalUbo = this->initUbo(this->renderer->getSwapChain()->width(), this->renderer->getSwapChain()->height());
		for (uint32_t i = 0; i < EngineDevice::MAX_FRAMES_IN_FLIGHT; i++) {
			this->globalUniforms->writeGlobalData(i, this->globalUbo);
		}

		std::thread renderThread(&EngineApp::renderLoop, std::ref(*this));

		while (!this->window.shouldClose()) {
			this->window.pollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - oldTime).count();

			CameraTransformation cameraTransformation = this->camera->getCameraTransformation();

			bool isMousePressed = false;
			bool isKeyboardPressed = false;

			cameraTransformation = this->mouseController->rotateInPlaceXZ(this->window.getWindow(), deltaTime, cameraTransformation, &isMousePressed);
			cameraTransformation = this->keyboardController->moveInPlaceXZ(this->window.getWindow(), deltaTime, cameraTransformation, &isKeyboardPressed);

			if (isMousePressed || isKeyboardPressed) {
				this->camera->setViewTransformation(cameraTransformation, 40.0f);
				CameraRay cameraRay = this->camera->getCameraRay();

				this->globalUbo.origin = cameraRay.origin;
				this->globalUbo.horizontal = cameraRay.horizontal;
				this->globalUbo.vertical = cameraRay.vertical;
				this->globalUbo.lowerLeftCorner = cameraRay.lowerLeftCorner;
				
				this->isCameraMoved = true;
			}

			if (t == 10) {
				std::string appTitle = std::string(APP_TITLE) + std::string(" | FPS: ") + std::to_string((1.0f / this->frameTime));
				glfwSetWindowTitle(this->window.getWindow(), appTitle.c_str());

				t = 0;
			} else {
				t++;
			}

			oldTime = newTime;
		}

		this->isRendering = false;
		renderThread.join();

		vkDeviceWaitIdle(this->device.getLogicalDevice());
	}

	void EngineApp::loadCornellBox() {
		this->primitiveModel = std::make_unique<EnginePrimitiveModel>(this->device);

		auto objects = std::make_shared<std::vector<Object>>();
		auto materials = std::make_shared<std::vector<Material>>();
		auto triangleLights = std::make_shared<std::vector<TriangleLight>>();
		auto vertices = std::make_shared<std::vector<RayTraceVertex>>();

		std::vector<std::shared_ptr<BoundBox>> boundBoxes{};
		std::vector<std::shared_ptr<TransformComponent>> transforms{};

		// ----------------------------------------------------------------------------

		// kanan
		transforms.emplace_back(std::make_shared<TransformComponent>(TransformComponent{ glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f) }));
		uint32_t transformIndex = static_cast<uint32_t>(transforms.size() - 1);
		
		objects->emplace_back(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		uint32_t objectIndex = static_cast<uint32_t>(objects->size() - 1);

		vertices->emplace_back(RayTraceVertex{ glm::vec3{555.0f, 0.0f, 0.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{555.0f, 555.0f, 0.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{555.0f, 555.0f, 555.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{555.0f, 0.0f, 555.0f}, glm::vec3{0.0f} });

		auto rightWallPrimitives = std::make_shared<std::vector<Primitive>>();
		rightWallPrimitives->emplace_back(Primitive{ glm::uvec3(0u, 1u, 2u), 1u });
		rightWallPrimitives->emplace_back(Primitive{ glm::uvec3(2u, 3u, 0u), 1u });

		this->primitiveModel->addPrimitive(rightWallPrimitives, vertices);

		boundBoxes.emplace_back(std::make_shared<ObjectBoundBox>(ObjectBoundBox{ static_cast<uint32_t>(boundBoxes.size() + 1), (*objects)[objectIndex], rightWallPrimitives, transforms[transformIndex], vertices }));
		uint32_t boundBoxIndex = static_cast<uint32_t>(boundBoxes.size() - 1);

		transforms[transformIndex]->objectMaximum = boundBoxes[boundBoxIndex]->getOriginalMax();
		transforms[transformIndex]->objectMinimum = boundBoxes[boundBoxIndex]->getOriginalMin();

		// ----------------------------------------------------------------------------
		
		// kiri
		transforms.emplace_back(std::make_shared<TransformComponent>(TransformComponent{ glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f) }));
		transformIndex = static_cast<uint32_t>(transforms.size() - 1);

		objects->emplace_back(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		objectIndex = static_cast<uint32_t>(objects->size() - 1);

		vertices->emplace_back(RayTraceVertex{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{0.0f, 555.0f, 0.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{0.0f, 555.0f, 555.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{0.0f, 0.0f, 555.0f}, glm::vec3{0.0f} });
		
		auto leftWallPrimitives = std::make_shared<std::vector<Primitive>>();
		leftWallPrimitives->emplace_back(Primitive{ glm::uvec3(4u, 5u, 6u), 2u });
		leftWallPrimitives->emplace_back(Primitive{ glm::uvec3(6u, 7u, 4u), 2u });
		
		this->primitiveModel->addPrimitive(leftWallPrimitives, vertices);
		
		boundBoxes.emplace_back(std::make_shared<ObjectBoundBox>(ObjectBoundBox{ static_cast<uint32_t>(boundBoxes.size() + 1), (*objects)[objectIndex], leftWallPrimitives, transforms[transformIndex], vertices }));
		boundBoxIndex = static_cast<uint32_t>(boundBoxes.size() - 1);

		transforms[transformIndex]->objectMaximum = boundBoxes[boundBoxIndex]->getOriginalMax();
		transforms[transformIndex]->objectMinimum = boundBoxes[boundBoxIndex]->getOriginalMin();

		// ----------------------------------------------------------------------------
		
		// bawah
		transforms.emplace_back(std::make_shared<TransformComponent>(TransformComponent{ glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f) }));
		transformIndex = static_cast<uint32_t>(transforms.size() - 1);

		objects->emplace_back(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		objectIndex = static_cast<uint32_t>(objects->size() - 1);

		auto bottomWallPrimitives = std::make_shared<std::vector<Primitive>>();
		bottomWallPrimitives->emplace_back(Primitive{ glm::uvec3(4u, 0u, 3u), 0u });
		bottomWallPrimitives->emplace_back(Primitive{ glm::uvec3(3u, 7u, 4u), 0u });
		
		this->primitiveModel->addPrimitive(bottomWallPrimitives, vertices);
		
		boundBoxes.emplace_back(std::make_shared<ObjectBoundBox>(ObjectBoundBox{ static_cast<uint32_t>(boundBoxes.size() + 1), (*objects)[objectIndex], bottomWallPrimitives, transforms[transformIndex], vertices }));
		boundBoxIndex = static_cast<uint32_t>(boundBoxes.size() - 1);

		transforms[transformIndex]->objectMaximum = boundBoxes[boundBoxIndex]->getOriginalMax();
		transforms[transformIndex]->objectMinimum = boundBoxes[boundBoxIndex]->getOriginalMin();

		// ----------------------------------------------------------------------------
		
		// atas
		transforms.emplace_back(std::make_shared<TransformComponent>(TransformComponent{ glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f) }));
		transformIndex = static_cast<uint32_t>(transforms.size() - 1);

		objects->emplace_back(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		objectIndex = static_cast<uint32_t>(objects->size() - 1);

		auto topWallPrimitives = std::make_shared<std::vector<Primitive>>();
		topWallPrimitives->emplace_back(Primitive{ glm::uvec3(5u, 1u, 2u), 0u });
		topWallPrimitives->emplace_back(Primitive{ glm::uvec3(2u, 6u, 5u), 0u });

		this->primitiveModel->addPrimitive(topWallPrimitives, vertices);

		boundBoxes.emplace_back(std::make_shared<ObjectBoundBox>(ObjectBoundBox{ static_cast<uint32_t>(boundBoxes.size() + 1), (*objects)[objectIndex], topWallPrimitives, transforms[transformIndex], vertices }));
		boundBoxIndex = static_cast<uint32_t>(boundBoxes.size() - 1);

		transforms[transformIndex]->objectMaximum = boundBoxes[boundBoxIndex]->getOriginalMax();
		transforms[transformIndex]->objectMinimum = boundBoxes[boundBoxIndex]->getOriginalMin();

		// ----------------------------------------------------------------------------
		
		// depan
		transforms.emplace_back(std::make_shared<TransformComponent>(TransformComponent{ glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f) }));
		transformIndex = static_cast<uint32_t>(transforms.size() - 1);

		objects->emplace_back(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		objectIndex = static_cast<uint32_t>(objects->size() - 1);

		auto frontWallPrimitives = std::make_shared<std::vector<Primitive>>();
		frontWallPrimitives->emplace_back(Primitive{ glm::uvec3(7u, 6u, 2u), 0u });
		frontWallPrimitives->emplace_back(Primitive{ glm::uvec3(2u, 3u, 7u), 0u });

		this->primitiveModel->addPrimitive(frontWallPrimitives, vertices);

		boundBoxes.emplace_back(std::make_shared<ObjectBoundBox>(ObjectBoundBox{ static_cast<uint32_t>(boundBoxes.size() + 1), (*objects)[objectIndex], frontWallPrimitives, transforms[transformIndex], vertices }));
		boundBoxIndex = static_cast<uint32_t>(boundBoxes.size() - 1);

		transforms[transformIndex]->objectMaximum = boundBoxes[boundBoxIndex]->getOriginalMax();
		transforms[transformIndex]->objectMinimum = boundBoxes[boundBoxIndex]->getOriginalMin();

		// ----------------------------------------------------------------------------

		vertices->emplace_back(RayTraceVertex{ glm::vec3{213.0f, 554.0f, 227.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{343.0f, 554.0f, 227.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{343.0f, 554.0f, 332.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{213.0f, 554.0f, 332.0f}, glm::vec3{0.0f} });

		triangleLights->emplace_back(TriangleLight{ glm::uvec3(8u, 9u, 10u), glm::vec3(100.0f, 100.0f, 100.0f) });
		triangleLights->emplace_back(TriangleLight{ glm::uvec3(10u, 11u, 8u), glm::vec3(100.0f, 100.0f, 100.0f) });

		// ----------------------------------------------------------------------------

		materials->emplace_back(Material{ glm::vec3(0.73f, 0.73f, 0.73f), 0.0f, 0.1f, 0.5f, 0u, 0u });
		materials->emplace_back(Material{ glm::vec3(0.12f, 0.45f, 0.15f), 0.0f, 0.1f, 0.5f, 0u, 0u });
		materials->emplace_back(Material{ glm::vec3(0.65f, 0.05f, 0.05f), 0.0f, 0.1f, 0.5f, 0u, 0u });
		materials->emplace_back(Material{ glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.1f, 0.5f, 1u, 0u });

		// ----------------------------------------------------------------------------

		// Object
		transforms.emplace_back(std::make_shared<TransformComponent>(TransformComponent{ glm::vec3(275.0f, 200.0f, 250.0f), glm::vec3(200.0f), glm::vec3(0.0f, glm::radians(180.0f), 0.0f)}));
		transformIndex = static_cast<uint32_t>(transforms.size() - 1);

		objects->emplace_back(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		objectIndex = static_cast<uint32_t>(objects->size() - 1);

		auto loadedModel = loadModelFromFile("models/viking_room.obj", 3u, static_cast<uint32_t>(vertices->size()));
		for (auto &&vertex : *loadedModel.vertices) {
			vertices->emplace_back(vertex);
		}

		this->primitiveModel->addPrimitive(loadedModel.primitives, vertices);

		boundBoxes.emplace_back(std::make_shared<ObjectBoundBox>(ObjectBoundBox{ static_cast<uint32_t>(boundBoxes.size() + 1), (*objects)[objectIndex], loadedModel.primitives, transforms[transformIndex], vertices }));
		boundBoxIndex = static_cast<uint32_t>(boundBoxes.size() - 1);

		transforms[transformIndex]->objectMaximum = boundBoxes[boundBoxIndex]->getOriginalMax();
		transforms[transformIndex]->objectMinimum = boundBoxes[boundBoxIndex]->getOriginalMin();

		// ----------------------------------------------------------------------------

		this->objectModel = std::make_unique<EngineObjectModel>(this->device, objects, boundBoxes);
		this->materialModel = std::make_unique<EngineMaterialModel>(this->device, materials);
		this->lightModel = std::make_unique<EngineLightModel>(this->device, triangleLights, vertices);
		this->transformationModel = std::make_unique<EngineTransformationModel>(this->device, transforms);
		this->rayTraceVertexModels = std::make_unique<EngineRayTraceVertexModel>(this->device, vertices);

		this->globalUniforms = std::make_unique<EngineGlobalUniform>(this->device);
		this->primitiveModel->createBuffers();

		this->colorTextures.emplace_back(std::make_unique<EngineTexture>(this->device, "textures/viking_room.png"));
		this->normalTextures.emplace_back(std::make_unique<EngineTexture>(this->device, "textures/viking_room.png"));

		this->numLights = static_cast<uint32_t>(triangleLights->size());
	}

	void EngineApp::loadSkyLight() {
		this->primitiveModel = std::make_unique<EnginePrimitiveModel>(this->device);

		auto objects = std::make_shared<std::vector<Object>>();
		auto materials = std::make_shared<std::vector<Material>>();
		auto triangleLights = std::make_shared<std::vector<TriangleLight>>();
		auto vertices = std::make_shared<std::vector<RayTraceVertex>>();

		std::vector<std::shared_ptr<BoundBox>> boundBoxes{};
		std::vector<std::shared_ptr<TransformComponent>> transforms{};

		// ----------------------------------------------------------------------------
		
		// bawah
		transforms.emplace_back(std::make_shared<TransformComponent>(TransformComponent{ glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f) }));
		uint32_t transformIndex = static_cast<uint32_t>(transforms.size() - 1);

		objects->emplace_back(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		uint32_t objectIndex = static_cast<uint32_t>(objects->size() - 1);

		vertices->emplace_back(RayTraceVertex{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{555.0f, 0.0f, 0.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{555.0f, 0.0f, 555.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{0.0f, 0.0f, 555.0f}, glm::vec3{0.0f} });

		auto bottomWallPrimitives = std::make_shared<std::vector<Primitive>>();
		bottomWallPrimitives->emplace_back(Primitive{ glm::uvec3(0u, 1u, 2u) });
		bottomWallPrimitives->emplace_back(Primitive{ glm::uvec3(2u, 3u, 0u) });
		
		this->primitiveModel->addPrimitive(bottomWallPrimitives, vertices);
		
		boundBoxes.emplace_back(std::make_shared<ObjectBoundBox>(ObjectBoundBox{ static_cast<uint32_t>(boundBoxes.size() + 1), (*objects)[objectIndex], bottomWallPrimitives, transforms[transformIndex], vertices }));
		uint32_t boundBoxIndex = static_cast<uint32_t>(boundBoxes.size() - 1);

		transforms[transformIndex]->objectMaximum = boundBoxes[boundBoxIndex]->getOriginalMax();
		transforms[transformIndex]->objectMinimum = boundBoxes[boundBoxIndex]->getOriginalMin();

		// ----------------------------------------------------------------------------

		materials->emplace_back(Material{ glm::vec3(0.73f, 0.73f, 0.73f), 0.0f, 0.1f, 0.5f, 0u, 0u });
		materials->emplace_back(Material{ glm::vec3(0.12f, 0.45f, 0.15f), 0.0f, 0.1f, 0.5f, 0u, 0u });
		materials->emplace_back(Material{ glm::vec3(0.65f, 0.05f, 0.05f), 0.0f, 0.1f, 0.5f, 0u, 0u });
		materials->emplace_back(Material{ glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.1f, 0.5f, 1u, 0u });

		// ----------------------------------------------------------------------------

		vertices->emplace_back(RayTraceVertex{ glm::vec3{21300.0f, 55400.0f, 22700.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{34300.0f, 55400.0f, 22700.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{34300.0f, 55400.0f, 33200.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{21300.0f, 55400.0f, 33200.0f}, glm::vec3{0.0f} });

		triangleLights->emplace_back(TriangleLight{ glm::uvec3(4u, 5u, 6u), glm::vec3(0.0f, 0.0f, 0.0f) });
		triangleLights->emplace_back(TriangleLight{ glm::uvec3(6u, 7u, 4u), glm::vec3(0.0f, 0.0f, 0.0f) });

		// ----------------------------------------------------------------------------

		// Object
		transforms.emplace_back(std::make_shared<TransformComponent>(TransformComponent{ glm::vec3(275.0f, 200.0f, 250.0f), glm::vec3(200.0f), glm::vec3(0.0f, glm::radians(180.0f), 0.0f)}));
		transformIndex = static_cast<uint32_t>(transforms.size() - 1);

		objects->emplace_back(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		objectIndex = static_cast<uint32_t>(objects->size() - 1);

		auto loadedModel = loadModelFromFile("models/viking_room.obj", 3u, static_cast<uint32_t>(vertices->size()));
		for (auto &&vertex : *loadedModel.vertices) {
			vertices->emplace_back(vertex);
		}

		this->primitiveModel->addPrimitive(loadedModel.primitives, vertices);

		boundBoxes.emplace_back(std::make_shared<ObjectBoundBox>(ObjectBoundBox{ static_cast<uint32_t>(boundBoxes.size() + 1), (*objects)[objectIndex], loadedModel.primitives, transforms[transformIndex], vertices }));
		boundBoxIndex = static_cast<uint32_t>(boundBoxes.size() - 1);

		transforms[transformIndex]->objectMaximum = boundBoxes[boundBoxIndex]->getOriginalMax();
		transforms[transformIndex]->objectMinimum = boundBoxes[boundBoxIndex]->getOriginalMin();

		// ----------------------------------------------------------------------------

		this->objectModel = std::make_unique<EngineObjectModel>(this->device, objects, boundBoxes);
		this->materialModel = std::make_unique<EngineMaterialModel>(this->device, materials);
		this->lightModel = std::make_unique<EngineLightModel>(this->device, triangleLights, vertices);
		this->transformationModel = std::make_unique<EngineTransformationModel>(this->device, transforms);
		this->rayTraceVertexModels = std::make_unique<EngineRayTraceVertexModel>(this->device, vertices);

		this->globalUniforms = std::make_unique<EngineGlobalUniform>(this->device);
		this->primitiveModel->createBuffers();

		this->colorTextures.emplace_back(std::make_unique<EngineTexture>(this->device, "textures/viking_room.png"));
		this->normalTextures.emplace_back(std::make_unique<EngineTexture>(this->device, "textures/viking_room.png"));

		this->numLights = 0u;
	}

	void EngineApp::loadQuadModels() {
		VertexModelData modelData{};

		std::vector<Vertex> vertices;

		Vertex vertex1 { glm::vec3(-1.0f, -1.0f, 0.0f) };
		vertices.emplace_back(vertex1);

		Vertex vertex2 { glm::vec3(1.0f, -1.0f, 0.0f) };
		vertices.emplace_back(vertex2);

		Vertex vertex3 { glm::vec3(1.0f, 1.0f, 0.0f) };
		vertices.emplace_back(vertex3);

		Vertex vertex4 { glm::vec3(-1.0f, 1.0f, 0.0f) };
		vertices.emplace_back(vertex4);

		modelData.vertices = vertices;
		modelData.indices = {
			0, 1, 2, 2, 3, 0
		};

		this->quadModels = std::make_shared<EngineVertexModel>(this->device, modelData);
	}

	RayTraceUbo EngineApp::initUbo(uint32_t width, uint32_t height) {
		RayTraceUbo ubo{};

		this->camera->setViewDirection(glm::vec3{278.0f, 278.0f, -800.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, 40.0f);
		CameraRay cameraRay = this->camera->getCameraRay();
		
		ubo.origin = cameraRay.origin;
		ubo.horizontal = cameraRay.horizontal;
		ubo.vertical = cameraRay.vertical;
		ubo.lowerLeftCorner = cameraRay.lowerLeftCorner;
		ubo.numLights = this->numLights;

		float phi = glm::radians(45.0f);
		float theta = glm::radians(45.0f);

		float sunX = glm::sin(theta) * glm::cos(phi);
		float sunY = glm::sin(theta) * glm::sin(phi);
		float sunZ = glm::cos(theta);

		ubo.sunLight.direction = glm::normalize(glm::vec3(sunX, sunY, sunZ));
		ubo.sunLight.color = glm::vec3(0.52f, 0.8f, 0.92f);

		return ubo;
	}

	void EngineApp::recreateSubRendererAndSubsystem() {
		uint32_t width = this->renderer->getSwapChain()->width();
		uint32_t height = this->renderer->getSwapChain()->height();

		std::shared_ptr<EngineDescriptorPool> descriptorPool = this->renderer->getDescriptorPool();
		std::vector<std::shared_ptr<EngineImage>> swapChainImages = this->renderer->getSwapChain()->getswapChainImages();

		this->swapChainSubRenderer = std::make_unique<EngineSwapChainSubRenderer>(this->device, this->renderer->getSwapChain()->getswapChainImages(), 
			this->renderer->getSwapChain()->getSwapChainImageFormat(), static_cast<int>(this->renderer->getSwapChain()->imageCount()), 
			width, height);

		this->rayTraceImage = std::make_unique<EngineRayTraceImage>(this->device, width, height, static_cast<uint32_t>(this->renderer->getSwapChain()->imageCount()));
		this->accumulateImages = std::make_unique<EngineAccumulateImage>(this->device, width, height, static_cast<uint32_t>(this->renderer->getSwapChain()->imageCount()));

		VkDescriptorBufferInfo buffersInfo[9] { 
			this->objectModel->getObjectInfo(), 
			this->objectModel->getBvhInfo(),
			this->primitiveModel->getPrimitiveInfo(), 
			this->primitiveModel->getBvhInfo(),
			this->rayTraceVertexModels->getVertexnfo(),
			this->lightModel->getLightInfo(),
			this->lightModel->getBvhInfo(),
			this->materialModel->getMaterialInfo(),
			this->transformationModel->getTransformationInfo() 
		};

		std::vector<VkDescriptorImageInfo> imagesInfo[2] {
			this->rayTraceImage->getImagesInfo(),
			this->accumulateImages->getImagesInfo()
		};

		std::vector<VkDescriptorImageInfo> texturesInfo[2];
		for (int i = 0; i < this->colorTextures.size(); i++) {
			texturesInfo[0].emplace_back(this->colorTextures[i]->getDescriptorInfo());
		}

		for (int i = 0; i < this->normalTextures.size(); i++) {
			texturesInfo[1].emplace_back(this->normalTextures[i]->getDescriptorInfo());
		}

		this->rayTraceDescSet = std::make_unique<EngineRayTraceDescSet>(this->device, this->renderer->getDescriptorPool(), this->globalUniforms->getBuffersInfo(), this->rayTraceImage->getImagesInfo(), buffersInfo, texturesInfo);
		this->samplingDescSet = std::make_unique<EngineSamplingDescSet>(this->device, this->renderer->getDescriptorPool(), imagesInfo);

		this->traceRayRender = std::make_unique<EngineTraceRayRenderSystem>(this->device, this->rayTraceDescSet->getDescSetLayout()->getDescriptorSetLayout(), width, height, 1);
		this->samplingRayRender = std::make_unique<EngineSamplingRayRasterRenderSystem>(this->device, this->samplingDescSet->getDescSetLayout()->getDescriptorSetLayout(), this->swapChainSubRenderer->getRenderPass()->getRenderPass());

		this->camera = std::make_shared<EngineCamera>(width, height);
	}
}
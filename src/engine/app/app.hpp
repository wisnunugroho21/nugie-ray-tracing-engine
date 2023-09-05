#pragma once

#include "../../vulkan/window/window.hpp"
#include "../../vulkan/device/device.hpp"
#include "../../vulkan/texture/texture.hpp"
#include "../../vulkan/buffer/buffer.hpp"
#include "../utils/camera/camera.hpp"
#include "../data/image/accumulate_image.hpp"
#include "../data/image/ray_trace_image.hpp"
#include "../data/model/primitive_model.hpp"
#include "../data/model/object_model.hpp"
#include "../data/model/light_model.hpp"
#include "../data/model/material_model.hpp"
#include "../data/model/transformation_model.hpp"
#include "../data/model/vertex_ray_trace_model.hpp"
#include "../data/buffer/global_uniform.hpp"
#include "../data/buffer/storage/hit_record_storage_buffer.hpp"
#include "../data/buffer/storage/indirect_shade_storage_buffer.hpp"
#include "../data/buffer/storage/light_shade_storage_buffer.hpp"
#include "../data/buffer/storage/miss_record_storage_buffer.hpp"
#include "../data/buffer/storage/ray_data_storage_buffer.hpp"
#include "../data/buffer/storage/indirect_sampler_storage_buffer.hpp"
#include "../data/buffer/storage/indirect_data_storage_buffer.hpp"
#include "../data/buffer/storage/direct_shade_storage_buffer.hpp"
#include "../data/buffer/storage/direct_data_storage_buffer.hpp"
#include "../data/descSet/ray_tracing/indirect_lambert_desc_set.hpp"
#include "../data/descSet/ray_tracing/direct_lambert_desc_set.hpp"
#include "../data/descSet/ray_tracing/sun_direct_lambert_desc_set.hpp"
#include "../data/descSet/ray_tracing/integrator_desc_set.hpp"
#include "../data/descSet/ray_tracing/intersect_light_desc_set.hpp"
#include "../data/descSet/ray_tracing/intersect_object_desc_set.hpp"
#include "../data/descSet/ray_tracing/light_shade_desc_set.hpp"
#include "../data/descSet/ray_tracing/miss_desc_set.hpp"
#include "../data/descSet/ray_tracing/indirect_sampler_desc_set.hpp"
#include "../data/descSet/ray_tracing/direct_sampler_desc_set.hpp"
#include "../data/descSet/ray_tracing/sun_direct_sampler_desc_set.hpp"
#include "../data/descSet/sampling_desc_set.hpp"
#include "../renderer/hybrid_renderer.hpp"
#include "../renderer_sub/swapchain_sub_renderer.hpp"
#include "../renderer_system/ray_tracing/indirect_lambert_render_system.hpp"
#include "../renderer_system/ray_tracing/direct_lambert_render_system.hpp"
#include "../renderer_system/ray_tracing/sun_direct_lambert_render_system.hpp"
#include "../renderer_system/ray_tracing/integrator_render_system.hpp"
#include "../renderer_system/ray_tracing/intersect_light_render_system.hpp"
#include "../renderer_system/ray_tracing/intersect_object_render_system.hpp"
#include "../renderer_system/ray_tracing/light_shade_render_system.hpp"
#include "../renderer_system/ray_tracing/miss_render_system.hpp"
#include "../renderer_system/ray_tracing/indirect_sampler_render_system.hpp"
#include "../renderer_system/ray_tracing/direct_sampler_render_system.hpp"
#include "../renderer_system/ray_tracing/sun_direct_sampler_render_system.hpp"
#include "../renderer_system/sampling_ray_raster_render_system.hpp"
#include "../utils/load_model/load_model.hpp"
#include "../utils/camera/camera.hpp"
#include "../controller/keyboard/keyboard_controller.hpp"
#include "../controller/mouse/mouse_controller.hpp"

#include <memory>
#include <vector>

#define APP_TITLE "Testing Vulkan"

namespace nugiEngine {
	class EngineApp
	{
		public:
			static constexpr int WIDTH = 800;
			static constexpr int HEIGHT = 800;

			EngineApp();
			~EngineApp();

			void run();
			void renderLoop();

		private:
			void loadCornellBox();
			void loadSkyLight();
			void loadQuadModels();

			RayTraceUbo initUbo(uint32_t width, uint32_t height);
			void recreateSubRendererAndSubsystem();

			EngineWindow window{WIDTH, HEIGHT, APP_TITLE};
			EngineDevice device{window};
			
			std::unique_ptr<EngineHybridRenderer> renderer{};
			std::unique_ptr<EngineSwapChainSubRenderer> swapChainSubRenderer{};

			std::unique_ptr<EngineIndirectLambertRenderSystem> indirectLambertRender{};
			std::unique_ptr<EngineDirectLambertRenderSystem> directLambertRender{};
			std::unique_ptr<EngineSunDirectLambertRenderSystem> sunDirectLambertRender{};
			std::unique_ptr<EngineIntegratorRenderSystem> integratorRender{};
			std::unique_ptr<EngineIntersectObjectRenderSystem> intersectObjectRender{};
			std::unique_ptr<EngineIntersectLightRenderSystem> intersectLightRender{};
			std::unique_ptr<EngineLightShadeRenderSystem> lightShadeRender{};
			std::unique_ptr<EngineMissRenderSystem> missRender{};
			std::unique_ptr<EngineIndirectSamplerRenderSystem> indirectSamplerRender{};
			std::unique_ptr<EngineDirectSamplerRenderSystem> directSamplerRender{};
			std::unique_ptr<EngineSunDirectSamplerRenderSystem> sunDirectSamplerRender{};
			std::unique_ptr<EngineSamplingRayRasterRenderSystem> samplingRayRender{};

			std::unique_ptr<EngineAccumulateImage> accumulateImages{};
			std::unique_ptr<EngineRayTraceImage> indirectImage{};
			std::unique_ptr<EngineGlobalUniform> globalUniforms{};

			std::unique_ptr<EnginePrimitiveModel> primitiveModel{};
			std::unique_ptr<EngineObjectModel> objectModel{};
			std::unique_ptr<EngineLightModel> lightModel{};
			std::unique_ptr<EngineMaterialModel> materialModel{};
			std::unique_ptr<EngineTransformationModel> transformationModel{};
			std::shared_ptr<EngineVertexModel> quadModels{};
			std::shared_ptr<EngineRayTraceVertexModel> rayTraceVertexModels{};

			std::shared_ptr<EngineRayDataStorageBuffer> objectRayDataBuffer{};
			std::shared_ptr<EngineRayDataStorageBuffer> lightRayDataBuffer{};
			std::shared_ptr<EngineHitRecordStorageBuffer> directObjectHitRecordBuffer{};
			std::shared_ptr<EngineHitRecordStorageBuffer> directLightHitRecordBuffer{};
			std::shared_ptr<EngineHitRecordStorageBuffer> indirectObjectHitRecordBuffer{};
			std::shared_ptr<EngineHitRecordStorageBuffer> indirectLightHitRecordBuffer{};
			std::shared_ptr<EngineIndirectShadeStorageBuffer> indirectLambertShadeBuffer{};
			std::shared_ptr<EngineDirectShadeStorageBuffer> directLambertShadeBuffer{};
			std::shared_ptr<EngineDirectShadeStorageBuffer> sunDirectLambertShadeBuffer{};
			std::shared_ptr<EngineLightShadeStorageBuffer> lightShadeBuffer{};
			std::shared_ptr<EngineMissRecordStorageBuffer> missBuffer{};
			std::shared_ptr<EngineIndirectSamplerStorageBuffer> indirectSamplerBuffer{};
			std::shared_ptr<EngineIndirectDataStorageBuffer> indirectDataBuffer{};
			std::shared_ptr<EngineDirectDataStorageBuffer> directDataBuffer{};

			std::unique_ptr<EngineIndirectLambertDescSet> indirectLambertDescSet{};
			std::unique_ptr<EngineDirectLambertDescSet> directLambertDescSet{};
			std::unique_ptr<EngineSunDirectLambertDescSet> sunDirectLambertDescSet{};
			std::unique_ptr<EngineIntegratorDescSet> integratorDescSet{};
			std::unique_ptr<EngineIntersectObjectDescSet> directIntersectObjectDescSet{};
			std::unique_ptr<EngineIntersectLightDescSet> directIntersectLightDescSet{};
			std::unique_ptr<EngineIntersectObjectDescSet> indirectIntersectObjectDescSet{};
			std::unique_ptr<EngineIntersectLightDescSet> indirectIntersectLightDescSet{};
			std::unique_ptr<EngineLightShadeDescSet> lightShadeDescSet{};
			std::unique_ptr<EngineMissDescSet> missDescSet{};
			std::unique_ptr<EngineIndirectSamplerDescSet> indirectSamplerDescSet{};
			std::unique_ptr<EngineDirectSamplerDescSet> directSamplerDescSet{};
			std::unique_ptr<EngineSunDirectSamplerDescSet> sunDirectSamplerDescSet{};
			std::unique_ptr<EngineSamplingDescSet> samplingDescSet{};

			std::vector<std::unique_ptr<EngineTexture>> colorTextures{};
			std::vector<std::unique_ptr<EngineTexture>> normalTextures{};

			std::shared_ptr<EngineCamera> camera{};
			std::shared_ptr<EngineKeyboardController> keyboardController{};
			std::shared_ptr<EngineMouseController> mouseController{};

			uint32_t randomSeed = 0, numLights = 0;
			bool isRendering = true, isCameraMoved = false;
			float frameTime = 0;

			RayTraceUbo globalUbo;
	};
}
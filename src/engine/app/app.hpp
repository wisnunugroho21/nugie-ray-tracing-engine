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
#include "../data/descSet/ray_tracing/indirect_shade_desc_set.hpp"
#include "../data/descSet/ray_tracing/direct_shade_desc_set.hpp"
#include "../data/descSet/ray_tracing/sun_direct_shade_desc_set.hpp"
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
#include "../renderer_system/ray_tracing/indirect_shade_render_system.hpp"
#include "../renderer_system/ray_tracing/direct_shade_render_system.hpp"
#include "../renderer_system/ray_tracing/sun_direct_shade_render_system.hpp"
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

namespace NugieApp {
	class App
	{
		public:
			static constexpr int WIDTH = 800;
			static constexpr int HEIGHT = 800;

			App();
			~App();

			void run();
			void renderLoop();

		private:
			void loadCornellBox();
			void loadSkyLight();
			void loadQuadModels();

			RayTraceUbo initUbo(uint32_t width, uint32_t height);
			void recreateSubRendererAndSubsystem();

			std::unique_ptr<NugieVulkan::Window> window;
			std::unique_ptr<NugieVulkan::Device> device;
			
			std::unique_ptr<HybridRenderer> renderer{};
			std::unique_ptr<SwapChainSubRenderer> swapChainSubRenderer{};

			std::unique_ptr<IndirectShadeRenderSystem> indirectShadeRender{};
			std::unique_ptr<DirectShadeRenderSystem> directShadeRender{};
			std::unique_ptr<SunDirectShadeRenderSystem> sunDirectShadeRender{};
			std::unique_ptr<IntegratorRenderSystem> integratorRender{};
			std::unique_ptr<IntersectObjectRenderSystem> intersectObjectRender{};
			std::unique_ptr<IntersectLightRenderSystem> intersectLightRender{};
			std::unique_ptr<LightShadeRenderSystem> lightShadeRender{};
			std::unique_ptr<MissRenderSystem> missRender{};
			std::unique_ptr<IndirectSamplerRenderSystem> indirectSamplerRender{};
			std::unique_ptr<DirectSamplerRenderSystem> directSamplerRender{};
			std::unique_ptr<SunDirectSamplerRenderSystem> sunDirectSamplerRender{};
			std::unique_ptr<SamplingRayRasterRenderSystem> samplingRayRender{};

			std::unique_ptr<AccumulateImage> accumulateImages{};
			std::unique_ptr<RayTraceImage> indirectImage{};
			std::unique_ptr<GlobalUniform> globalUniforms{};

			std::unique_ptr<PrimitiveModel> primitiveModel{};
			std::unique_ptr<ObjectModel> objectModel{};
			std::unique_ptr<LightModel> lightModel{};
			std::unique_ptr<MaterialModel> materialModel{};
			std::unique_ptr<TransformationModel> transformationModel{};
			std::shared_ptr<VertexModel> quadModels{};
			std::shared_ptr<RayTraceVertexModel> rayTraceVertexModels{};

			std::shared_ptr<RayDataStorageBuffer> objectRayDataBuffer{};
			std::shared_ptr<RayDataStorageBuffer> lightRayDataBuffer{};
			std::shared_ptr<HitRecordStorageBuffer> directObjectHitRecordBuffer{};
			std::shared_ptr<HitRecordStorageBuffer> directLightHitRecordBuffer{};
			std::shared_ptr<HitRecordStorageBuffer> indirectObjectHitRecordBuffer{};
			std::shared_ptr<HitRecordStorageBuffer> indirectLightHitRecordBuffer{};
			std::shared_ptr<IndirectShadeStorageBuffer> indirectShadeShadeBuffer{};
			std::shared_ptr<DirectShadeStorageBuffer> directShadeShadeBuffer{};
			std::shared_ptr<DirectShadeStorageBuffer> sunDirectShadeShadeBuffer{};
			std::shared_ptr<LightShadeStorageBuffer> lightShadeBuffer{};
			std::shared_ptr<MissRecordStorageBuffer> missBuffer{};
			std::shared_ptr<IndirectSamplerStorageBuffer> indirectSamplerBuffer{};
			std::shared_ptr<IndirectDataStorageBuffer> indirectDataBuffer{};
			std::shared_ptr<DirectDataStorageBuffer> directDataBuffer{};

			std::unique_ptr<IndirectShadeDescSet> indirectShadeDescSet{};
			std::unique_ptr<DirectShadeDescSet> directShadeDescSet{};
			std::unique_ptr<SunDirectShadeDescSet> sunDirectShadeDescSet{};
			std::unique_ptr<IntegratorDescSet> integratorDescSet{};
			std::unique_ptr<IntersectObjectDescSet> directIntersectObjectDescSet{};
			std::unique_ptr<IntersectLightDescSet> directIntersectLightDescSet{};
			std::unique_ptr<IntersectObjectDescSet> indirectIntersectObjectDescSet{};
			std::unique_ptr<IntersectLightDescSet> indirectIntersectLightDescSet{};
			std::unique_ptr<LightShadeDescSet> lightShadeDescSet{};
			std::unique_ptr<MissDescSet> missDescSet{};
			std::unique_ptr<IndirectSamplerDescSet> indirectSamplerDescSet{};
			std::unique_ptr<DirectSamplerDescSet> directSamplerDescSet{};
			std::unique_ptr<SunDirectSamplerDescSet> sunDirectSamplerDescSet{};
			std::unique_ptr<SamplingDescSet> samplingDescSet{};

			std::shared_ptr<Camera> camera{};
			std::shared_ptr<KeyboardController> keyboardController{};
			std::shared_ptr<MouseController> mouseController{};

			uint32_t randomSeed = 0, numLights = 0;
			bool isRendering = true, isCameraMoved = false;
			float frameTime = 0;

			RayTraceUbo globalUbo;
	};
}
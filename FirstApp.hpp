#pragma once

#include "LittleVulkanEngineWindow.hpp"
#include "LittleVulkanEnginePipeline.hpp"
#include "LittleVulkanEngineDevice.hpp"

namespace LittleVulkanEngine {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		LveWindow lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		LveDevice lveDevice{ lveWindow };
		LvePipeline lvePipeline{
			lveDevice,
			"shaders/SimpleShader.vert.spv",
			"shaders/SimpleShader.frag.spv",
			LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)
		};
	};
} // namespace LittleVulkanEngine
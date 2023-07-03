#pragma once

#include "LittleVulkanEngineWindow.hpp"
#include "LittleVulkanEnginePipeline.hpp"

namespace LittleVulkanEngine {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		LveWindow lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		LvePipeline lvePipeline{ "shaders/SimpleShader.vert.spv", "shaders/SimpleShader.frag.spv" };
	};
} // namespace LittleVulkanEngine
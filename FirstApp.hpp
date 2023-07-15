#pragma once

#include "SgDescriptors.hpp"
#include "SgDevice.hpp"
#include "SgGameObject.hpp"
#include "SgWindow.hpp"
#include "SgRenderer.hpp"

// std
#include <memory>
#include <vector>

namespace SunGlassEngine {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		void run();

	private:
		void loadGameObjects();

		SgWindow sgWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		SgDevice sgDevice{ sgWindow };
		SgRenderer sgRenderer{ sgWindow, sgDevice };

		std::unique_ptr<SgDescriptorPool> globalPool{};
		std::vector<SgGameObject> gameObjects;
	};
} // namespace SunGlassEngine
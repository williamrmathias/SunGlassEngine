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
	class OceanApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		OceanApp();
		~OceanApp();

		OceanApp(const OceanApp&) = delete;
		OceanApp& operator=(const OceanApp&) = delete;

		void run();

	private:
		void loadGameObjects();

		SgWindow sgWindow{ WIDTH, HEIGHT, "Ocean Simulator" };
		SgDevice sgDevice{ sgWindow };
		SgRenderer sgRenderer{ sgWindow, sgDevice };

		std::unique_ptr<SgDescriptorPool> globalPool{};
		SgGameObject::Map gameObjects;
	};
} // namespace SunGlassEngine
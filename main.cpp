#include "FirstApp.hpp"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
	LittleVulkanEngine::FirstApp app{};

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <iostream>

int main() {
	VkInstance instance;

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "tuga4D";
	appInfo.pApplicationName = "Minha applicação tuga";

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	if (VkResult result = vkCreateInstance(&createInfo, nullptr, &instance); result == VK_SUCCESS) {
		std::cout << "vulkan success!" << std::endl;
	} else {
		throw std::runtime_error("failed to create vulkan instance!");
	}

	glm::vec3 num1 = { 1, 0 , 1 };
	glm::vec3 num2 = { 4, 2 , 5 };

	glm::vec3 result = num1 * num2;

	std::cout << result.x << result.y << result.z << std::endl;
}
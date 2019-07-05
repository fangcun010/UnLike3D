/*
MIT License

Copyright(c) 2019 fangcun

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "defferredpipelinetype.h"
#include "defferredpipeline.h"

#include <iostream>
#include <fstream>
#include <set>
#include <algorithm>
#include <chrono>

#undef max
#undef min

static VkInstance vk_instance;

VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSetKHR(
	VkCommandBuffer                             commandBuffer,
	VkPipelineBindPoint                         pipelineBindPoint,
	VkPipelineLayout                            layout,
	uint32_t                                    set,
	uint32_t                                    descriptorWriteCount,
	const VkWriteDescriptorSet*                 pDescriptorWrites) {
	static PFN_vkCmdPushDescriptorSetKHR func = nullptr;
	if(func==nullptr)
		func= (PFN_vkCmdPushDescriptorSetKHR)glfwGetInstanceProcAddress(vk_instance, "vkCmdPushDescriptorSetKHR");
	func(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
}

namespace unlike3d {
	ukObjectTypeInterface *DefferredPipelineType::object_type_interface_ = nullptr;

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT *callback_data, void *user_data) {
		std::cerr << "Validation Layer:";
		if (message_severity&VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
			std::cerr << "verbose" << std::endl;
		}
		if (message_severity&VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
			std::cerr << "info" << std::endl;
		}
		if (message_severity&VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			std::cerr << "warning" << std::endl;
		}
		if (message_severity&VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			std::cerr << "error" << std::endl;
		}

		std::cerr<< callback_data->pMessage << std::endl;
		return VK_FALSE;
	}

	static VkResult CreateDebugUtilsMessengeEXT(VkInstance vk_instance, const VkDebugUtilsMessengerCreateInfoEXT *create_info,
		const VkAllocationCallbacks *allocator, VkDebugUtilsMessengerEXT *debug_messenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vk_instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
			return func(vk_instance, create_info, allocator, debug_messenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	static void DestroyDebugUtilsMessengerEXT(VkInstance vk_instance, VkDebugUtilsMessengerEXT debug_messenger,
		const VkAllocationCallbacks *allocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vk_instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
			func(vk_instance, debug_messenger, allocator);
	}

	void DefferredPipelineType::InitDebugMessenger() {
		if (!ENABLE_VALIDATION_LAYERS) return;
		VkDebugUtilsMessengerCreateInfoEXT create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		create_info.pfnUserCallback = DebugCallback;
		if (CreateDebugUtilsMessengeEXT(vk_instance_, &create_info, nullptr, &debug_messenger_)!=VK_SUCCESS) {
			throw std::runtime_error("Failed to set up debug messenger!");
		}
	}

	std::vector<const char *> DefferredPipelineType::GetRequiredExtensions() {
		uint32_t glfw_extension_count = 0;
		const char **glfw_extensions;
		glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
		std::vector<const char *> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
		if (ENABLE_VALIDATION_LAYERS)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		return extensions;
	}

	bool DefferredPipelineType::CheckValidationLayerSupport() {
		uint32_t layer_count;
		vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
		std::vector<VkLayerProperties> available_layers(layer_count);
		vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
		for (const char *layer_name : validation_layers_) {
			bool layer_found = false;
			for (const auto &layer_properties : available_layers) {
				if (!strcmp(layer_name, layer_properties.layerName)) {
					layer_found = true;
					break;
				}
			}
			if (!layer_found)
				return false;
		}
		return true;
	}

	DefferredPipelineType::QueueFamilyIndices DefferredPipelineType::FindQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;
		uint32_t queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
		std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());
		uint32_t i = 0;
		for (const auto &queue_family : queue_families) {
			if (queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.SetGraphicsFamilyIndex(i);
			}
			VkBool32 present_support = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i,surface_, &present_support);
			if (queue_family.queueCount > 0 && present_support) {
				indices.SetPresentFamilyIndex(i);
			}
			if (indices.IsComplete())
				break;
			i++;
		}
		return indices;
	}

	bool DefferredPipelineType::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extension_count;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
		std::vector<VkExtensionProperties> available_extensions(extension_count);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());
		std::set<std::string> required_extensions(device_extensions_.begin(), device_extensions_.end());
		for (const auto &extension : available_extensions)
			required_extensions.erase(extension.extensionName);
		return required_extensions.empty();
	}

	bool DefferredPipelineType::IsDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndices indices = FindQueueFamilies(device);
		bool extensions_supported = CheckDeviceExtensionSupport(device);
		bool swap_chain_adequate = false;
		if (extensions_supported) {
			SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(device);
			swap_chain_adequate=!swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
		}
		return indices.IsComplete() && extensions_supported && swap_chain_adequate;
	}

	void DefferredPipelineType::CreateInstance() {
		if (ENABLE_VALIDATION_LAYERS && !CheckValidationLayerSupport()) {
			throw std::runtime_error("Validation layers requested,but not available!");
		}
		VkApplicationInfo app_info = {};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pApplicationName = "Defferred Pipeline";
		app_info.pEngineName = "No Engine";
		app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &app_info;

		auto extensions = GetRequiredExtensions();
		create_info.enabledExtensionCount = extensions.size();
		create_info.ppEnabledExtensionNames = extensions.data();

		if (ENABLE_VALIDATION_LAYERS) {
			create_info.enabledLayerCount = validation_layers_.size();
			create_info.ppEnabledLayerNames = validation_layers_.data();

			VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {};
			debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debug_create_info.pfnUserCallback = DebugCallback;

			create_info.pNext = &debug_create_info;
		}
		else {
			create_info.enabledLayerCount = 0;
		}

		if (vkCreateInstance(&create_info, nullptr, &vk_instance_) != VK_SUCCESS)
			throw std::runtime_error("Error to create vk instance!");
		vk_instance = vk_instance_;
	}

	void DefferredPipelineType::CreateSurface() {
		if (glfwCreateWindowSurface(vk_instance_, window_, nullptr, &surface_) != VK_SUCCESS)
			throw std::runtime_error("Failed to create window surface!");
	}

	void DefferredPipelineType::CreateLogicalDevice() {
		QueueFamilyIndices indices = FindQueueFamilies(physical_device_);
		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
		std::set<uint32_t> unique_queue_families = { indices.GetGraphicsFamilyIndex(),
			indices.GetPresentFamilyIndex() };
		float queue_priority;
		std::vector<float> queue_priorities;

		//graphic queue
		VkDeviceQueueCreateInfo queue_create_info = {};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueFamilyIndex = indices.GetGraphicsFamilyIndex();
		queue_create_info.queueCount = GetGraphicsQueueCount();
		queue_create_infos.push_back(queue_create_info);
		
		//present queue
		if (indices.GetGraphicsFamilyIndex() != indices.GetPresentFamilyIndex()) {

			queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_info.queueFamilyIndex = indices.GetPresentFamilyIndex();
			queue_create_info.queueCount = 1;
			queue_create_info.pQueuePriorities = &queue_priority;
			queue_create_infos.push_back(queue_create_info);
		}
		else {
			queue_create_infos[0].queueCount++;
			queue_priorities.resize(queue_create_infos[0].queueCount);
			for (auto i = 0; i < queue_create_infos[0].queueCount; i++)
				queue_priorities[i] = 1.0f;
			queue_create_infos[0].pQueuePriorities = queue_priorities.data();
		}
		
		VkPhysicalDeviceFeatures device_features = {};
		device_features.sparseBinding = true;
		device_features.sparseResidencyBuffer = true;
		device_features.sparseResidencyAliased = true;
		device_features.samplerAnisotropy = true;
		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.queueCreateInfoCount = queue_create_infos.size();
		create_info.pQueueCreateInfos = queue_create_infos.data();
		create_info.pEnabledFeatures = &device_features;
		create_info.enabledExtensionCount = device_extensions_.size();
		create_info.ppEnabledExtensionNames = device_extensions_.data();
		if (ENABLE_VALIDATION_LAYERS) {
			create_info.enabledLayerCount = validation_layers_.size();
			create_info.ppEnabledLayerNames = validation_layers_.data();
		}
		else
			create_info.enabledLayerCount = 0;
		if (vkCreateDevice(physical_device_, &create_info, nullptr, &device_) != VK_SUCCESS)
			throw std::runtime_error("Failed to create logical device!");
		graphics_queues_.resize(GetGraphicsQueueCount());
		unsigned int queue_count = GetGraphicsQueueCount();
		for(unsigned int i=0;i<queue_count;i++)
			vkGetDeviceQueue(device_, indices.GetGraphicsFamilyIndex(), i, &graphics_queues_[i]);
		if(indices.GetGraphicsFamilyIndex()!=indices.GetPresentFamilyIndex())
			vkGetDeviceQueue(device_, indices.GetPresentFamilyIndex(), 0, &present_queue_);
		else
			vkGetDeviceQueue(device_, indices.GetPresentFamilyIndex(), GetGraphicsQueueCount(), &present_queue_);
	}

	void DefferredPipelineType::SelectPhysicalDevice() {
		uint32_t device_count = 0;
		vkEnumeratePhysicalDevices(vk_instance_, &device_count, nullptr);
		if (device_count == 0)
			throw std::runtime_error("Failed to find GPUs with Vulkan support!");
		std::vector<VkPhysicalDevice> devices(device_count);
		vkEnumeratePhysicalDevices(vk_instance_, &device_count, devices.data());
		for (const auto &device : devices) {
			if (IsDeviceSuitable(device)) {
				physical_device_ = device;
				break;
			}
		}
		if (physical_device_ == VK_NULL_HANDLE)
			throw std::runtime_error("Failed to find a suitable GPU!");
	}

	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &available_present_modes) {
		VkPresentModeKHR best_mode = VK_PRESENT_MODE_FIFO_KHR;
		for (const auto &available_present_mode : available_present_modes) {
			if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
				return available_present_mode;
			else if (available_present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
				best_mode = available_present_mode;
			}
		}
		return best_mode;
	}

	VkExtent2D DefferredPipelineType::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;
		else {
			int width, height;
			glfwGetFramebufferSize(window_, &width, &height);
			window_w_ = width; window_h_ = height;
			VkExtent2D actual_extent = { width,height};
			actual_extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width,
				actual_extent.width));
			actual_extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height,
				actual_extent.height));
			return actual_extent;
		}
	}

	VkSurfaceFormatKHR DefferredPipelineType::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available_formats) {
		if (available_formats.size() == 1 && available_formats[0].format == VK_FORMAT_UNDEFINED) {
			return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}
		for (const auto &available_format : available_formats) {
			if (available_format.format == VK_FORMAT_B8G8R8A8_UNORM && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return available_format;
			}
		}
		return available_formats[0];
	}

	DefferredPipelineType::SwapChainSupportDetails DefferredPipelineType::QuerySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);
		uint32_t format_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, nullptr);
		if (format_count != 0) {
			details.formats.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, details.formats.data());
		}
		uint32_t present_mode_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_mode_count, nullptr);
		if (present_mode_count != 0) {
			details.present_modes.resize(present_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_mode_count, details.present_modes.data());
		}
		return details;
	}

	void DefferredPipelineType::CreateImageViews() {
		swap_chain_image_views_.resize(swap_chain_images_.size());
		for (size_t i = 0; i < swap_chain_images_.size(); i++) {
			VkImageViewCreateInfo create_info = {};
			create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			create_info.image = swap_chain_images_[i];
			create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			create_info.format = swap_chain_image_format_;
			create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			create_info.subresourceRange.baseMipLevel = 0;
			create_info.subresourceRange.levelCount = 1;
			create_info.subresourceRange.baseArrayLayer = 0;
			create_info.subresourceRange.layerCount = 1;
			if (vkCreateImageView(device_, &create_info, nullptr, &swap_chain_image_views_[i])!=VK_SUCCESS)
				throw std::runtime_error("Failed to create image views!");
		}
	}

	void DefferredPipelineType::CreateCommandPools() {
		QueueFamilyIndices queue_family_indices = FindQueueFamilies(physical_device_);
		VkCommandPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		pool_info.queueFamilyIndex = queue_family_indices.GetGraphicsFamilyIndex();
		unsigned int command_pool_count = GetThreadCount();
		command_pools_.resize(command_pool_count);
		for(unsigned int i=0;i<command_pool_count;i++)
			if (vkCreateCommandPool(device_, &pool_info, nullptr, &command_pools_[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create command pool!");
	}

	void DefferredPipelineType::CreateCommandBuffers() {
		for (unsigned int i = 0; i < MAX_FRAMES_IN_FLIGHT;i++) {
			command_buffers_[i].resize(GetThreadCount());
			unsigned command_buffer_count = GetThreadCount();
			for (unsigned int j = 0; j < command_buffer_count; j++) {
				VkCommandBufferAllocateInfo alloc_info = {};
				alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				alloc_info.commandPool = command_pools_[j];
				alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				alloc_info.commandBufferCount = 1;
				if (vkAllocateCommandBuffers(device_, &alloc_info, &command_buffers_[i][j]) != VK_SUCCESS)
					throw std::runtime_error("Failed to allocate command buffers!");
			}
			VkCommandBufferAllocateInfo alloc_info = {};
			alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			alloc_info.commandPool = command_pools_[0];
			alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			alloc_info.commandBufferCount = 1;
			if (vkAllocateCommandBuffers(device_, &alloc_info, &clear_command_buffers_[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to allocate command buffers!");
		}
	}

	void DefferredPipelineType::CreateSwapChain() {
		SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(physical_device_);
		VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swap_chain_support.formats);
		VkPresentModeKHR present_mode =ChooseSwapPresentMode(swap_chain_support.present_modes);
		VkExtent2D extent = ChooseSwapExtent(swap_chain_support.capabilities);
		uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
		if (swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount) {
			image_count = swap_chain_support.capabilities.maxImageCount;
		}
		VkSwapchainCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = surface_;
		create_info.minImageCount = image_count;
		create_info.imageFormat = surface_format.format;
		create_info.imageExtent = extent;
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		QueueFamilyIndices indices = FindQueueFamilies(physical_device_);
		uint32_t queue_family_indices[] = {indices.GetGraphicsFamilyIndex(),indices.GetPresentFamilyIndex()};
		if (indices.GetGraphicsFamilyIndex() != indices.GetPresentFamilyIndex()) {
			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			create_info.queueFamilyIndexCount = 2;
			create_info.pQueueFamilyIndices = queue_family_indices;
		}
		else
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.preTransform = swap_chain_support.capabilities.currentTransform;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.presentMode = present_mode;
		create_info.clipped = VK_TRUE;
		create_info.oldSwapchain = VK_NULL_HANDLE;
		if (vkCreateSwapchainKHR(device_, &create_info, nullptr, &swap_chain_) != VK_SUCCESS)
			throw std::runtime_error("Failed to create swap chain!");
		vkGetSwapchainImagesKHR(device_, swap_chain_, &image_count, nullptr);
		swap_chain_images_.resize(image_count);
		vkGetSwapchainImagesKHR(device_, swap_chain_, &image_count, swap_chain_images_.data());
		swap_chain_image_format_ = surface_format.format;
		swap_chain_extent_ = extent;
	}

	std::vector<char> DefferredPipelineType::ReadFile(const std::string &name) {
		std::string file_name = "C:/Work/Unlike3D/env/Shaders/" + name;
		std::ifstream file(file_name, std::ios::ate | std::ios::binary);
		if (!file.is_open())
			throw std::runtime_error("Failed to open file!");
		size_t file_size = (size_t)file.tellg();
		std::vector<char> buffer(file_size);
		file.seekg(0);
		file.read(buffer.data(), file_size);
		file.close();
		return buffer;
	}

	VkShaderModule DefferredPipelineType::CreateShaderModule(const std::vector<char> &code) {
		VkShaderModuleCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = code.size();
		create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());
		VkShaderModule shader_module;
		if (vkCreateShaderModule(device_, &create_info, nullptr, &shader_module) != VK_SUCCESS)
			throw std::runtime_error("Failed to create shader module!");
		return shader_module;
	}

	void DefferredPipelineType::CreateRenderPass() {
		VkAttachmentDescription color_attachment = {};
		color_attachment.format = swap_chain_image_format_;
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		VkAttachmentReference color_attachment_ref = {};
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		VkAttachmentDescription depth_attachment = {};
		depth_attachment.format = FindDepthFormat();
		depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		VkAttachmentReference depth_attachment_ref = {};
		depth_attachment_ref.attachment = 1;
		depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_ref;
		subpass.pDepthStencilAttachment = &depth_attachment_ref;
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | 
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		VkAttachmentDescription attachments[] = {color_attachment,depth_attachment};
		VkRenderPassCreateInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_info.attachmentCount = 2;
		render_pass_info.pAttachments = attachments;
		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = &subpass;
		render_pass_info.dependencyCount = 1;
		render_pass_info.pDependencies = &dependency;
		if (vkCreateRenderPass(device_, &render_pass_info, nullptr, &render_pass_) != VK_SUCCESS)
			throw std::runtime_error("Faile to create render pass!");
	}

	void DefferredPipelineType::CreateFramebuffers() {
		swap_chain_framebuffers_.resize(swap_chain_image_views_.size());
		for (size_t i = 0; i < swap_chain_image_views_.size(); i++) {
			VkImageView attachments[] = {
				swap_chain_image_views_[i],
				swap_chain_depth_image_views_[i]
			};
			VkFramebufferCreateInfo framebuffer_info = {};
			framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebuffer_info.renderPass = render_pass_;
			framebuffer_info.attachmentCount = 2;
			framebuffer_info.pAttachments = attachments;
			framebuffer_info.width = swap_chain_extent_.width;
			framebuffer_info.height = swap_chain_extent_.height;
			framebuffer_info.layers = 1;
			if (vkCreateFramebuffer(device_, &framebuffer_info, nullptr, &swap_chain_framebuffers_[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create framebuffer!");
		}
	}

	void DefferredPipelineType::CreateSyncObjects() {
		image_available_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
		render_finished_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
		unsigned int thread_count = GetThreadCount();
		for (unsigned int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			clear_end_semaphores_[i].resize(thread_count);
			draw_end_semaphores_[i].resize(thread_count);
		}
		in_flight_fences_.resize(MAX_FRAMES_IN_FLIGHT);
		VkSemaphoreCreateInfo semaphore_info = {};
		semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VkFenceCreateInfo fence_info = {};
		fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore(device_, &semaphore_info, nullptr, &image_available_semaphores_[i]) != VK_SUCCESS ||
				vkCreateSemaphore(device_, &semaphore_info, nullptr, &render_finished_semaphores_[i]) != VK_SUCCESS ||
				vkCreateFence(device_, &fence_info, nullptr, &in_flight_fences_[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create synchronization objects for a frame!");
			for (unsigned int j = 0; j < thread_count; j++) {
				if (vkCreateSemaphore(device_, &semaphore_info, nullptr, &clear_end_semaphores_[i][j]) != VK_SUCCESS)
					throw std::runtime_error("Failed to create synchronization objects for a frame!");
				if (vkCreateSemaphore(device_, &semaphore_info, nullptr, &draw_end_semaphores_[i][j]) != VK_SUCCESS)
					throw std::runtime_error("Failed to create synchronization objects for a frame!");
			}
		}
	}

	uint32_t DefferredPipelineType::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties mem_properties;
		vkGetPhysicalDeviceMemoryProperties(physical_device_, &mem_properties);
		for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
			if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}
		throw std::runtime_error("Failed to find suitable memory type!");
	}

	void DefferredPipelineType::CreateBufferWithoutMemory(VkDeviceSize size, VkBufferCreateFlags flags, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
		VkBuffer &buffer) {
		VkBufferCreateInfo buffer_info = {};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.flags = flags;
		buffer_info.size = size;
		buffer_info.usage = usage;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if (vkCreateBuffer(device_, &buffer_info, nullptr, &buffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to create buffer!");
	}

	void DefferredPipelineType::CreateBuffer(VkDeviceSize size,VkBufferCreateFlags flags, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
		VkBuffer &buffer, VkDeviceMemory &buffer_memory) {
		VkBufferCreateInfo buffer_info = {};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.flags = flags;
		buffer_info.size = size;
		buffer_info.usage = usage;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if (vkCreateBuffer(device_, &buffer_info, nullptr, &buffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to create buffer!");
		VkMemoryRequirements mem_requirements;
		vkGetBufferMemoryRequirements(device_, buffer, &mem_requirements);
		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = mem_requirements.size;
		alloc_info.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits, properties);
		if (vkAllocateMemory(device_, &alloc_info, nullptr, &buffer_memory) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate buffer memory!");
		vkBindBufferMemory(device_, buffer, buffer_memory, 0);
	}

	void DefferredPipelineType::CopyBuffer(unsigned int thread_id,VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) {
		VkCommandBufferAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandPool = command_pools_[thread_id];
		alloc_info.commandBufferCount = 1;
		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers(device_, &alloc_info, &command_buffer);
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(command_buffer, &begin_info);
			VkBufferCopy copy_region = {};
			copy_region.size = size;
			vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);
		vkEndCommandBuffer(command_buffer);
		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;
		vkQueueSubmit(graphics_queues_[thread_id], 1, &submit_info, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphics_queues_[thread_id]);
		vkFreeCommandBuffers(device_, command_pools_[thread_id],1,&command_buffer);
	}

	VkImage DefferredPipelineType::CreateTextureImage(unsigned int thread_id,unsigned int tex_width,unsigned int tex_height,
		unsigned char *pixels) {
		VkDeviceSize image_size = tex_width * tex_height * 4;
		VkBuffer stage_buffer;
		VkDeviceMemory stage_buffer_memory;
		CreateBuffer(image_size, 0, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stage_buffer, stage_buffer_memory);
		void *data;
		vkMapMemory(device_, stage_buffer_memory, 0, image_size, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(image_size));
		vkUnmapMemory(device_, stage_buffer_memory);
		VkImage texture_image;
		VkDeviceMemory texture_image_memory;
		CreateImage(texture_image, texture_image_memory, tex_width, tex_height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		TransitionImageLayout(thread_id, texture_image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyBufferToImage(thread_id,stage_buffer, texture_image, static_cast<uint32_t>(tex_width),
			static_cast<uint32_t>(tex_height));
		TransitionImageLayout(thread_id, texture_image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		vkDestroyBuffer(device_, stage_buffer, nullptr);
		vkFreeMemory(device_, stage_buffer_memory, nullptr);
		return texture_image;
	}

	void DefferredPipelineType::CreateImage(VkImage &image, VkDeviceMemory &image_memory, unsigned int width, unsigned int height,
		VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
		VkImageCreateInfo image_info = {};
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.extent.width = width;
		image_info.extent.height = height;
		image_info.extent.depth = 1;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;
		image_info.format = format;
		image_info.tiling = tiling;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.usage = usage;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if (vkCreateImage(device_, &image_info, nullptr, &image) != VK_SUCCESS)
			throw std::runtime_error("Failed to create image!");
		VkMemoryRequirements mem_requirements;
		vkGetImageMemoryRequirements(device_, image, &mem_requirements);
		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = mem_requirements.size;
		alloc_info.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits, properties);
		if (vkAllocateMemory(device_, &alloc_info, nullptr, &image_memory) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate image memory!");
		vkBindImageMemory(device_, image, image_memory, 0);
	}

	void DefferredPipelineType::TransitionImageLayout(unsigned int thread_id,VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout) {
		VkCommandBuffer command_buffer = BeginSingleTimeCommands(thread_id);
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = old_layout;
		barrier.newLayout = new_layout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;

		if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			if (HasStencilComponent(format)) {
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		VkPipelineStageFlags source_stage;
		VkPipelineStageFlags dest_stage;
		if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dest_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dest_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dest_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else
			throw std::invalid_argument("Unsupported layout transition!");
		vkCmdPipelineBarrier(command_buffer, source_stage, dest_stage,
			0, 0, nullptr, 0, nullptr, 1, &barrier);
		EndSingleTimeCommands(thread_id, command_buffer);
	}

	VkFormat DefferredPipelineType::FindSupportedFormat(const std::vector<VkFormat> &candidates,VkImageTiling tiling,VkFormatFeatureFlags features){
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physical_device_, format, &props);
			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}else if(tiling==VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features)==features){
				return format;
			}
		}
	}

	VkFormat DefferredPipelineType::FindDepthFormat() {
		return FindSupportedFormat({
			VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	bool DefferredPipelineType::HasStencilComponent(VkFormat format) {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	void DefferredPipelineType::CreateDepthBuffers() {
		VkFormat depth_format = FindDepthFormat();
		auto count = swap_chain_images_.size();
		swap_chain_depth_images_.resize(count);
		swap_chain_depth_image_views_.resize(count);
		swap_chain_depth_image_memories_.resize(count);
		for (auto i = 0; i < count; i++) {
			CreateImage(swap_chain_depth_images_[i], swap_chain_depth_image_memories_[i], swap_chain_extent_.width, swap_chain_extent_.height, depth_format,
				VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			swap_chain_depth_image_views_[i] = CreateImageView(swap_chain_depth_images_[i], depth_format,VK_IMAGE_ASPECT_DEPTH_BIT);
			TransitionImageLayout(0, swap_chain_depth_images_[i], depth_format, VK_IMAGE_LAYOUT_UNDEFINED, 
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		}
	}

	void DefferredPipelineType::CopyBufferToImage(unsigned int thread_id,VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
		VkCommandBuffer command_buffer = BeginSingleTimeCommands(thread_id);
		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.layerCount = 1;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageOffset = { 0,0,0 };
		region.imageExtent = { width,height,1 };
		vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
		EndSingleTimeCommands(thread_id, command_buffer);
	}

	VkCommandBuffer DefferredPipelineType::BeginSingleTimeCommands(unsigned int thread_id) {
		VkCommandBufferAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandPool = command_pools_[thread_id];
		alloc_info.commandBufferCount = 1;
		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers(device_, &alloc_info, &command_buffer);
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(command_buffer, &begin_info);
		return command_buffer;
	}

	void DefferredPipelineType::EndSingleTimeCommands(unsigned int thread_id,
		VkCommandBuffer command_buffer) {
		vkEndCommandBuffer(command_buffer);
		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;
		vkQueueSubmit(graphics_queues_[thread_id], 1, &submit_info, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphics_queues_[thread_id]);
		vkFreeCommandBuffers(device_, command_pools_[thread_id], 1, &command_buffer);
	}

	VkImageView DefferredPipelineType::CreateImageView(VkImage image, VkFormat format) {
		VkImageViewCreateInfo view_info = {};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = image;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = format;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;
		VkImageView image_view;
		if (vkCreateImageView(device_, &view_info, nullptr, &image_view) != VK_SUCCESS)
			throw std::runtime_error("Failed to create texture image view!");
		return image_view;
	}

	VkImageView DefferredPipelineType::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlagBits aspect_flags) {
		VkImageViewCreateInfo view_info = {};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = image;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = format;
		view_info.subresourceRange.aspectMask = aspect_flags;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;
		VkImageView image_view;
		if (vkCreateImageView(device_, &view_info, nullptr, &image_view) != VK_SUCCESS)
			throw std::runtime_error("Failed to create image view!");
		return image_view;
	}

	VkSampler DefferredPipelineType::CreateTextureSampler(glTF::Sampler &_sampler) {
		VkSampler sampler;
		VkSamplerCreateInfo sampler_info = {};
		sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		if (_sampler.mag_filter == glTF::NEAREST)
			sampler_info.magFilter = VK_FILTER_NEAREST;
		else if(_sampler.mag_filter==glTF::LINEAR)
			sampler_info.magFilter = VK_FILTER_LINEAR;
		else
			sampler_info.magFilter = VK_FILTER_NEAREST;

		if(_sampler.min_filter==glTF::NEAREST)
			sampler_info.minFilter = VK_FILTER_NEAREST;
		else if(_sampler.min_filter==glTF::LINEAR)
			sampler_info.minFilter = VK_FILTER_LINEAR;
		else
			sampler_info.minFilter = VK_FILTER_NEAREST;

		if (_sampler.wrap_s == glTF::REPEAT)
			sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		else if (_sampler.wrap_s == glTF::CLAMP_TO_EDGE)
			sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		else if (_sampler.wrap_s == glTF::MIRRORED_REPEAT)
			sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;

		if(_sampler.wrap_t==glTF::REPEAT)
			sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		else if(_sampler.wrap_t==glTF::CLAMP_TO_EDGE)
			sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		else if(_sampler.wrap_t==glTF::MIRRORED_REPEAT)
			sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;

		sampler_info.anisotropyEnable = VK_TRUE;
		sampler_info.maxAnisotropy = 16;
		sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.compareEnable = VK_FALSE;
		sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
		sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		if (vkCreateSampler(device_, &sampler_info, nullptr, &sampler) != VK_SUCCESS)
			throw std::runtime_error("Failed to create texture sampler!");
		return sampler;
	}
	
	
	void DefferredPipelineType::CreateVertexBuffer(unsigned int thread_id,VkBuffer &vertex_buffer,VkDeviceSize buffer_size,void *buffer_data) {
		VkBuffer staging_buffer;
		VkDeviceMemory staging_buffer_memory;
		CreateBuffer(buffer_size,0, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);
		void *data;
		vkMapMemory(device_, staging_buffer_memory, 0, buffer_size, 0, &data);
		memcpy(data, buffer_data, (size_t)buffer_size);
		vkUnmapMemory(device_, staging_buffer_memory);
		CreateBuffer(buffer_size,0,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertex_buffer, vertex_buffer_memory_);
		CopyBuffer(thread_id,staging_buffer,vertex_buffer,buffer_size);
		vkDestroyBuffer(device_, staging_buffer, nullptr);
		vkFreeMemory(device_, staging_buffer_memory, nullptr);
	}

	void DefferredPipelineType::CreateIndexBuffer(unsigned int thread_id,VkBuffer &index_buffer,VkDeviceSize buffer_size,void *buffer_data) {
		VkBuffer staging_buffer;
		VkDeviceMemory staging_buffer_memory;
		CreateBuffer(buffer_size,0, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);
		void *data;
		vkMapMemory(device_, staging_buffer_memory, 0, buffer_size, 0, &data);
		memcpy(data,buffer_data, (size_t)buffer_size);
		vkUnmapMemory(device_, staging_buffer_memory);
		CreateBuffer(buffer_size, VK_BUFFER_CREATE_SPARSE_BINDING_BIT | VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, index_buffer, index_buffer_memory_);
		CopyBuffer(thread_id,staging_buffer, index_buffer, buffer_size);
		vkDestroyBuffer(device_, staging_buffer, nullptr);
		vkFreeMemory(device_, staging_buffer_memory, nullptr);
	}

	void DefferredPipelineType::CreateUniformBuffers() {
		VkDeviceSize buffer_size = sizeof(UniformBufferObject);
		uniform_buffers_.resize(swap_chain_images_.size());
		uniform_buffers_memory_.resize(swap_chain_images_.size());
		for (size_t i = 0; i < swap_chain_images_.size(); i++)
			CreateBuffer(buffer_size,0, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniform_buffers_[i], uniform_buffers_memory_[i]);
	}

	void DefferredPipelineType::UpdateUniformBuffer(uint32_t current_image) {
		static auto start_time = std::chrono::high_resolution_clock::now();
		auto current_time = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
		UniformBufferObject ubo = {};
//		ubo.model=math3d::Rotate
	}

	void DefferredPipelineType::CreateGraphicsPipeline() {
		auto vert_shader_code = ReadFile("common.vert.spv");
		auto frag_shader_code = ReadFile("common.frag.spv");
		VkShaderModule vert_shader_module = CreateShaderModule(vert_shader_code);
		VkShaderModule frag_shader_module = CreateShaderModule(frag_shader_code);
		VkPipelineShaderStageCreateInfo vert_shader_stage_info = {};
		vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vert_shader_stage_info.module = vert_shader_module;
		vert_shader_stage_info.pName = "main";
		VkPipelineShaderStageCreateInfo frag_shader_stage_info = {};
		frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		frag_shader_stage_info.module = frag_shader_module;
		frag_shader_stage_info.pName = "main";
		VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info,frag_shader_stage_info };
		VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
		vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		auto binding_description = Vertex::GetBindingDescription();
		auto attribute_description = Vertex::GetAttributeDescriptions();
		vertex_input_info.vertexBindingDescriptionCount = 1;
		vertex_input_info.vertexAttributeDescriptionCount = 1;
		vertex_input_info.pVertexBindingDescriptions = &binding_description;
		VkVertexInputAttributeDescription ad = {};
		ad.binding = 0;
		ad.format = VK_FORMAT_R32G32B32_SFLOAT;
		
	//	vertex_input_info.pVertexAttributeDescriptions = attribute_description.data();
		vertex_input_info.pVertexAttributeDescriptions = &ad;
		VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
		input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		input_assembly.primitiveRestartEnable = VK_FALSE;
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swap_chain_extent_.width;
		viewport.height = (float)swap_chain_extent_.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor = {};
		scissor.offset = { 0,0 };
		scissor.extent = swap_chain_extent_;
		VkPipelineViewportStateCreateInfo viewport_state = {};
		viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_state.viewportCount = 1;
		viewport_state.pViewports = &viewport;
		viewport_state.scissorCount = 1;
		viewport_state.pScissors = &scissor;
		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		VkPipelineMultisampleStateCreateInfo multi_sampling = {};
		multi_sampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multi_sampling.sampleShadingEnable = VK_FALSE;
		multi_sampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
		depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depth_stencil.depthTestEnable = VK_TRUE;
		depth_stencil.depthWriteEnable = VK_TRUE;
		depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depth_stencil.depthBoundsTestEnable = VK_FALSE;
		depth_stencil.stencilTestEnable = VK_FALSE;
		VkPipelineColorBlendAttachmentState color_blend_attachment = {};
		color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		color_blend_attachment.blendEnable = VK_FALSE;
		VkPipelineColorBlendStateCreateInfo color_blending = {};
		color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blending.logicOpEnable = VK_FALSE;
		color_blending.logicOp = VK_LOGIC_OP_COPY;
		color_blending.attachmentCount = 1;
		color_blending.pAttachments = &color_blend_attachment;
		color_blending.blendConstants[0] = 0.0f;
		color_blending.blendConstants[1] = 0.0f;
		color_blending.blendConstants[2] = 0.0f;
		color_blending.blendConstants[3] = 0.0f;
		VkPipelineLayoutCreateInfo pipeline_layout_info = {};
		pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_info.setLayoutCount = 0;
		pipeline_layout_info.pushConstantRangeCount=0;
		if (vkCreatePipelineLayout(device_, &pipeline_layout_info, nullptr, &pipeline_layout_) != VK_SUCCESS)
			throw std::runtime_error("Failed to create pipeline layout!");
		VkGraphicsPipelineCreateInfo pipeline_info = {};
		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.stageCount = 2;
		pipeline_info.pStages = shader_stages;
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &input_assembly;
		pipeline_info.pViewportState = &viewport_state;
		pipeline_info.pRasterizationState = &rasterizer;
		pipeline_info.pMultisampleState = &multi_sampling;
		pipeline_info.pDepthStencilState = &depth_stencil;
		pipeline_info.pColorBlendState = &color_blending;
		pipeline_info.layout = pipeline_layout_;
		pipeline_info.renderPass = render_pass_;
		pipeline_info.subpass = 0;
		pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
		if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &graphics_pipeline_) != VK_SUCCESS)
			throw std::runtime_error("Failed to create graphics pipeline!");
		vkDestroyShaderModule(device_, frag_shader_module, nullptr);
		vkDestroyShaderModule(device_, vert_shader_module, nullptr);
	}

	void DefferredPipelineType::DestroySwapChain() {
		for (auto framebuffer : swap_chain_framebuffers_)
			vkDestroyFramebuffer(device_, framebuffer, nullptr);
		unsigned int thread_count = GetThreadCount();
		for (unsigned int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			for (unsigned int j = 0; j < thread_count; j++)
				vkFreeCommandBuffers(device_, command_pools_[j], 1, &command_buffers_[i][j]);
			vkFreeCommandBuffers(device_, command_pools_[0], 1, &clear_command_buffers_[i]);
		}
		vkDestroyPipeline(device_, graphics_pipeline_, nullptr);
		vkDestroyPipelineLayout(device_, pipeline_layout_, nullptr);
		vkDestroyRenderPass(device_, render_pass_, nullptr);
		for (auto image_view : swap_chain_image_views_)
			vkDestroyImageView(device_, image_view, nullptr);
		vkDestroySwapchainKHR(device_, swap_chain_, nullptr);
	}

	void DefferredPipelineType::ReCreateSwapChain() {
		std::cout << "re create swap chain..." << std::endl;
		int width, height;
		width = height = 0;
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(window_, &width, &height);
			glfwPollEvents();
		}
		std::cout << "wait event" << std::endl;
		vkDeviceWaitIdle(device_);
		DestroySwapChain();
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateGraphicsPipeline();
		CreateFramebuffers();
		CreateCommandBuffers();
	}

	void DefferredPipelineType::InitVulkan() {
		std::cout << "init vulkan..." << std::endl;
		CreateInstance();
		InitDebugMessenger();
		CreateSurface();
		SelectPhysicalDevice();
		OutputGPUInfo();
		CreateLogicalDevice();
		CreateSwapChain();
		CreateImageViews();
		CreateCommandPools();
		CreateDepthBuffers();
		CreateRenderPass();
		CreateGraphicsPipeline();
		CreateFramebuffers();
		CreateCommandBuffers();
		CreateSyncObjects();
		CreateDescriptorPools();
	}

	void DefferredPipelineType::BeginCommand(unsigned int thread_id) {
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		if (vkBeginCommandBuffer(command_buffers_[current_frame_][thread_id], &begin_info) != VK_SUCCESS)
			throw std::runtime_error("Failed to begin recording command buffer!");
	}

	void DefferredPipelineType::EndCommand(unsigned int thread_id) {
		if (vkEndCommandBuffer(command_buffers_[current_frame_][thread_id]) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer!");
	}

	void DefferredPipelineType::SubmitCommand(unsigned int thread_id) {
		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.waitSemaphoreCount = 0;
		submit_info.pWaitSemaphores = nullptr;
		submit_info.pWaitDstStageMask = nullptr;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffers_[current_frame_][thread_id];
		submit_info.signalSemaphoreCount = 0;
		submit_info.pSignalSemaphores = nullptr;
		if (vkQueueSubmit(graphics_queues_[thread_id], 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS)
			throw std::runtime_error("Faile to submit draw command buffer!");
	}

	void DefferredPipelineType::SubmitCommand(unsigned int thread_id,VkSemaphore *wait_semaphore, VkSemaphore *signal_semaphore) {
		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		if (wait_semaphore == nullptr) {
			submit_info.waitSemaphoreCount = 0;
			submit_info.pWaitSemaphores = nullptr;
			submit_info.pWaitDstStageMask = nullptr;
		}
		else {
			submit_info.waitSemaphoreCount = 1;
			submit_info.pWaitSemaphores = wait_semaphore;
			submit_info.pWaitDstStageMask = wait_stages;
		}
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffers_[current_frame_][thread_id];
		if (signal_semaphore == nullptr) {
			submit_info.signalSemaphoreCount = 0;
			submit_info.pSignalSemaphores = nullptr;
		}
		else{
			submit_info.signalSemaphoreCount = 1;
			submit_info.pSignalSemaphores = signal_semaphore;
		}
		if (vkQueueSubmit(graphics_queues_[thread_id], 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS)
			throw std::runtime_error("Faile to submit draw command buffer!");
	}

	void DefferredPipelineType::SubmitCommand(unsigned int thread_id, unsigned int wait_semaphore_count,
		VkSemaphore *wait_semaphores,unsigned int signal_semaphore_count, VkSemaphore *signal_semaphores) {
		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		std::vector< VkPipelineStageFlags> wait_stages;
		if (wait_semaphores == nullptr) {
			submit_info.waitSemaphoreCount = 0;
			submit_info.pWaitSemaphores = nullptr;
			submit_info.pWaitDstStageMask = nullptr;
		}
		else {
			submit_info.waitSemaphoreCount = wait_semaphore_count;
			wait_stages.resize(wait_semaphore_count);
			for (auto i = 0; i < wait_semaphore_count; i++)
				wait_stages[i] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			submit_info.pWaitSemaphores = wait_semaphores;
			submit_info.pWaitDstStageMask = wait_stages.data();
		}
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffers_[current_frame_][thread_id];
		if (signal_semaphores == nullptr) {
			submit_info.signalSemaphoreCount = 0;
			submit_info.pSignalSemaphores = nullptr;
		}
		else {
			submit_info.signalSemaphoreCount = signal_semaphore_count;
			submit_info.pSignalSemaphores = signal_semaphores;
		}
		if (vkQueueSubmit(graphics_queues_[thread_id], 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS)
			throw std::runtime_error("Faile to submit draw command buffer!");
	}
	
	void DefferredPipelineType::WaitFrameEnd() {
		if (need_recreate_swap_chain_ == true || framebuffer_resized_==true) {
			ReCreateSwapChain();
			need_recreate_swap_chain_ = false;
			framebuffer_resized_ = false;
		}
		
		do {
			vkWaitForFences(device_, 1, &in_flight_fences_[current_frame_], VK_TRUE,
				std::numeric_limits<uint64_t>::max());
			VkResult result = vkAcquireNextImageKHR(device_, swap_chain_, std::numeric_limits<uint64_t>::max(),
				image_available_semaphores_[current_frame_], VK_NULL_HANDLE, &current_swap_chain_image_index_);
			if (result == VK_ERROR_OUT_OF_DATE_KHR) {
				ReCreateSwapChain();
				need_recreate_swap_chain_ = false;
				framebuffer_resized_ = false;
			}
			else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
				throw std::runtime_error("Failed to acquire swap chain image!");
			break;
		} while (true);
	}

	void DefferredPipelineType::ClearCommand(unsigned int thread_id) {
		VkRenderPassBeginInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = render_pass_;
		render_pass_info.framebuffer = swap_chain_framebuffers_[current_swap_chain_image_index_];
		render_pass_info.renderArea.offset = { 0,0 };
		render_pass_info.renderArea.extent = swap_chain_extent_;
		render_pass_info.clearValueCount = 0;
		render_pass_info.pClearValues = nullptr;
		vkCmdBeginRenderPass(clear_command_buffers_[current_frame_], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(clear_command_buffers_[current_frame_], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);
		VkClearAttachment clear_color_attachment = {};
		clear_color_attachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		clear_color_attachment.colorAttachment = 0;
		clear_color_attachment.clearValue.color.float32[0] = 0.0;
		VkClearAttachment clear_depth_attachment = {};
		clear_depth_attachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		clear_depth_attachment.clearValue.color.float32[0] = 1.0;
		VkClearRect clear_rect;
		clear_rect.layerCount = 1;
		clear_rect.baseArrayLayer = 0;
		clear_rect.rect.offset = { 0,0 };
		clear_rect.rect.extent = swap_chain_extent_;
		VkClearAttachment clear_attachments[] = { clear_color_attachment,clear_depth_attachment };
		vkCmdClearAttachments(clear_command_buffers_[current_frame_], 2, clear_attachments, 1, &clear_rect);
		vkCmdEndRenderPass(clear_command_buffers_[current_frame_]);
	}

	void DefferredPipelineType::DrawCommand(unsigned int thread_id,PrimitiveBindInfo &primitive_bind_info) {
		VkRenderPassBeginInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = render_pass_;
		render_pass_info.framebuffer = swap_chain_framebuffers_[current_swap_chain_image_index_];
		render_pass_info.renderArea.offset = { 0,0 };
		render_pass_info.renderArea.extent = swap_chain_extent_;
		render_pass_info.clearValueCount = 0;
		render_pass_info.pClearValues = nullptr;
		vkCmdBeginRenderPass(command_buffers_[current_frame_][thread_id], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(command_buffers_[current_frame_][thread_id], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);
		vkCmdBindVertexBuffers(command_buffers_[current_frame_][thread_id], 0, primitive_bind_info.buffers.size(),
			primitive_bind_info.buffers.data(), primitive_bind_info.offsets.data());
		for (auto i = 0; i < primitive_bind_info.buffers.size(); i++)
			std::cout <<"ver:"<< primitive_bind_info.offsets[i] << std::endl;
		vkCmdBindIndexBuffer(command_buffers_[current_frame_][thread_id], primitive_bind_info.index_buffer, primitive_bind_info.index_buffer_offset, VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(command_buffers_[current_frame_][thread_id], primitive_bind_info.index_count,1,0,0,0);
		vkCmdEndRenderPass(command_buffers_[current_frame_][thread_id]);
	}

	void DefferredPipelineType::Present() {

	}

	void DefferredPipelineType::MakeInverseBindMatrices(unsigned int thread_id, glTF *gltf, glTF::Accessor *accessor, std::vector <math3d::Matrix> &mats) {
		auto &data = gltf->GetData();
		auto buffer_view_index = accessor->buffer_view;
		auto &buffer_view = data.buffer_views[buffer_view_index];
		unsigned char *mats_data =(unsigned char *)data.buffers[buffer_view.buffer].data + buffer_view.byte_offset + accessor->byte_offset;
		auto count = accessor->count;
		memcpy(mats.data(), mats_data ,sizeof(math3d::Matrix)*count);
	}

	void DefferredPipelineType::LoadglTFSource(unsigned int thread_id,const std::string &source, RenderObjectInfo &render_object_info) {
		glTF *gltf = new glTF();
		render_object_info.data1 = gltf;
		gltf->LoadSource(source);
		glTF::Data &data = gltf->GetData();
		auto buffer_count = data.buffers.size();
		render_object_info.buffers.resize(buffer_count);
		for (auto i = 0; i < buffer_count; i++) {
			unsigned int buffer_index=AddBuffer(thread_id, data.buffers[i].data, data.buffers[i].byte_length);
			render_object_info.buffers[i] = buffer_index;
		}
		auto animation_count = data.animations.size();
		render_object_info.animations.resize(animation_count);
		for (auto i = 0; i < animation_count; i++) {
			render_object_info.animations[i].loop = true;
			render_object_info.animations[i].run = true;
			render_object_info.animations[i].animation_id = i;
			QueryPerformanceCounter(&render_object_info.animations[i].start_tick);
		}
		auto skin_count = data.skins.size();
		auto node_count = data.nodes.size();
		render_object_info.skins.resize(skin_count);
		render_object_info.nodes.resize(node_count);
		for (auto i = 0; i < skin_count; i++) {
			auto &skin = data.skins[i];
			auto joint_count = skin.joints.size();
			render_object_info.skins[i].inverse_bind_mats.resize(joint_count);
			render_object_info.skins[i].joint_mats.resize(joint_count);
			glTF::Accessor *inverse_bind_matrices_accessor = &data.accessors[skin.inverse_bind_materices];
			MakeInverseBindMatrices(thread_id, gltf, inverse_bind_matrices_accessor, render_object_info.skins[i].inverse_bind_mats);
			for (auto j = 0; j < joint_count; j++) {
				auto joint_node_index = skin.joints[j];
				JointNodeInfo joint_node_info;
				joint_node_info.target_skin = i;
				joint_node_info.joint_index = j;
				render_object_info.nodes[joint_node_index].skin_targets.push_back(joint_node_info);
			}
		}
		for (auto i = 0; i < node_count; i++) {
			auto &node = data.nodes[i];
			if (node.use_skin) {
				auto skin_index = node.skin;
				auto &skin = render_object_info.skins[skin_index];
				skin.node_targets.push_back(i);
				render_object_info.nodes[i].joint_mats.resize(skin.joint_mats.size());
			}
		}
		auto texture_count = data.textures.size();
		render_object_info.textures.resize(texture_count);
		for (auto i = 0; i < texture_count; i++) {
			auto &texture = data.textures[i];
			auto &image = data.images[texture.source];
			auto &sampler = data.samplers[texture.sampler];
			render_object_info.textures[i].image=CreateTextureImage(thread_id, image.width, image.height, image.data);
			render_object_info.textures[i].image_view = CreateImageView(render_object_info.textures[i].image, VK_FORMAT_R8G8B8A8_UNORM);
			render_object_info.textures[i].sampler = CreateTextureSampler(sampler);
		}
		auto mesh_count = data.meshes.size();
		for (auto i = 0; i < mesh_count; i++) {
			auto primitive_count = data.meshes[i].primitives.size();
			for (auto j = 0; j < primitive_count; j++) {
				auto &primitive = data.meshes[i].primitives[j];
				unsigned int attribute_mask = gltf->GetAttributeMask(primitive.attributes);
				glTF::PipelineBindInfo pipeline_bind_info;
				gltf->MakePipelineBindInfo(pipeline_bind_info, primitive);
				VkPipeline pipeline;
				char temp[256];
				std::string shader_name = "common";
				shader_name+= itoa(attribute_mask, temp, 10);
				auto vert_shader_code = ReadFile(shader_name+".vert.spv");
				auto frag_shader_code = ReadFile(shader_name+".frag.spv");
				VkShaderModule vert_shader_module = CreateShaderModule(vert_shader_code);
				VkShaderModule frag_shader_module = CreateShaderModule(frag_shader_code);
				VkPipelineShaderStageCreateInfo vert_shader_stage_info = {};
				vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
				vert_shader_stage_info.module = vert_shader_module;
				vert_shader_stage_info.pName = "main";
				VkPipelineShaderStageCreateInfo frag_shader_stage_info = {};
				frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				frag_shader_stage_info.module = frag_shader_module;
				frag_shader_stage_info.pName = "main";
				VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info,frag_shader_stage_info };
				VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
				vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
				vertex_input_info.vertexBindingDescriptionCount = pipeline_bind_info.count;
				vertex_input_info.vertexAttributeDescriptionCount = pipeline_bind_info.count;
				vertex_input_info.pVertexBindingDescriptions = pipeline_bind_info.bindings.data();
				vertex_input_info.pVertexAttributeDescriptions = pipeline_bind_info.attributes.data();
				VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
				input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
				input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				input_assembly.primitiveRestartEnable = VK_FALSE;
				VkViewport viewport = {};
				viewport.x = 0.0f;
				viewport.y = 0.0f;
				viewport.width = (float)swap_chain_extent_.width;
				viewport.height = (float)swap_chain_extent_.height;
				viewport.minDepth = 0.0f;
				viewport.maxDepth = 1.0f;
				VkRect2D scissor = {};
				scissor.offset = { 0,0 };
				scissor.extent = swap_chain_extent_;
				VkPipelineViewportStateCreateInfo viewport_state = {};
				viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
				viewport_state.viewportCount = 1;
				viewport_state.pViewports = &viewport;
				viewport_state.scissorCount = 1;
				viewport_state.pScissors = &scissor;
				VkPipelineRasterizationStateCreateInfo rasterizer = {};
				rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
				rasterizer.depthClampEnable = VK_FALSE;
				rasterizer.rasterizerDiscardEnable = VK_FALSE;
				rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
				rasterizer.lineWidth = 1.0f;
				rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
				rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
				rasterizer.depthBiasEnable = VK_FALSE;
				VkPipelineMultisampleStateCreateInfo multi_sampling = {};
				multi_sampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				multi_sampling.sampleShadingEnable = VK_FALSE;
				multi_sampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
				VkPipelineColorBlendAttachmentState color_blend_attachment = {};
				color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
					VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
				color_blend_attachment.blendEnable = VK_FALSE;
				VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
				depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
				depth_stencil.depthTestEnable = VK_TRUE;
				depth_stencil.depthWriteEnable = VK_TRUE;
				depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
				depth_stencil.depthBoundsTestEnable = VK_FALSE;
				depth_stencil.stencilTestEnable = VK_FALSE;
				VkPipelineColorBlendStateCreateInfo color_blending = {};
				color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
				color_blending.logicOpEnable = VK_FALSE;
				color_blending.logicOp = VK_LOGIC_OP_COPY;
				color_blending.attachmentCount = 1;
				color_blending.pAttachments = &color_blend_attachment;
				color_blending.blendConstants[0] = 0.0f;
				color_blending.blendConstants[1] = 0.0f;
				color_blending.blendConstants[2] = 0.0f;
				color_blending.blendConstants[3] = 0.0f;
				VkDescriptorSetLayoutBinding descriptor_set_layout_binding0 = {};
				descriptor_set_layout_binding0.binding = 0;
				descriptor_set_layout_binding0.descriptorCount = 1;
				descriptor_set_layout_binding0.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptor_set_layout_binding0.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
				descriptor_set_layout_binding0.pImmutableSamplers = nullptr;
				VkDescriptorSetLayoutBinding descriptor_set_layout_binding1 = {};
				descriptor_set_layout_binding1.binding = 1;
				descriptor_set_layout_binding1.descriptorCount = 1;
				descriptor_set_layout_binding1.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptor_set_layout_binding1.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
				descriptor_set_layout_binding1.pImmutableSamplers = nullptr;
				VkDescriptorSetLayoutBinding descriptor_set_layout_binding2 = {};
				descriptor_set_layout_binding2.binding = 2;
				descriptor_set_layout_binding2.descriptorCount = 1;
				descriptor_set_layout_binding2.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptor_set_layout_binding2.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
				descriptor_set_layout_binding2.pImmutableSamplers = nullptr;
				VkDescriptorSetLayoutBinding descriptor_set_layout_bindings[] = {descriptor_set_layout_binding0,descriptor_set_layout_binding1,
					descriptor_set_layout_binding2};
				VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info= {};
				descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				descriptor_set_layout_create_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
				descriptor_set_layout_create_info.bindingCount = 3;
				descriptor_set_layout_create_info.pBindings = descriptor_set_layout_bindings;
				VkDescriptorSetLayout descriptor_set_layout;
				vkCreateDescriptorSetLayout(device_, &descriptor_set_layout_create_info, nullptr, &descriptor_set_layout);
				descriptor_set_layouts_.push_back(descriptor_set_layout);
				VkPipelineLayoutCreateInfo pipeline_layout_info = {};
				pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
				pipeline_layout_info.setLayoutCount = 1;
				pipeline_layout_info.pSetLayouts = &descriptor_set_layout;
				pipeline_layout_info.pushConstantRangeCount = 1;
				pipeline_layout_info.pPushConstantRanges = &pipeline_bind_info.push_constant_range;
				VkPipelineLayout pipeline_layout;
				if (vkCreatePipelineLayout(device_, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS)
					throw std::runtime_error("Failed to create pipeline layout!");
				pipeline_layouts_.push_back(pipeline_layout);
				VkGraphicsPipelineCreateInfo pipeline_info = {};
				pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				pipeline_info.stageCount = 2;
				pipeline_info.pStages = shader_stages;
				pipeline_info.pVertexInputState = &vertex_input_info;
				pipeline_info.pInputAssemblyState = &input_assembly;
				pipeline_info.pViewportState = &viewport_state;
				pipeline_info.pRasterizationState = &rasterizer;
				pipeline_info.pMultisampleState = &multi_sampling;
				pipeline_info.pDepthStencilState = &depth_stencil;
				pipeline_info.pColorBlendState = &color_blending;
				pipeline_info.layout = pipeline_layout;
				pipeline_info.renderPass = render_pass_;
				pipeline_info.subpass = 0;
				pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
				if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS)
					throw std::runtime_error("Failed to create graphics pipeline!");
				vkDestroyShaderModule(device_, frag_shader_module, nullptr);
				vkDestroyShaderModule(device_, vert_shader_module, nullptr);
				primitive.pipeline_id = AddGraphicPipeline(thread_id, pipeline);
				MakePrimitiveBindInfo(thread_id, gltf, &primitive,descriptor_set_layout, render_object_info);
			}
		}
	}

	DefferredPipelineType::AccessorInfo DefferredPipelineType::GetAccessorInfo(unsigned int thread_id, glTF *gltf,glTF::Accessor *accessor,
		RenderObjectInfo &render_object_info) {
		AccessorInfo accessor_info;
		auto buffer_view_index = accessor->buffer_view;
		auto &buffer_view = gltf->GetData().buffer_views[buffer_view_index];
		auto buffer_index = render_object_info.buffers[buffer_view.buffer];
		if(accessor->is_sparse){
			auto &buffer = gltf->GetData().buffers[buffer_view.buffer];
			unsigned char *buffer_data = new unsigned char[buffer.byte_length];
			memcpy(buffer_data, buffer.data, buffer.byte_length);
			auto &sparse = accessor->sparse;
			auto &indices_buffer_view = gltf->GetData().buffer_views[sparse.indices.buffer_view];
			auto &indices_buffer= gltf->GetData().buffers[indices_buffer_view.buffer];
			unsigned char *indices_data = indices_buffer.data+ sparse.indices.byte_offset+indices_buffer_view.byte_offset;
			auto &values_buffer_view = gltf->GetData().buffer_views[sparse.values.buffer_view];
			auto &values_buffer= gltf->GetData().buffers[gltf->GetData().buffer_views[sparse.values.buffer_view].buffer];
			unsigned char *values_data = values_buffer.data + sparse.values.byte_offset+values_buffer_view.byte_offset;
			unsigned int type_size = gltf->GetFormatByteSize(accessor->type, accessor->component_type);
			for (auto i = 0; i < sparse.count; i++) {
				if (sparse.indices.component_type == glTF::UNSIGNED_BYTE) {

				}
				else if (sparse.indices.component_type == glTF::UNSIGNED_SHORT) {
					unsigned int index = *(unsigned short *)&indices_data[i*glTF::UNSIGNED_SHORT_SIZE];
					memcpy(buffer_data + accessor->byte_offset +buffer_view.byte_offset + index * type_size,
						values_data + i * type_size , type_size);
				}
				else if (sparse.indices.component_type == glTF::UNSIGNED_INT) {

				}
			}
			unsigned int buffer_index = AddBuffer(thread_id, buffer_data, buffer.byte_length);
			accessor_info.buffer = buffers_[buffer_index];
		}
		else {
			accessor_info.buffer = buffers_[buffer_index];
		}
		accessor_info.offset = accessor->byte_offset + buffer_view.byte_offset;

		return accessor_info;
	}

	void DefferredPipelineType::MakePrimitiveBindInfo(unsigned int thread_id,glTF *gltf, glTF::Primitive *primitive,
		VkDescriptorSetLayout descriptor_set_layout,
		RenderObjectInfo &render_object_info) {
		PrimitiveBindInfo primitive_bind_info;
		AccessorInfo accessor_info;
		auto attribute_count = primitive->attributes.size();
		for (auto i = 0; i < attribute_count; i++) {
			auto &name = primitive->attributes[i].name;
			auto accessor_index = primitive->attributes[i].value;
			if (name == "POSITION") {
				accessor_info = GetAccessorInfo(thread_id, gltf, &gltf->GetData().accessors[accessor_index], render_object_info);
			}
			else if (name == "NORMAL") {
				accessor_info = GetAccessorInfo(thread_id, gltf, &gltf->GetData().accessors[accessor_index], render_object_info);
			}
			else if (name == "TANGENT") {
				accessor_info = GetAccessorInfo(thread_id, gltf, &gltf->GetData().accessors[accessor_index], render_object_info);
			}
			else if (name == "TEXCOORD_0") {
				accessor_info = GetAccessorInfo(thread_id, gltf, &gltf->GetData().accessors[accessor_index], render_object_info);
			}
			else if (name == "TEXCOORD_1") {
				accessor_info = GetAccessorInfo(thread_id, gltf, &gltf->GetData().accessors[accessor_index], render_object_info);
			}
			else if (name == "COLOR_0") {
				accessor_info = GetAccessorInfo(thread_id, gltf, &gltf->GetData().accessors[accessor_index], render_object_info);
			}
			else if (name == "JOINTS_0") {
				accessor_info = GetAccessorInfo(thread_id, gltf, &gltf->GetData().accessors[accessor_index], render_object_info);
			}
			else if (name == "WEIGHTS_0") {
				accessor_info = GetAccessorInfo(thread_id, gltf, &gltf->GetData().accessors[accessor_index], render_object_info);
			}
			primitive_bind_info.buffers.push_back(accessor_info.buffer);
			primitive_bind_info.offsets.push_back(accessor_info.offset);
		}
		auto index_accessor = primitive->indices;
		accessor_info = GetAccessorInfo(thread_id, gltf, &gltf->GetData().accessors[index_accessor], render_object_info);
		primitive_bind_info.index_buffer = accessor_info.buffer;
		primitive_bind_info.index_buffer_offset = accessor_info.offset;
		primitive_bind_info.index_count = gltf->GetData().accessors[index_accessor].count;
		if (primitive->use_material) {
			auto index_material = primitive->material;
			primitive_bind_info.material = &gltf->GetData().materials[index_material];
		}
		else
			primitive_bind_info.material = nullptr;
		for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			primitive_bind_info.descriptor_sets[i].resize(1);
			primitive_bind_info.descriptor_set_buffers[i].resize(1);
			primitive_bind_info.descriptor_set_memories[i].resize(1);
			VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {};
			descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptor_set_allocate_info.descriptorPool = descriptor_pools_[0][0];
			descriptor_set_allocate_info.descriptorSetCount = 1;
			descriptor_set_allocate_info.pSetLayouts = &descriptor_set_layout;
			CreateBuffer(sizeof(glTF::pbrMetallicRoughness), 0, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, primitive_bind_info.descriptor_set_buffers[i][0], 
				primitive_bind_info.descriptor_set_memories[i][0]);
			if (vkAllocateDescriptorSets(device_, &descriptor_set_allocate_info, primitive_bind_info.descriptor_sets[i].data()) != VK_SUCCESS)
				throw std::runtime_error("Failed to allocate descriptor sets!");
		}
		render_object_info.primitives.push_back(primitive_bind_info);
		primitive->primitive_bind_info_id = render_object_info.primitives.size() - 1;
	}

	unsigned int DefferredPipelineType::AddGraphicPipeline(unsigned int thread_id, VkPipeline graphics_pipeline) {
		graphics_pipelines_.push_back(graphics_pipeline);
		return graphics_pipelines_.size() - 1;
	}

	void DefferredPipelineType::DrawMesh(unsigned int thread_id, glTF *gltf,NodeInfo *node_info,glTF::Mesh *mesh,RenderObjectInfo &render_object_info) {
		auto primitive_count = mesh->primitives.size();
		for (auto i = 0; i < primitive_count; i++) {
			auto &primitive = mesh->primitives[i];
			auto &primitive_bind_info = render_object_info.primitives[primitive.primitive_bind_info_id];
			VkRenderPassBeginInfo render_pass_info = {};
			render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			render_pass_info.renderPass = render_pass_;
			render_pass_info.framebuffer = swap_chain_framebuffers_[current_swap_chain_image_index_];
			render_pass_info.renderArea.offset = { 0,0 };
			render_pass_info.renderArea.extent = swap_chain_extent_;
			render_pass_info.clearValueCount = 0;
			render_pass_info.pClearValues = nullptr;
			vkCmdBeginRenderPass(command_buffers_[current_frame_][thread_id], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(command_buffers_[current_frame_][thread_id], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipelines_[primitive.pipeline_id]);
			/*if (primitive.use_material) {
				VkDescriptorBufferInfo descriptor_buffer_info = {};
				VkDeviceMemory buffer_memory;
				VkBuffer descriptor_buffer;
				CreateBuffer(sizeof(glTF::pbrMetallicRoughness), 0, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
					VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, descriptor_buffer, buffer_memory);
				void *data;
				vkMapMemory(device_, buffer_memory, 0, sizeof(glTF::pbrMetallicRoughness), 0, &data);
				memcpy(data, &primitive_bind_info.material->pbr_metallic_roughness, sizeof(glTF::pbrMetallicRoughness));
				vkUnmapMemory(device_, buffer_memory);
				descriptor_buffer_info.buffer = descriptor_buffer;
				descriptor_buffer_info.offset = 0;
				descriptor_buffer_info.range = sizeof(glTF::pbrMetallicRoughness);
				VkWriteDescriptorSet write_descriptor_set = {};
				write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				write_descriptor_set.dstSet = primitive_bind_info.descriptor_sets[0];
				write_descriptor_set.descriptorCount = 1;
				write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				write_descriptor_set.dstBinding = 0;
				write_descriptor_set.dstArrayElement = 0;
				write_descriptor_set.pBufferInfo = &descriptor_buffer_info;
				vkUpdateDescriptorSets(device_, 1, &write_descriptor_set, 0, nullptr);
				vkDestroyBuffer(device_, descriptor_buffer, nullptr);
				vkFreeMemory(device_, buffer_memory, nullptr);
				if (primitive_bind_info.material->pbr_metallic_roughness.texture_switch[0]) {
					VkDescriptorImageInfo image_info = {};
					auto texture_index = primitive_bind_info.material->base_color_texture.index;
					image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					image_info.imageView = render_object_info.textures[texture_index].image_view;
					image_info.sampler = render_object_info.textures[texture_index].sampler;
					write_descriptor_set = {};
					write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					write_descriptor_set.dstSet = primitive_bind_info.descriptor_sets[0];
					write_descriptor_set.descriptorCount = 1;
					write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					write_descriptor_set.dstBinding = 1;
					write_descriptor_set.dstArrayElement = 0;
					write_descriptor_set.pImageInfo = &image_info;
					vkUpdateDescriptorSets(device_,1, &write_descriptor_set,0,nullptr);
				}
			}*/
			vkCmdBindDescriptorSets(command_buffers_[current_frame_][thread_id], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layouts_[primitive.pipeline_id],
				0,1,&primitive_bind_info.descriptor_sets[current_frame_][0],0,nullptr);
			vkCmdPushConstants(command_buffers_[current_frame_][thread_id], pipeline_layouts_[primitive.pipeline_id], VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(math3d::Matrix),render_object_info.matrix.mat);
			if(node_info->joint_mats.size())
				vkCmdPushConstants(command_buffers_[current_frame_][thread_id], pipeline_layouts_[primitive.pipeline_id], VK_SHADER_STAGE_VERTEX_BIT, sizeof(math3d::Matrix), node_info->joint_mats.size()*sizeof(math3d::Matrix), node_info->joint_mats.data());
			vkCmdBindVertexBuffers(command_buffers_[current_frame_][thread_id], 0, primitive_bind_info.buffers.size(),
				primitive_bind_info.buffers.data(),primitive_bind_info.offsets.data());
			vkCmdBindIndexBuffer(command_buffers_[current_frame_][thread_id], primitive_bind_info.index_buffer, primitive_bind_info.index_buffer_offset, VK_INDEX_TYPE_UINT16);
			vkCmdDrawIndexed(command_buffers_[current_frame_][thread_id], primitive_bind_info.index_count, 1, 0, 0, 0);
			vkCmdEndRenderPass(command_buffers_[current_frame_][thread_id]);
		}
	}

	void DefferredPipelineType::DrawNode(unsigned int thread_id, glTF *gltf, glTF::Node *node,NodeInfo *node_info,RenderObjectInfo &render_object_info) {
		math3d::Matrix base_matrix = render_object_info.matrix;
		render_object_info.matrix = base_matrix * node->matrix;
		if (node->use_mesh) {
			auto &mesh = gltf->GetData().meshes[node->mesh];
			DrawMesh(thread_id, gltf,node_info, &mesh, render_object_info);
		}
		auto children_count = node->children.size();
		for(auto i=0;i<children_count;i++){
			auto node_index = node->children[i];
			DrawNode(thread_id, gltf, &gltf->GetData().nodes[node_index], &render_object_info.nodes[node_index],render_object_info);
		}
		render_object_info.matrix = base_matrix;
	}

	void DefferredPipelineType::DrawObject(unsigned int thread_id, RenderObjectInfo &render_object_info) {
		glTF *gltf = (glTF *)render_object_info.data1;
		auto node_count = gltf->GetData().scenes[0].nodes.size();
		for (auto i = 0; i < node_count; i++) {
			auto node_index = gltf->GetData().scenes[0].nodes[i];
			DrawNode(thread_id, gltf, &gltf->GetData().nodes[node_index], &render_object_info.nodes[node_index],render_object_info);
		}
	}

	DefferredPipelineType::AnimationSamplerInfo DefferredPipelineType::GetAnimationSamplerInfo(unsigned int thread_id, glTF *gltf,
		unsigned int input,unsigned int output) {
		AnimationSamplerInfo animation_sampler_info;
		auto &data = gltf->GetData();
		auto &input_accessor=data.accessors[input];
		auto &output_accessor=data.accessors[output];
		auto &input_buffer_view = data.buffer_views[input_accessor.buffer_view];
		auto &output_buffer_view = data.buffer_views[output_accessor.buffer_view];
		auto &input_buffer = data.buffers[input_buffer_view.buffer];
		auto &output_buffer = data.buffers[output_buffer_view.buffer];
		animation_sampler_info.count = input_accessor.count;
		animation_sampler_info.input_type = input_accessor.type;
		animation_sampler_info.input_component_type = input_accessor.component_type;
		animation_sampler_info.output_type = output_accessor.type;
		animation_sampler_info.output_component_type = output_accessor.component_type;
		animation_sampler_info.input_values = input_buffer.data + input_accessor.byte_offset + input_buffer_view.byte_offset;
		animation_sampler_info.output_values = output_buffer.data + output_accessor.byte_offset + output_buffer_view.byte_offset;
		return animation_sampler_info;
	}

	void DefferredPipelineType::UpdateAnimation(unsigned int thread_id, glTF *gltf, RenderObjectInfo &render_object_info) {
		auto animation_count = render_object_info.animations.size();
		auto &data = gltf->GetData();
		LARGE_INTEGER now_tick;
		LARGE_INTEGER tick;
		QueryPerformanceCounter(&now_tick);
		for (auto i = 0; i < animation_count; i++) {
			auto &animation = render_object_info.animations[i];
			if (animation.last_tick.QuadPart > now_tick.QuadPart)
				tick.QuadPart = animation.last_tick.QuadPart;
			else
				tick.QuadPart = now_tick.QuadPart;
			animation.last_tick.QuadPart = tick.QuadPart;
			float dur_tick = (tick.QuadPart - animation.start_tick.QuadPart)/(float)timer_frequency.QuadPart;
			if (!animation.run) continue;
			auto &animation_info = data.animations[animation.animation_id];
			auto &samplers = animation_info.samplers;
			auto channel_count = animation_info.channels.size();
			for (auto j = 0; j < channel_count; j++) {
				auto &channel = animation_info.channels[j];
				auto &sampler = animation_info.samplers[channel.sampler];
				auto &target_node = data.nodes[channel.target.node];
				float last_key_frame_tick, next_key_frame_tick;
				AnimationSamplerInfo animation_sampler_info = GetAnimationSamplerInfo(thread_id, gltf, sampler.input, sampler.output);
				if (channel.target.path == "rotation") {
					target_node.use_rotation = true;
					int key_frame=-1;
					if (animation_sampler_info.input_component_type == glTF::FLOAT && animation_sampler_info.input_type == glTF::SCALAR) {
						float *values = (float *)animation_sampler_info.input_values;
						for (auto k = 0; k < animation_sampler_info.count; k++) {
							if (k + 1 < animation_sampler_info.count) {
								if (values[k] <= dur_tick && values[k+1] >= dur_tick) {
									last_key_frame_tick = values[k];
									next_key_frame_tick = values[k + 1];
									key_frame = k;
									break;
								}
							}
						}
					}
					if (key_frame == -1) key_frame = animation_sampler_info.count - 1;
					if (animation_sampler_info.output_component_type == glTF::FLOAT && animation_sampler_info.output_type == glTF::VEC4) {
						float *values = (float *)animation_sampler_info.output_values;
						if (key_frame == animation_sampler_info.count - 1) {
							target_node.rotation[0] = values[key_frame * 4];
							target_node.rotation[1] = values[key_frame * 4 + 1];
							target_node.rotation[2] = values[key_frame * 4 + 2];
							target_node.rotation[3] = values[key_frame * 4 + 3];
						}
						else {
							math3d::Vector3D q, r;
							q.x = values[key_frame * 4]; q.y = values[key_frame * 4 + 1]; q.z = values[key_frame * 4 + 2]; q.w = values[key_frame * 4 + 3];
							r.x = values[(key_frame+1) * 4]; r.y = values[(key_frame +1)* 4 + 1]; r.z = values[(key_frame +1)* 4 + 2]; r.w = values[(key_frame+1) * 4 + 3];
							float t = (dur_tick-last_key_frame_tick) / (next_key_frame_tick - last_key_frame_tick);
							if (t > 1) t = 1;
							if (t < 0) t = 0;
							math3d::Vector3D result = Slerp(q, r, t);
							
							target_node.rotation[0] = result.x;
							target_node.rotation[1] = result.y;
							target_node.rotation[2] = result.z;
							target_node.rotation[3] = result.w;
						}
					}
					if (animation.loop && key_frame >= animation_sampler_info.count - 1)
						QueryPerformanceCounter(&animation.start_tick);
					target_node.UpdateMatrix();	
				}
				else if (channel.target.path == "translation") {
					target_node.use_translation = true;
					int key_frame = -1;
					if (animation_sampler_info.input_component_type == glTF::FLOAT && animation_sampler_info.input_type == glTF::SCALAR) {
						float *values = (float *)animation_sampler_info.input_values;
						for (auto k = 0; k < animation_sampler_info.count; k++) {
							if (k + 1 < animation_sampler_info.count) {
								if (values[k] <= dur_tick && values[k + 1] >= dur_tick) {
									last_key_frame_tick = values[k];
									next_key_frame_tick = values[k + 1];
									key_frame = k;
									break;
								}
							}
						}
					}
					if (key_frame == -1) key_frame = animation_sampler_info.count - 1;
					if (animation_sampler_info.output_component_type == glTF::FLOAT && animation_sampler_info.output_type == glTF::VEC3) {
						float *values = (float *)animation_sampler_info.output_values;
						if (key_frame == animation_sampler_info.count - 1) {
							target_node.translation[0] = values[key_frame * 3];
							target_node.translation[1] = values[key_frame * 3 + 1];
							target_node.translation[2] = values[key_frame * 3 + 2];
						}
						else {
							math3d::Vector3D q, r;
							q.x = values[key_frame * 3]; q.y = values[key_frame * 3 + 1]; q.z = values[key_frame * 3 + 2];
							r.x = values[(key_frame + 1) * 3]; r.y = values[(key_frame + 1) * 3 + 1]; r.z = values[(key_frame + 1) * 3 + 2];
							float t = (dur_tick - last_key_frame_tick) / (next_key_frame_tick - last_key_frame_tick);
							if (t > 1) t = 1;
							if (t < 0) t = 0;
							math3d::Vector3D result;
							result.x = q.x + (r.x - q.x)*t;
							result.y = q.y + (r.y - q.y)*t;
							result.z = q.z + (r.z - q.z)*t;

							target_node.translation[0] = result.x;
							target_node.translation[1] = result.y;
							target_node.translation[2] = result.z;
						}
					}
					if (animation.loop && key_frame >= animation_sampler_info.count - 1)
						QueryPerformanceCounter(&animation.start_tick);
					target_node.UpdateMatrix();
				}
				else if (channel.target.path == "scale") {
					target_node.use_scale = true;
					int key_frame = -1;
					if (animation_sampler_info.input_component_type == glTF::FLOAT && animation_sampler_info.input_type == glTF::SCALAR) {
						float *values = (float *)animation_sampler_info.input_values;
						for (auto k = 0; k < animation_sampler_info.count; k++) {
							if (k + 1 < animation_sampler_info.count) {
								if (values[k] <= dur_tick && values[k + 1] >= dur_tick) {
									last_key_frame_tick = values[k];
									next_key_frame_tick = values[k + 1];
									key_frame = k;
									break;
								}
							}
						}
					}
					if (key_frame == -1) key_frame = animation_sampler_info.count - 1;
					if (animation_sampler_info.output_component_type == glTF::FLOAT && animation_sampler_info.output_type == glTF::VEC3) {
						float *values = (float *)animation_sampler_info.output_values;
						if (key_frame == animation_sampler_info.count - 1) {
							target_node.scale[0] = values[key_frame * 3];
							target_node.scale[1] = values[key_frame * 3 + 1];
							target_node.scale[2] = values[key_frame * 3 + 2];
						}
						else {
							math3d::Vector3D q, r;
							q.x = values[key_frame * 3]; q.y = values[key_frame * 3 + 1]; q.z = values[key_frame * 3 + 2];
							r.x = values[(key_frame + 1) * 3]; r.y = values[(key_frame + 1) * 3 + 1]; r.z = values[(key_frame + 1) * 3 + 2];
							float t = (dur_tick - last_key_frame_tick) / (next_key_frame_tick - last_key_frame_tick);
							if (t > 1) t = 1;
							if (t < 0) t = 0;
							math3d::Vector3D result;
							result.x = q.x + (r.x - q.x)*t;
							result.y = q.y + (r.y - q.y)*t;
							result.z = q.z + (r.z - q.z)*t;

							target_node.scale[0] = result.x;
							target_node.scale[1] = result.y;
							target_node.scale[2] = result.z;
						}
					}
					if (animation.loop && key_frame >= animation_sampler_info.count - 1)
						QueryPerformanceCounter(&animation.start_tick);
					target_node.UpdateMatrix();
				}
			}
		}
		auto primitive_count = render_object_info.primitives.size();
		for (auto i = 0; i < primitive_count; i++) {
			auto &primitive_bind_info = render_object_info.primitives[i];
			VkDescriptorBufferInfo descriptor_buffer_info = {};
			void *data;
			vkMapMemory(device_, primitive_bind_info.descriptor_set_memories[current_frame_][0], 0, sizeof(glTF::pbrMetallicRoughness), 0, &data);
			memcpy(data, &primitive_bind_info.material->pbr_metallic_roughness, sizeof(glTF::pbrMetallicRoughness));
			vkUnmapMemory(device_, primitive_bind_info.descriptor_set_memories[current_frame_][0]);
			descriptor_buffer_info.buffer = primitive_bind_info.descriptor_set_buffers[current_frame_][0];
			descriptor_buffer_info.offset = 0;
			descriptor_buffer_info.range = sizeof(glTF::pbrMetallicRoughness);
			VkWriteDescriptorSet write_descriptor_set = {};
			write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write_descriptor_set.dstSet = primitive_bind_info.descriptor_sets[current_frame_][0];
			write_descriptor_set.descriptorCount = 1;
			write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			write_descriptor_set.dstBinding = 0;
			write_descriptor_set.dstArrayElement = 0;
			write_descriptor_set.pBufferInfo = &descriptor_buffer_info;
			vkUpdateDescriptorSets(device_, 1, &write_descriptor_set, 0, nullptr);
			if (primitive_bind_info.material->pbr_metallic_roughness.texture_switch[0]) {
				VkDescriptorImageInfo image_info = {};
				auto texture_index = primitive_bind_info.material->base_color_texture.index;
				image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				image_info.imageView = render_object_info.textures[texture_index].image_view;
				image_info.sampler = render_object_info.textures[texture_index].sampler;
				write_descriptor_set = {};
				write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				write_descriptor_set.dstSet = primitive_bind_info.descriptor_sets[current_frame_][0];
				write_descriptor_set.descriptorCount = 1;
				write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				write_descriptor_set.dstBinding = 1;
				write_descriptor_set.dstArrayElement = 0;
				write_descriptor_set.pImageInfo = &image_info;
				vkUpdateDescriptorSets(device_, 1, &write_descriptor_set, 0, nullptr);
			}
		}

	}

	void DefferredPipelineType::UpdateSkin(unsigned int thread_id, glTF *gltf, RenderObjectInfo &render_object_info) {
		auto &data = gltf->GetData();
		auto &scene = data.scenes[0];
		auto node_count = scene.nodes.size();
		math3d::Matrix base_matrix = render_object_info.matrix;
		for (auto i = 0; i < node_count; i++) {
			auto node_index = scene.nodes[i];
			auto &node = data.nodes[node_index];
			UpdateJointMatrix(thread_id, gltf, &node,&render_object_info.nodes[node_index], render_object_info);
		}
		render_object_info.matrix = base_matrix;
		auto skin_count = render_object_info.skins.size();
		for (auto i = 0; i < skin_count; i++) {
			auto &skin = render_object_info.skins[i];
			auto target_node_count = skin.node_targets.size();
			auto joint_count = skin.joint_mats.size();
			for (auto j= 0; j < target_node_count; j++) {
				auto target_node_index = skin.node_targets[j];
				auto &target_node_info = render_object_info.nodes[target_node_index];
				for (auto k = 0; k < joint_count; k++) {
					target_node_info.joint_mats[k] = target_node_info.joint_mats[k] * skin.joint_mats[k];
				}
			}
		}
	}

	void DefferredPipelineType::UpdateJointMatrix(unsigned int thread_id, glTF *gltf,glTF::Node *node,NodeInfo *node_info, RenderObjectInfo &render_object_info) {
		auto &data = gltf->GetData();
		math3d::Matrix base_matrix = render_object_info.matrix;
		render_object_info.matrix = base_matrix * node->matrix;
		auto children_count = node->children.size();
		for (auto i = 0; i < children_count; i++) {
			auto sub_node_index = node->children[i];
			auto &sub_node = data.nodes[sub_node_index];
			UpdateJointMatrix(thread_id, gltf, &sub_node, &render_object_info.nodes[sub_node_index],render_object_info);
		}
		auto skin_count = node_info->skin_targets.size();
		for (auto i = 0; i < skin_count; i++) {
			auto skin_index = node_info->skin_targets[i].target_skin;
			auto joint_index = node_info->skin_targets[i].joint_index;
			auto &skin = render_object_info.skins[skin_index];
			auto &joint_mat = skin.joint_mats[joint_index];
			joint_mat = render_object_info.matrix*skin.inverse_bind_mats[joint_index];
		}
		if (node->use_skin) {
			auto joint_count = node_info->joint_mats.size();
			auto invert_mat = math3d::MatrixInvert(render_object_info.matrix);
			for (auto i = 0; i < joint_count; i++) {
				node_info->joint_mats[i] = invert_mat;
			}
		}
		render_object_info.matrix = base_matrix;
	}

	unsigned int DefferredPipelineType::AddBuffer(unsigned int thread_id, void *buffer_data, unsigned int buffer_size) {
		VkBuffer buffer,staging_buffer;
		VkDeviceMemory staging_buffer_memory, buffer_memory;
		CreateBuffer(buffer_size,0, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);
		void *data;
		vkMapMemory(device_, staging_buffer_memory, 0, buffer_size, 0, &data);
		memcpy(data, buffer_data, (size_t)buffer_size);
		vkUnmapMemory(device_, staging_buffer_memory);
		CreateBuffer(buffer_size,0,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT |
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,buffer,buffer_memory);
		CopyBuffer(thread_id, staging_buffer, buffer, buffer_size);
		vkDestroyBuffer(device_, staging_buffer, nullptr);
		vkFreeMemory(device_, staging_buffer_memory, nullptr);
		unsigned int buffer_id = buffers_.size();
		buffers_.push_back(buffer);
		memories_.push_back(buffer_memory);
		return buffer_id;
	}

	unsigned int DefferredPipelineType::AddIndexBuffer(unsigned int thread_id,void *buffer_data,unsigned int buffer_size) {
		VkBuffer index_buffer;
		CreateIndexBuffer(thread_id, index_buffer, buffer_size, buffer_data);
		index_buffers_.push_back(index_buffer);
		index_sizes_.push_back(buffer_size/sizeof(uint16_t));
		return index_buffers_.size() - 1;
	}

	unsigned int DefferredPipelineType::AddVertexBuffer(unsigned int thread_id,void *buffer_data, unsigned int buffer_size) {
		VkBuffer vertex_buffer;
		CreateVertexBuffer(thread_id, vertex_buffer, buffer_size, buffer_data);
		vertex_buffers_.push_back(vertex_buffer);
		return vertex_buffers_.size() - 1;
	}

	void DefferredPipelineType::CreateDescriptorSet(VkDescriptorSetLayout &set_layout,VkDescriptorSet &descriptor_set) {
		VkDescriptorPoolSize descriptor_pool_size = {};
		descriptor_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptor_pool_size.descriptorCount = 1;
		VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
		descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptor_pool_create_info.maxSets = 1;
		descriptor_pool_create_info.poolSizeCount=1;
		descriptor_pool_create_info.pPoolSizes = &descriptor_pool_size;
		VkDescriptorPool descriptor_pool;
		if (vkCreateDescriptorPool(device_, &descriptor_pool_create_info, nullptr, &descriptor_pool) != VK_SUCCESS)
			throw std::runtime_error("Failed to create descriptor pool!");
		VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {};
		descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptor_set_allocate_info.descriptorPool = descriptor_pool;
		descriptor_set_allocate_info.descriptorSetCount = 1;
		descriptor_set_allocate_info.pSetLayouts = &set_layout;
		if (vkAllocateDescriptorSets(device_, &descriptor_set_allocate_info, &descriptor_set) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate descriptor set!");
		VkDescriptorBufferInfo descriptor_buffer_info = {};
		VkDeviceMemory buffer_memory;
		VkBuffer descriptor_buffer;
		CreateBuffer(4 * 4, 0, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, descriptor_buffer,buffer_memory);
		void *data;
		math3d::Vector3D vec;
		vec.x = 1; vec.y = 1; vec.z = 0; vec.w = 1;
		vkMapMemory(device_, buffer_memory, 0, 4 * 4, 0, &data);
			memcpy(data, &vec, 4 * 4);
		vkUnmapMemory(device_, buffer_memory);
		descriptor_buffer_info.buffer = descriptor_buffer;
		VkWriteDescriptorSet write_descriptor_set = {};
		write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set.descriptorCount = 1;
		write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write_descriptor_set.dstBinding = 0;
		write_descriptor_set.dstArrayElement = 0;
		write_descriptor_set.dstSet = descriptor_set;
		write_descriptor_set.pBufferInfo = &descriptor_buffer_info;
		vkUpdateDescriptorSets(device_, 1, &write_descriptor_set, 0, nullptr);
	}

	void DefferredPipelineType::CreateDescriptorPools() {
		unsigned int thread_count = GetThreadCount();
		VkDescriptorPoolSize descriptor_pool_size1 = {};
		descriptor_pool_size1.descriptorCount = 1000;
		descriptor_pool_size1.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		VkDescriptorPoolSize descriptor_pool_size2 = {};
		descriptor_pool_size2.descriptorCount = 1000;
		descriptor_pool_size2.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		VkDescriptorPoolSize descriptor_pool_sizes[] = {descriptor_pool_size1,descriptor_pool_size2};
		VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
		descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptor_pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;
		descriptor_pool_create_info.maxSets = 1000;
		descriptor_pool_create_info.poolSizeCount = 2;
		descriptor_pool_create_info.pPoolSizes = descriptor_pool_sizes;
		for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			descriptor_pools_[i].resize(thread_count);
			for (auto j = 0; j < thread_count; j++) {
				vkCreateDescriptorPool(device_, &descriptor_pool_create_info, nullptr, &descriptor_pools_[i][j]);
			}
		}
	}

	void DefferredPipelineType::OutputGPUInfo() {
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(physical_device_, &properties);
		std::cout << "Device Name:" << properties.deviceName << std::endl;
		std::cout << "Api Version:" << properties.apiVersion << std::endl;
		std::cout << "Vendor ID:" << properties.vendorID << std::endl;
		std::cout << "Device ID:" << properties.deviceID << std::endl;
		std::cout << "Device Type:" << properties.deviceType << std::endl;
		std::cout << "Driver Version:" << properties.driverVersion << std::endl;
		std::cout << "Max memory alloc:" << properties.limits.maxMemoryAllocationCount << std::endl;
	}

	static void FramebufferResizeCallback(GLFWwindow *window, int width,int height) {
		std::cout << "framebuffer resized..." << std::endl;
		auto pipeline_obj = reinterpret_cast<DefferredPipelineType *>(glfwGetWindowUserPointer(window));
		pipeline_obj->SetFramebufferResized(true);
	}

	bool DefferredPipelineType::Init() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		unsigned int wnd_w = atoi(GetProperty("WindowWidth").c_str());
		unsigned int wnd_h = atoi(GetProperty("WindowHeight").c_str());
		std::string wnd_title = GetProperty("WindowTitle");

		GLFWwindow *window = glfwCreateWindow(wnd_w, wnd_h, wnd_title.c_str(), nullptr, nullptr);
		if (window == nullptr) return false;
		window_ = window;
		GetNativeObject()->data5.point = window;
		window_w_ = wnd_w;
		window_h_ = wnd_h;
		glfwSetWindowUserPointer(window_, this);
		glfwSetFramebufferSizeCallback(window_,FramebufferResizeCallback);
		
		std::vector<FunctionParameter> fp;
		std::vector<FunctionReturn> fr;
		GetApplication()->CallFunction("GetThreadCount", fp, fr);
		thread_count_ = fr[0].integer_value;
		std::cout<<"thread count:" <<thread_count_<< std::endl;
		InitVulkan();
		return true;
	}

	bool DefferredPipelineType::Destroy() {
		vkDeviceWaitIdle(device_);
		DestroySwapChain();
	//	vkDestroyBuffer(device_, index_buffer_, nullptr);
		vkFreeMemory(device_, index_buffer_memory_, nullptr);
//		vkDestroyBuffer(device_, vertex_buffer_, nullptr);
		vkFreeMemory(device_, vertex_buffer_memory_, nullptr);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(device_, render_finished_semaphores_[i], nullptr);
			vkDestroySemaphore(device_, image_available_semaphores_[i], nullptr);
			vkDestroyFence(device_, in_flight_fences_[i], nullptr);
		}
		unsigned int count = GetThreadCount();
		for(unsigned int i=0;i<count;i++)
			vkDestroyCommandPool(device_, command_pools_[i], nullptr);
		vkDestroyDevice(device_, nullptr);
		if (ENABLE_VALIDATION_LAYERS)
			DestroyDebugUtilsMessengerEXT(vk_instance_, debug_messenger_, nullptr);
		vkDestroySurfaceKHR(vk_instance_, surface_, nullptr);
		vkDestroyInstance(vk_instance_, nullptr);
		glfwDestroyWindow(window_);
		glfwTerminate();
		return true;
	}

	void DefferredPipelineType::SetFramebufferResized(bool value) {
		framebuffer_resized_ = value;
	}

	void DefferredPipelineType::SetProperty(const std::string &name, const std::string &value) {
		properties_[name] = value;
	}

	GLFWwindow *DefferredPipelineType::GetWindow() {
		return window_;
	}

	const std::string &DefferredPipelineType::GetProperty(const std::string &name) {
		return properties_[name];
	}

	unsigned int DefferredPipelineType::GetGraphicsQueueCount() {
		return GetThreadCount();
	}

	unsigned int DefferredPipelineType::GetPresentQueueCount() {
		return 1;
	}

	unsigned int DefferredPipelineType::GetThreadCount() {
		return thread_count_;
	}

	void DefferredPipelineType::SetApplication(ukObject *application_object) {
		application_native_object_ = application_object;
		ObjectType *object_type = new ObjectType();
		object_type->LoadFromObjectTypeInterface(application_object->object_type_interface);
		application_object_ = new Object();
		application_object_->SetNativeObject(application_native_object_);
		application_object_->SetObjectType(object_type);
	}

	Object *DefferredPipelineType::GetApplication() {
		return application_object_;
	}

	void DefferredPipelineType::SetNativeObject(ukObject *native_object) {
		native_object_ = native_object;
	}

	ukObject *DefferredPipelineType::GetNativeObject() {
		return native_object_;
	}

	ukFunctionDefineListInterface *DefferredPipelineType::InitObjectTypeInterfaceFunctions() {
		ukFunctionDefineListInterface *function_list = ukMakeFunctionDefineListInterface(1);
		ukFunctionReturnDefineListInterface *return_list=ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "result", ukINTEGER);
		ukSetFunctionDefineInterface(function_list, 0, "Init",ukSCRIPT,
			ukMakeFunctionParameterDefineListInterface(0),
			return_list,
			DefferredPipelineTypeInitFunction);
		return function_list;
	}

	ukPropertyDefineListInterface *DefferredPipelineType::InitObjectTypeInterfaceProperties() {
		ukPropertyDefineListInterface *property_list = ukMakePropertyDefineListInterface(0);
		return property_list;
	}

	ukObjectDefineListInterface *DefferredPipelineType::InitObjectTypeInterfaceObjects() {
		ukObjectDefineListInterface *object_list = ukMakeObjectDefineListInterface(0);
		return object_list;
	}

	void DefferredPipelineType::InitObjectTypeInterface() {
		ukFunctionDefineListInterface *func_list = InitObjectTypeInterfaceFunctions();
		ukPropertyDefineListInterface *prop_list = InitObjectTypeInterfaceProperties();
		ukObjectDefineListInterface *obj_list = InitObjectTypeInterfaceObjects();
		object_type_interface_ = ukMakeObjectTypeInterface(OBJECT_TYPE_DEFFERREDPIPELINE, DefferredPipelineTypeCreateObject,
			DefferredPipelineTypeDestroyObject,
			DefferredPipelineTypeGetProperty,
			DefferredPipelineTypeSetProperty,
			func_list,
			prop_list,
			obj_list);
	}

	ukObjectTypeInterface *DefferredPipelineType::GetObjectTypeInterface() {
		if (object_type_interface_ == nullptr) {
			InitObjectTypeInterface();
		}
		return object_type_interface_;
	}

	DefferredPipelineType::DefferredPipelineType() {
		validation_layers_ = {
			"VK_LAYER_LUNARG_standard_validation"
		};
		device_extensions_ = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
			"VK_EXT_descriptor_indexing",
			"VK_KHR_maintenance3"
		};
		physical_device_ = VK_NULL_HANDLE;
		current_frame_ = 0;
		framebuffer_resized_ = false;
		need_recreate_swap_chain_ = false;
		QueryPerformanceFrequency(&timer_frequency);
	}

	DefferredPipelineType::~DefferredPipelineType() {

	}
}

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
#ifndef _DEFFERREDPIPELINETYPE_H_INCLUDED
#define _DEFFERREDPIPELINETYPE_H_INCLUDED

#include <array>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <objecttype.h>
#include "renderobjectinfo.h"
#include <tinymath3d.h>
#include <gltf.h>

#ifdef _DEBUG
#define			ENABLE_VALIDATION_LAYERS							true
#else
#define			ENABLE_VALIDATION_LAYERS							false
#endif
#define			OBJECT_TYPE_DEFFERREDPIPELINE						"DefferredPipeline"

namespace unlike3d {
	class DefferredPipelineType {
	public:
		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> present_modes;
		};
		struct QueueFamilyIndices {
			bool set_graphics_family_index;
			bool set_present_family_index;
			uint32_t set_count;
			uint32_t graphics_family_index;
			uint32_t present_family_index;
			QueueFamilyIndices() {
				set_count = 0;
				set_graphics_family_index = false;
				set_present_family_index = false;
			}
			void SetGraphicsFamilyIndex(uint32_t index) {
				graphics_family_index = index;
				if (!set_graphics_family_index) {
					set_count++;
					set_graphics_family_index = true;
				}
			}
			uint32_t GetGraphicsFamilyIndex() {
				return graphics_family_index;
			}
			void SetPresentFamilyIndex(uint32_t index) {
				present_family_index = index;
				if (!set_present_family_index) {
					set_count++;
					set_present_family_index = true;
				}
			}
			uint32_t GetPresentFamilyIndex() {
				return present_family_index;
			}
			bool IsComplete() {
				return set_count==2;
			}
		};
		struct Vertex {
			math3d::Vector3D pos;
			static VkVertexInputBindingDescription GetBindingDescription() {
				VkVertexInputBindingDescription binding_description = {};
				binding_description.binding = 0;
				binding_description.stride = sizeof(Vertex);
				binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				return binding_description;
			}
			static std::array<VkVertexInputAttributeDescription, 1> GetAttributeDescriptions() {
				std::array<VkVertexInputAttributeDescription, 1> attribute_descriptions = {};
				attribute_descriptions[0].binding = 0;
				attribute_descriptions[0].location = 0;
				attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
				attribute_descriptions[0].offset = offsetof(Vertex, pos);
				return attribute_descriptions;
			}
		};
		struct UniformBufferObject {
			alignas(16) math3d::Matrix model;
			alignas(16) math3d::Matrix view;
			alignas(16) math3d::Matrix proj;
		};
		struct AccessorInfo {
			VkBuffer buffer;
			VkDeviceSize offset;
		};
		struct AnimationSamplerInfo {
			unsigned int input_type;
			unsigned int input_component_type;
			unsigned int output_type;
			unsigned int output_component_type;
			unsigned int count;
			unsigned char *input_values;
			unsigned char *output_values;
		};
	public:
		VkInstance vk_instance_;
		bool framebuffer_resized_;
		bool need_recreate_swap_chain_;
		std::vector<const char *> validation_layers_;
		std::vector<const char *> device_extensions_;
		VkDebugUtilsMessengerEXT debug_messenger_;
		VkPhysicalDevice physical_device_;
		VkDevice device_;
		std::vector<VkQueue> graphics_queues_;
		VkQueue present_queue_;
		VkSurfaceKHR surface_;
		VkSwapchainKHR swap_chain_;
		std::vector<VkImage> swap_chain_depth_images_;
		std::vector<VkImageView> swap_chain_depth_image_views_;
		std::vector<VkDeviceMemory> swap_chain_depth_image_memories_;
		std::vector<VkImage> swap_chain_images_;
		std::vector<VkImageView> swap_chain_image_views_;
		VkFormat swap_chain_image_format_;
		VkExtent2D swap_chain_extent_;
		GLFWwindow *window_;
		uint32_t window_w_, window_h_;
		VkPipelineLayout pipeline_layout_;
		std::vector<VkPipelineLayout> pipeline_layouts_;
		std::vector<VkDescriptorSetLayout> descriptor_set_layouts_;
		std::vector<VkDescriptorSet> descriptor_sets_;
		VkRenderPass render_pass_;
		std::vector<VkFramebuffer> swap_chain_framebuffers_;
		VkPipeline graphics_pipeline_;
		std::vector<VkPipeline> graphics_pipelines_;
		std::vector<VkCommandPool> command_pools_;
		VkCommandBuffer clear_command_buffers_[MAX_FRAMES_IN_FLIGHT];
		std::vector<VkCommandBuffer> command_buffers_[MAX_FRAMES_IN_FLIGHT];
		std::vector<VkSemaphore> clear_end_semaphores_[MAX_FRAMES_IN_FLIGHT];
		std::vector<VkSemaphore> draw_end_semaphores_[MAX_FRAMES_IN_FLIGHT];
		std::vector<VkSemaphore> image_available_semaphores_;
		std::vector<VkSemaphore> render_finished_semaphores_; 
		std::vector<VkFence> in_flight_fences_;
		std::vector<VkDescriptorPool> descriptor_pools_[MAX_FRAMES_IN_FLIGHT];
		size_t current_frame_;
		std::vector<Vertex> vertices_;
		std::vector<uint16_t> indices_;
		std::vector<VkBuffer> vertex_buffers_;
		std::vector<VkBuffer> index_buffers_;
		std::vector<uint32_t> index_sizes_;
		std::vector<VkBuffer> buffers_;
		std::vector<VkDeviceMemory> memories_;
		VkDeviceMemory vertex_buffer_memory_;
		VkDeviceMemory index_buffer_memory_;
		VkDescriptorPool descriptor_pool_;
		std::vector<VkBuffer> uniform_buffers_;
		std::vector<VkDeviceMemory> uniform_buffers_memory_;
		uint32_t current_swap_chain_image_index_;
		unsigned int thread_count_;
		LARGE_INTEGER timer_frequency;
	private:
		static ukObjectTypeInterface *object_type_interface_;
		std::map<std::string, std::string> properties_;
		ukObject *application_native_object_;
		Object *application_object_;
		ukObject *native_object_;
	private:
		static ukFunctionDefineListInterface *InitObjectTypeInterfaceFunctions();
		static ukPropertyDefineListInterface *InitObjectTypeInterfaceProperties();
		static ukObjectDefineListInterface *InitObjectTypeInterfaceObjects();
	public:
		static std::vector<char> ReadFile(const std::string &file_name);
		bool IsDeviceSuitable(VkPhysicalDevice device);
		VkShaderModule CreateShaderModule(const std::vector<char> &code);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		std::vector<const char *> GetRequiredExtensions();
		bool CheckValidationLayerSupport();
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available_formats);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
		void CreateBufferWithoutMemory(VkDeviceSize size, VkBufferCreateFlags flags, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
			VkBuffer &buffer);
		void CreateBuffer(VkDeviceSize size, VkBufferCreateFlags flags, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
			VkBuffer &buffer, VkDeviceMemory &buffer_memory);
		uint32_t FindMemoryType(uint32_t type_filter,
			VkMemoryPropertyFlags properties);
		void CopyBuffer(unsigned int thread_id,VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
		VkCommandBuffer BeginSingleTimeCommands(unsigned int thread_id);
		void CopyBufferToImage(unsigned int thread_id,VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		void EndSingleTimeCommands(unsigned int thread_id,VkCommandBuffer command_buffer);
		void TransitionImageLayout(unsigned int thread_id,VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
		VkImage CreateTextureImage(unsigned int thread_id, unsigned int tex_width,unsigned int tex_height,unsigned char *pixels);
		VkImageView CreateImageView(VkImage image, VkFormat format);
		VkSampler CreateTextureSampler(glTF::Sampler &sampler);
		VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlagBits aspect_flags);
		void CreateImage(VkImage &tex_image, VkDeviceMemory &tex_memory, unsigned int tex_width, unsigned int tex_height,
			VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
		void SetFramebufferResized(bool value);
		void DestroySwapChain();
		void ReCreateSwapChain();
		void InitDebugMessenger();
		void CreateSurface();
		void CreateInstance();
		void CreateLogicalDevice();
		void SelectPhysicalDevice();
		void CreateSwapChain();
		void CreateDescriptorPools();
		void CreateImageViews();
		void CreateRenderPass();
		void CreateDepthBuffers();
		void CreateFramebuffers();
		void CreateVertexBuffer(unsigned int thread_id, VkBuffer &vertex_buffer,
			VkDeviceSize buffer_size, void *buffer_data);
		void CreateIndexBuffer(unsigned int thread_id, VkBuffer &index_buffer,
			VkDeviceSize buffer_size, void *buffer_data);
		void CreateGraphicsPipeline();
		void CreateCommandPools();
		void CreateCommandBuffers();
		void CreateSyncObjects();
		void CreateDescriptorSet(VkDescriptorSetLayout &set_layout, VkDescriptorSet &descriptor_set);
		void CreateUniformBuffers();
		VkFormat FindDepthFormat();
		bool HasStencilComponent(VkFormat format);
		VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		void UpdateUniformBuffer(uint32_t current_image);
		void OutputGPUInfo();
		void InitVulkan();
	public:
		virtual bool Init();
		virtual bool Destroy();

		virtual GLFWwindow *GetWindow();

		static void InitObjectTypeInterface();
		static ukObjectTypeInterface *GetObjectTypeInterface();

		void SetProperty(const std::string &name, const std::string &value);
		const std::string &GetProperty(const std::string &name);

		void WaitFrameEnd();
		void BeginCommand(unsigned int thread_id);
		void EndCommand(unsigned int thread_id);
		void SubmitCommand(unsigned int thread_id);
		void SubmitCommand(unsigned int thread_id,VkSemaphore *wait_semaphore, VkSemaphore *signal_semaphore);
		void SubmitCommand(unsigned int thread_id, unsigned int wait_semaphore_count,
			VkSemaphore *wait_semaphore, unsigned int signal_semaphore_count, VkSemaphore *signal_semaphores);
		void ClearCommand(unsigned int thread_id);
		void DrawCommand(unsigned int thread_id, PrimitiveBindInfo &primitive_bind_info);
		AccessorInfo GetAccessorInfo(unsigned int thread_id, glTF *gltf, glTF::Accessor *accessor,RenderObjectInfo &render_object_info);
		AnimationSamplerInfo GetAnimationSamplerInfo(unsigned int thread_id, glTF *gltf, unsigned int input, unsigned int output);
		void MakeInverseBindMatrices(unsigned int thread_id,glTF *gltf,glTF::Accessor *accessor,std::vector <math3d::Matrix> &mats);
		void MakePrimitiveBindInfo(unsigned int thread_id,glTF *gltf, glTF::Primitive *primitive, VkDescriptorSetLayout descriptor_set_layout,
			RenderObjectInfo &render_object_info);
		void DrawMesh(unsigned int thread_id, glTF *gltf,NodeInfo *node_info, glTF::Mesh *mesh,RenderObjectInfo &render_object_info);
		void DrawNode(unsigned int thread_id, glTF *gltf, glTF::Node *node,NodeInfo *node_info,RenderObjectInfo &render_object_info);
		void DrawObject(unsigned int thread_id, RenderObjectInfo &render_object_info);
		void Present();
		void LoadglTFSource(unsigned int thread_id,const std::string &source, RenderObjectInfo &render_object_info);
		void UpdateAnimation(unsigned int thread_id, glTF *gltf, RenderObjectInfo &render_object_info);
		void UpdateSkin(unsigned int thread_id, glTF *gltf, RenderObjectInfo &render_object_info);
		void UpdateJointMatrix(unsigned int thread_id, glTF *gltf,glTF::Node *node, NodeInfo *node_info, RenderObjectInfo &render_object_info);
		unsigned int AddGraphicPipeline(unsigned int thread_id, VkPipeline graphics_pipeline);
		unsigned int AddBuffer(unsigned int thread_id, void *buffer_data, unsigned int buffer_size);
		unsigned int AddIndexBuffer(unsigned int thread_id,void *buffer_data,unsigned int buffer_size);
		unsigned int AddVertexBuffer(unsigned int thread_id,void *buffer_data, unsigned int buffer_size);
		unsigned int GetPresentQueueCount();
		unsigned int GetGraphicsQueueCount();
		unsigned int GetThreadCount();

		void SetApplication(ukObject *application_object);
		Object *GetApplication();

		ukObject *GetNativeObject();
		void SetNativeObject(ukObject *native_object);

		DefferredPipelineType();
		virtual ~DefferredPipelineType();
	};
}

#endif
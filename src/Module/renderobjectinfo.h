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
#ifndef _RENDER_OBJECT_INFO_H_INCLUDED
#define _RENDER_OBJECT_INFO_H_INCLUDED

#include <vector>
#include <chrono>

#include <vulkan/vulkan.h>
#include <tinymath3d.h>
#include <gltf.h>

#include <Windows.h>

#define			MAX_FRAMES_IN_FLIGHT								2

namespace unlike3d {
	struct PrimitiveBindInfo {
		VkBuffer index_buffer;
		VkDeviceSize index_count;
		VkDeviceSize index_buffer_offset;
		std::vector<VkBuffer> buffers;
		std::vector<VkDeviceSize> offsets;
		std::vector<VkDescriptorSet> descriptor_sets[MAX_FRAMES_IN_FLIGHT];
		std::vector<VkBuffer> descriptor_set_buffers[MAX_FRAMES_IN_FLIGHT];
		std::vector<VkDeviceMemory> descriptor_set_memories[MAX_FRAMES_IN_FLIGHT];
		glTF::Material *material;
	};
	struct AnimationInfo {
		unsigned int animation_id;
		bool run;
		bool loop;
		LARGE_INTEGER start_tick;
		LARGE_INTEGER last_tick;
	};
	struct SkinInfo {
		std::vector<math3d::Matrix> inverse_bind_mats;
		std::vector<math3d::Matrix> joint_mats;
		std::vector<unsigned int> node_targets;
	};
	struct JointNodeInfo {
		unsigned int target_skin;
		unsigned int joint_index;
	};
	struct NodeInfo {
		std::vector<JointNodeInfo> skin_targets;
		std::vector<math3d::Matrix> joint_mats;
	};
	struct TextureInfo {
		VkImageView image_view;
		VkImage image;
		VkSampler sampler;
	};
	struct RenderObjectInfo {
		math3d::Matrix matrix;
		bool use_translation;
		bool use_rotation;
		bool use_scale;
		float translation[3];
		float rotation[4];
		float scale[3];
		std::vector<unsigned int> buffers;
		std::vector<PrimitiveBindInfo> primitives;
		std::vector<AnimationInfo> animations;
		std::vector<SkinInfo> skins;
		std::vector<NodeInfo> nodes;
		std::vector<TextureInfo> textures;
		void *data1, *data2, *data3;
	};
	void InitRenderObjectInfoMatrix(RenderObjectInfo *render_object_info);
	void UpdateRenderObjectInfoMatrix(RenderObjectInfo *render_object_info);
}

#endif
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
#include <gltf.h>
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>
#include <algorithm>

namespace unlike3d {
	glTF::glTF() {
	}

	glTF::~glTF() {

	}

	void glTF::LoadScenes() {
		auto count = model_.scenes.size();
		data_.scenes.resize(count);
		for (auto i = 0; i <count; i++) {
			auto &src_scene = model_.scenes[i];
			auto &dst_scene = data_.scenes[i];
			dst_scene.name = src_scene.name;
			auto node_count = src_scene.nodes.size();
			dst_scene.nodes.resize(node_count);
			for (auto j = 0; j < node_count; j++) {
				auto &src_node = src_scene.nodes[j];
				auto &dst_node = dst_scene.nodes[j];
				dst_node = src_node;
			}
		}
	}

	void glTF::LoadNodes() {
		auto node_count = model_.nodes.size();
		data_.nodes.resize(node_count);
		for (auto i = 0; i < node_count; i++) {
			auto &src_node = model_.nodes[i];
			auto &dst_node = data_.nodes[i];
			dst_node.name = src_node.name;
			dst_node.camera = src_node.camera;
			auto children_count = src_node.children.size();
			dst_node.children.resize(children_count);
			for (auto j = 0; j < children_count; j++) {
				auto &dst_child = dst_node.children[j];
				auto &src_child = src_node.children[j];
				dst_child = src_child;
			}

			if (src_node.skin != -1) {
				dst_node.use_skin = true;
				dst_node.skin = src_node.skin;
			}
			else {
				dst_node.use_skin = false;
			}

			if (src_node.mesh != -1) {
				dst_node.use_mesh = true;
				dst_node.mesh = src_node.mesh;
			}
			else
				dst_node.use_mesh = false;
			auto translation_length = src_node.translation.size();
			if (translation_length) {
				dst_node.use_translation = true;
				for (auto j = 0; j < translation_length; j++) {
					dst_node.translation[j] = src_node.translation[j];
				}
			}
			else
				dst_node.use_translation = false;
			auto rotation_length = src_node.rotation.size();
			if (rotation_length) {
				dst_node.use_rotation = true;
				for (auto j = 0; j < rotation_length; j++) {
					dst_node.rotation[j] = src_node.rotation[j];
				}
			}
			else
				dst_node.use_rotation = false;
			auto scale_length = src_node.scale.size();
			if (scale_length) {
				dst_node.use_scale = true;
				for (auto j = 0; j < scale_length; j++) {
					dst_node.scale[j] = src_node.scale[j];
				}
			}
			else
				dst_node.use_scale = false;

			auto matrix_length = src_node.matrix.size();
			if (matrix_length) {
				for (auto j = 0; j < matrix_length; j++) {
					//		dst_node.matrix[j] = src_node.matrix[j];
				}
			}
			else {
				math3d::Matrix t_matrix, r_matrix, s_matrix;
				t_matrix=r_matrix=s_matrix=math3d::IdentityMatrix();
				if(translation_length)
					t_matrix = math3d::Translate(t_matrix, dst_node.translation[0], dst_node.translation[1],
						dst_node.translation[2]);
				if (rotation_length)
					r_matrix = math3d::MakeMatrixFromQuaternion(dst_node.rotation[0], dst_node.rotation[1],
						dst_node.rotation[2], dst_node.rotation[3]);
				if (scale_length)
					s_matrix = math3d::Scale(s_matrix, dst_node.scale[0], dst_node.scale[1], dst_node.scale[2]);
				dst_node.matrix = t_matrix * r_matrix*s_matrix;
			}

			auto weight_count = src_node.weights.size();
			if (weight_count) {
				for (auto j = 0; j < weight_count; j++) {
					dst_node.weights[j] = src_node.weights[j];
				}
			}
		}
	}

	bool cmp_attribute(glTF::Attribute &attrib_a, glTF::Attribute &attrib_b) {
		return glTF::GetAttributeSortKey(attrib_a.name)<glTF::GetAttributeSortKey(attrib_b.name);
	}

	void glTF::LoadMeshes() {
		auto &dst_meshes = data_.meshes;
		auto &src_meshes = model_.meshes;
		auto mesh_count = src_meshes.size();
		dst_meshes.resize(mesh_count);
		for (auto i = 0; i < mesh_count; i++) {
			auto &src_mesh = src_meshes[i];
			auto &dst_mesh = dst_meshes[i];
			dst_mesh.name = src_mesh.name;
			auto primitive_count = src_mesh.primitives.size();
			dst_mesh.primitives.resize(primitive_count);
			for (auto j = 0; j < primitive_count; j++) {
				auto &src_primitive = src_mesh.primitives[j];
				auto &dst_primitive = dst_mesh.primitives[j];
				auto attribute_count = src_primitive.attributes.size();
				dst_primitive.attributes.resize(attribute_count);
				unsigned int attribute_index = 0;
				for (auto itor=src_primitive.attributes.begin();itor!=src_primitive.attributes.end();itor++) {
					auto &dst_attribute = dst_primitive.attributes[attribute_index];
					dst_attribute.name = itor->first;
					dst_attribute.value = itor->second;
					attribute_index++;
				}
				
				std::sort(dst_primitive.attributes.begin(), dst_primitive.attributes.end(),cmp_attribute);
				dst_primitive.indices = src_primitive.indices;
				if (src_primitive.material == -1)
					dst_primitive.use_material = false;
				else
					dst_primitive.use_material = true;
				dst_primitive.material = src_primitive.material;
			}

			auto weight_count = src_mesh.weights.size();
			for (auto j = 0; j < weight_count; j++) {
				dst_mesh.weights[j] = src_mesh.weights[j];
			}
		}
	}

	void glTF::LoadBuffers() {
		auto &dst_buffers = data_.buffers;
		auto &src_buffers = model_.buffers;
		auto buffer_count = src_buffers.size();
		dst_buffers.resize(buffer_count);
		for (auto i = 0; i < buffer_count; i++) {
			auto &src_buffer = src_buffers[i];
			auto &dst_buffer = dst_buffers[i];
			dst_buffer.name = src_buffer.name;
			dst_buffer.uri = src_buffer.uri;
			dst_buffer.byte_length = src_buffer.data.size();
			dst_buffer.data = src_buffer.data.data();
		}
	}

	void glTF::LoadBufferViews() {
		auto &dst_buffer_views = data_.buffer_views;
		auto &src_buffer_views = model_.bufferViews;
		auto buffer_view_count = src_buffer_views.size();
		dst_buffer_views.resize(buffer_view_count);
		for (auto i = 0; i < buffer_view_count; i++) {
			auto &src_buffer_view = src_buffer_views[i];
			auto &dst_buffer_view = dst_buffer_views[i];
			dst_buffer_view.name = src_buffer_view.name;
			dst_buffer_view.buffer = src_buffer_view.buffer;
			dst_buffer_view.byte_offset = src_buffer_view.byteOffset;
			dst_buffer_view.byte_length = src_buffer_view.byteLength;
			dst_buffer_view.byte_stride = src_buffer_view.byteStride;
			dst_buffer_view.target = src_buffer_view.target;
		}
	}

	void glTF::LoadAnimations() {
		auto &dst_animations = data_.animations;
		auto &src_animations = model_.animations;
		auto animation_count = src_animations.size();
		dst_animations.resize(animation_count);
		for (auto i = 0; i < animation_count; i++) {
			auto &dst_animation = dst_animations[i];
			auto &src_animation = src_animations[i];
			dst_animation.name = src_animation.name;
			auto sampler_count = src_animation.samplers.size();
			dst_animation.samplers.resize(sampler_count);
			for (auto j = 0; j < sampler_count; j++) {
				auto &dst_sampler = dst_animation.samplers[j];
				auto &src_sampler = src_animation.samplers[j];
				dst_sampler.input = src_sampler.input;
				dst_sampler.output = src_sampler.output;
				dst_sampler.interpolation = src_sampler.interpolation;
			}
			auto channel_count = src_animation.channels.size();
			dst_animation.channels.resize(channel_count);
			for (auto j = 0; j < channel_count; j++) {
				auto &dst_channel = dst_animation.channels[j];
				auto &src_channel = src_animation.channels[j];
				dst_channel.sampler = src_channel.sampler;
				dst_channel.target.node = src_channel.target_node;
				dst_channel.target.path = src_channel.target_path;
			}
		}
	}

	void glTF::LoadSkins() {
		auto &dst_skins = data_.skins;
		auto &src_skins = model_.skins;
		auto skin_count = src_skins.size();
		dst_skins.resize(skin_count);
		for (auto i = 0; i < skin_count; i++) {
			auto &src_skin = src_skins[i];
			auto &dst_skin = dst_skins[i];
			dst_skin.inverse_bind_materices = src_skin.inverseBindMatrices;
			auto joint_count = src_skin.joints.size();
			dst_skin.joints.resize(joint_count);
			for (auto j = 0; j < joint_count; j++) {
				dst_skin.joints[j] = src_skin.joints[j];
			}
		}
	}

	void glTF::LoadAccessors() {
		auto &dst_accessors = data_.accessors;
		auto &src_accessors = model_.accessors;
		auto accessor_count = src_accessors.size();
		dst_accessors.resize(accessor_count);
		for (auto i = 0; i < accessor_count; i++) {
			auto &src_accessor = src_accessors[i];
			auto &dst_accessor = dst_accessors[i];
			dst_accessor.name = src_accessor.name;
			dst_accessor.buffer_view = src_accessor.bufferView;
			dst_accessor.byte_offset = src_accessor.byteOffset;
			dst_accessor.component_type = src_accessor.componentType;
			dst_accessor.normalized = src_accessor.normalized;
			dst_accessor.count = src_accessor.count;
			dst_accessor.type = src_accessor.type;
			if (src_accessor.maxValues.size()) {
				auto max_count = src_accessor.maxValues.size();
				dst_accessor.max.resize(max_count);
				for (auto j = 0; j < max_count; j++) {
					dst_accessor.max[j] = src_accessor.maxValues[j];
				}
			}
			if (src_accessor.minValues.size()) {
				auto min_count = src_accessor.minValues.size();
				dst_accessor.min.resize(min_count);
				for (auto j = 0; j < min_count; j++) {
					dst_accessor.min[j] = src_accessor.minValues[j];
				}
			}
			if (src_accessor.sparse.isSparse) {
				dst_accessor.is_sparse = true;
				auto &src_sparse = src_accessor.sparse;
				auto &dst_sparse = dst_accessor.sparse;
				dst_sparse.count = src_sparse.count;
				dst_sparse.indices.component_type = src_sparse.indices.componentType;
				dst_sparse.indices.buffer_view = src_sparse.indices.bufferView;
				dst_sparse.indices.byte_offset = src_sparse.indices.byteOffset;
				dst_sparse.values.buffer_view = src_sparse.values.bufferView;
				dst_sparse.values.byte_offset = src_sparse.values.byteOffset;
			}
			else
				dst_accessor.is_sparse = false;
		}
	}
	
	void glTF::LoadMaterials() {
		auto &src_materials = model_.materials;
		auto &dst_materials = data_.materials;
		auto material_count = src_materials.size();
		dst_materials.resize(material_count);
		for (auto i = 0; i < material_count; i++) {
			auto &src_material = src_materials[i];
			auto &dst_material = dst_materials[i];
			dst_material.name = src_material.name;
			auto &src_values = src_material.values;
			dst_material.pbr_metallic_roughness.base_color_factor[0] = 1.0f;
			dst_material.pbr_metallic_roughness.base_color_factor[1] = 1.0f;
			dst_material.pbr_metallic_roughness.base_color_factor[2] = 1.0f;
			dst_material.pbr_metallic_roughness.base_color_factor[3] = 1.0f;
			dst_material.pbr_metallic_roughness.metallic_factor = 1.0f;
			dst_material.pbr_metallic_roughness.roughness_factor = 1.0f;
			dst_material.pbr_metallic_roughness.texture_switch[0] = false;
			dst_material.pbr_metallic_roughness.texture_switch[1] = false;
			dst_material.pbr_metallic_roughness.texture_switch[2] = false;
			dst_material.pbr_metallic_roughness.texture_switch[3] = false;
			dst_material.base_color_texture.tex_coord = 0;
			for (auto itor = src_values.begin(); itor != src_values.end();itor++) {
				auto &name = itor->first;
				auto &value = itor->second;
				if (name == "baseColorFactor") {
					dst_material.pbr_metallic_roughness.base_color_factor[0] = value.number_array[0];
					dst_material.pbr_metallic_roughness.base_color_factor[1] = value.number_array[1];
					dst_material.pbr_metallic_roughness.base_color_factor[2] = value.number_array[2];
					dst_material.pbr_metallic_roughness.base_color_factor[3] = value.number_array[3];
				}
				else if (name == "metallicFactor") {
					dst_material.pbr_metallic_roughness.metallic_factor = value.number_value;
				}
				else if (name == "roughnessFactor") {
					dst_material.pbr_metallic_roughness.roughness_factor =value.number_value;
				}
				else if (name == "baseColorTexture") {
 					dst_material.pbr_metallic_roughness.texture_switch[0] = true;
					auto &dst_base_color_texture = dst_material.base_color_texture;
					dst_base_color_texture.index = (unsigned int)value.json_double_value["index"];
				}
			}
		}
	}

	void glTF::LoadTextures() {
		auto &src_textures = model_.textures;
		auto &dst_textures = data_.textures;
		auto texture_count = src_textures.size();
		dst_textures.resize(texture_count);
		for (auto i = 0; i < texture_count; i++) {
			auto &src_texture = src_textures[i];
			auto &dst_texture = dst_textures[i];
			dst_texture.name = src_texture.name;
			dst_texture.sampler = src_texture.sampler;
			dst_texture.source = src_texture.source;
		}
	}

	void glTF::LoadImages() {
		auto &src_images= model_.images;
		auto &dst_images = data_.images;
		auto image_count = src_images.size();
		dst_images.resize(image_count);
		for (auto i = 0; i < image_count; i++) {
			auto &src_image = src_images[i];
			auto &dst_image = dst_images[i];
			dst_image.name = src_image.name;
			dst_image.mime_type = src_image.mimeType;
			dst_image.uri = src_image.uri;
			dst_image.buffer_view = src_image.bufferView;
			dst_image.width = src_image.width;
			dst_image.height = src_image.height;
			dst_image.bits = src_image.bits;
			dst_image.component = src_image.component;
			dst_image.pixel_type = src_image.pixel_type;
			dst_image.data = src_image.image.data();
		}
	}

	void glTF::LoadSamplers() {
		auto &src_samplers = model_.samplers;
		auto &dst_samplers = data_.samplers;
		auto sampler_count = src_samplers.size();
		dst_samplers.resize(sampler_count);
		for (auto i = 0; i < sampler_count; i++) {
			auto &src_sampler = src_samplers[i];
			auto &dst_sampler = dst_samplers[i];
			dst_sampler.mag_filter = src_sampler.magFilter;
			dst_sampler.min_filter = src_sampler.minFilter;
			dst_sampler.wrap_s = src_sampler.wrapS;
			dst_sampler.wrap_t = src_sampler.wrapT;
		}
	}

	bool glTF::LoadSource(const std::string &source) {
		std::string err, warn;
		bool ok;
		auto len = source.length();
		if(source[len-1]=='f')
			ok=loader_.LoadASCIIFromFile(&model_, &err, &warn, source);
		else
			ok = loader_.LoadBinaryFromFile(&model_, &err, &warn, source);
		if (!ok) return false;
		LoadScenes();
		LoadNodes();
		LoadMeshes();
		LoadBuffers();
		LoadBufferViews();
		LoadAccessors();
		LoadAnimations();
		LoadMaterials();
		LoadTextures();
		LoadImages();
		LoadSamplers();
		LoadSkins();

		return true;
	}

	unsigned int glTF::GetFormatByteSize(unsigned int type, unsigned int component_type) {
		unsigned int size;
		if (type == VEC3) size = 3;
		else if (type == VEC2) size = 2;
		else if (type == VEC4) size = 4;
			std::runtime_error("Unknown format size!");

		if (component_type == FLOAT)
			size *= FLOAT_SIZE;
		else if (component_type == UNSIGNED_BYTE)
			size *= UNSIGNED_BYTE_SIZE;
		else
			std::runtime_error("Unknown format size!");
		return size;
	}

	VkFormat glTF::MakeFormat(unsigned int type,unsigned int component_type) {
		if (type == VEC3 && component_type == FLOAT)
			return VK_FORMAT_R32G32B32_SFLOAT;
		else if (type == VEC4 && component_type == UNSIGNED_SHORT)
			return VK_FORMAT_R16G16B16A16_UINT;
		else if (type == VEC4 && component_type == FLOAT)
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		else if (type == VEC2 && component_type == FLOAT)
			return VK_FORMAT_R32G32_SFLOAT;
		else
			throw std::runtime_error("Unknown format!");
	}

	void glTF::MakeAttributeInfo(VkVertexInputAttributeDescription &attribute_desc, glTF::Accessor &accessor) {
		VkVertexInputBindingDescription bind;
		attribute_desc.offset = 0;
		attribute_desc.format = MakeFormat(accessor.type, accessor.component_type);
	}

	void glTF::MakePipelineBindInfo(glTF::PipelineBindInfo &pipeline_bind_info,Primitive &primitive) {
		pipeline_bind_info.push_constant_range = {};
		pipeline_bind_info.push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pipeline_bind_info.push_constant_range.offset = 0;
		pipeline_bind_info.push_constant_range.size = 64*4;
		pipeline_bind_info.count= primitive.attributes.size();
		pipeline_bind_info.bindings.resize(pipeline_bind_info.count);
		pipeline_bind_info.attributes.resize(pipeline_bind_info.count);
		auto attribute_count = primitive.attributes.size();
		for (auto i = 0; i < attribute_count; i++) {
			auto &attribute = primitive.attributes[i];
			auto &dst_attribute = pipeline_bind_info.attributes[i];
			MakeAttributeInfo(dst_attribute, data_.accessors[attribute.value]);
			dst_attribute.binding = i;
			dst_attribute.location = i;
			pipeline_bind_info.bindings[i].binding = i;
			pipeline_bind_info.bindings[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			if (data_.buffer_views[data_.accessors[attribute.value].buffer_view].byte_stride)
				pipeline_bind_info.bindings[i].stride = data_.buffer_views[data_.accessors[attribute.value].buffer_view].byte_stride;
			else
				pipeline_bind_info.bindings[i].stride = GetFormatByteSize(data_.accessors[attribute.value].type, data_.accessors[attribute.value].component_type);
		}
	}

	unsigned int glTF::GetAttributeMask(std::vector<Attribute> &attributes) {
		unsigned int mask = 0;
		auto attribute_count = attributes.size();
		for (auto i = 0; i < attribute_count; i++) {
			auto &name = attributes[i].name;
			mask |= (1 << GetAttributeSortKey(name));
		}
		return mask;
	}

	void glTF::Node::UpdateMatrix() {
		math3d::Matrix t_matrix, r_matrix, s_matrix;
		t_matrix = r_matrix = s_matrix=math3d::IdentityMatrix();
		if (use_translation)
			t_matrix = math3d::Translate(t_matrix, translation[0], translation[1],translation[2]);
		if (use_rotation)
			r_matrix = math3d::MakeMatrixFromQuaternion(rotation[0], rotation[1],rotation[2],rotation[3]);
		if (use_scale)
			s_matrix = math3d::Scale(s_matrix, scale[0], scale[1], scale[2]);
		matrix = t_matrix * r_matrix*s_matrix;
	}

	std::map<std::string,unsigned int> glTF::attribute_sort_keys = {
		{"POSITION",POSITION},
		{"NORMAL",NORMAL},
		{"TANGENT",TANGENT},
		{"TEXCOORD_0",TEXCOORD_0},
		{"TEXCOORD_1",TEXCOORD_1},
		{"COLOR_0",COLOR_0},
		{"JOINTS_0",JOINTS_0},
		{"WEIGHTS_0",WEIGHTS_0}
	};
	unsigned int glTF::GetAttributeSortKey(const std::string &name) {
		return attribute_sort_keys[name];
	}

	glTF::Data &glTF::GetData() {
		return data_;
	}
}

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
#ifndef _GLTF_H_INCLUDED
#define _GLTF_H_INCLUDED
#include <tiny_gltf.h>
#include <vulkan/vulkan.h>

#include <vector>
#include <tinymath3d.h>

namespace unlike3d {
	class glTF {
	public:
		enum {
			UNUSED = -1
		};
		enum {
			BYTE = 5120,
			UNSIGNED_BYTE = 5121,
			SHORT = 5122,
			UNSIGNED_SHORT = 5123,
			UNSIGNED_INT = 5125,
			FLOAT = 5126
		};
		enum {
			BYTE_SIZE = 1,
			UNSIGNED_BYTE_SIZE = 1,
			SHORT_SIZE = 2,
			UNSIGNED_SHORT_SIZE = 2,
			UNSIGNED_INT_SIZE = 4,
			FLOAT_SIZE = 4
		};
		enum {
			POSITION = 0,
			NORMAL = 1,
			TANGENT = 2,
			TEXCOORD_0 = 3,
			TEXCOORD_1 = 4,
			COLOR_0 = 5,
			JOINTS_0 = 6,
			WEIGHTS_0 = 7
		};
		enum {
			BASE_COLOR_FACTOR=0,
			METALLIC_FACTOR=1,
			ROUGHNESS_FACTOR=2
		};
		enum {
			SCALAR = 65,
			VEC2 = 2,
			VEC3 = 3,
			VEC4 = 4,
			MAT2 = 4,
			MAT3 = 9,
			MAT4 = 16
		};
		enum {
			NEAREST=9728,
			LINEAR=9729,
			NEAREST_MIPMAP_NEAREST=9984,
			LINEAR_MIPMAP_NEAREST=9985,
			NEAREST_MIPMAP_LINEAR=9986,
			LINEAR_MIPMAP_LINEAR=9987
		};
		enum {
			CLAMP_TO_EDGE=33071,
			MIRRORED_REPEAT=33648,
			REPEAT=10497
		};
		struct Scene {
			std::string name;
			std::vector<unsigned int> nodes;
		};
		struct Node {
			std::string name;
			unsigned int camera;
			std::vector<unsigned int> children;
			bool use_skin;
			unsigned int skin;
			math3d::Matrix matrix;
			bool use_mesh;
			unsigned int mesh;
			bool use_translation;
			float translation[3];
			bool use_rotation;
			float rotation[4];
			bool use_scale;
			float scale[3];
			std::vector<float> weights;
			void UpdateMatrix();
		};
		struct OrthographicCameraProperties {};
		struct PerspectiveCameraProperties {};
		struct Camera {
			std::string name;
			std::string type;
			OrthographicCameraProperties orthographic;
			PerspectiveCameraProperties perspective;
		};
		struct Attribute {
			std::string name;
			unsigned int value;
		};
		struct Target {
			unsigned int node;
			std::string path;
		};
		struct Primitive {
			unsigned int primitive_bind_info_id;
			unsigned int pipeline_id;
			std::vector<Attribute> attributes;
			unsigned int indices;
			bool use_material;
			unsigned int material;
			unsigned int mode;
			std::vector<Target> targets;
		};
		struct Mesh {
			std::string name;
			std::vector<Primitive> primitives;
			std::vector<float> weights;
		};
		struct Skin {
			std::string name;
			unsigned int skeleton;
			std::vector<unsigned int> joints;
			unsigned int inverse_bind_materices;
		};
		struct AnimationChannelTarget {
			unsigned int node;
			std::string path;
		};
		struct AnimationChannel {
			unsigned int sampler;
			AnimationChannelTarget target;
		};
		struct AnimationSampler {
			unsigned int input;
			std::string interpolation;
			unsigned int output;
		};
		struct Animation {
			std::string name;
			std::vector<AnimationChannel> channels;
			std::vector<AnimationSampler> samplers;
		};
		struct BufferView {
			std::string name;
			unsigned int buffer;
			unsigned int byte_offset;
			unsigned int byte_stride;
			unsigned int byte_length;
			unsigned int target;
		};
		struct Buffer {
			std::string name;
			std::string uri;
			unsigned int byte_length;
			unsigned char *data;
		};
		struct SparseIndices {
			unsigned int buffer_view;
			unsigned int byte_offset;
			unsigned int component_type;
		};
		struct SparseValues {
			unsigned int buffer_view;
			unsigned int byte_offset;
		};
		struct Sparse {
			unsigned int count;
			SparseIndices indices;
			SparseValues values;
		};
		struct Accessor {
			std::string name;
			unsigned int buffer_view;
			unsigned int byte_offset;
			unsigned int component_type;
			bool normalized;
			unsigned int count;
			unsigned int type;
			std::vector<float> max;
			std::vector<float> min;
			bool is_sparse;
			Sparse sparse;
		};
		struct pbrMetallicRoughness {
			float base_color_factor[4];
			float metallic_factor;
			float roughness_factor;
			bool texture_switch[4];
		};
		struct BaseColorTextureInfo {
			unsigned int index;
			unsigned int tex_coord;
		};
		struct NormalTextureInfo {
			unsigned int index;
			unsigned int tex_coord;
			float scale;
		};
		struct OcclusionTextureInfo {
			unsigned int index;
			unsigned int tex_coord;
			float scale;
		};
		struct EmissiveTextureInfo {
			unsigned int index;
			unsigned int tex_coord;
			float stength;
		};
		struct Material {
			std::string name;
			pbrMetallicRoughness pbr_metallic_roughness;
			BaseColorTextureInfo base_color_texture;
			NormalTextureInfo normal_texture;
			OcclusionTextureInfo occlusion_texture;
			EmissiveTextureInfo emissive_texture;
			float emissive_factor[3];
			std::string alpha_mode;
			float alpha_cutoff;
			bool double_sided;
		};
		struct Texture {
			std::string name;
			unsigned int sampler;
			unsigned int source;
		};
		struct Image {
			std::string name;
			std::string mime_type;
			std::string uri;
			unsigned int buffer_view;
			unsigned int width, height;
			unsigned int bits;
			unsigned int pixel_type;
			unsigned int component;
			unsigned char *data;
		};
		struct Sampler {
			std::string name;
			unsigned int mag_filter;
			unsigned int min_filter;
			unsigned int wrap_s;
			unsigned int wrap_t;
		};
		struct Data {
			std::vector<Scene> scenes;
			std::vector<Node> nodes;
			std::vector<Camera> cameras;
			std::vector<Mesh> meshes;
			std::vector<Skin> skins;
			std::vector<Animation> animations;
			std::vector<Accessor> accessors;
			std::vector<Material> materials;
			std::vector<Texture> textures;
			std::vector<Image> images;
			std::vector<Sampler> samplers;
			std::vector<Buffer> buffers;
			std::vector<BufferView> buffer_views;
		};
		struct PipelineBindInfo {
			unsigned int count;
			std::vector<VkVertexInputBindingDescription> bindings;
			std::vector<VkVertexInputAttributeDescription> attributes;
			VkPushConstantRange push_constant_range;
		};
	public:
		static Material default_material;
	private:
		void LoadScenes();
		void LoadNodes();
		void LoadMeshes();
		void LoadBuffers();
		void LoadBufferViews();
		void LoadAccessors();
		void LoadAnimations();
		void LoadSkins();
		void LoadMaterials();
		void LoadTextures();
		void LoadImages();
		void LoadSamplers();
	private:
		tinygltf::TinyGLTF loader_;
		tinygltf::Model model_;
		Data data_;
		static std::map<std::string, unsigned int> attribute_sort_keys;
	public:
		glTF();
		virtual ~glTF();
		static unsigned int GetAttributeSortKey(const std::string &name);
		bool LoadSource(const std::string &source);
		unsigned int GetFormatByteSize(unsigned int type, unsigned int component_type);
		VkFormat MakeFormat(unsigned int type,unsigned int component_type);
		unsigned int GetAttributeMask(std::vector<Attribute> &attributes);
		void MakeAttributeInfo(VkVertexInputAttributeDescription &attribute_desc, Accessor &accessor);
		void MakePipelineBindInfo(PipelineBindInfo &pipeline_bind_info,Primitive &primitive);
		Data &GetData();
	};
}

#endif
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding=0) uniform PBRUbo{
	vec4 base_color_factor;
	float metallic_factor;
	float roughness_factor;
	bvec4 texture_switch;
} pbr_ubo;

layout(binding=1) uniform sampler2D base_color_sampler;

layout(location = 0) in vec2 texcoord_0;

layout(location = 0) out vec4 out_color;

void main() {
	if(pbr_ubo.texture_switch[0])
		out_color = texture(base_color_sampler,texcoord_0);
	else
		out_color=pbr_ubo.base_color_factor;
}
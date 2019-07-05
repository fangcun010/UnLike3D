#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform matrix{
	mat4 mat;
	mat4 joint_mats[3];
} consts;

layout(set=0,binding=0) uniform PBRUbo{
	vec4 base_color_factor;
	float metallic_factor;
	float roughness_factor;
	bvec4 texture_switch;
} pbr_ubo;

layout(binding=1) uniform sampler2D base_color_sampler;
layout(binding=2) uniform sampler2D metallic_roughness_sampler;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_texcoord_0;

layout(location = 0) out vec2 texcoord_0;

void main() {
	/*mat4 skin_mat=in_weights.x*consts.joint_mats[in_joint.x]+
					in_weights.y*consts.joint_mats[in_joint.y]+
					in_weights.z*consts.joint_mats[in_joint.z]+
					in_weights.w*consts.joint_mats[in_joint.w];*/

    gl_Position = consts.mat*vec4(in_position,1.0);
	gl_Position.y=-gl_Position.y;
    texcoord_0 = vec2(in_texcoord_0.x,in_texcoord_0.y);
}
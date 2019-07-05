#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform matrix{
	mat4 mat;
	mat4 joint_mats[3];
} consts;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_weights;

layout(location = 0) out vec3 frag_color;

void main() {
/*	mat4 skin_mat=in_weights.x*consts.joint_mats[in_joint.x]+
					in_weights.y*consts.joint_mats[in_joint.y]+
					in_weights.z*consts.joint_mats[in_joint.z]+
					in_weights.w*consts.joint_mats[in_joint.w];*/

    gl_Position = vec4(in_position,1.0);
//	vec4 pos=skin_mat*gl_Position;
    frag_color = vec3(in_weights.x,0,0);
}

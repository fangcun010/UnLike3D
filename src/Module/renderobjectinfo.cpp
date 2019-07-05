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
#include "renderobjectinfo.h"

namespace unlike3d{
	void InitRenderObjectInfoMatrix(RenderObjectInfo *render_object_info) {
		render_object_info->use_translation = false;
		render_object_info->use_rotation = false;
		render_object_info->use_scale = false;
		render_object_info->matrix = math3d::IdentityMatrix();
	}

	void UpdateRenderObjectInfoMatrix(RenderObjectInfo *render_object_info) {
		math3d::Matrix t_matrix, r_matrix, s_matrix;
		t_matrix = r_matrix = s_matrix=math3d::IdentityMatrix();
		if (render_object_info->use_translation)
			t_matrix=math3d::Translate(t_matrix,render_object_info->translation[0], render_object_info->translation[1],
				render_object_info->translation[2]);
		if (render_object_info->use_rotation)
			r_matrix=math3d::MakeMatrixFromQuaternion(render_object_info->rotation[0], render_object_info->rotation[1],
				render_object_info->rotation[2], render_object_info->rotation[3]);
		if (render_object_info->use_scale)
			s_matrix=math3d::Scale(s_matrix, render_object_info->scale[0], render_object_info->scale[1],
				render_object_info->scale[2]);
		render_object_info->matrix = t_matrix * r_matrix*s_matrix;
	}
}
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
#ifndef _CAMERA_H_INCLUDED
#define _CAMERA_H_INCLUDED

#include <tinymath3d.h>

namespace unlike3d {
	class Camera {
	private:
		float fovy_;
		float aspect_;
		float near_;
		float far_;
		math3d::Vector3D position_;
		math3d::Vector3D direction_;
		math3d::Vector3D up_;
		math3d::Matrix projection_matrix_;
		math3d::Matrix view_matrix_;
		math3d::Frustum frustum_;
	private:
		void MakeFrustum();
	public:
		math3d::Matrix *GetProjectionMatrix();
		math3d::Matrix *GetViewMatrix();
		math3d::Frustum *GetFrustum();
		void LookAt(math3d::Vector3D &eye, math3d::Vector3D &center, math3d::Vector3D &up);
		void Perspective(float fovy, float aspect, float near, float far);

		Camera();
		virtual ~Camera();
	};
}

#endif
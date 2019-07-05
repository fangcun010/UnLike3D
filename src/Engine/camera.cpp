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
#include <camera.h>

namespace unlike3d {
	math3d::Matrix *Camera::GetProjectionMatrix() {
		return &projection_matrix_;
	}

	math3d::Matrix *Camera::GetViewMatrix() {
		return &view_matrix_;
	}

	math3d::Frustum *Camera::GetFrustum() {
		return &frustum_;
	}

	void Camera::MakeFrustum() {

	}

	void Camera::LookAt(math3d::Vector3D &position, math3d::Vector3D &direction, math3d::Vector3D &up) {
		position_ = position;
		direction_ = direction;
		up_ = up;
		math3d::Vector3D center = direction;
		center.x += position.x;
		center.y += position.y;
		center.z += position.z;
		view_matrix_ = math3d::LookAtMatrix(position.x, position.y, position.z, center.x, center.y, center.z,
																					up.x, up.y, up.z);
	}

	void Camera::Perspective(float fovy, float aspect, float near, float far) {
		fovy_ = fovy;
		aspect_ = aspect;
		near_ = near;
		far_ = far;
		projection_matrix_ = math3d::PerspectiveMatrix(fovy, aspect, near, far);
	}

	Camera::Camera() {

	}

	Camera::~Camera() {

	}
}
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
#include <iostream>
#include "utMath.h"
#include <tinymath3d.h>

using namespace std;

int main() {
	Horde3D::Quaternion q(0,0,0.707,0.707);	
	Horde3D::Matrix4f mat(q);

	for (auto i = 0; i < 4; i++) {
		std::cout << mat.getRow(i).x << "," << mat.getRow(i).y << "," << mat.getRow(i).z << "," << mat.getRow(i).w << std::endl;
	}
	std::cout << "================================================" << std::endl;
	math3d::Matrix mmat;
	mmat = math3d::MakeMatrixFromQuaternion(0, 0, 0.707, 0.707);

	for (auto i = 0; i < 4; i++) {
		for (auto j = 0; j < 4; j++)
			std::cout << mmat.mat[j][i] << ",";
		std::cout << endl;
	}

	return 0;
}
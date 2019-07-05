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
#ifndef _MATH3D_H_INCLUDED
#define _MATH3D_H_INCLUDED

namespace math3d{
    const float EPS=1e-9f;
    const float PI=3.14159265358979323846f;
    const float INF=1e20f;

    struct Vector3D{
        float x,y,z,w;
    };

    struct Matrix{
        float mat[4][4];
    };

    typedef struct Matrix Matrix;

    typedef struct Vector3D Vector3D;
    typedef Vector3D Point3D;
    typedef Vector3D Quaternion;

	Vector3D Normalize(const Vector3D &v);

	Vector3D operator + (const Vector3D &v1, const Vector3D &v2);

	Vector3D operator - (const Vector3D &v1, const Vector3D &v2);

	float Dot(const Vector3D &v1, const Vector3D &v2);

	Vector3D Cross(const Vector3D &v1, const Vector3D &v2);

	Matrix ZeroMatrix();

	Matrix IdentityMatrix();

	Matrix operator * (const Matrix &m1, const Matrix &m2);
    
	Vector3D operator * (const Matrix &m, const Vector3D &v);

	Matrix Translate(const Matrix &m, float x, float y, float z);

	Matrix Scale(const Matrix &m, float x, float y, float z);

	Matrix Rotate(const Matrix &m, float angle, float x, float y, float z);

	Matrix LookAtMatrix(float eye_x, float eye_y, float eye_z,
		float center_x, float center_y, float center_z,
		float up_x, float up_y, float up_z);

	Matrix OrthoMatrix(float left, float right, float bottom, float top,
		float near, float far);

	Matrix FrustumMatrix(float left, float right, float bottom, float top,
		float near, float far);

	Matrix PerspectiveMatrix(float fov_y, float aspect, float near, float far);

	Matrix MakeMatrixFromQuaternion(float x, float y, float z, float w);

	Vector3D Slerp(Vector3D q, Vector3D r, float t);

	float MatrixDeterminant(Matrix &mat);

	Matrix MatrixInvert(Matrix &mat);

	Matrix MakeMatrix(float translation[], float rotation[], float scale[]);

    struct AABB{
        Point3D min;
        Point3D max;
    };

    typedef struct AABB AABB;

    struct Plane{
        Vector3D normal;
        float distance;
    };

    typedef struct Plane Plane;

	Plane MakePlane(const Point3D pt0, const Point3D pt1, const Point3D pt2);

	float PointToPlane(Point3D pt, Plane plane);

    struct Frustum{
        Plane planes[6];
        Vector3D origin;
        Vector3D corners[8];
    };

    typedef struct Frustum Frustum;

    struct BoundingBox{
        Plane planes[6];
        Vector3D origin;
        Vector3D corners[8];
    };

    typedef struct BoundingBox BoundingBox;

    const int INSIDE=0;
    const int OUTSIDE=1;
    const int INTERSECT=2;

	int InFrustum(const Frustum &frustum, const BoundingBox &bbox);
}

#endif //MATH3D_HPP_INCLUDED
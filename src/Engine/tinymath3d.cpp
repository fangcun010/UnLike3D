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
#include <cmath>
#include <tinymath3d.h>

namespace math3d{
    Vector3D Normalize(const Vector3D &v){
        return {v.x/v.w,v.y/v.w,v.z/v.w,1};
    }

    Vector3D operator + (const Vector3D &v1,const Vector3D &v2){
        return {v1.x+v2.x,v1.y+v2.y,v1.z+v2.z,1};
    }

    Vector3D operator - (const Vector3D &v1,const Vector3D &v2){
        return {v1.x-v2.x,v1.y-v2.y,v1.z-v2.z,1};
    }

    float Dot(const Vector3D &v1,const Vector3D &v2){
        return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
    }

    Vector3D Cross(const Vector3D &v1,const Vector3D &v2){
        return {v1.y*v2.z-v1.z*v2.y,-(v1.x*v2.z-v1.z*v2.x),v1.x*v2.y-v1.y*v2.x,1};
    }

    Matrix ZeroMatrix(){
        Matrix ret;

        ret.mat[0][0]=0;    ret.mat[1][0]=0;	ret.mat[2][0]=0;    ret.mat[3][0]=0;
	    ret.mat[0][1]=0;	ret.mat[1][1]=0;	ret.mat[2][1]=0;	ret.mat[3][1]=0;
	    ret.mat[0][2]=0;	ret.mat[1][2]=0;	ret.mat[2][2]=0;	ret.mat[3][2]=0;
	    ret.mat[0][3]=0;	ret.mat[1][3]=0;	ret.mat[2][3]=0;	ret.mat[3][3]=0;

        return ret;
    }

    Matrix IdentityMatrix(){
        Matrix ret;

        ret.mat[0][0]=1;		ret.mat[1][0]=0;		ret.mat[2][0]=0;		ret.mat[3][0]=0;
	    ret.mat[0][1]=0;		ret.mat[1][1]=1;		ret.mat[2][1]=0;		ret.mat[3][1]=0;
	    ret.mat[0][2]=0;		ret.mat[1][2]=0;		ret.mat[2][2]=1;		ret.mat[3][2]=0;
	    ret.mat[0][3]=0;		ret.mat[1][3]=0;		ret.mat[2][3]=0;		ret.mat[3][3]=1;

        return ret;
    }

    Matrix operator * (const Matrix &m1,const Matrix &m2){
        Matrix ret;
		ret = ZeroMatrix();
        for(int i=0;i<4;i++)
	    for(int j=0;j<4;j++)
	    for(int k=0;k<4;k++)
		ret.mat[j][i]+=m1.mat[k][i]*m2.mat[j][k];
        return ret;
    }
    
    Vector3D operator * (const Matrix &m,const Vector3D &v){
        return {
            m.mat[0][0]*v.x+m.mat[1][0]*v.y+m.mat[2][0]*v.z+m.mat[3][0]*v.w,
            m.mat[0][1]*v.x+m.mat[1][1]*v.y+m.mat[2][1]*v.z+m.mat[3][1]*v.w,
            m.mat[0][2]*v.x+m.mat[1][2]*v.y+m.mat[2][2]*v.z+m.mat[3][2]*v.w,
            m.mat[0][3]*v.x+m.mat[1][3]*v.y+m.mat[2][3]*v.z+m.mat[3][3]*v.w
        };
    }

    Matrix Translate(const Matrix &m,float x,float y,float z){
        Matrix ret;
        ret.mat[0][0]=1;	ret.mat[1][0]=0;	ret.mat[2][0]=0;	ret.mat[3][0]=x;
	    ret.mat[0][1]=0;	ret.mat[1][1]=1;	ret.mat[2][1]=0;	ret.mat[3][1]=y;
	    ret.mat[0][2]=0;	ret.mat[1][2]=0;	ret.mat[2][2]=1;	ret.mat[3][2]=z;
	    ret.mat[0][3]=0;	ret.mat[1][3]=0;	ret.mat[2][3]=0;	ret.mat[3][3]=1;
        return m*ret;
    }

    Matrix Scale(const Matrix &m,float x,float y,float z){
        Matrix ret;
        ret.mat[0][0]=x;	ret.mat[1][0]=0;	ret.mat[2][0]=0;	ret.mat[3][0]=0;
	    ret.mat[0][1]=0;	ret.mat[1][1]=y;	ret.mat[2][1]=0;	ret.mat[3][1]=0;
	    ret.mat[0][2]=0;	ret.mat[1][2]=0;	ret.mat[2][2]=z;	ret.mat[3][2]=0;
	    ret.mat[0][3]=0;	ret.mat[1][3]=0;	ret.mat[2][3]=0;	ret.mat[3][3]=1;
        return m*ret;
    }

    Matrix Rotate(const Matrix &m,float angle,float x,float y,float z){
        Vector3D v={x,y,z,1};
        Matrix ret;
        v=Normalize(v);
        ret.mat[0][0]=cosf(angle)+(1-cosf(angle))*v.x*v.x;
	    ret.mat[1][0]=(1-cosf(angle))*v.x*v.y-v.z*sinf(angle);
	    ret.mat[2][0]=(1-cosf(angle))*v.x*v.z+v.y*sinf(angle);
	    ret.mat[3][0]=0;

	    ret.mat[0][1]=(1-cosf(angle))*v.x*v.y+v.z*sinf(angle);
	    ret.mat[1][1]=cosf(angle)+(1-cosf(angle))*v.y*v.y;
	    ret.mat[2][1]=(1-cosf(angle))*v.y*v.z-v.x*sinf(angle);
	    ret.mat[3][1]=0;

	    ret.mat[0][2]=(1-cosf(angle))*v.x*v.z-v.y*sinf(angle);
	    ret.mat[1][2]=(1-cosf(angle))*v.y*v.z+v.x*sinf(angle);
	    ret.mat[2][2]=cosf(angle)+(1.0f-cosf(angle))*v.z*v.z;
	    ret.mat[3][2]=0;

	    ret.mat[0][3]=0;
	    ret.mat[1][3]=0;
	    ret.mat[2][3]=0;
	    ret.mat[3][3]=1;
        return m*ret;
    }

    Matrix LookAtMatrix(float eye_x,float eye_y,float eye_z,
                    float center_x,float center_y,float center_z,
                    float up_x,float up_y,float up_z){
        Matrix ret;
        Vector3D eye,center,up;
        Vector3D r,u,v;
        Vector3D t1,t2,t3;
        float v1,v2,v3;

        eye={eye_x,eye_y,eye_z,1};
        center={center_x,center_y,center_z,1};
        up={up_x,up_y,up_z,1};

        v=center-eye;
        r=Cross(v,up);
        r=Normalize(r);
        u=Cross(r,v);

        t3=eye;
	    t1.x=-t3.x;t1.y=-t3.y;t1.z=-t3.z;
	    t2=t1;

        v1=Dot(t1,r);
        v2=Dot(t2,u);
        v3=Dot(t3,v);

        ret.mat[0][0]=r.x;	ret.mat[1][0]=r.y;	ret.mat[2][0]=r.z;	ret.mat[3][0]=v1;
	    ret.mat[0][1]=u.x;	ret.mat[1][1]=u.y;	ret.mat[2][1]=u.z;	ret.mat[3][1]=v2;
	    ret.mat[0][2]=-v.x;	ret.mat[1][2]=-v.y;	ret.mat[2][2]=-v.z;	ret.mat[3][2]=v3;
	    ret.mat[0][3]=0;	ret.mat[1][3]=0;	ret.mat[2][3]=0;	ret.mat[3][3]=1;

        return ret;
    }

    Matrix OrthoMatrix(float left,float right,float bottom,float top,
                    float near,float far){
        Matrix ret;

		near=-near;
		far=-far;

        ret.mat[0][0]=2/(right-left);   ret.mat[1][0]=0;				ret.mat[2][0]=0;			ret.mat[3][0]=-(right+left)/(right-left);
	    ret.mat[0][1]=0;			    ret.mat[1][1]=2/(top-bottom);	ret.mat[2][1]=0;			ret.mat[3][1]=-(top+bottom)/(top-bottom);
	    ret.mat[0][2]=0;			    ret.mat[1][2]=0;				ret.mat[2][2]=2/(far-near);	ret.mat[3][2]=-(far+near)/(far-near);
	    ret.mat[0][3]=0;			    ret.mat[1][3]=0;				ret.mat[2][3]=0;			ret.mat[3][3]=1;

        return ret;
    }

    Matrix FrustumMatrix(float left,float right,float bottom,float top,
                    float near,float far){
        Matrix ret;

        ret.mat[0][0]=(2*near)/(right-left);	ret.mat[1][0]=0;					    ret.mat[2][0]=(right+left)/(right-left);    ret.mat[3][0]=0;
	    ret.mat[0][1]=0;						ret.mat[1][1]=(2*near)/(top-bottom);	ret.mat[2][1]=(top+bottom)/(top-bottom);	ret.mat[3][1]=0;
	    ret.mat[0][2]=0;						ret.mat[1][2]=0;						ret.mat[2][2]=-(far+near)/(far-near);	    ret.mat[3][2]=-(2*far*near)/(far-near);
	    ret.mat[0][3]=0;						ret.mat[1][3]=0;						ret.mat[2][3]=-1;						    ret.mat[3][3]=0;

        return ret;
    }

    Matrix PerspectiveMatrix(float fov_y,float aspect,float near,float far){
        Matrix ret;
        fov_y=fov_y/180*PI;
	    float c=1.0f/tanf(fov_y/2);

	    ret.mat[0][0]=c/aspect;	ret.mat[1][0]=0;	ret.mat[2][0]=0;						ret.mat[3][0]=0;
	    ret.mat[0][1]=0;		ret.mat[1][1]=c;	ret.mat[2][1]=0;						ret.mat[3][1]=0;
	    ret.mat[0][2]=0;		ret.mat[1][2]=0;	ret.mat[2][2]=-(far+near)/(far-near);	ret.mat[3][2]=-(2*far*near)/(far-near);
	    ret.mat[0][3]=0;		ret.mat[1][3]=0;    ret.mat[2][3]=-1;					    ret.mat[3][3]=0;
        
        return ret;
    }

	Matrix MakeMatrixFromQuaternion(float x, float y, float z, float w) {
		Matrix ret;
		ret.mat[0][0] = 1-2*(y*y+z*z);		ret.mat[1][0] = 2*(x*y-w*z);		ret.mat[2][0] = 2*(x*z+w*y);		ret.mat[3][0] = 0;
		ret.mat[0][1] = 2*(x*y+w*z);		ret.mat[1][1] = 1.0f-2*(x*x+z*z);	ret.mat[2][1] = 2*(y*z-w*x);		ret.mat[3][1] = 0;
		ret.mat[0][2] = 2*(x*z-w*y);		ret.mat[1][2] = 2*(y*z+w*x);		ret.mat[2][2] = 1.0f-2*(x*x+y*y);	ret.mat[3][2] = 0;
		ret.mat[0][3] = 0;					ret.mat[1][3] = 0;				    ret.mat[2][3] = 0;				    ret.mat[3][3] = 1.0f;
		
		return ret;
	}

	Vector3D Slerp(Vector3D q, Vector3D r, float t) {
		Vector3D ret;
		float cos_theta = q.x*r.x + q.y*r.y + q.z*r.z + q.w*r.w;
		if (cos_theta < 0) {
			cos_theta = -cos_theta;
			r.x = -r.x; r.y = -r.y;
			r.z = -r.z; r.w = -r.w;
		}
		float scale0 = 1 - t, scale1 = t;
		if (1 - cos_theta > 0.001f) {
			float theta = acosf(cos_theta);
			float sin_theta = sinf(theta);
			scale0 = sinf(1 - t)*theta / sin_theta;
			scale1 = sinf(t*theta) / sin_theta;
		}
		ret.x = q.x*scale0 + r.x*scale1;
		ret.y = q.y*scale0 + r.y*scale1;
		ret.z = q.z*scale0 + r.z*scale1;
		ret.w = q.w*scale0 + r.w*scale1;
		return ret;
	}

	float MatrixDeterminant(Matrix &mat) {
		float r1, r2, r3, r4, r5;
		r1 = mat.mat[0][2] * mat.mat[1][1] * mat.mat[2][3] * mat.mat[3][0] - mat.mat[0][1] * mat.mat[1][2] * mat.mat[2][3] * mat.mat[3][0] - mat.mat[0][3] * mat.mat[1][2] * mat.mat[2][0] * mat.mat[3][1] + mat.mat[0][2] * mat.mat[1][3] * mat.mat[2][0] * mat.mat[3][1];
		r2 = mat.mat[0][3] * mat.mat[1][0] * mat.mat[2][2] * mat.mat[3][1] - mat.mat[0][0] * mat.mat[1][3] * mat.mat[2][2] * mat.mat[3][1] - mat.mat[0][2] * mat.mat[1][0] * mat.mat[2][3] * mat.mat[3][1] + mat.mat[0][0] * mat.mat[1][2] * mat.mat[2][3] * mat.mat[3][1];
		r3 = mat.mat[0][3] * mat.mat[1][1] * mat.mat[2][0] * mat.mat[3][2] - mat.mat[0][1] * mat.mat[1][3] * mat.mat[2][0] * mat.mat[3][2] - mat.mat[0][3] * mat.mat[1][0] * mat.mat[2][1] * mat.mat[3][2] + mat.mat[0][0] * mat.mat[1][3] * mat.mat[2][1] * mat.mat[3][2];
		r4 = mat.mat[0][1] * mat.mat[1][0] * mat.mat[2][3] * mat.mat[3][2] - mat.mat[0][0] * mat.mat[1][1] * mat.mat[2][3] * mat.mat[3][2] - mat.mat[0][2] * mat.mat[1][1] * mat.mat[2][0] * mat.mat[3][3] + mat.mat[0][1] * mat.mat[1][2] * mat.mat[2][0] * mat.mat[3][3];
		r5 = mat.mat[0][2] * mat.mat[1][0] * mat.mat[2][1] * mat.mat[3][3] - mat.mat[0][0] * mat.mat[1][2] * mat.mat[2][1] * mat.mat[3][3] - mat.mat[0][1] * mat.mat[1][0] * mat.mat[2][2] * mat.mat[3][3] + mat.mat[0][0] * mat.mat[1][1] * mat.mat[2][2] * mat.mat[3][3];
		return r1 + r2 + r3 + r4 + r5 ;
	}

	Matrix MatrixInvert(Matrix &mat) {
		Matrix m=IdentityMatrix();

		float d = MatrixDeterminant(mat);
		if (abs(d) < EPS) return m;
		d = 1.0f / d;

		m.mat[0][0] = d * (mat.mat[1][2] * mat.mat[2][3] * mat.mat[3][1] - mat.mat[1][3] * mat.mat[2][2] * mat.mat[3][1] + mat.mat[1][3] * mat.mat[2][1] * mat.mat[3][2] - mat.mat[1][1] * mat.mat[2][3] * mat.mat[3][2] - mat.mat[1][2] * mat.mat[2][1] * mat.mat[3][3] + mat.mat[1][1] * mat.mat[2][2] * mat.mat[3][3]);
		m.mat[0][1] = d * (mat.mat[0][3] * mat.mat[2][2] * mat.mat[3][1] - mat.mat[0][2] * mat.mat[2][3] * mat.mat[3][1] - mat.mat[0][3] * mat.mat[2][1] * mat.mat[3][2] + mat.mat[0][1] * mat.mat[2][3] * mat.mat[3][2] + mat.mat[0][2] * mat.mat[2][1] * mat.mat[3][3] - mat.mat[0][1] * mat.mat[2][2] * mat.mat[3][3]);
		m.mat[0][2] = d * (mat.mat[0][2] * mat.mat[1][3] * mat.mat[3][1] - mat.mat[0][3] * mat.mat[1][2] * mat.mat[3][1] + mat.mat[0][3] * mat.mat[1][1] * mat.mat[3][2] - mat.mat[0][1] * mat.mat[1][3] * mat.mat[3][2] - mat.mat[0][2] * mat.mat[1][1] * mat.mat[3][3] + mat.mat[0][1] * mat.mat[1][2] * mat.mat[3][3]);
		m.mat[0][3] = d * (mat.mat[0][3] * mat.mat[1][2] * mat.mat[2][1] - mat.mat[0][2] * mat.mat[1][3] * mat.mat[2][1] - mat.mat[0][3] * mat.mat[1][1] * mat.mat[2][2] + mat.mat[0][1] * mat.mat[1][3] * mat.mat[2][2] + mat.mat[0][2] * mat.mat[1][1] * mat.mat[2][3] - mat.mat[0][1] * mat.mat[1][2] * mat.mat[2][3]);
		m.mat[1][0] = d * (mat.mat[1][3] * mat.mat[2][2] * mat.mat[3][0] - mat.mat[1][2] * mat.mat[2][3] * mat.mat[3][0] - mat.mat[1][3] * mat.mat[2][0] * mat.mat[3][2] + mat.mat[1][0] * mat.mat[2][3] * mat.mat[3][2] + mat.mat[1][2] * mat.mat[2][0] * mat.mat[3][3] - mat.mat[1][0] * mat.mat[2][2] * mat.mat[3][3]);
		m.mat[1][1] = d * (mat.mat[0][2] * mat.mat[2][3] * mat.mat[3][0] - mat.mat[0][3] * mat.mat[2][2] * mat.mat[3][0] + mat.mat[0][3] * mat.mat[2][0] * mat.mat[3][2] - mat.mat[0][0] * mat.mat[2][3] * mat.mat[3][2] - mat.mat[0][2] * mat.mat[2][0] * mat.mat[3][3] + mat.mat[0][0] * mat.mat[2][2] * mat.mat[3][3]);
		m.mat[1][2] = d * (mat.mat[0][3] * mat.mat[1][2] * mat.mat[3][0] - mat.mat[0][2] * mat.mat[1][3] * mat.mat[3][0] - mat.mat[0][3] * mat.mat[1][0] * mat.mat[3][2] + mat.mat[0][0] * mat.mat[1][3] * mat.mat[3][2] + mat.mat[0][2] * mat.mat[1][0] * mat.mat[3][3] - mat.mat[0][0] * mat.mat[1][2] * mat.mat[3][3]);
		m.mat[1][3] = d * (mat.mat[0][2] * mat.mat[1][3] * mat.mat[2][0] - mat.mat[0][3] * mat.mat[1][2] * mat.mat[2][0] + mat.mat[0][3] * mat.mat[1][0] * mat.mat[2][2] - mat.mat[0][0] * mat.mat[1][3] * mat.mat[2][2] - mat.mat[0][2] * mat.mat[1][0] * mat.mat[2][3] + mat.mat[0][0] * mat.mat[1][2] * mat.mat[2][3]);
		m.mat[2][0] = d * (mat.mat[1][1] * mat.mat[2][3] * mat.mat[3][0] - mat.mat[1][3] * mat.mat[2][1] * mat.mat[3][0] + mat.mat[1][3] * mat.mat[2][0] * mat.mat[3][1] - mat.mat[1][0] * mat.mat[2][3] * mat.mat[3][1] - mat.mat[1][1] * mat.mat[2][0] * mat.mat[3][3] + mat.mat[1][0] * mat.mat[2][1] * mat.mat[3][3]);
		m.mat[2][1] = d * (mat.mat[0][3] * mat.mat[2][1] * mat.mat[3][0] - mat.mat[0][1] * mat.mat[2][3] * mat.mat[3][0] - mat.mat[0][3] * mat.mat[2][0] * mat.mat[3][1] + mat.mat[0][0] * mat.mat[2][3] * mat.mat[3][1] + mat.mat[0][1] * mat.mat[2][0] * mat.mat[3][3] - mat.mat[0][0] * mat.mat[2][1] * mat.mat[3][3]);
		m.mat[2][2] = d * (mat.mat[0][1] * mat.mat[1][3] * mat.mat[3][0] - mat.mat[0][3] * mat.mat[1][1] * mat.mat[3][0] + mat.mat[0][3] * mat.mat[1][0] * mat.mat[3][1] - mat.mat[0][0] * mat.mat[1][3] * mat.mat[3][1] - mat.mat[0][1] * mat.mat[1][0] * mat.mat[3][3] + mat.mat[0][0] * mat.mat[1][1] * mat.mat[3][3]);
		m.mat[2][3] = d * (mat.mat[0][3] * mat.mat[1][1] * mat.mat[2][0] - mat.mat[0][1] * mat.mat[1][3] * mat.mat[2][0] - mat.mat[0][3] * mat.mat[1][0] * mat.mat[2][1] + mat.mat[0][0] * mat.mat[1][3] * mat.mat[2][1] + mat.mat[0][1] * mat.mat[1][0] * mat.mat[2][3] - mat.mat[0][0] * mat.mat[1][1] * mat.mat[2][3]);
		m.mat[3][0] = d * (mat.mat[1][2] * mat.mat[2][1] * mat.mat[3][0] - mat.mat[1][1] * mat.mat[2][2] * mat.mat[3][0] - mat.mat[1][2] * mat.mat[2][0] * mat.mat[3][1] + mat.mat[1][0] * mat.mat[2][2] * mat.mat[3][1] + mat.mat[1][1] * mat.mat[2][0] * mat.mat[3][2] - mat.mat[1][0] * mat.mat[2][1] * mat.mat[3][2]);
		m.mat[3][1] = d * (mat.mat[0][1] * mat.mat[2][2] * mat.mat[3][0] - mat.mat[0][2] * mat.mat[2][1] * mat.mat[3][0] + mat.mat[0][2] * mat.mat[2][0] * mat.mat[3][1] - mat.mat[0][0] * mat.mat[2][2] * mat.mat[3][1] - mat.mat[0][1] * mat.mat[2][0] * mat.mat[3][2] + mat.mat[0][0] * mat.mat[2][1] * mat.mat[3][2]);
		m.mat[3][2] = d * (mat.mat[0][2] * mat.mat[1][1] * mat.mat[3][0] - mat.mat[0][1] * mat.mat[1][2] * mat.mat[3][0] - mat.mat[0][2] * mat.mat[1][0] * mat.mat[3][1] + mat.mat[0][0] * mat.mat[1][2] * mat.mat[3][1] + mat.mat[0][1] * mat.mat[1][0] * mat.mat[3][2] - mat.mat[0][0] * mat.mat[1][1] * mat.mat[3][2]);
		m.mat[3][3] = d * (mat.mat[0][1] * mat.mat[1][2] * mat.mat[2][0] - mat.mat[0][2] * mat.mat[1][1] * mat.mat[2][0] + mat.mat[0][2] * mat.mat[1][0] * mat.mat[2][1] - mat.mat[0][0] * mat.mat[1][2] * mat.mat[2][1] - mat.mat[0][1] * mat.mat[1][0] * mat.mat[2][2] + mat.mat[0][0] * mat.mat[1][1] * mat.mat[2][2]);

		return m;
	}

	Matrix MakeMatrix(float translation[], float rotation[], float scale[]) {
		Matrix trans_mat, rotate_mat, scale_mat;
		trans_mat=Translate(IdentityMatrix(), translation[0], translation[1], translation[2]);
		rotate_mat = MakeMatrixFromQuaternion(rotation[0], rotation[1], rotation[2], rotation[3]);
		scale_mat = Scale(IdentityMatrix(), scale[0], scale[1], scale[2]);
		return trans_mat*rotate_mat*scale_mat;
	}

    Plane MakePlane(Point3D pt0,Point3D pt1,Point3D pt2){
        Plane ret;
        ret.normal=pt1-pt0;
        ret.normal=Normalize(Cross(ret.normal,pt2-pt0));
        ret.distance=-Dot(ret.normal,pt0);
        return ret;
    }

    float PointToPlane(Point3D pt,Plane plane){
        return Dot(Normalize(plane.normal),pt)+plane.distance;        
    }

    int InFrustum(const Frustum &frustum,const BoundingBox &bbox){//Not strictly
        bool intersect=false;
        for(int i=0;i<6;i++){
            float mindis,maxdis;
            mindis=INF;maxdis=-INF;
            for(int j=0;j<8;j++){
                float dis=PointToPlane(bbox.corners[j],frustum.planes[i]);
                if(dis>maxdis)
                    maxdis=dis;
                if(dis<mindis)
                    mindis=dis;
            }
            if(mindis>0 && maxdis>0) return OUTSIDE;
            else if(mindis<0 && maxdis>0) intersect=true;
        }
        return intersect?INTERSECT:INSIDE;
    }
}

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
#ifndef _SCENENODE_H_INCLUDED
#define _SCENENODE_H_INCLUDED

#include <tinymath3d.h>
#include <vector>

namespace unlike3d {
	class SceneNode {
	protected:
		math3d::BoundingBox bounding_box_;
		unsigned int handled_;
		bool new_thread_;
		bool dirty_;
		std::vector<SceneNode *> children_;
		SceneNode *parent_;
	public:
		virtual bool Init();
		virtual int InFrustum(math3d::Frustum &frustum);
		virtual void MarkDirty();

		virtual std::vector<SceneNode *> &GetChildren();
		virtual SceneNode *GetParent();

		

		SceneNode();
		virtual ~SceneNode();
	};
}

#endif
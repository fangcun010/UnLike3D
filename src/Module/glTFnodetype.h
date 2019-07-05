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
#ifndef _GLTF_NODE_TYPE_H_INCLUDED
#define _GLTF_NODE_TYPE_H_INCLUDED

#include <objecttype.h>
#include <tinymath3d.h>
#include <tiny_gltf.h>
#include "renderobjectinfo.h"

#define			OBJECT_TYPE_GLTFNODE						"glTFNode"

namespace unlike3d {
	class glTFNodeType {
	private:
		Object *resource_object_;
		tinygltf::TinyGLTF glTF_loader_;
		tinygltf::Model glTF_model_;
		std::string glTF_source_;
		ukSceneNode *scene_node_;
		math3d::Vector3D position_;
		RenderObjectInfo *render_object_info_;
	private:
		static ukObjectTypeInterface *object_type_interface_;
	private:
		static ukFunctionDefineListInterface *InitObjectTypeInterfaceFunctions();
		static ukPropertyDefineListInterface *InitObjectTypeInterfaceProperties();
		static ukObjectDefineListInterface *InitObjectTypeInterfaceObjects();
	public:
		virtual bool Init();

		static void InitObjectTypeInterface();
		static ukObjectTypeInterface *GetObjectTypeInterface();

		virtual void SetProperty(const std::string &name, const std::string &value);
		virtual void *GetProperty(const std::string &name);

		virtual void SetPosition(float x, float y, float z);

		virtual void SetRenderObjectInfo(RenderObjectInfo *render_object_info);
		virtual void SetSceneNode(ukSceneNode *scene_node);
		virtual const std::string &GetglTFSource();

		virtual Object *GetResource();
		virtual void SetResource(ukObject *object);

		virtual bool LoadglTFSource(const std::string &name);

		glTFNodeType();
		virtual ~glTFNodeType();
	};
}

#endif
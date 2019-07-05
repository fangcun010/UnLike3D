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
#include "glTFnodetype.h"
#include "glTFnode.h"

#include "renderobjectinfo.h"
#include <iostream>

namespace unlike3d {
	ukObjectTypeInterface *glTFNodeType::object_type_interface_ = nullptr;

	bool glTFNodeType::Init() {
		return true;
	}

	Object *glTFNodeType::GetResource() {
		return resource_object_;
	}

	void glTFNodeType::SetResource(ukObject *object) {
		ObjectType *obj_type = new ObjectType();
		obj_type->LoadFromObjectTypeInterface(object->object_type_interface);
		Object *obj = new Object();
		obj->SetObjectType(obj_type);
		obj->SetNativeObject(object);
		resource_object_ = obj;
	}

	bool glTFNodeType::LoadglTFSource(const std::string &name) {
		std::string err, warn;
		glTF_loader_.LoadBinaryFromFile(&glTF_model_, &err, &warn, name);
		return true;
	}

	void glTFNodeType::SetProperty(const std::string &name, const std::string &value) {
		std::vector<FunctionParameter> fp;
		std::vector<FunctionReturn> fr;
		if (name == "source") {
			fp.resize(1);
			fp[0].string_value = value;
			GetResource()->CallFunction("GetBinarySource", fp, fr);
			std::string source = fr[0].string_value;;
			//LoadglTFSource(source);
			glTF_source_ = source;
		}
		else if (name == "position") {
			float x, y, z;
			sscanf(value.c_str(), "(%f,%f,%f)", &x, &y, &z);
			SetPosition(x, y, z);
		}
	}

	void *glTFNodeType::GetProperty(const std::string &name) {
		if (name == "source") {
			return (void *)GetglTFSource().c_str();
		}
	}

	const std::string &glTFNodeType::GetglTFSource() {
		return glTF_source_;
	}

	ukFunctionDefineListInterface *glTFNodeType::InitObjectTypeInterfaceFunctions() {
		ukFunctionDefineListInterface *function_list = ukMakeFunctionDefineListInterface(1);
		ukFunctionReturnDefineListInterface *return_list=ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "result", ukINTEGER);
		ukSetFunctionDefineInterface(function_list, 0, "Init",ukSCRIPT,
			ukMakeFunctionParameterDefineListInterface(0),
			return_list,
			glTFNodeTypeInitFunction);
		return function_list;
	}

	ukPropertyDefineListInterface *glTFNodeType::InitObjectTypeInterfaceProperties() {
		ukPropertyDefineListInterface *property_list = ukMakePropertyDefineListInterface(0);
		return property_list;
	}

	ukObjectDefineListInterface *glTFNodeType::InitObjectTypeInterfaceObjects() {
		ukObjectDefineListInterface *object_list = ukMakeObjectDefineListInterface(0);
		return object_list;
	}

	void glTFNodeType::InitObjectTypeInterface() {
		ukFunctionDefineListInterface *func_list = InitObjectTypeInterfaceFunctions();
		ukPropertyDefineListInterface *prop_list = InitObjectTypeInterfaceProperties();
		ukObjectDefineListInterface *obj_list = InitObjectTypeInterfaceObjects();
		object_type_interface_ = ukMakeObjectTypeInterface(OBJECT_TYPE_GLTFNODE, glTFNodeTypeCreateObject,
			glTFNodeTypeDestroyObject,
			glTFNodeTypeGetProperty,
			glTFNodeTypeSetProperty,
			func_list,
			prop_list,
			obj_list);
	}

	ukObjectTypeInterface *glTFNodeType::GetObjectTypeInterface() {
		if (object_type_interface_ == nullptr) {
			InitObjectTypeInterface();
		}
		return object_type_interface_;
	}

	void glTFNodeType::SetPosition(float x, float y, float z) {
		RenderObjectInfo *render_object_info = (RenderObjectInfo *)scene_node_->data3.point;
		render_object_info->translation[0] = x;
		render_object_info->translation[1] = y;
		render_object_info->translation[2] = z;
		render_object_info->use_translation = true;
		UpdateRenderObjectInfoMatrix(render_object_info);
	}

	void glTFNodeType::SetRenderObjectInfo(RenderObjectInfo *render_object_info) {
		render_object_info_ = render_object_info;
	}

	void glTFNodeType::SetSceneNode(ukSceneNode *scene_node) {
		scene_node_ = scene_node;
	}

	glTFNodeType::glTFNodeType() {

	}

	glTFNodeType::~glTFNodeType() {

	}
}

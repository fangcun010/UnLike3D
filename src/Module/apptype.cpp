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
#include "apptype.h"
#include "app.h"
#include <iostream>

namespace unlike3d {
	ukObjectTypeInterface *AppType::object_type_interface_ = nullptr;

	bool AppType::Init() {
		SetResourceObject();
		SetViewerObject();
		Object *res = GetResource();
		std::vector<FunctionParameter> fp;
		std::vector<FunctionReturn> fr;
		fp.resize(1);
		fp[0].point_value = module_manager_;
		res->CallFunction("SetModuleManager",fp ,fr);
		fp.clear();
		fr.clear();
		res->CallFunction("Init", fp, fr);
		bool ret = fr[0].integer_value;
		if (ret == false) return false;
		fr.clear();
		fp.clear();
		fp.resize(1);
		fp[0].string_value = resource_xml_file_;
		fr.clear();
		res->CallFunction("LoadResource", fp, fr);
		ret = fr[0].integer_value;
		if (ret == false) return false;
		Object *viewer = GetViewer();
		fr.clear();
		fp.clear();
		viewer->CallFunction("Init", fp, fr);
		ret = fr[0].integer_value;
		if (ret == false) return false;
		viewer->CallFunction("SetApplication", GetNativeObject(),nullptr);
		viewer->CallFunction("SetResource",res->GetNativeObject(),nullptr);
		return true;
	}

	bool AppType::Loop() {
		std::vector<FunctionParameter> fp;
		std::vector<FunctionReturn> fr;
		GetViewer()->CallFunction("Loop", fp, fr);
		return fr[0].integer_value;
	}

	void AppType::SetModuleManager(ModuleManager *module_manager) {
		module_manager_ = module_manager;
	}

	void AppType::SetResourceXMLFile(const char *strFile) {
		resource_xml_file_=strFile;
	}

	void AppType::SetResourceObject() {
		for (unsigned int i = 0; i < native_object_->object_count; i++) {
			ukObject *res_obj = native_object_->objects[i];
			if (!strcmp(res_obj->object_type_interface->name, "Resource")) {
				ObjectType *obj_type = new ObjectType();
				Object *obj = new Object();
				obj_type->LoadFromObjectTypeInterface(res_obj->object_type_interface);
				obj->SetNativeObject(res_obj);
				obj->SetObjectType(obj_type);
				resource_object_ = obj;
				resource_object_type_ = obj_type;
				break;
			}
		}
	}

	void AppType::SetViewerObject() {
		for (unsigned int i = 0; i < native_object_->object_count; i++) {
			ukObject *viewer_obj = native_object_->objects[i];
			if (!strcmp(viewer_obj->object_type_interface->name, "Viewer")) {
				ObjectType *obj_type = new ObjectType();
				Object *obj = new Object();
				obj_type->LoadFromObjectTypeInterface(viewer_obj->object_type_interface);
				obj->SetNativeObject(viewer_obj);
				obj->SetObjectType(obj_type);
				viewer_object_ = obj;
				viewer_object_type_ = obj_type;
				break;
			}
		}
	}

	void AppType::SetNativeObject(ukObject *native_object) {
		native_object_ = native_object;
	}

	ukObject *AppType::GetNativeObject() {
		return native_object_;
	}

	Object *AppType::GetResource() {
		return resource_object_;
	}

	Object *AppType::GetViewer() {
		return viewer_object_;
	}

	unsigned int AppType::GetThreadCount() {
		std::vector<FunctionParameter> fp;
		std::vector<FunctionReturn> fr;
		GetViewer()->CallFunction("GetThreadCount", fp, fr);
		return fr[0].integer_value;
	}

	void AppType::AddThreadTask(int thread_id, ukVoidFuncPoint function_point,void *parameter) {
		std::vector<FunctionParameter> fp;
		std::vector<FunctionReturn> fr;
		fp.resize(3);
		fp[0].integer_value = thread_id;
		fp[1].point_value = function_point;
		fp[2].point_value = parameter;
		GetViewer()->CallFunction("AddThreadTask", fp, fr);
	}

	void AppType::WaitAllThreadTaskEnd() {
		std::vector<FunctionParameter> fp;
		std::vector<FunctionReturn> fr;
		GetViewer()->CallFunction("WaitAllThreadTaskEnd",fp,fr);
	}

	ukFunctionDefineListInterface *AppType::InitObjectTypeInterfaceFunctions() {
		ukFunctionDefineListInterface *function_list = ukMakeFunctionDefineListInterface(7);
		ukFunctionReturnDefineListInterface *return_list = ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "result", ukINTEGER);
		ukSetFunctionDefineInterface(function_list, 0, "Init",ukSCRIPT,
			ukMakeFunctionParameterDefineListInterface(0),
			return_list,
			AppTypeInitFunction);
		ukFunctionParameterDefineListInterface *parameter_list = ukMakeFunctionParameterDefineListInterface(1);
		ukSetFunctionParameterDefineInterface(parameter_list, 0, "ResourceXMLFile", ukSTRING);
		ukSetFunctionDefineInterface(function_list, 1, "SetResourceXMLFile", ukENGINE,
			parameter_list,
			ukMakeFunctionReturnDefineListInterface(0),
			AppTypeSetResourceXMLFileFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(1);
		ukSetFunctionParameterDefineInterface(parameter_list, 0, "ModuleManager", ukPOINT);
		ukSetFunctionDefineInterface(function_list, 2, "SetModuleManager", ukENGINE,
			parameter_list,
			ukMakeFunctionReturnDefineListInterface(0),
			AppTypeSetModuleManagerFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(0);
		return_list = ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "result", ukINTEGER);
		ukSetFunctionDefineInterface(function_list, 3, "Loop", ukSCRIPT,
			parameter_list,
			return_list,
			AppTypeLoopFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(0);
		return_list = ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "ThreadCount", ukINTEGER);
		ukSetFunctionDefineInterface(function_list, 4, "GetThreadCount", ukENGINE,
			parameter_list,
			return_list,
			AppTypeGetThreadCountFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(3);
		ukSetFunctionParameterDefineInterface(parameter_list, 0, "ThreadID", ukINTEGER);
		ukSetFunctionParameterDefineInterface(parameter_list, 1, "Task", ukPOINT);
		ukSetFunctionParameterDefineInterface(parameter_list, 2, "Parameter", ukPOINT);
		return_list = ukMakeFunctionReturnDefineListInterface(0);
		ukSetFunctionDefineInterface(function_list, 5, "AddThreadTask", ukENGINE,
			parameter_list,
			return_list,
			AppTypeAddThreadTaskFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(0);;
		return_list = ukMakeFunctionReturnDefineListInterface(0);
		ukSetFunctionDefineInterface(function_list, 6, "WaitAllThreadTaskEnd", ukENGINE,
			parameter_list,
			return_list,
			AppTypeWaitAllThreadTaskEndFunction);
		return function_list;
	}

	ukPropertyDefineListInterface *AppType::InitObjectTypeInterfaceProperties() {
		ukPropertyDefineListInterface *property_list = ukMakePropertyDefineListInterface(0);
		return property_list;
	}

	ukObjectDefineListInterface *AppType::InitObjectTypeInterfaceObjects() {
		ukObjectDefineListInterface *object_list = ukMakeObjectDefineListInterface(2);
		ukSetObjectDefineInterface(object_list, 0, "viewer", ukNORMAL, "viewer", "Viewer");
		ukSetObjectDefineInterface(object_list, 1, "resource",ukNORMAL, "resource", "Resource");
		return object_list;
	}

	void AppType::InitObjectTypeInterface() {
		ukFunctionDefineListInterface *func_list = InitObjectTypeInterfaceFunctions();
		ukPropertyDefineListInterface *prop_list = InitObjectTypeInterfaceProperties();
		ukObjectDefineListInterface *obj_list = InitObjectTypeInterfaceObjects();
		object_type_interface_ = ukMakeObjectTypeInterface(OBJECT_TYPE_APP, AppTypeCreateObject,
			AppTypeDestroyObject,
			AppTypeGetProperty,
			AppTypeSetProperty,
			func_list,
			prop_list,
			obj_list);
	}

	ukObjectTypeInterface *AppType::GetObjectTypeInterface() {
		if (object_type_interface_ == nullptr) {
			InitObjectTypeInterface();
		}
		return object_type_interface_;
	}

	AppType::AppType() :resource_object_(nullptr), viewer_object_(nullptr) {

	}

	AppType::~AppType() {

	}
}

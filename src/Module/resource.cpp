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
#include <windows.h>
#include <iostream>
#include <string>
#include <map>
#include "resource.h"
#include "resourcetype.h"

#define DLL_EXPORT extern "C" __declspec(dllexport)

std::map<std::string, ukObjectTypeInterface *> obj_type_map;

DLL_EXPORT void *ResourceTypeCreateObject() {
	ukObject *obj = new ukObject();
	obj->data1.point = new unlike3d::ResourceType();
	return obj;
}

DLL_EXPORT bool ResourceTypeDestroyObject(void *object) {
	ukObject *obj = (ukObject *)object;
	unlike3d::ResourceType *res_obj = (unlike3d::ResourceType *)obj->data1.point;
	delete res_obj;
	delete obj;
	return true;
}

DLL_EXPORT void *ResourceTypeGetProperty(void *object, void *name) {
	return nullptr;
}

DLL_EXPORT void ResourceTypeSetProperty(void *object, void *name, void *value) {

}

DLL_EXPORT void ResourceTypeInitFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "resource_object init..." << std::endl;
	ukObject *obj= (ukObject *)object;
	unlike3d::ResourceType *res_obj = (unlike3d::ResourceType *)obj->data1.point;
	bool ret=res_obj->Init();
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	ret_list->count = 1;
	ret_list->return_point[0].integer_value = ret;
}

DLL_EXPORT void ResourceTypeSetModuleManagerFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "resource_object set_module_manager..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ResourceType *res_obj = (unlike3d::ResourceType *)obj->data1.point;
	ukFunctionParameterListInterface *param_list = (ukFunctionParameterListInterface *)parameter_list;
	res_obj->SetModuleManager((unlike3d::ModuleManager *)param_list->parameter_point[0].point_value);
}

DLL_EXPORT void ResourceTypeGetSceneSourceFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "resource_object get_scene_source..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ResourceType *res_obj = (unlike3d::ResourceType *)obj->data1.point;
	ukFunctionParameterListInterface *param_list = (ukFunctionParameterListInterface *)parameter_list;
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	std::string ret = res_obj->GetSceneSource(param_list->parameter_point[0].string_value.buffer);
	ret_list->count = 1;
	ukStringCopy(&ret_list->return_point[0].string_value, ret.c_str());
}

DLL_EXPORT void ResourceTypeGetBinarySourceFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "resource_object get_binary_source..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ResourceType *res_obj = (unlike3d::ResourceType *)obj->data1.point;
	ukFunctionParameterListInterface *param_list = (ukFunctionParameterListInterface *)parameter_list;
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	std::string ret = res_obj->GetBinarySource(param_list->parameter_point[0].string_value.buffer);
	ret_list->count = 1;
	ukStringCopy(&ret_list->return_point[0].string_value, ret.c_str());
}

DLL_EXPORT void ResourceTypeLoadResourceFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "resource_object load_resource..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ResourceType *res_obj = (unlike3d::ResourceType *)obj->data1.point;
	ukFunctionParameterListInterface *param_list = (ukFunctionParameterListInterface *)parameter_list;
	std::cout << "load resource xml file:" << param_list->parameter_point[0].string_value.buffer << std::endl;
	bool ret = res_obj->LoadResource(param_list->parameter_point[0].string_value.buffer);
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	ret_list->count = 1;
	ret_list->return_point[0].integer_value = ret;
}

DLL_EXPORT void ResourceTypeLoadNodeSetFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "resource_object load_node_set..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ResourceType *res_obj = (unlike3d::ResourceType *)obj->data1.point;
	ukFunctionParameterListInterface *param_list = (ukFunctionParameterListInterface *)parameter_list;
	bool ret = res_obj->LoadNodeSet(param_list->parameter_point[0].string_value.buffer);
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	ret_list->count = 1;
	ret_list->return_point[0].integer_value = ret;
}

DLL_EXPORT void ResourceTypeCreateSceneNodeFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "resource_object create_scene_node..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ResourceType *res_obj = (unlike3d::ResourceType *)obj->data1.point;
	std::string name = (char *)parameter_list;
	unlike3d::Object **node_object = (unlike3d::Object **)return_list;
	*node_object = res_obj->CreateSceneNode(name);
}

DLL_EXPORT void ResourceTypeCreateObjectFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "resource_object create_object..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ResourceType *res_obj = (unlike3d::ResourceType *)obj->data1.point;
	ukFunctionParameterListInterface *param_list = (ukFunctionParameterListInterface *)parameter_list;
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	std::string module = param_list->parameter_point[0].string_value.buffer;
	std::string obj_type = param_list->parameter_point[1].string_value.buffer;
	if (res_obj->LoadModuleObjectType(module,obj_type)) {
		ret_list->return_point[0].point_value=res_obj->CreateObject(module.c_str(),obj_type.c_str());
	}
	else {
		ret_list->return_point[0].point_value = nullptr;
	}
}

DLL_EXPORT void ResourceTypeGetFunctionFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "resource_object get_function..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ResourceType *res_obj = (unlike3d::ResourceType *)obj->data1.point;
	ukFunctionParameterListInterface *param_list = (ukFunctionParameterListInterface *)parameter_list;
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	std::string module = param_list->parameter_point[0].string_value.buffer;
	std::string function_name = param_list->parameter_point[1].string_value.buffer;
	ukBoolFuncPoint func_point = res_obj->GetFunction(module.c_str(), function_name.c_str());
	ret_list->return_point[0].point_value = func_point;
}

DLL_EXPORT void ResourceTypeGetPipelineSourceFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "resource_object get_pipeline_source..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ResourceType *res_obj = (unlike3d::ResourceType *)obj->data1.point;
	ukFunctionParameterListInterface *param_list = (ukFunctionParameterListInterface *)parameter_list;
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	std::string ret = res_obj->GetPipelineSource(param_list->parameter_point[0].string_value.buffer);
	ukStringCopy(&ret_list->return_point[0].string_value, ret.c_str());
}

void ResourceTypeInit() {
	unlike3d::ResourceType::InitObjectTypeInterface();
}

DLL_EXPORT bool ukInit(void *data) {
	ResourceTypeInit();
	obj_type_map[OBJECT_TYPE_RESOURCE] = unlike3d::ResourceType::GetObjectTypeInterface();

	return true;
}

DLL_EXPORT ukObjectTypeInterface *ukGetObjectType(const char *strName) {
	return obj_type_map[strName];
}

#undef DLL_EXPORT
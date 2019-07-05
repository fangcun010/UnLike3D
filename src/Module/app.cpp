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
#include "apptype.h"
#include "app.h"
#include "apptype.h"

#define DLL_EXPORT extern "C" __declspec(dllexport)

std::map<std::string, ukObjectTypeInterface *> obj_type_map;

DLL_EXPORT void *AppTypeCreateObject() {
	ukObject *obj = new ukObject();
	obj->data1.point = new unlike3d::AppType();
	return obj;
}

DLL_EXPORT bool AppTypeDestroyObject(void *object) {
	ukObject *obj = (ukObject *)object;
	unlike3d::AppType *app_obj = (unlike3d::AppType *)obj->data1.point;
	delete app_obj;
	delete obj;
	return true;
}

DLL_EXPORT void *AppTypeGetProperty(void *object, void *name) {
	return nullptr;
}

DLL_EXPORT void AppTypeSetProperty(void *object, void *name, void *value) {

}

DLL_EXPORT void AppTypeInitFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "app_object init..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::AppType *app_obj = (unlike3d::AppType *)obj->data1.point;
	app_obj->SetNativeObject(obj);
	bool ret=app_obj->Init();
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	ret_list->return_point[0].integer_value = ret;
}

DLL_EXPORT void AppTypeSetResourceXMLFileFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "app_object set_resource_xml_file..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::AppType *app_obj = (unlike3d::AppType *)obj->data1.point;
	ukFunctionParameterListInterface *param_list = (ukFunctionParameterListInterface *)parameter_list;
	app_obj->SetResourceXMLFile(param_list->parameter_point[0].string_value.buffer);
}

DLL_EXPORT void AppTypeSetModuleManagerFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "app_object set_module_manager..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::AppType *app_obj = (unlike3d::AppType *)obj->data1.point;
	ukFunctionParameterListInterface *param_list = (ukFunctionParameterListInterface *)parameter_list;
	app_obj->SetModuleManager((unlike3d::ModuleManager *)param_list->parameter_point[0].point_value);
}

DLL_EXPORT void AppTypeLoopFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "app_object loop..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::AppType *app_obj = (unlike3d::AppType *)obj->data1.point;
	ukFunctionParameterListInterface *param_list = (ukFunctionParameterListInterface *)parameter_list;
	bool ret=app_obj->Loop();
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	ret_list->return_point[0].integer_value = ret;
}

DLL_EXPORT void AppTypeGetThreadCountFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "app_object get_thread_count..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::AppType *app_obj = (unlike3d::AppType *)obj->data1.point;
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	ret_list->return_point[0].integer_value = app_obj->GetThreadCount();
}

DLL_EXPORT void AppTypeAddThreadTaskFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "app_object add_thread_task..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::AppType *app_obj = (unlike3d::AppType *)obj->data1.point;
	ukFunctionParameterListInterface *param_list = (ukFunctionParameterListInterface *)parameter_list;
	int thread_id = param_list->parameter_point[0].integer_value;
	ukVoidFuncPoint function_point = (ukVoidFuncPoint)param_list->parameter_point[1].point_value;
	void *parameter = param_list->parameter_point[2].point_value;
	app_obj->AddThreadTask(thread_id,function_point,parameter);
}

DLL_EXPORT void AppTypeWaitAllThreadTaskEndFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "app_object wait_all_thread_task_end..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::AppType *app_obj = (unlike3d::AppType *)obj->data1.point;
	app_obj->WaitAllThreadTaskEnd();
}

void AppTypeInit() {
	unlike3d::AppType::InitObjectTypeInterface();
}

DLL_EXPORT bool ukInit(void *data) {
	AppTypeInit();
	obj_type_map[OBJECT_TYPE_APP] = unlike3d::AppType::GetObjectTypeInterface();
	return true;
}

DLL_EXPORT ukObjectTypeInterface *ukGetObjectType(const char *strName) {
	return obj_type_map[strName];
}

#undef DLL_EXPORT
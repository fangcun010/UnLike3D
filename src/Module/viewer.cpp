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
#include <string>
#include <map>
#include "viewertype.h"
#include "viewer.h"

#define DLL_EXPORT extern "C" __declspec(dllexport)

std::map<std::string, ukObjectTypeInterface *> obj_type_map;

DLL_EXPORT void *ViewerTypeCreateObject() {
	ukObject *obj = new ukObject();
	obj->data1.point = new unlike3d::ViewerType();
	return obj;
}

DLL_EXPORT bool ViewerTypeDestroyObject(void *object) {
	ukObject *obj = (ukObject *)object;
	unlike3d::ViewerType *viewer_obj = (unlike3d::ViewerType *)obj->data1.point;
	delete viewer_obj;
	delete obj;
	return true;
}

DLL_EXPORT void *ViewerTypeGetProperty(void *object, void *name) {
	return nullptr;
}

DLL_EXPORT void ViewerTypeSetProperty(void *object, void *name, void *value) {

}

DLL_EXPORT void ViewerTypeLoadSceneFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "viewer_object load_scene..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ViewerType *viewer_obj = (unlike3d::ViewerType *)obj->data1.point;
	ukFunctionParameterListInterface *param_list = (ukFunctionParameterListInterface *)parameter_list;
	bool ret = viewer_obj->LoadScene(param_list->parameter_point[0].string_value.buffer);
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	ret_list->count = 1;
	ret_list->return_point[0].integer_value = ret;
}

DLL_EXPORT void ViewerTypeLoadPipelineFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "viewer_object load_pipeline..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ViewerType *viewer_obj = (unlike3d::ViewerType *)obj->data1.point;
	ukFunctionParameterListInterface *param_list = (ukFunctionParameterListInterface *)parameter_list;
	bool ret = viewer_obj->LoadPipeline(param_list->parameter_point[0].string_value.buffer);
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	ret_list->count = 1;
	ret_list->return_point[0].integer_value = ret;
}

DLL_EXPORT void ViewerTypeSetResourceFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "viewer_object set_resource..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ViewerType *viewer_obj = (unlike3d::ViewerType *)obj->data1.point;
	ukObject *res_native_object = (ukObject *)parameter_list;
	unlike3d::ObjectType *res_object_type = new unlike3d::ObjectType();
	unlike3d::Object *res_object = new unlike3d::Object();
	res_object_type->LoadFromObjectTypeInterface(res_native_object->object_type_interface);
	res_object->SetObjectType(res_object_type);
	res_object->SetNativeObject(res_native_object);
	viewer_obj->SetResource(res_object);
}

DLL_EXPORT void ViewerTypeSetApplicationFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "viewer_object set_application..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ViewerType *viewer_obj = (unlike3d::ViewerType *)obj->data1.point;
	ukObject *app_native_object = (ukObject *)parameter_list;
	unlike3d::ObjectType *app_object_type = new unlike3d::ObjectType();
	unlike3d::Object *app_object = new unlike3d::Object();
	app_object_type->LoadFromObjectTypeInterface(app_native_object->object_type_interface);
	app_object->SetObjectType(app_object_type);
	app_object->SetNativeObject(app_native_object);
	viewer_obj->SetApplication(app_object);
}

DLL_EXPORT void ViewerTypeInitThreadPoolFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "viewer_object init_thread_count..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ViewerType *viewer_obj = (unlike3d::ViewerType *)obj->data1.point;
	ukFunctionParameterListInterface *param_list= (ukFunctionParameterListInterface *)parameter_list;
	viewer_obj->InitThreadPool(param_list->parameter_point[0].integer_value);
}

DLL_EXPORT void ViewerTypeInitThreadPoolWithRecommendThreadCountFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "viewer_object init_thread_count..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ViewerType *viewer_obj = (unlike3d::ViewerType *)obj->data1.point;
	viewer_obj->InitThreadPoolWithRecommendThreadCount();
}

DLL_EXPORT void ViewerTypeGetThreadCountFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "viewer_object get_thread_count..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ViewerType *viewer_obj = (unlike3d::ViewerType *)obj->data1.point;
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	ret_list->return_point[0].integer_value = viewer_obj->GetThreadCount();
}

static void ukDoThreadTask(void *_thread_id, void *_function_point, void *_parameter, void *data4, void *data5) {

}

DLL_EXPORT void ViewerTypeAddThreadTaskFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "viewer_object add_thread_task..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ViewerType *viewer_obj = (unlike3d::ViewerType *)obj->data1.point;
	ukFunctionParameterListInterface *param_list = (ukFunctionParameterListInterface *)parameter_list;
	unlike3d::ThreadPool::Task task;
	task.function_point = ukDoThreadTask;
	int thread_id = param_list->parameter_point[0].integer_value;
	task.data1_point = param_list->parameter_point[1].point_value;
	task.data2_point = param_list->parameter_point[2].point_value;
	viewer_obj->AddThreadTask(thread_id, task);
}

DLL_EXPORT void ViewerTypeWaitAllThreadTaskEndFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "viewer_object wait_all_thread_task_end..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ViewerType *viewer_obj = (unlike3d::ViewerType *)obj->data1.point;
	viewer_obj->WaitAllThreadTaskEnd();
}

DLL_EXPORT void ViewerTypeLoopFunction(void *object, void *parameter_list, void *return_list) {
	ukObject *obj = (ukObject *)object;
	unlike3d::ViewerType *viewer_obj = (unlike3d::ViewerType *)obj->data1.point;
	bool ret = viewer_obj->Loop();
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	ret_list->return_point[0].integer_value = ret;
}

DLL_EXPORT void ViewerTypeInitPipelineFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "viewer_object init_pipeline..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ViewerType *viewer_obj = (unlike3d::ViewerType *)obj->data1.point;
	bool ret = viewer_obj->InitPipeline();
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	ret_list->return_point[0].integer_value = ret;
}

DLL_EXPORT void ViewerTypeDestroyPipelineFunction(void *object, void *parameter_list, void *return_list) {
	std::cout << "viewer_object destroy_pipeline..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ViewerType *viewer_obj = (unlike3d::ViewerType *)obj->data1.point;
	bool ret = viewer_obj->DestroyPipeline();
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	ret_list->return_point[0].integer_value = ret;
}

DLL_EXPORT void ViewerTypeInitFunction(void *object, void *parameter_list,void *return_list) {
	std::cout << "viewer_object init..." << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::ViewerType *viewer_obj = (unlike3d::ViewerType *)obj->data1.point;
	bool ret=viewer_obj->Init();
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	ret_list->return_point[0].integer_value = ret;
}

DLL_EXPORT void ViewerTypeLookFunction(void *object, void *parameter_list, void *return_list) {
	ukObject *obj = (ukObject *)object;
	unlike3d::ViewerType *viewer_obj = (unlike3d::ViewerType *)obj->data1.point;
	bool ret=viewer_obj->Look();
	ukFunctionReturnListInterface *ret_list = (ukFunctionReturnListInterface *)return_list;
	ret_list->return_point[0].integer_value = ret;
}

void ViewerTypeInit() {
	unlike3d::ViewerType::InitObjectTypeInterface();
}

DLL_EXPORT bool ukInit(void *data) {
	ViewerTypeInit();
	obj_type_map[OBJECT_TYPE_VIEWER] = unlike3d::ViewerType::GetObjectTypeInterface();

	return true;
}

DLL_EXPORT ukObjectTypeInterface *ukGetObjectType(const char *strName) {
	return obj_type_map[strName];
}

#undef DLL_EXPORT
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
#include "viewertype.h"
#include "viewer.h"

#include <iostream>
#include <algorithm>

namespace unlike3d {
	ukObjectTypeInterface *ViewerType::object_type_interface_=nullptr;

	bool ViewerType::Init() {
		return true;
	}

	void ViewerType::SetApplication(Object *application_object) {
		application_object_ = application_object;
		pipeline_.SetApplication(application_object_);
	}

	Object *ViewerType::GetApplication() {
		return application_object_;
	}

	void ViewerType::SetResource(Object *resource_object) {
		resource_object_ = resource_object;
		scene_xml_.SetResource(resource_object_);
		pipeline_.SetResource(resource_object_);
	}

	Object *ViewerType::GetResource() {
		return resource_object_;
	}

	bool ViewerType::InitPipeline() {
		return pipeline_.InitPipeline();
	}

	bool ViewerType::DestroyPipeline() {
		return pipeline_.DestroyPipeline();
	}

	static void ukViewerTypeAddHandleObject(ViewerType *viewer_obj,ukSceneNode *scene_node) {
		for (unsigned int i = 0; i <= MAX_HANDLE_QUEUE_COUNT-1; i++)
			if ((1 << i)&scene_node->handled)
				viewer_obj->handle_object_queue_[i].Push(scene_node);
	}

	static void ukViewerTypeCullObjects(void *_thread_id,void *_viewer_obj, void *_scene_node_list, void *_frustum, void *data4) {
		ViewerType *viewer_obj = (ViewerType *)_viewer_obj;
		ukSceneNodeList *scene_node_list = (ukSceneNodeList *)_scene_node_list;
		math3d::Frustum *frustum = (math3d::Frustum *)_frustum;
		unsigned int count = scene_node_list->count;
		if (count == 0) return;
		ukSceneNode **list = scene_node_list->list;
		for (unsigned int i = 0; i < count; i++) {
			int ret = list[i]->in_frustum(list[i], frustum);
			if (ret == math3d::INSIDE) {
				ukViewerTypeAddHandleObject(viewer_obj, list[i]);
				ukViewerTypeAddAllSubObjects(_thread_id,viewer_obj, list[i],nullptr,nullptr);
			}
			else if (ret == math3d::INTERSECT) {
				if (list[i]->new_thread == true) {
					ThreadPool::Task task;
					task.data1_point = viewer_obj;
					task.data2_point = &list[i]->children;
					task.data3_point = frustum;
					ukViewerTypeAddHandleObject(viewer_obj, list[i]);
					viewer_obj->thread_pool_.AddTask(task);
				}
				else {
					ukViewerTypeAddHandleObject(viewer_obj, list[i]);
					ukViewerTypeCullObjects(_thread_id,viewer_obj, &list[i]->children, frustum, nullptr);
				}
			}
		}
	}

	static void ukViewerTypeAddAllSubObjects(void *_thread_id,void *_viewer_obj,void *_scene_node,void *data3,void *data4) {
		ViewerType *viewer_obj = (ViewerType *)_viewer_obj;
		ukSceneNode *scene_node = (ukSceneNode *)_scene_node;
		unsigned int count = scene_node->children.count;
		if (count == 0) return;
		if (scene_node->new_thread == true) {
			ukSceneNode **list = scene_node->children.list;
			for (unsigned int i = 0; i < count; i++) {
				ThreadPool::Task task;
				task.data1_point = viewer_obj;
				task.data2_point = &list[i]->children;
				task.function_point = ukViewerTypeAddAllSubObjects;
				ukViewerTypeAddHandleObject(viewer_obj, list[i]);
				viewer_obj->thread_pool_.AddTask(task);
			}
		}
		else
		{
			ukSceneNode **list = scene_node->children.list;
			for (unsigned int i = 0; i < count; i++) {
				ukViewerTypeAddHandleObject(viewer_obj, list[i]);
				ukViewerTypeAddAllSubObjects(_thread_id,viewer_obj,list[i],nullptr,nullptr);
			}
		}
	}

	void ViewerType::CullObjects(ukSceneNodeList *scene_node_list) {
		thread_pool_.ResetStatus();
		ukViewerTypeCullObjects(&thread_pool_.thread_ids_[0],this, scene_node_list, GetCamera().GetFrustum(), nullptr);
		ThreadPool::Task task;
		while (thread_pool_.GetStatus() != ThreadPool::ENDING) {
			if (thread_pool_.GetTask(task)) {
				task.function_point(&thread_pool_.thread_ids_[0],task.data1_point, task.data2_point, task.data3_point,
					task.data4_point);
				thread_pool_.task_count_--;
			}
		}
	}

	void ViewerType::CopyHandleObjectToSortList() {
		for (unsigned int i = 0;i < MAX_HANDLE_QUEUE_COUNT ; i++) {
			ukSceneNode *node;
			sort_list_[i].clear();
			while (handle_object_queue_[i].Pop(node)) {
				sort_list_[i].push_back(node);
			}
			const Pipeline::SortEntry *sort_entry = pipeline_.GetSortEntry(i);
			if (sort_entry == nullptr) continue;
			std::sort(sort_list_[i].begin(), sort_list_[i].end(), [&pipeline_obj = *pipeline_.GetPipelineObject(),
				&sort_func=sort_entry->function_point](const ukSceneNode *node_a, const ukSceneNode *node_b)->bool{
				return sort_func(&pipeline_obj, (void *)node_a, (void *)node_b);
			});
		}
	}

	void ViewerType::DoLoadSceneFunctions(ukSceneNodeList *node_list) {
		unsigned int count = node_list->count;
		for (unsigned int i = 0; i < count; i++) {
			ukSceneNode *node = node_list->list[i];
			for (unsigned int j = 0; j < MAX_SORT_LIST_COUNT; j++) {
				if ((1 << j)&(node->handled)) {
					std::vector<Pipeline::SceneFunctionEntry> &loadscene_functions = pipeline_.GetLoadSceneFunctionEntry(j+1);
					for (unsigned k = 0; k <loadscene_functions.size() ; k++) {
						Object *object = loadscene_functions[k].object;
						loadscene_functions[k].function_point(object->GetNativeObject(),node);
					}
				}
			}
			DoLoadSceneFunctions(&node->children);
		}
	}

	bool ViewerType::LoadScene(const std::string &name) {
		Object *res = GetResource();
		std::vector<FunctionParameter> fp;
		std::vector<FunctionReturn> fr;
		fp.resize(1);
		fp[0].string_value = name;
		res->CallFunction("GetSceneSource", fp, fr);
		std::cout<<"Scene Source:"<< fr[0].string_value.c_str() << std::endl;
		std::string xml_file = fr[0].string_value;
		scene_node_list_=scene_xml_.LoadXMLFile(xml_file);
		if (scene_node_list_ == nullptr) return false;
		std::vector<Pipeline::SceneFunctionEntry> &loadscene_functions = pipeline_.GetLoadSceneFunctionEntry(0);
		unsigned int count = loadscene_functions.size();
		for (unsigned int i = 0; i < count; i++) {
			Object *object = loadscene_functions[i].object;
			loadscene_functions[i].function_point(object->GetNativeObject(), nullptr);
		}
		DoLoadSceneFunctions(scene_node_list_);
		return true;
	}

	bool ViewerType::LoadPipeline(const std::string &name) {
		Object *res = GetResource();
		std::vector<FunctionParameter> fp;
		std::vector<FunctionReturn> fr;
		fp.resize(1);
		fp[0].string_value = name;
		res->CallFunction("GetPipelineSource", fp, fr);
		std::cout << "Pipeline Source:" << fr[0].string_value.c_str() << std::endl;
		std::string xml_file = fr[0].string_value;
		if (pipeline_.LoadFromXMLFile(xml_file)==false) return false;
		return true;
	}

	ukSceneNodeList *ViewerType::GetSceneNodeList() {
		return scene_node_list_;
	}

	void ViewerType::InitThreadPool(unsigned int thread_count) {
		thread_pool_.InitThreadPool(thread_count);
	}

	void ViewerType::InitThreadPoolWithRecommendThreadCount() {
		unsigned int count = std::thread::hardware_concurrency() - 2;
		if (count <= 0) count=2;
		InitThreadPool(count);
		//InitThreadPool(4);
	}

	Camera &ViewerType::GetCamera() {
		return camera_;
	}

	void ViewerType::ResetStepCount(unsigned int index) {
		std::vector<Pipeline::Step> &steps = pipeline_.GetSteps();
		std::vector<unsigned int> &handles = steps[index].handles;
		steps[index].now_count=steps[index].now_step_count=steps[index].step_count = 0;
		unsigned int handle_count = handles.size();
		if (handle_count == 0) steps[index].step_count = 1;
		for (unsigned int i = 0; i < handle_count; i++) {
			unsigned int handle_index = handles[i];
			steps[index].step_count += sort_list_[handle_index].size();
		}
	}

	static void ukViewerTypeDoStepNode(void *_thread_id,void *_viewer_obj,void *_pipeline, void *_step, void *_scene_node) {
		ViewerType *viewer_obj = (ViewerType *)_viewer_obj;
		Pipeline *pipeline_obj = (Pipeline *)_pipeline;
		Pipeline::Step *step = (Pipeline::Step *)_step;
		ukSceneNode *scene_node = (ukSceneNode *)_scene_node;

		ukBoolFunc3Point func_point = step->function_point;
		
		bool ret = func_point(_thread_id,step->ref_object->GetNativeObject(), scene_node);
		unsigned int old_step_count;

		do {
			old_step_count = step->now_step_count;
		} while (!step->now_step_count.compare_exchange_weak(old_step_count, old_step_count + 1));

		ThreadPool::Task task;

		if (old_step_count + 1 == step->step_count) {
			unsigned int count = step->to_steps.size();
			std::vector<Pipeline::Step> &steps = pipeline_obj->GetSteps();
			for (unsigned int i = 0; i < count; i++) {
				unsigned int index = step->to_steps[i];
				unsigned int old_now_count;
				Pipeline::Step &tstep = steps[index];
				do {
					old_now_count = tstep.now_count;
				} while (!tstep.now_count.compare_exchange_weak(old_now_count, old_now_count + 1));
				if (old_now_count + 1 == tstep.start_count) {
					task.data1_point = viewer_obj;
					task.data2_point = pipeline_obj;
					task.data3_point = &tstep;
					std::vector<unsigned int> &handles = tstep.handles;
					unsigned int handle_count = handles.size();
					if (steps[i].parallel == true) {
						for (unsigned int j = 0; j < handle_count; j++) {
							unsigned int node_count = viewer_obj->GetSortList(handles[j]).size();
							for (unsigned int k = 0; k < node_count; k++) {
								task.data4_point = viewer_obj->GetSortList(handles[j])[k];
								task.function_point = ukViewerTypeDoStepNode;
								if (steps[i].thread_id == -1)
									viewer_obj->GetThreadPool().AddTask(task);
								else
									viewer_obj->GetThreadPool().AddTask(tstep.thread_id, task);
							}
						}
					}
					else {
						task.function_point = ukViewerTypeDoStep;
						if (tstep.thread_id == -1)
							viewer_obj->GetThreadPool().AddTask(task);
						else
							viewer_obj->GetThreadPool().AddTask(tstep.thread_id, task);
					}
				}
			}
		}
	}

	static void ukViewerTypeDoStep(void *_thread_id,void *_viewer_obj,void *_pipeline, void *_step,void *data4) {
		ViewerType *viewer_obj = (ViewerType *)_viewer_obj;
		Pipeline *pipeline_obj = (Pipeline *)_pipeline;
		Pipeline::Step *step = (Pipeline::Step *)_step;

		ukBoolFunc3Point func_point = step->function_point;
		unsigned int handle_count = step->handles.size();
		if (handle_count == 0) {
			func_point(_thread_id,step->ref_object->GetNativeObject(), nullptr);
		}
		else {
			for (unsigned int i = 0; i < handle_count; i++) {
				unsigned int handle_index = step->handles[i];
				std::vector<ukSceneNode *> &list = viewer_obj->GetSortList(handle_index);
				unsigned int node_count = viewer_obj->GetSortList(handle_index).size();
				for (unsigned int j = 0; j < node_count; j++) {
					ukSceneNode *scene_node = list[j];
					func_point(_thread_id,step->ref_object->GetNativeObject(), scene_node);
				}
			}
		}

		ThreadPool::Task task;
		unsigned int count = step->to_steps.size();
		std::vector<Pipeline::Step> &steps = pipeline_obj->GetSteps();
		for (unsigned int i = 0; i < count; i++) {
			unsigned int index = step->to_steps[i];
			unsigned int old_now_count;
			Pipeline::Step &tstep = steps[index];
			do {
				old_now_count = tstep.now_count;
			} while (!tstep.now_count.compare_exchange_weak(old_now_count, old_now_count + 1));
			if (old_now_count + 1 == tstep.start_count) {
				task.data1_point = viewer_obj;
				task.data2_point = pipeline_obj;
				task.data3_point = &tstep;
				std::vector<unsigned int> &handles = tstep.handles;
				unsigned int handle_count = handles.size();
				if (tstep.parallel == true) {
					for (unsigned int j = 0; j < handle_count; j++) {
						unsigned int node_count = viewer_obj->GetSortList(handles[j]).size();
						for (unsigned int k = 0; k < node_count; k++) {
							task.data4_point = viewer_obj->GetSortList(handles[j])[k];
							task.function_point = ukViewerTypeDoStepNode;
							if (tstep.thread_id == -1)
								viewer_obj->GetThreadPool().AddTask(task);
							else
								viewer_obj->GetThreadPool().AddTask(tstep.thread_id, task);
						}
					}
				}
				else {
					task.function_point = ukViewerTypeDoStep;
					if (tstep.thread_id == -1)
						viewer_obj->GetThreadPool().AddTask(task);
					else
						viewer_obj->GetThreadPool().AddTask(tstep.thread_id, task);
				}
			}
		}
	}

	bool ViewerType::Look() {
		LARGE_INTEGER start_tick_count, end_tick_count;
		QueryPerformanceCounter(&start_tick_count);
		CullObjects(scene_node_list_);
		CopyHandleObjectToSortList();
		std::vector<Pipeline::Step> &steps = pipeline_.GetSteps();
		unsigned int count = steps.size();
		for (unsigned int i = 0; i < count; i++)
			ResetStepCount(i);
		thread_pool_.ResetStatus();
		ThreadPool::Task task;
		for (unsigned int i = 0; i < count; i++) {
			if (steps[i].start_count == 0) {
				task.data1_point = this;
				task.data2_point = &pipeline_;
				task.data3_point = &steps[i];
				std::vector<unsigned int> &handles = steps[i].handles;
				unsigned int handle_count = handles.size();
				if (steps[i].parallel == true) {
					for (unsigned int j = 0; j < handle_count; j++) {
						unsigned int node_count = sort_list_[handles[j]].size();
						for (unsigned int k = 0; k < node_count; k++) {
							task.data4_point = sort_list_[handles[j]][k];
							task.function_point = ukViewerTypeDoStepNode;
							if (steps[i].thread_id == -1)
								thread_pool_.AddTask(task);
							else
								thread_pool_.AddTask(steps[i].thread_id, task);
						}
					}
				}
				else {
					task.function_point = ukViewerTypeDoStep;
					if (steps[i].thread_id == -1)
						thread_pool_.AddTask(task);
					else
						thread_pool_.AddTask(steps[i].thread_id, task);
				}
			}
		}

		bool loop = pipeline_.LoopPipeline();
		while (thread_pool_.GetStatus() != ThreadPool::ENDING) {
			if (thread_pool_.GetTask(0, task)) {
				task.function_point(&thread_pool_.thread_ids_[0],task.data1_point, task.data2_point, task.data3_point,
					task.data4_point);
				thread_pool_.task_count_--;
			}
			else if (thread_pool_.GetTask(task)) {
				task.function_point(&thread_pool_.thread_ids_[0],task.data1_point, task.data2_point, task.data3_point,
					task.data4_point);
				thread_pool_.task_count_--;
			}
		}

		QueryPerformanceCounter(&end_tick_count);
		//std::cout<<"Loop:" << (end_tick_count.QuadPart - start_tick_count.QuadPart)*1000/timer_frequency_.QuadPart<<"ms" << std::endl;
		return loop;
	}

	bool ViewerType::Loop() {
		while (Look());
		return true;
	}

	std::vector<ukSceneNode *> &ViewerType::GetSortList(unsigned int bit_index) {
		return sort_list_[bit_index];
	}

	ThreadPool &ViewerType::GetThreadPool() {
		return thread_pool_;
	}

	unsigned int ViewerType::GetThreadCount() {
		return thread_pool_.GetThreadCount() + 1;
	}

	void ViewerType::AddThreadTask(int thread_id, ThreadPool::Task task) {
		if (thread_id == -1)
			thread_pool_.AddTask(task);
		else
			thread_pool_.AddTask(thread_id, task);
	}

	void ViewerType::WaitAllThreadTaskEnd() {
		ThreadPool::Task task;
		while (thread_pool_.GetStatus() != ThreadPool::ENDING) {
			if (thread_pool_.GetTask(0, task)) {
				task.function_point(&thread_pool_.thread_ids_[0],task.data1_point, task.data2_point, task.data3_point,
					task.data4_point);
				thread_pool_.task_count_--;
			}
			else if (thread_pool_.GetTask(task)) {
				task.function_point(&thread_pool_.thread_ids_[0],task.data1_point, task.data2_point, task.data3_point,
					task.data4_point);
				thread_pool_.task_count_--;
			}
		}
	}

	ukFunctionDefineListInterface *ViewerType::InitObjectTypeInterfaceFunctions() {
		ukFunctionDefineListInterface *function_list = ukMakeFunctionDefineListInterface(14);
		ukFunctionParameterDefineListInterface *parameter_list = ukMakeFunctionParameterDefineListInterface(0);
		ukFunctionReturnDefineListInterface *return_list = ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "result", ukINTEGER);
		ukSetFunctionDefineInterface(function_list, 0, "Init",ukSCRIPT,
			parameter_list,
			return_list,
			ViewerTypeInitFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(1);
		ukSetFunctionParameterDefineInterface(parameter_list, 0, "Resource", ukPOINT);
		return_list = ukMakeFunctionReturnDefineListInterface(0);
		ukSetFunctionDefineInterface(function_list, 1, "SetResource", ukENGINE,
			parameter_list,
			return_list,
			ViewerTypeSetResourceFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(1);
		ukSetFunctionParameterDefineInterface(parameter_list, 0, "Name", ukSTRING);
		return_list = ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "result", ukINTEGER);
		ukSetFunctionDefineInterface(function_list, 2, "LoadScene", ukSCRIPT,
			parameter_list,
			return_list,
			ViewerTypeLoadSceneFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(1);
		ukSetFunctionParameterDefineInterface(parameter_list, 0, "ThreadCount",ukINTEGER);
		return_list = ukMakeFunctionReturnDefineListInterface(0);
		ukSetFunctionDefineInterface(function_list, 3, "InitThreadPool", ukSCRIPT,
			parameter_list,
			return_list,
			ViewerTypeInitThreadPoolFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(0);
		return_list = ukMakeFunctionReturnDefineListInterface(0);
		ukSetFunctionDefineInterface(function_list, 4, "InitThreadPoolWithRecommendThreadCount", ukSCRIPT,
			parameter_list,
			return_list,
			ViewerTypeInitThreadPoolWithRecommendThreadCountFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(1);
		ukSetFunctionParameterDefineInterface(parameter_list, 0, "Name", ukSTRING);
		return_list = ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "result", ukINTEGER);
		ukSetFunctionDefineInterface(function_list, 5, "LoadPipeline", ukSCRIPT,
			parameter_list,
			return_list,
			ViewerTypeLoadPipelineFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(0);
		return_list = ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "result", ukINTEGER);
		ukSetFunctionDefineInterface(function_list, 6, "InitPipeline", ukSCRIPT,
			parameter_list,
			return_list,
			ViewerTypeInitPipelineFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(0);
		return_list = ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "result", ukINTEGER);
		ukSetFunctionDefineInterface(function_list, 7, "DestroyPipeline", ukSCRIPT,
			parameter_list,
			return_list,
			ViewerTypeDestroyPipelineFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(0);
		return_list = ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "result", ukINTEGER);
		ukSetFunctionDefineInterface(function_list, 8, "Look", ukSCRIPT,
			parameter_list,
			return_list,
			ViewerTypeLookFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(0);
		return_list = ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "result", ukINTEGER);
		ukSetFunctionDefineInterface(function_list, 9, "Loop", ukSCRIPT,
			parameter_list,
			return_list,
			ViewerTypeLoopFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(1);
		ukSetFunctionParameterDefineInterface(parameter_list, 0, "Application", ukPOINT);
		return_list = ukMakeFunctionReturnDefineListInterface(0);
		ukSetFunctionDefineInterface(function_list, 10, "SetApplication", ukENGINE,
			parameter_list,
			return_list,
			ViewerTypeSetApplicationFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(0);
		return_list = ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "ThreadCount", ukINTEGER);
		ukSetFunctionDefineInterface(function_list, 11, "GetThreadCount", ukENGINE,
			parameter_list,
			return_list,
			ViewerTypeGetThreadCountFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(3);
		ukSetFunctionParameterDefineInterface(parameter_list, 0, "ThreadID", ukINTEGER);
		ukSetFunctionParameterDefineInterface(parameter_list, 1, "Task", ukPOINT);
		ukSetFunctionParameterDefineInterface(parameter_list, 2, "Parameter", ukPOINT);
		return_list = ukMakeFunctionReturnDefineListInterface(0);
		ukSetFunctionDefineInterface(function_list, 12, "AddThreadTask", ukENGINE,
			parameter_list,
			return_list,
			ViewerTypeAddThreadTaskFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(0);;
		return_list = ukMakeFunctionReturnDefineListInterface(0);
		ukSetFunctionDefineInterface(function_list, 13, "WaitAllThreadTaskEnd", ukENGINE,
			parameter_list,
			return_list,
			ViewerTypeWaitAllThreadTaskEndFunction);
		return function_list;
	}

	ukPropertyDefineListInterface *ViewerType::InitObjectTypeInterfaceProperties() {
		ukPropertyDefineListInterface *property_list = ukMakePropertyDefineListInterface(0);
		return property_list;
	}

	ukObjectDefineListInterface *ViewerType::InitObjectTypeInterfaceObjects() {
		ukObjectDefineListInterface *object_list = ukMakeObjectDefineListInterface(0);
		return object_list;
	}

	void ViewerType::InitObjectTypeInterface() {
		ukFunctionDefineListInterface *func_list = InitObjectTypeInterfaceFunctions();
		ukPropertyDefineListInterface *prop_list = InitObjectTypeInterfaceProperties();
		ukObjectDefineListInterface *obj_list = InitObjectTypeInterfaceObjects();
		object_type_interface_ = ukMakeObjectTypeInterface(OBJECT_TYPE_VIEWER, ViewerTypeCreateObject,
			ViewerTypeDestroyObject,
			ViewerTypeGetProperty,
			ViewerTypeSetProperty,
			func_list,
			prop_list,
			obj_list);
	}

	ukObjectTypeInterface *ViewerType::GetObjectTypeInterface() {
		if (object_type_interface_ == nullptr) {
			InitObjectTypeInterface();
		}
		return object_type_interface_;
	}

	ViewerType::ViewerType() {
		QueryPerformanceFrequency(&timer_frequency_);
	}

	ViewerType::~ViewerType() {

	}
}

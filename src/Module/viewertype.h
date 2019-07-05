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
#ifndef _VIEWERTYPE_H_INCLUDED
#define _VIEWERTYPE_H_INCLUDED

#include <objecttype.h>
#include <threadpool.h>
#include <camera.h>
#include <pipeline.h>
#include "scenexml.h"

#define			OBJECT_TYPE_VIEWER							"Viewer"
#define			MAX_HANDLE_QUEUE_COUNT						32

namespace unlike3d {
	class ViewerType {
	public:
		friend static void ukViewerTypeDoStep(void *_viewer_obj, void *_pipeline, void *_step, void *data4, void *data5);
		friend static void ukViewerTypeDoStepNode(void *_viewer_obj, void *_pipeline, void *_step, void *_scene_node, void *data5);
		friend static void ukViewerTypeAddHandleObject(ViewerType *viewer_obj, ukSceneNode *scene_node);
		friend static void ukViewerTypeAddAllSubObjects(void *_viewer_obj, void *_scene_node, void *data3, void *data4, void *data5);
		friend static void ukViewerTypeCullObjects(void *_viewer_obj, void *_scene_node_list, void *_frustum, void *data4, void *data5);
	private:
		LARGE_INTEGER timer_frequency_;
		static ukObjectTypeInterface *object_type_interface_;
		Object *application_object_;
		Object *resource_object_;
		Camera camera_;
		Pipeline pipeline_;
		SceneXML scene_xml_;
		ukSceneNodeList *scene_node_list_;
		ThreadPool thread_pool_;
		LockFreeQueue<ukSceneNode *> handle_object_queue_[MAX_HANDLE_QUEUE_COUNT];
		std::vector<ukSceneNode *> sort_list_[MAX_HANDLE_QUEUE_COUNT];
	private:
		static ukFunctionDefineListInterface *InitObjectTypeInterfaceFunctions();
		static ukPropertyDefineListInterface *InitObjectTypeInterfaceProperties();
		static ukObjectDefineListInterface *InitObjectTypeInterfaceObjects();
		void CopyHandleObjectToSortList();
		void ResetStepCount(unsigned int step_index);
		void DoLoadSceneFunctions(ukSceneNodeList *node_list);
	public:
		virtual bool Init();
		virtual bool Look();
		virtual bool Loop();

		virtual void SetApplication(Object *application_object);
		virtual Object *GetApplication();
		virtual void SetResource(Object *resource_object);
		virtual Object *GetResource();

		virtual Camera &GetCamera();

		virtual void InitThreadPool(unsigned int thread_count);
		virtual void InitThreadPoolWithRecommendThreadCount();

		virtual bool LoadPipeline(const std::string &name);
		virtual bool InitPipeline();
		virtual bool DestroyPipeline();

		virtual std::vector<ukSceneNode *> &GetSortList(unsigned int bit_index);
		virtual ThreadPool &GetThreadPool();

		virtual unsigned int GetThreadCount();
		virtual void AddThreadTask(int thread_id, ThreadPool::Task);
		virtual void WaitAllThreadTaskEnd();

		virtual void CullObjects(ukSceneNodeList *scene_node_list);
		virtual bool LoadScene(const std::string &name);
		virtual ukSceneNodeList *GetSceneNodeList();

		static void InitObjectTypeInterface();
		static ukObjectTypeInterface *GetObjectTypeInterface();

		ViewerType();
		virtual ~ViewerType();
	};
}

#endif
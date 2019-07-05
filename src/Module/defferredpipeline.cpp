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
#include "glTFnodetype.h"
#include "defferredpipeline.h"
#include "defferredpipelinetype.h"

#define DLL_EXPORT extern "C" __declspec(dllexport)

std::map<std::string, ukObjectTypeInterface *> obj_type_map;

DLL_EXPORT void *DefferredPipelineTypeCreateObject() {
	ukObject *obj = new ukObject();
	unlike3d::DefferredPipelineType *pipeline_obj= new unlike3d::DefferredPipelineType();
	pipeline_obj->SetNativeObject(obj);
	obj->data1.point = pipeline_obj;
	return obj;
}

DLL_EXPORT bool DefferredPipelineTypeDestroyObject(void *object) {
	ukObject *obj = (ukObject *)object;
	unlike3d::DefferredPipelineType *pipeline_obj = (unlike3d::DefferredPipelineType *)obj->data1.point;
	delete pipeline_obj;;
	delete obj;
	return true;
}

DLL_EXPORT void *DefferredPipelineTypeGetProperty(void *object, void *name) {
	return nullptr;
}

DLL_EXPORT void DefferredPipelineTypeSetProperty(void *object, void *name, void *value) {
	ukObject *obj = (ukObject *)object;
	unlike3d::DefferredPipelineType *pipeline_obj = (unlike3d::DefferredPipelineType *)obj->data1.point;
	std::string property_name = (const char *)name;
	if (property_name == "ukApplicationObject") {
		ukObject *app_object = (ukObject *)value;
		pipeline_obj->SetApplication(app_object);
		return;
	}
	else if(property_name=="ukResourceObject"){
		return;
	}
	std::cout << (char *)name << ":" << (char *)value << std::endl;
	pipeline_obj->SetProperty((const char *)name, (const char *)value);
}

static std::vector<unlike3d::DefferredPipelineType::Vertex> vertices_[4] = {
	{
			 {{0.0f, -0.3f},
			{{0.3f, 0.3f}},
			{{-0.3f, 0.3f}}
	}
	}
};

static std::vector<uint16_t> indices_[4] = { { 0,1,2 },{ 0,1,2 },{ 0,1,2 },{ 0,1,2 }
};

DLL_EXPORT void LoadSceneDefferredPipeline(void *object, void *node) {
	std::cout << "DefferredPipeline Load Scene Function" << std::endl;
	ukObject *obj = (ukObject *)object;
	unlike3d::DefferredPipelineType *pipeline_obj = (unlike3d::DefferredPipelineType *)obj->data1.point;
	ukSceneNode *scene_node = (ukSceneNode *)node;
	unlike3d::glTFNodeType *gltf_node = (unlike3d::glTFNodeType *)scene_node->data1.point;
	unlike3d::RenderObjectInfo *render_object_info =(unlike3d::RenderObjectInfo *) scene_node->data3.point;
	pipeline_obj->LoadglTFSource(0, gltf_node->GetglTFSource(), *render_object_info);	
}

DLL_EXPORT void DestroySceneDefferredPipeline(void *object, void *node) {
	
}

DLL_EXPORT void DefferredPipelineTypeInitFunction(void *object, void *parameter_list, void *return_list) {

}

DLL_EXPORT bool InitDefferredPipeline(void *_object) {
	std::cout << "defferred_pipeline:init_defferred_pipeline" << std::endl;
	ukObject *obj = (ukObject *)_object;
	unlike3d::DefferredPipelineType *pipeline_obj = (unlike3d::DefferredPipelineType *)obj->data1.point;
	return pipeline_obj->Init();
}

DLL_EXPORT bool DestroyDefferredPipeline(void *_object) {
	std::cout << "defferred_pipeline:destroy_defferred_pipeline" << std::endl;
	ukObject *obj = (ukObject *)_object;
	unlike3d::DefferredPipelineType *pipeline_obj = (unlike3d::DefferredPipelineType *)obj->data1.point;
	return pipeline_obj->Destroy();
}

DLL_EXPORT bool LoopDefferredPipeline(void *_object) {
	ukObject *obj = (ukObject *)_object;
	unlike3d::DefferredPipelineType *pipeline_obj = (unlike3d::DefferredPipelineType *)obj->data1.point;
	glfwPollEvents();
//	pipeline_obj->DrawFrame();
	if (glfwWindowShouldClose((GLFWwindow *)obj->data5.point))
		return false;
	return true;
}

DLL_EXPORT bool Start_Frame(void *_thread_id, void *_object, void *_scene_node) {
	unsigned int thread_id = *(unsigned int *)_thread_id;
	ukObject *obj = (ukObject *)_object;
	unlike3d::DefferredPipelineType *pipeline_obj = (unlike3d::DefferredPipelineType *)obj->data1.point;
	pipeline_obj->WaitFrameEnd();
	
	return true;
}

DLL_EXPORT bool Update_Animation(void *_thread_id, void *_object, void *_scene_node) {
	unsigned int thread_id = *(unsigned int *)_thread_id;
	ukObject *obj = (ukObject *)_object;
	ukSceneNode *scene_node = (ukSceneNode *)_scene_node;
	unlike3d::DefferredPipelineType *pipeline_obj = (unlike3d::DefferredPipelineType *)obj->data1.point;
	unlike3d::RenderObjectInfo *render_object_info = (unlike3d::RenderObjectInfo *)scene_node->data3.point;
	unlike3d::glTF *gltf = (unlike3d::glTF *)render_object_info->data1;
	pipeline_obj->UpdateAnimation(thread_id, gltf, *render_object_info);
	return true;
}

DLL_EXPORT bool Update_Skin(void *_thread_id, void *_object, void *_scene_node) {
	unsigned int thread_id = *(unsigned int *)_thread_id;
	ukObject *obj = (ukObject *)_object;
	ukSceneNode *scene_node = (ukSceneNode *)_scene_node;
	unlike3d::DefferredPipelineType *pipeline_obj = (unlike3d::DefferredPipelineType *)obj->data1.point;
	unlike3d::RenderObjectInfo *render_object_info = (unlike3d::RenderObjectInfo *)scene_node->data3.point;
	unlike3d::glTF *gltf = (unlike3d::glTF *)render_object_info->data1;
	pipeline_obj->UpdateSkin(thread_id, gltf, *render_object_info);
	return true;
}

DLL_EXPORT bool Clear(void *_thread_id,void *_object, void *_scene_node) {
	unsigned int thread_id = *(unsigned int *)_thread_id;
	ukObject *obj = (ukObject *)_object;
	unlike3d::DefferredPipelineType *pipeline_obj = (unlike3d::DefferredPipelineType *)obj->data1.point;
	
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	if (vkBeginCommandBuffer(pipeline_obj->clear_command_buffers_[pipeline_obj->current_frame_], &begin_info) != VK_SUCCESS)
		throw std::runtime_error("Failed to begin recording command buffer!");
		pipeline_obj->ClearCommand(thread_id);
	if (vkEndCommandBuffer(pipeline_obj->clear_command_buffers_[pipeline_obj->current_frame_]) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer!");
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = &pipeline_obj->image_available_semaphores_[pipeline_obj->current_frame_];
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &pipeline_obj->clear_command_buffers_[pipeline_obj->current_frame_];
	submit_info.signalSemaphoreCount = pipeline_obj->GetThreadCount();
	submit_info.pSignalSemaphores = pipeline_obj->clear_end_semaphores_[pipeline_obj->current_frame_].data();
	if (vkQueueSubmit(pipeline_obj->graphics_queues_[thread_id], 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS)
		throw std::runtime_error("Faile to submit draw command buffer!");

	return true;
}

DLL_EXPORT bool Draw_Start(void *_thread_id, void *_object, void *_scene_node) {
	unsigned int thread_id = *(unsigned int *)_thread_id;
	ukObject *obj = (ukObject *)_object;
	unlike3d::DefferredPipelineType *pipeline_obj = (unlike3d::DefferredPipelineType *)obj->data1.point;
	
	unsigned int thread_count = pipeline_obj->GetThreadCount();
	for (unsigned int i = 0; i < thread_count; i++) {
		pipeline_obj->BeginCommand(i);
	}
	
	return true;
}


DLL_EXPORT bool Draw_End(void *_thread_id, void *_object, void *_scene_node) {
	unsigned int thread_id = *(unsigned int *)_thread_id;
	ukObject *obj = (ukObject *)_object;
	unlike3d::DefferredPipelineType *pipeline_obj = (unlike3d::DefferredPipelineType *)obj->data1.point;
	
	unsigned int thread_count = pipeline_obj->GetThreadCount();
	for (unsigned int i = 0; i < thread_count; i++) {
		pipeline_obj->EndCommand(i);
		pipeline_obj->SubmitCommand(i, 1, &pipeline_obj->clear_end_semaphores_[pipeline_obj->current_frame_][i],
			1, &pipeline_obj->draw_end_semaphores_[pipeline_obj->current_frame_][i]);
	}
	return true;
}


DLL_EXPORT bool Draw_Object(void *_thread_id,void *_object, void *_scene_node) {
	unsigned int thread_id = *(unsigned int *)_thread_id;
	ukObject *obj = (ukObject *)_object;
	ukSceneNode *scene_node = (ukSceneNode *)_scene_node;
	unlike3d::DefferredPipelineType *pipeline_obj = (unlike3d::DefferredPipelineType *)obj->data1.point;
	unlike3d::RenderObjectInfo *render_object_info = (unlike3d::RenderObjectInfo *)scene_node->data3.point;
	pipeline_obj->DrawObject(thread_id, *render_object_info);
	return true;
}

DLL_EXPORT bool Display(void *_thread_id,void *_object, void *_scene_node) {
	unsigned int thread_id = *(unsigned int *)_thread_id;
	ukObject *obj = (ukObject *)_object;
	unlike3d::DefferredPipelineType *pipeline_obj = (unlike3d::DefferredPipelineType *)obj->data1.point;
	unsigned int thread_count = pipeline_obj->GetThreadCount();
		
	vkResetFences(pipeline_obj->device_, 1, &pipeline_obj->in_flight_fences_[pipeline_obj->current_frame_]);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	std::vector<VkPipelineStageFlags> wait_stages;
	submit_info.waitSemaphoreCount = thread_count;
	submit_info.pWaitSemaphores = pipeline_obj->draw_end_semaphores_[pipeline_obj->current_frame_].data();
	wait_stages.resize(thread_count);
	for (auto i = 0; i < thread_count; i++)
		wait_stages[i] = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	submit_info.pWaitDstStageMask = wait_stages.data();
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = &pipeline_obj->render_finished_semaphores_[pipeline_obj->current_frame_];
	if (vkQueueSubmit(pipeline_obj->graphics_queues_[thread_id], 1, &submit_info, pipeline_obj->in_flight_fences_[pipeline_obj->current_frame_]) != VK_SUCCESS)
		throw std::runtime_error("Faile to submit render end command buffer!");

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &pipeline_obj->render_finished_semaphores_[pipeline_obj->current_frame_];
	VkSwapchainKHR swap_chains[] = { pipeline_obj->swap_chain_ };
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swap_chains;
	present_info.pImageIndices = &pipeline_obj->current_swap_chain_image_index_;
	VkResult result = vkQueuePresentKHR(pipeline_obj->present_queue_, &present_info);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || pipeline_obj->framebuffer_resized_) {
		pipeline_obj->framebuffer_resized_ = false;
		pipeline_obj->need_recreate_swap_chain_ = true;
	}
	else if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to present swap chain image!");

	pipeline_obj->current_frame_ = (pipeline_obj->current_frame_ + 1) % MAX_FRAMES_IN_FLIGHT;

	return true;
}

DLL_EXPORT int CmpNode(void *_object, void *_scene_node_a,void *_scene_node_b) {
	ukSceneNode *scene_node_a = (ukSceneNode *)_scene_node_a;
	ukSceneNode *scene_node_b = (ukSceneNode *)_scene_node_b;
	return scene_node_a->handled<scene_node_b->handled;
}

void DefferredPipelineTypeInit() {
	unlike3d::DefferredPipelineType::InitObjectTypeInterface();
}

DLL_EXPORT bool ukInit(void *data) {
	DefferredPipelineTypeInit();
	obj_type_map[OBJECT_TYPE_DEFFERREDPIPELINE] = unlike3d::DefferredPipelineType::GetObjectTypeInterface();

	return true;
}

DLL_EXPORT ukObjectTypeInterface *ukGetObjectType(const char *strName) {
	return obj_type_map[strName];
}

#undef DLL_EXPORT
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
#include "glTFnode.h"
#include "glTFnodetype.h"
#include <tinymath3d.h>

#define DLL_EXPORT extern "C" __declspec(dllexport)

std::map<std::string, ukObjectTypeInterface *> obj_type_map;

static int glTFNodeTypeInFrustum(void *obj, void *frustum) {
	return math3d::INSIDE;
}

DLL_EXPORT void *glTFNodeTypeCreateObject() {
	ukObject *obj = new ukObject();
	unlike3d::glTFNodeType *gltf_node = new unlike3d::glTFNodeType();
	obj->data1.point = gltf_node;
	ukSceneNode *scene_node= new ukSceneNode();
	unlike3d::RenderObjectInfo *render_object_info=new unlike3d::RenderObjectInfo();
	scene_node->handled = 256+512;
	std::cout << "new gltf_node:"<<scene_node << std::endl;
	scene_node->in_frustum = glTFNodeTypeInFrustum;
	scene_node->data1.point = obj->data1.point;
	obj->data3.point = scene_node;
	scene_node->data3.point = render_object_info;
	gltf_node->SetSceneNode(scene_node);
	unlike3d::InitRenderObjectInfoMatrix(render_object_info);
	gltf_node->SetRenderObjectInfo(render_object_info);
	return obj;
}

DLL_EXPORT bool glTFNodeTypeDestroyObject(void *object) {
	ukObject *obj = (ukObject *)object;
	unlike3d::glTFNodeType *glTF_node_obj = (unlike3d::glTFNodeType *)obj->data1.point;
	delete glTF_node_obj;
	delete obj->data3.point;
	delete obj;
	return true;
}

DLL_EXPORT void *glTFNodeTypeGetProperty(void *object, void *name) {
	ukObject *obj = (ukObject *)object;
	unlike3d::glTFNodeType *glTF_node_obj = (unlike3d::glTFNodeType *)obj->data1.point;
	
	return nullptr;
}

DLL_EXPORT void glTFNodeTypeSetProperty(void *object, void *name, void *value) {
	ukObject *obj = (ukObject *)object;
	unlike3d::glTFNodeType *glTF_node_obj = (unlike3d::glTFNodeType *)obj->data1.point;
	std::string property_name = (const char *)name;
	if (property_name == "ukResource") {
		glTF_node_obj->SetResource((ukObject *)value);
		return;
	}
	glTF_node_obj->SetProperty(property_name, (const char *)value);
}

DLL_EXPORT void glTFNodeTypeInitFunction(void *object, void *parameter_list, void *return_list) {
	
}

void glTFNodeTypeInit() {
	unlike3d::glTFNodeType::InitObjectTypeInterface();
}

DLL_EXPORT bool ukInit(void *data) {
	glTFNodeTypeInit();
	obj_type_map[OBJECT_TYPE_GLTFNODE] = unlike3d::glTFNodeType::GetObjectTypeInterface();

	return true;
}

DLL_EXPORT ukObjectTypeInterface *ukGetObjectType(const char *strName) {
	return obj_type_map[strName];
}

#undef DLL_EXPORT
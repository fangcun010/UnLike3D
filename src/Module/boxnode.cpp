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
#include "boxnode.h"
#include "boxnodetype.h"
#include <tinymath3d.h>

#define DLL_EXPORT extern "C" __declspec(dllexport)

std::map<std::string, ukObjectTypeInterface *> obj_type_map;

static int BoxNodeTypeInFrustum(void *obj, void *frustum) {
	return math3d::INSIDE;
}

DLL_EXPORT void *BoxNodeTypeCreateObject() {
	ukObject *obj = new ukObject();
	obj->data1.point = new unlike3d::BoxNodeType();
	ukSceneNode *scene_node= new ukSceneNode();
	scene_node->handled = 1;
	scene_node->in_frustum = BoxNodeTypeInFrustum;
	scene_node->data1.point = obj->data1.point;
	obj->data3.point = scene_node;
	return obj;
}

DLL_EXPORT bool BoxNodeTypeDestroyObject(void *object) {
	ukObject *obj = (ukObject *)object;
	unlike3d::BoxNodeType *box_node_obj = (unlike3d::BoxNodeType *)obj->data1.point;
	delete box_node_obj;
	delete obj->data3.point;
	delete obj;
	return true;
}

DLL_EXPORT void *BoxNodeTypeGetProperty(void *object, void *name) {
	return nullptr;
}

DLL_EXPORT void BoxNodeTypeSetProperty(void *object, void *name, void *value) {
	std::cout << (char *)value << std::endl;
}

DLL_EXPORT void BoxNodeTypeInitFunction(void *object, void *parameter_list, void *return_list) {
	
}

void BoxNodeTypeInit() {
	unlike3d::BoxNodeType::InitObjectTypeInterface();
}

DLL_EXPORT bool ukInit(void *data) {
	BoxNodeTypeInit();
	obj_type_map[OBJECT_TYPE_BOXNODE] = unlike3d::BoxNodeType::GetObjectTypeInterface();

	return true;
}

DLL_EXPORT ukObjectTypeInterface *ukGetObjectType(const char *strName) {
	return obj_type_map[strName];
}

#undef DLL_EXPORT
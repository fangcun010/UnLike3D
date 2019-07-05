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
#include "scenexml.h"

namespace unlike3d {
	void SceneXML::ParserSceneNodeEntry(tinyxml2::XMLElement *element,Object *parent_obj,ukSceneNodeList *node_list,
					ukSceneNode *parent) {
		std::vector<ukSceneNode *> list;
		for (; element != nullptr; element = element->NextSiblingElement()) {
			if (!strcmp(element->Value(), "node")) {
				std::string name = element->Attribute("name");
				std::string node_type = element->Attribute("node_type");
				Object *node_object;
				GetResource()->CallFunction("CreateSceneNode",(void *)node_type.c_str(), &node_object);
				node_object->SetName(name.c_str());
				ukSceneNode *scene_node = (ukSceneNode *)node_object->GetNativeObject()->data3.point;
				scene_node->data2.point = node_object;
				node_object->SetProperty("ukResource", GetResource()->GetNativeObject());
				list.push_back(scene_node);
				scene_node->parent = parent;
				ParserSceneNodeEntry(element->FirstChildElement(),node_object, &scene_node->children,scene_node);
			}
			else if(!strcmp(element->Value(),"property")){
				if (parent_obj == nullptr || parent == nullptr) continue;
				std::string name = element->Attribute("name");
				std::string value = element->Attribute("value");
				parent_obj->SetProperty(name, (void *)value.c_str());
			}
		}
		node_list->count = list.size();
		node_list->list = new ukSceneNode *[list.size()];
		for (unsigned int i = 0; i < node_list->count; i++)
			node_list->list[i] = list[i];
	}

	ukSceneNodeList *SceneXML::LoadXMLFile(const std::string &name) {
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError err=doc.LoadFile(name.c_str());
		if (err != tinyxml2::XML_SUCCESS)
			return nullptr;
		ukSceneNodeList *scene_node_list = new ukSceneNodeList();
		ParserSceneNodeEntry(doc.FirstChildElement(),nullptr,scene_node_list,nullptr);
		return scene_node_list;
	}

	void SceneXML::SetResource(Object *resource_object) {
		resource_object_ = resource_object;
	}

	Object *SceneXML::GetResource() {
		return resource_object_;
	}

	SceneXML::SceneXML() {

	}

	SceneXML::~SceneXML() {

	}
}
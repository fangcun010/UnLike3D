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
#include "resourcetype.h"
#include "resource.h"

#include <iostream>

namespace unlike3d {
	ukObjectTypeInterface *ResourceType::object_type_interface_ = nullptr;

	bool ResourceType::Init() {
		return true;
	}

	std::string ResourceType::GetBinarySource(const std::string &name) {
		std::string source = resource_xml_.GetBasePath()+resource_xml_.GetBinariesPath()+"/"+name;
		return source;
	}

	std::string ResourceType::GetSceneSource(const std::string &name) {
		const ResourceXML::SceneEntry *entry=resource_xml_.GetSceneEntry(name);
		if (entry == nullptr) return "";
		std::string source = resource_xml_.GetBasePath()+resource_xml_.GetScenesPath()
										+ "/"+entry->src;
		return source;
	}

	std::string ResourceType::GetNodeSetSource(const std::string &name) {
		const ResourceXML::NodeSetEntry *entry = resource_xml_.GetNodeSetEntry(name);
		if (entry == nullptr) return "";
		std::string source = resource_xml_.GetBasePath() + resource_xml_.GetNodesPath()
										+ "/" + entry->src;
		return source;
	}

	ObjectType *ResourceType::GetSceneNodeObjectType(const std::string &name) {
		if (nodes_map_.find(name) == nodes_map_.end()) return nullptr;
		unsigned int index = nodes_map_[name];
		std::string module = node_list_[index].module;
		std::string object_type = node_list_[index].object_type;
		return GetObjectType(module.c_str(), object_type.c_str());
	}

	Object *ResourceType::CreateSceneNode(const std::string &name) {
		ObjectType *object_type = GetSceneNodeObjectType(name);
		if (object_type == nullptr) return nullptr;
		Object *object = object_type->CreateObject();
		return object;
	}

	void ResourceType::SetModuleManager(ModuleManager *module_manager) {
		module_manager_ = module_manager;
	}

	bool ResourceType::LoadResource(const char *strName) {
		resource_xml_.LoadResourceXML(strName);
		return true;
	}

	bool ResourceType::ParserNodeEntry(tinyxml2::XMLElement *element) {
		for (; element != nullptr; element = element->NextSiblingElement()) {
			if (!strcmp(element->Value(), "node")) {
				std::string name = element->Attribute("name");
				std::string module = element->Attribute("module");
				std::string object_type = element->Attribute("object_type");
				NodeEntry entry;
				entry.name = name;
				entry.module = module;
				entry.object_type = object_type;
				nodes_map_[name] = node_list_.size();
				node_list_.push_back(entry);
				if (!LoadModuleObjectType(module, object_type)) return false;
				ObjectType *obj_type=GetObjectType(module.c_str(), object_type.c_str());
				if (obj_type == nullptr) return false;
			}
		}
		return true;
	}

	bool ResourceType::LoadNodeSet(const char *strName) {
		std::string xml_file = GetNodeSetSource(strName);
		tinyxml2::XMLDocument doc;
		std::cout << "load_node_set:" << xml_file.c_str() << std::endl;
		doc.LoadFile(xml_file.c_str());
		return ParserNodeEntry(doc.FirstChildElement());
	}

	ResourceType::NodeEntry *ResourceType::GetNodeEntry(const std::string &name) {
		if (nodes_map_.find(name) == nodes_map_.end())
			return nullptr;
		unsigned int index = nodes_map_[name];
		return &node_list_[index];
	}

	ModuleHandle ResourceType::LoadModule(const char *module_name) {
		std::string path = resource_xml_.GetBasePath() + resource_xml_.GetLibrariesPath() +
			"/" + module_name + UNLIKE3D_MODULE_SUFFIX;
		return module_manager_->LoadModule(path.c_str());
	}

	void ResourceType::RemoveModule(const char *strPath) {
		return module_manager_->RemoveModule(strPath);
	}

	ObjectType *ResourceType::GetObjectType(const char *module_name, const char *obj_type) {
		std::string path=resource_xml_.GetBasePath() + resource_xml_.GetLibrariesPath() +
			"/" + module_name + UNLIKE3D_MODULE_SUFFIX;
		return module_manager_->GetObjectType(path.c_str(), obj_type);
	}

	Object *ResourceType::CreateObject(ObjectType *obj_type) {
		return module_manager_->CreateObject(obj_type);
	}

	Object *ResourceType::CreateObject(const char *module, const char *object_type) {
		ObjectType *obj_type = GetObjectType(module, object_type);
		if (obj_type == nullptr) return nullptr;
		return CreateObject(obj_type);
	}

	bool ResourceType::DestroyObject(Object *object) {
		ObjectType *obj_type = object->GetObjectType();
		return obj_type->DestroyObject(object);
	}

	ukBoolFuncPoint ResourceType::GetFunction(const char *module_name, const char *function_name) {
		std::string module_path = resource_xml_.GetBasePath() + resource_xml_.GetLibrariesPath() +
			"/" + module_name + UNLIKE3D_MODULE_SUFFIX;
		return module_manager_->GetFunction(module_path.c_str(), function_name);
	}

	const ResourceXML::ModuleObjectTypeEntry *ResourceType::GetModuleObjectTypeEntry(const std::string &name,
		const std::string &object_type) {
		return resource_xml_.GetModuleObjectTypeEntry(name, object_type);
	}

	bool ResourceType::LoadModuleObjectType(const std::string &module, const std::string &object_type) {
		const ResourceXML::ModuleObjectTypeEntry *entry = GetModuleObjectTypeEntry(module, object_type);
		if (entry == nullptr) return false;
		if (entry->requires.size()) {
			unsigned int count = entry->requires.size();
			for (unsigned int i = 0; i < count; i++) {
				unsigned int require_index = entry->requires[i];
				std::string require_module = resource_xml_.GetModuleObjectTypeList()[require_index].module;
				std::string require_object_type = resource_xml_.GetModuleObjectTypeList()[require_index].object_type;
				if (!module_manager_->IsLoaded(require_module.c_str(),
					require_object_type.c_str())) {
					bool ret = LoadModuleObjectType(require_module,require_object_type);
					if (!ret) return false;
				}
			}
		}
		return GetObjectType(module.c_str(), object_type.c_str());
	}

	std::string ResourceType::GetPipelineSource(const std::string &name) {
		const ResourceXML::PipelineEntry *entry = resource_xml_.GetPipelineEntry(name);
		if (entry == nullptr) return "";
		std::string source = resource_xml_.GetBasePath() + resource_xml_.GetPipelinesPath()
			+ "/" + entry->src;
		return source;
	}

	ukFunctionDefineListInterface *ResourceType::InitObjectTypeInterfaceFunctions() {
		ukFunctionDefineListInterface *function_list = ukMakeFunctionDefineListInterface(10);
		ukFunctionReturnDefineListInterface *return_list=ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "result", ukINTEGER);
		ukSetFunctionDefineInterface(function_list, 0, "Init",ukSCRIPT,
			ukMakeFunctionParameterDefineListInterface(0),
			return_list,
			ResourceTypeInitFunction);
		ukFunctionParameterDefineListInterface *parameter_list = ukMakeFunctionParameterDefineListInterface(1);
		ukSetFunctionParameterDefineInterface(parameter_list, 0, "ResourceXMLFile", ukSTRING);
		return_list = ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "result", ukINTEGER);
		ukSetFunctionDefineInterface(function_list, 1, "LoadResource", ukSCRIPT,
			parameter_list,
			return_list,
			ResourceTypeLoadResourceFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(1);
		ukSetFunctionParameterDefineInterface(parameter_list, 0, "name", ukSTRING);
		return_list = ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "result", ukSTRING);
		ukSetFunctionDefineInterface(function_list, 2, "GetSceneSource", ukENGINE,
			parameter_list,
			return_list,
			ResourceTypeGetSceneSourceFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(1);
		ukSetFunctionParameterDefineInterface(parameter_list, 0, "name", ukSTRING);
		return_list = ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "result", ukINTEGER);
		ukSetFunctionDefineInterface(function_list, 3, "LoadNodeSet", ukSCRIPT,
			parameter_list,
			return_list,
			ResourceTypeLoadNodeSetFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(1);
		ukSetFunctionParameterDefineInterface(parameter_list, 0, "ModuleManager", ukPOINT);
		return_list = ukMakeFunctionReturnDefineListInterface(0);
		ukSetFunctionDefineInterface(function_list, 4, "SetModuleManager", ukENGINE,
			parameter_list,
			return_list,
			ResourceTypeSetModuleManagerFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(0);
		return_list = ukMakeFunctionReturnDefineListInterface(0);
		ukSetFunctionDefineInterface(function_list, 5, "CreateSceneNode", ukENGINE,
			parameter_list,
			return_list,
			ResourceTypeCreateSceneNodeFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(2);
		ukSetFunctionParameterDefineInterface(parameter_list, 0, "module", ukSTRING);
		ukSetFunctionParameterDefineInterface(parameter_list, 1, "object_type", ukSTRING);
		return_list = ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "object", ukPOINT);
		ukSetFunctionDefineInterface(function_list, 6, "CreateObject", ukENGINE,
			parameter_list,
			return_list,
			ResourceTypeCreateObjectFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(1);
		ukSetFunctionParameterDefineInterface(parameter_list, 0, "name", ukSTRING);
		return_list = ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "result", ukSTRING);
		ukSetFunctionDefineInterface(function_list, 7, "GetPipelineSource", ukENGINE,
			parameter_list,
			return_list,
			ResourceTypeGetPipelineSourceFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(2);
		ukSetFunctionParameterDefineInterface(parameter_list, 0, "module", ukSTRING);
		ukSetFunctionParameterDefineInterface(parameter_list, 1, "function", ukSTRING);
		return_list = ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "function_point", ukPOINT);
		ukSetFunctionDefineInterface(function_list, 8, "GetFunction", ukENGINE,
			parameter_list,
			return_list,
			ResourceTypeGetFunctionFunction);
		parameter_list = ukMakeFunctionParameterDefineListInterface(1);
		ukSetFunctionParameterDefineInterface(parameter_list, 0, "name", ukSTRING);
		return_list = ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "result", ukSTRING);
		ukSetFunctionDefineInterface(function_list, 9, "GetBinarySource", ukENGINE,
			parameter_list,
			return_list,
			ResourceTypeGetBinarySourceFunction);
		return function_list;
	}

	ukPropertyDefineListInterface *ResourceType::InitObjectTypeInterfaceProperties() {
		ukPropertyDefineListInterface *property_list = ukMakePropertyDefineListInterface(0);
		return property_list;
	}

	ukObjectDefineListInterface *ResourceType::InitObjectTypeInterfaceObjects() {
		ukObjectDefineListInterface *object_list = ukMakeObjectDefineListInterface(0);
		return object_list;
	}

	void ResourceType::InitObjectTypeInterface() {
		ukFunctionDefineListInterface *func_list = InitObjectTypeInterfaceFunctions();
		ukPropertyDefineListInterface *prop_list = InitObjectTypeInterfaceProperties();
		ukObjectDefineListInterface *obj_list = InitObjectTypeInterfaceObjects();
		object_type_interface_ = ukMakeObjectTypeInterface(OBJECT_TYPE_RESOURCE, ResourceTypeCreateObject,
			ResourceTypeDestroyObject,
			ResourceTypeGetProperty,
			ResourceTypeSetProperty,
			func_list,
			prop_list,
			obj_list);
	}

	ukObjectTypeInterface *ResourceType::GetObjectTypeInterface() {
		if (object_type_interface_ == nullptr) {
			InitObjectTypeInterface();
		}
		return object_type_interface_;
	}

	ResourceType::ResourceType() {

	}

	ResourceType::~ResourceType() {

	}
}

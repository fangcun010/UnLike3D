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
#include <tinyxml2.h>

#include <crossplatform.h>
#include "resourcexml.h"
#include <iostream>

#define				ukRESOURCE_LIST_XML					"list.xml"

namespace unlike3d {
	bool ResourceXML::LoadResourceXML(const char *strName) {
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError err = doc.LoadFile(strName);
		if (err != tinyxml2::XML_SUCCESS)
			return false;
		for (tinyxml2::XMLElement *element = doc.FirstChildElement(); element != nullptr;
			element = element->NextSiblingElement()) {
			if (!strcmp(element->Value(), "binaries")) {
				binaries_path_ = element->Attribute("path");
			}
			else if (!strcmp(element->Value(), "keys")) {
				keys_path_ = element->Attribute("path");
			}
			else if (!strcmp(element->Value(), "libraries")) {
				libraries_path_ = element->Attribute("path");
			}
			else if (!strcmp(element->Value(), "nodes")) {
				nodes_path_ = element->Attribute("path");
			}
			else if (!strcmp(element->Value(), "pipelines")) {
				pipelines_path_ = element->Attribute("path");
			}
			else if (!strcmp(element->Value(), "scenes")) {
				scenes_path_ = element->Attribute("path");
			}
			else if (!strcmp(element->Value(), "scripts")) {
				scripts_path_ = element->Attribute("path");
			}
			else if (!strcmp(element->Value(), "shaders")) {
				shaders_path_ = element->Attribute("path");
			}
		}
		base_path_ = CrossPlatform::GetFileDir(strName);
		LoadBinaryList();
		LoadKeySetList();
		LoadLibraryList();
		LoadNodeSetList();
		LoadPipelineList();
		LoadSceneList();
		LoadScriptList();
		LoadShaderList();
		
		return true;
	}

	void ResourceXML::ParserBinaryEntry(tinyxml2::XMLElement *element, const std::string &base_name) {
		for (;element != nullptr;element = element->NextSiblingElement()) {
			if (!strcmp(element->Value(),"binary")) {
				std::string name = element->Attribute("name");
				std::string src = element->Attribute("src");
				BinaryEntry entry;
				entry.name = base_name+name;
				entry.src = src;
				binaries_map_[entry.name] = binary_list_.size();
				binary_list_.push_back(entry);
			}
			else if (!strcmp(element->Value(), "directory")) {
				std::string name = element->Attribute("name");
				ParserBinaryEntry(element->FirstChildElement(), base_name + name+"/");
			}
		}
	}

	void ResourceXML::LoadBinaryList() {
		std::string xml_file = GetBasePath() + GetBinariesPath() + "/" + ukRESOURCE_LIST_XML;
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError err=doc.LoadFile(xml_file.c_str());
		if (err != tinyxml2::XML_SUCCESS) return;
		binary_list_.clear();
		binaries_map_.clear();
		ParserBinaryEntry(doc.FirstChildElement(), "");
	}

	void ResourceXML::ParserKeySetEntry(tinyxml2::XMLElement *element, const std::string &base_name) {
		for (; element != nullptr; element = element->NextSiblingElement()) {
			if (!strcmp(element->Value(), "keyset")) {
				std::string name = element->Attribute("name");
				std::string src = element->Attribute("src");
				KeySetEntry entry;
				entry.name = base_name + name;
				entry.src = src;
				keys_map_[entry.name] = key_set_list_.size();
				key_set_list_.push_back(entry);
			}
			else if (!strcmp(element->Value(), "directory")) {
				std::string name = element->Attribute("name");
				ParserBinaryEntry(element->FirstChildElement(), base_name + name + "/");
			}
		}
	}

	void ResourceXML::LoadKeySetList() {
		std::string xml_file = GetBasePath() + GetKeysPath() + "/" + ukRESOURCE_LIST_XML;
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError err = doc.LoadFile(xml_file.c_str());
		if (err != tinyxml2::XML_SUCCESS) return;
		key_set_list_.clear();
		keys_map_.clear();
		ParserKeySetEntry(doc.FirstChildElement(), "");
	}

	void ResourceXML::ParserLibraryEntry(tinyxml2::XMLElement *element, const std::string &base_name) {
		for (; element != nullptr; element = element->NextSiblingElement()) {
			if (!strcmp(element->Value(), "library")) {
				std::string name = element->Attribute("name");
				std::string module = element->Attribute("module");
				std::string object_type = element->Attribute("object_type");
				LibraryEntry entry;
				entry.name = base_name + name;
				entry.module = module;
				entry.object_type = object_type;
				libraries_map_[entry.name] = library_list_.size();
				library_list_.push_back(entry);
				if (module_obj_type_map_.find(module) == module_obj_type_map_.end() ||
						module_obj_type_map_[module].find(object_type)== module_obj_type_map_[module].end()){
					ModuleObjectTypeEntry module_object_type_entry;
					module_object_type_entry.module = module;
					module_object_type_entry.object_type = object_type;
					module_obj_type_map_[module][object_type] = module_obj_type_list_.size();
					module_obj_type_list_.push_back(module_object_type_entry);
				}
				for (tinyxml2::XMLElement *sub_element = element->FirstChildElement(); sub_element != nullptr;
						sub_element = sub_element->NextSiblingElement()) {
					if (!strcmp(sub_element->Value(), "require")) {
						std::string require_module = sub_element->Attribute("module");
						std::string require_object_type = sub_element->Attribute("object_type");
						if (module_obj_type_map_.find(require_module) == module_obj_type_map_.end() ||
							module_obj_type_map_[require_module].find(require_object_type) ==
							module_obj_type_map_[require_module].end()) {
							ModuleObjectTypeEntry module_object_type_entry;
							module_object_type_entry.module = require_module;
							module_object_type_entry.object_type = require_object_type;
							module_obj_type_map_[require_module][require_object_type] = module_obj_type_list_.size();
							module_obj_type_list_.push_back(module_object_type_entry);
						}
						unsigned require_index = module_obj_type_map_[require_module][require_object_type];
						module_obj_type_list_[module_obj_type_map_[module][object_type]].requires.push_back(require_index);
					}
				}
			}
			else if (!strcmp(element->Value(), "directory")) {
				std::string name = element->Attribute("name");
				ParserLibraryEntry(element->FirstChildElement(), base_name + name + "/");
			}
		}
	}

	void ResourceXML::LoadLibraryList() {
		std::string xml_file = GetBasePath() + GetLibrariesPath() + "/" + ukRESOURCE_LIST_XML;
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError err = doc.LoadFile(xml_file.c_str());
		if (err != tinyxml2::XML_SUCCESS) return;
		library_list_.clear();
		libraries_map_.clear();
		module_obj_type_list_.clear();
		module_obj_type_map_.clear();
		ParserLibraryEntry(doc.FirstChildElement(), "");
	}

	void ResourceXML::ParserNodeSetEntry(tinyxml2::XMLElement *element, const std::string &base_name) {
		for (; element != nullptr; element = element->NextSiblingElement()) {
			if (!strcmp(element->Value(), "nodeset")) {
				std::string name = element->Attribute("name");
				std::string src = element->Attribute("src");
				NodeSetEntry entry;
				entry.name = base_name + name;
				entry.src = src;
				nodes_map_[entry.name] = key_set_list_.size();
				node_set_list_.push_back(entry);
			}
			else if (!strcmp(element->Value(), "directory")) {
				std::string name = element->Attribute("name");
				ParserNodeSetEntry(element->FirstChildElement(), base_name + name + "/");
			}
		}
	}

	void ResourceXML::LoadNodeSetList() {
		std::string xml_file = GetBasePath() + GetNodesPath() + "/" + ukRESOURCE_LIST_XML;
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError err = doc.LoadFile(xml_file.c_str());
		if (err != tinyxml2::XML_SUCCESS) return;
		node_set_list_.clear();
		nodes_map_.clear();
		ParserNodeSetEntry(doc.FirstChildElement(),"");
	}

	void ResourceXML::ParserPipelineEntry(tinyxml2::XMLElement *element, const std::string &base_name) {
		for (; element != nullptr; element = element->NextSiblingElement()) {
			if (!strcmp(element->Value(), "pipeline")) {
				std::string name = element->Attribute("name");
				std::string src = element->Attribute("src");
				PipelineEntry entry;
				entry.name = base_name + name;
				entry.src = src;
				pipelines_map_[entry.name] = key_set_list_.size();
				pipeline_list_.push_back(entry);
			}
			else if (!strcmp(element->Value(), "directory")) {
				std::string name = element->Attribute("name");
				ParserPipelineEntry(element->FirstChildElement(), base_name + name + "/");
			}
		}
	}

	void ResourceXML::LoadPipelineList() {
		std::string xml_file = GetBasePath() + GetPipelinesPath() + "/" + ukRESOURCE_LIST_XML;
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError err = doc.LoadFile(xml_file.c_str());
		if (err != tinyxml2::XML_SUCCESS) return;
		pipeline_list_.clear();
		pipelines_map_.clear();
		ParserPipelineEntry(doc.FirstChildElement(),"");
	}

	void ResourceXML::ParserSceneEntry(tinyxml2::XMLElement *element, const std::string &base_name) {
		for (; element != nullptr; element = element->NextSiblingElement()) {
			if (!strcmp(element->Value(), "scene")) {
				std::string name = element->Attribute("name");
				std::string src = element->Attribute("src");
				SceneEntry entry;
				entry.name = base_name + name;
				entry.src = src;
				scenes_map_[entry.name] = key_set_list_.size();
				scene_list_.push_back(entry);
			}
			else if (!strcmp(element->Value(), "directory")) {
				std::string name = element->Attribute("name");
				ParserSceneEntry(element->FirstChildElement(), base_name + name + "/");
			}
		}
	}

	void ResourceXML::LoadSceneList() {
		std::string xml_file = GetBasePath() + GetScenesPath() + "/" + ukRESOURCE_LIST_XML;
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError err = doc.LoadFile(xml_file.c_str());
		if (err != tinyxml2::XML_SUCCESS) return;
		scene_list_.clear();
		scenes_map_.clear();
		ParserSceneEntry(doc.FirstChildElement(), "");
	}

	void ResourceXML::ParserScriptEntry(tinyxml2::XMLElement *element, const std::string &base_name) {
		for (; element != nullptr; element = element->NextSiblingElement()) {
			if (!strcmp(element->Value(), "script")) {
				std::string name = element->Attribute("name");
				std::string src = element->Attribute("src");
				ScriptEntry entry;
				entry.name = base_name + name;
				entry.src = src;
				scripts_map_[entry.name] = script_list_.size();
				script_list_.push_back(entry);
			}
			else if (!strcmp(element->Value(), "directory")) {
				std::string name = element->Attribute("name");
				ParserScriptEntry(element->FirstChildElement(), base_name + name + "/");
			}
		}
	}

	void ResourceXML::LoadScriptList() {
		std::string xml_file = GetBasePath() + GetScriptsPath() + "/" + ukRESOURCE_LIST_XML;
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError err = doc.LoadFile(xml_file.c_str());
		if (err != tinyxml2::XML_SUCCESS) return;
		script_list_.clear();
		scripts_map_.clear();
		ParserScriptEntry(doc.FirstChildElement(), "");
	}

	void ResourceXML::ParserShaderEntry(tinyxml2::XMLElement *element, const std::string &base_name) {
		for (; element != nullptr; element = element->NextSiblingElement()) {
			if (!strcmp(element->Value(), "shader")) {
				std::string name = element->Attribute("name");
				std::string src = element->Attribute("src");
				ShaderEntry entry;
				entry.name = base_name + name;
				entry.src = src;
				shaders_map_[entry.name] = script_list_.size();
				shader_list_.push_back(entry);
			}
			else if (!strcmp(element->Value(), "directory")) {
				std::string name = element->Attribute("name");
				ParserShaderEntry(element->FirstChildElement(), base_name + name + "/");
			}
		}
	}

	void ResourceXML::LoadShaderList() {
		std::string xml_file = GetBasePath() + GetShadersPath() + "/" + ukRESOURCE_LIST_XML;
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError err = doc.LoadFile(xml_file.c_str());
		if (err != tinyxml2::XML_SUCCESS) return;
		shader_list_.clear();
		shaders_map_.clear();
		ParserShaderEntry(doc.FirstChildElement(), "");
	}

	const std::string &ResourceXML::GetResourceXMLFile() const {
		return resource_xml_file_;
	}

	const std::string &ResourceXML::GetBinariesPath() const {
		return binaries_path_;
	}

	const std::string &ResourceXML::GetKeysPath() const {
		return keys_path_;
	}

	const std::string &ResourceXML::GetLibrariesPath() const {
		return libraries_path_;
	}

	const std::string &ResourceXML::GetNodesPath() const {
		return nodes_path_;
	}

	const std::string &ResourceXML::GetPipelinesPath() const {
		return pipelines_path_;
	}

	const std::string &ResourceXML::GetScenesPath() const {
		return scenes_path_;
	}

	const std::string &ResourceXML::GetScriptsPath() const {
		return scripts_path_;
	}

	const std::string &ResourceXML::GetShadersPath() const {
		return shaders_path_;
	}

	const std::string &ResourceXML::GetBasePath() const {
		return base_path_;
	}

	const std::vector<ResourceXML::BinaryEntry> &ResourceXML::GetBinaryList() const {
		return binary_list_;
	}

	const std::vector<ResourceXML::KeySetEntry> &ResourceXML::GetKeySetList() const {
		return key_set_list_;
	}

	const std::vector<ResourceXML::LibraryEntry> &ResourceXML::GetLibraryList() const {
		return library_list_;
	}

	const std::vector<ResourceXML::NodeSetEntry> &ResourceXML::GetNodeSetList() const {
		return node_set_list_;
	}

	const std::vector<ResourceXML::PipelineEntry> &ResourceXML::GetPipelineList() const {
		return pipeline_list_;
	}

	const std::vector<ResourceXML::SceneEntry> &ResourceXML::GetSceneList() const {
		return scene_list_;
	}

	const std::vector<ResourceXML::ScriptEntry> &ResourceXML::GetScriptList() const {
		return script_list_;
	}

	const std::vector<ResourceXML::ShaderEntry> &ResourceXML::GetShaderList() const {
		return shader_list_;
	}

	const std::vector<ResourceXML::ModuleObjectTypeEntry> &ResourceXML::GetModuleObjectTypeList() const {
		return module_obj_type_list_;
	}

	const ResourceXML::BinaryEntry *ResourceXML::GetBinaryEntry(const std::string &name)  {
		std::map<std::string, unsigned int>::iterator itor = binaries_map_.find(name);
		if (itor != binaries_map_.end())
			return &binary_list_[itor->second];
		return nullptr;
	}

	const ResourceXML::KeySetEntry *ResourceXML::GetKeySetEntry(const std::string &name) {
		std::map<std::string, unsigned int>::iterator itor = keys_map_.find(name);
		if (itor != keys_map_.end())
			return &key_set_list_[itor->second];
		return nullptr;
	}

	const ResourceXML::LibraryEntry *ResourceXML::GetLibraryEntry(const std::string &name) {
		std::map<std::string, unsigned int>::iterator itor = libraries_map_.find(name);
		if(itor != libraries_map_.end())
			return &library_list_[itor->second];
		return nullptr;
	}

	const ResourceXML::NodeSetEntry *ResourceXML::GetNodeSetEntry(const std::string &name){
		std::map<std::string, unsigned int>::iterator itor = nodes_map_.find(name);
		if (itor != nodes_map_.end())
			return &node_set_list_[itor->second];
		return nullptr;
	}

	const ResourceXML::PipelineEntry *ResourceXML::GetPipelineEntry(const std::string &name) {
		std::map<std::string, unsigned int>::iterator itor = pipelines_map_.find(name);
		if (itor != pipelines_map_.end())
			return &pipeline_list_[itor->second];
		return nullptr;
	}

	const ResourceXML::SceneEntry *ResourceXML::GetSceneEntry(const std::string &name) {
		std::map<std::string, unsigned int>::iterator itor = scenes_map_.find(name);
		if (itor != scenes_map_.end())
			return &scene_list_[itor->second];
		return nullptr;
	}

	const ResourceXML::ScriptEntry *ResourceXML::GetScriptEntry(const std::string &name) {
		std::map<std::string, unsigned int>::iterator itor = scripts_map_.find(name);
		if (itor != scripts_map_.end())
			return &script_list_[itor->second];
		return nullptr;
	}

	const ResourceXML::ShaderEntry *ResourceXML::GetShaderEntry(const std::string &name) {
		std::map<std::string, unsigned int>::iterator itor = shaders_map_.find(name);
		if (itor != shaders_map_.end())
			return &shader_list_[itor->second];
		return nullptr;
	}

	const ResourceXML::ModuleObjectTypeEntry *ResourceXML::GetModuleObjectTypeEntry(
		const std::string module,
		const std::string object_type) {
		if(module_obj_type_map_.find(module)==module_obj_type_map_.end() ||
			module_obj_type_map_[module].find(object_type)== module_obj_type_map_[module].end())
			return nullptr;
		unsigned int index = module_obj_type_map_[module][object_type];
		return &module_obj_type_list_[index];
	}

	ResourceXML::ResourceXML() {
	}

	ResourceXML::~ResourceXML() {
	}
}
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
#include <set>
#include <map>
#include <queue>

#include <tinyxml2.h>

#include <appxml.h>

namespace unlike3d {
	AppXML::AppXML() {

	}

	bool AppXML::LoadAppXML(const char *strName) {
		tinyxml2::XMLDocument doc;

		tinyxml2::XMLError err = doc.LoadFile(strName);

		if (err != tinyxml2::XML_SUCCESS) return false;
		app_xml_file_ = strName;
		std::map<std::string, std::map<std::string, unsigned int>> ms;
		std::map<unsigned int, unsigned int> ind;
		std::vector<ModuleLoadEntry> vs;
		std::vector<ModuleLoadEntry> objs;
		std::map<unsigned int, std::vector<unsigned int>> es;
		unsigned int index = 0;
		for (tinyxml2::XMLElement *element = doc.FirstChildElement(); element != nullptr;
			element = element->NextSiblingElement()) {
			std::string module_name, object_name, object_type;
			module_name = ""; object_name = ""; object_type = "";
			if (!strcmp(element->Value(), "module")) {
				module_name=element->Attribute("name");
				object_type = element->Attribute("object_type");
				if (ms.find(module_name) == ms.end() || ms[module_name].find(object_type)==ms[module_name].end()) {
					ModuleLoadEntry entry;
					entry.module_name = module_name;
					entry.object_type = object_type;
					entry.object_name = "";
					vs.push_back(entry);
					ind[index] = 0;
					ms[module_name][object_type] = index++;
				}
				unsigned int module_entry_id = ms[module_name][object_type];
				for (tinyxml2::XMLElement *sub_element = element->FirstChildElement(); sub_element != nullptr;
					sub_element = sub_element->NextSiblingElement()) {
					std::string sub_module_name, sub_object_name, sub_object_type;
					sub_module_name = ""; sub_object_name = ""; sub_object_type = "";
					if (!strcmp(sub_element->Value(), "require")) {
						sub_module_name = sub_element->Attribute("module");
						sub_object_type = sub_element->Attribute("object_type");
						if (ms.find(sub_module_name) == ms.end() || ms[sub_module_name].find(sub_object_type) == ms[sub_module_name].end()) {
							ModuleLoadEntry entry;
							entry.module_name = sub_module_name;
							entry.object_type = sub_object_type;
							entry.object_name = "";
							vs.push_back(entry);
							ind[index] = 0;
							ms[sub_module_name][sub_object_type] = index++;
						}
						unsigned int require_module_id = ms[sub_module_name][sub_object_type];
						es[require_module_id].push_back(module_entry_id);
						ind[module_entry_id]++;
					}
				}
			}
			else if (!strcmp(element->Value(), "object")) {
				object_name = element->Attribute("name");
				module_name = element->Attribute("module");
				object_type = element->Attribute("object_type");
				ModuleLoadEntry entry;
				entry.module_name = module_name;
				entry.object_type = object_type;
				entry.object_name = object_name;
				objs.push_back(entry);
				if (ms.find(module_name) == ms.end() || ms[module_name].find(object_type) == ms[module_name].end()) {
					ModuleLoadEntry entry;
					entry.module_name = module_name;
					entry.object_type = object_type;
					entry.object_name ="";
					vs.push_back(entry);
					ind[index] = 0;
					ms[module_name][object_type] = index++;
				}
				unsigned int module_entry_id = ms[module_name][object_type];
				for (tinyxml2::XMLElement *sub_element = element->FirstChildElement(); sub_element != nullptr;
					sub_element = sub_element->NextSiblingElement()) {
					std::string sub_module_name, sub_object_name, sub_object_type;
					sub_module_name = ""; sub_object_name = ""; sub_object_type = "";
					if (!strcmp(sub_element->Value(), "require")) {
						sub_module_name = sub_element->Attribute("module");
						sub_object_type = sub_element->Attribute("object_type");
						if (ms.find(sub_module_name) == ms.end() || ms[sub_module_name].find(sub_object_type) == ms[sub_module_name].end()) {
							ModuleLoadEntry entry;
							entry.module_name = sub_module_name;
							entry.object_type = sub_object_type;
							entry.object_name = "";
							vs.push_back(entry);
							ind[index] = 0;
							ms[sub_module_name][sub_object_type] = index++;
						}
						unsigned int require_module_id = ms[sub_module_name][sub_object_type];
						es[require_module_id].push_back(module_entry_id);
						ind[module_entry_id]++;
					}
				}
			}
			else if (!strcmp(element->Value(), "resource")) {
				resource_xml_file_ = element->Attribute("src");
			}
			else if (!strcmp(element->Value(), "libraries")) {
				libraries_path_ = element->Attribute("path");

			}
			else if (!strcmp(element->Value(), "script")) {
				script_ = element->Attribute("src");
			}
		}
		module_load_entries_.clear();
		std::queue<unsigned int> que;
		for (unsigned int i = 0; i < index; i++)
			if (ind[i] == 0) que.push(i);
		while (!que.empty()) {
			unsigned int id = que.front(); que.pop();
			module_load_entries_.push_back(vs[id]);
			for (unsigned int i = 0; i < es[id].size(); i++) {
				unsigned int to = es[id][i];
				ind[to]--;
				if (ind[to] == 0) que.push(to);
			}
		}
		for (unsigned int i = 0; i < objs.size(); i++)
			module_load_entries_.push_back(objs[i]);
		return true;
	}

	const std::string &AppXML::GetLibrariesPath() const{
		return libraries_path_;
	}

	const std::string &AppXML::GetResourceXMLFile() const {
		return resource_xml_file_;
	}

	const std::string &AppXML::GetScriptName() const{
		return script_;
	}

	const std::vector<AppXML::ModuleLoadEntry> &AppXML::GetModuleLoadEntries() const{
		return module_load_entries_;
	}

	const std::string &AppXML::GetAppXMLFile() const{
		return app_xml_file_;
	}

	AppXML::~AppXML() {

	}

}
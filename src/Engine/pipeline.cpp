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
#include <pipeline.h>

namespace unlike3d {
	void Pipeline::ParserSortsEntry(tinyxml2::XMLElement *element) {
		memset(sort_function_map_, 0, sizeof(sort_function_map_));
		for (; element != nullptr;
			element = element->NextSiblingElement()) {
			if (!strcmp(element->Value(), "sort")) {
				std::string handle = element->Attribute("handle");
				std::string module = element->Attribute("module");
				std::string function = element->Attribute("function");
				SortEntry entry;
				entry.handle = atoi(handle.c_str());
				entry.module = module;
				entry.function = function;
				entry.function_point =(ukIntFuncPointPointPoint) GetFunction(module, function);
				for (unsigned int i = 0; i < MAX_SORT_LIST_COUNT; i++) {
					if ((1 << i)&entry.handle) {
						sort_function_map_[i] = sorts_.size()+1;
					}
				}
				sorts_.push_back(entry);
			}
		}
	}

	const Pipeline::SortEntry *Pipeline::GetSortEntry(unsigned int bit_index) {
		unsigned index = sort_function_map_[bit_index];
		if (index == 0) return nullptr;
		return &sorts_[index - 1];
	}
	
	void Pipeline::ParserObjectsEntry(tinyxml2::XMLElement *element) {
		for (; element != nullptr; element=element->NextSiblingElement()) {
			if (!strcmp(element->Value(), "object")) {
				std::string name = element->Attribute("name");
				std::string module = element->Attribute("module");
				std::string object_type = element->Attribute("object_type");
				Object *obj = CreateObject(module, object_type);
				if (obj == nullptr) {
					//error
				}
				obj->SetProperty("ukApplicationObject", GetApplication()->GetNativeObject());
				obj->SetProperty("ukResourceObject", GetResource()->GetNativeObject());
				obj->GetNativeObject()->data4.point = GetResource();
				ObjectEntry entry;
				entry.name = name;
				entry.module = module;
				entry.object_type = object_type;
				entry.object = obj;
				pipeline_objects_map_[name]= pipeline_objects_.size();
				pipeline_objects_.push_back(entry);
				tinyxml2::XMLElement *sub_element = element->FirstChildElement("property");
				ParserPropertiesEntry(obj, sub_element);
			}
		}
	}

	void Pipeline::ParserCommandsEntry(tinyxml2::XMLElement *element) {
		for (; element != nullptr;
			element = element->NextSiblingElement()) {
			if (!strcmp(element->Value(), "command")) {
				std::string name = element->Attribute("name");
				std::string thread_id;
				if (element->FindAttribute("thread_id"))
					thread_id = element->Attribute("thread_id");
				std::string handle = element->Attribute("handle");
				std::string module = element->Attribute("module");
				std::string function = element->Attribute("function");
				std::string ref_object = element->Attribute("ref_object");
				CommandEntry entry;
				entry.name = name;
				if (thread_id != "") entry.thread_id = atoi(thread_id.c_str());
				else entry.thread_id = -1;
				entry.handle = atoi(handle.c_str());
				entry.module = module;
				entry.function = function;
				entry.function_point = (ukBoolFunc3Point)GetFunction(module, function);
				if (entry.function_point == nullptr) {
					//error
				}
				unsigned int ref_object_index = pipeline_objects_map_[name];
				entry.ref_object = pipeline_objects_[ref_object_index].object;
				commands_map_[name] = commands_.size();
				commands_.push_back(entry);
			}
		}
	}

	void Pipeline::ParserStepsEntry(tinyxml2::XMLElement *element) {
		for (element = element->FirstChildElement();element!=nullptr; element = element->NextSiblingElement()) {
			if (!strcmp(element->Value(), "step")) {
				std::string name = element->Attribute("name");
				std::string command = element->Attribute("command");
				const tinyxml2::XMLAttribute *attr=element->FindAttribute("parallel");
				std::string parallel;
				if(attr)
					parallel = element->Attribute("parallel");
				FlowchartStepEntry entry;
				entry.name = name;
				entry.command = command;
				if (parallel != "true") entry.parallel = false;
				else entry.parallel = true;
				flowchart_steps_map_[name] = flowchart_steps_.size();
				flowchart_steps_.push_back(entry);
			}
		}
		unsigned int count = flowchart_steps_.size();
		steps_.resize(count);
		for (unsigned int i = 0; i < count; i++) {
			steps_[i].to_steps.clear();
			steps_[i].start_count = 0;
			std::string cmd_name = flowchart_steps_[i].command;
			unsigned int cmd_index = commands_map_[cmd_name];
			steps_[i].ref_object = commands_[cmd_index].ref_object;
			steps_[i].thread_id = commands_[cmd_index].thread_id;
			steps_[i].function_point = commands_[cmd_index].function_point;
			steps_[i].parallel = flowchart_steps_[i].parallel;
			steps_[i].handles.clear();
			for (unsigned int j = 0; j < MAX_SORT_LIST_COUNT; j++) {
				if ((1 << j)&commands_[cmd_index].handle) {
					steps_[i].handles.push_back(j);
				}
			}
		}
	}

	void Pipeline::ParserEdgesEntry(tinyxml2::XMLElement *element) {
		for (element = element->FirstChildElement();element!=nullptr; element = element->NextSiblingElement()) {
			if (!strcmp(element->Value(), "edge")) {
				std::string from = element->Attribute("from");
				std::string to = element->Attribute("to");
				FlowchartEdgeEntry entry;
				entry.from = from;
				entry.from_step_index = flowchart_steps_map_[from];
				entry.to = to;
				entry.to_step_index = flowchart_steps_map_[to];
				steps_[entry.to_step_index].start_count++;
				steps_[entry.from_step_index].to_steps.push_back(entry.to_step_index);
				flowchart_edges_.push_back(entry);
			}
		}
	}

	void Pipeline::ParserFlowchartEntry(tinyxml2::XMLElement *element) {
		tinyxml2::XMLElement *sub_element = element->FirstChildElement("steps");
		if (sub_element != nullptr)
			ParserStepsEntry(sub_element);
		sub_element = element->FirstChildElement("edges");
		if (sub_element != nullptr)
			ParserEdgesEntry(sub_element);
	}

	void Pipeline::ParserFunctionsEntry(tinyxml2::XMLElement *element) {
		FunctionEntry entry;
		SceneFunctionEntry scene_function_entry;
		for (; element != nullptr;
			element = element->NextSiblingElement()) {
			if (!strcmp(element->Value(), "init")) {
				std::string module = element->Attribute("module");
				std::string function = element->Attribute("function");
				std::string ref_object = element->Attribute("ref_object");
				ukBoolFuncPoint func_point = (ukBoolFuncPoint)GetFunction(module, function);
				entry.function_point = func_point;
				unsigned int ref_object_index = pipeline_objects_map_[ref_object];
				entry.object = pipeline_objects_[ref_object_index].object;
				init_functions_.push_back(entry);
			}
			else if (!strcmp(element->Value(), "destroy")) {
				std::string module = element->Attribute("module");
				std::string function = element->Attribute("function");
				std::string ref_object = element->Attribute("ref_object");
				ukBoolFuncPoint func_point = (ukBoolFuncPoint)GetFunction(module, function);
				entry.function_point = func_point;
				unsigned int ref_object_index = pipeline_objects_map_[ref_object];
				entry.object = pipeline_objects_[ref_object_index].object;
				destroy_functions_.push_back(entry);
			}
			else if (!strcmp(element->Value(), "loop")) {
				std::string module = element->Attribute("module");
				std::string function = element->Attribute("function");
				std::string ref_object = element->Attribute("ref_object");
				ukBoolFuncPoint func_point = (ukBoolFuncPoint)GetFunction(module, function);
				entry.function_point = func_point;
				unsigned int ref_object_index = pipeline_objects_map_[ref_object];
				entry.object = pipeline_objects_[ref_object_index].object;
				loop_functions_.push_back(entry);
			}
			else if (!strcmp(element->Value(), "loadscene")) {
				std::string module = element->Attribute("module");
				std::string function = element->Attribute("function");
				std::string ref_object = element->Attribute("ref_object");
				std::string handle = element->Attribute("handle");
				unsigned int handle_value = atoi(handle.c_str());
				scene_function_entry.function_point = (ukBoolFuncPointPoint)GetFunction(module, function);
				unsigned int ref_object_index = pipeline_objects_map_[ref_object];
				scene_function_entry.object = pipeline_objects_[ref_object_index].object;
				if (handle_value == 0) {
					loadscene_functions_[0].push_back(scene_function_entry);
				}
				else {
					for (unsigned int i = 0; i < MAX_SORT_LIST_COUNT; i++) {
						if ((1 << i)&handle_value) {
							loadscene_functions_[i + 1].push_back(scene_function_entry);
						}
					}
				}
			}
			else if (!strcmp(element->Value(), "destroyscene")) {
				std::string module = element->Attribute("module");
				std::string function = element->Attribute("function");
				std::string ref_object = element->Attribute("ref_object");
				std::string handle = element->Attribute("handle");
				unsigned int handle_value = atoi(handle.c_str());
				scene_function_entry.function_point = (ukBoolFuncPointPoint)GetFunction(module, function);
				unsigned int ref_object_index = pipeline_objects_map_[ref_object];
				scene_function_entry.object = pipeline_objects_[ref_object_index].object;
				if (handle_value == 0) {
					destroyscene_functions_[0].push_back(scene_function_entry);
				}
				else {
					for (unsigned int i = 0; i < MAX_SORT_LIST_COUNT; i++) {
						if ((1 << i)&handle_value) {
							destroyscene_functions_[i + 1].push_back(scene_function_entry);
						}
					}
				}
			}
		}
	}

	std::vector<Pipeline::SceneFunctionEntry> &Pipeline::GetLoadSceneFunctionEntry(unsigned int index) {
		return loadscene_functions_[index];
	}

	void Pipeline::ParserPropertiesEntry(Object *object,tinyxml2::XMLElement *element) {
		for (; element != nullptr;
			element = element->NextSiblingElement()) {
			if (!strcmp(element->Value(), "property")) {
				std::string name = element->Attribute("name");
				std::string value = element->Attribute("value");
				
				object->SetProperty(name, (void *)value.c_str());
			}
		}
	}

	bool Pipeline::LoadFromXMLFile(const std::string &name) {
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError err=doc.LoadFile(name.c_str());
		if (err != tinyxml2::XML_SUCCESS) return false;
		
		tinyxml2::XMLElement *objs_element = doc.FirstChildElement("objects");
		if (objs_element == nullptr) return false;
		pipeline_objects_.clear();
		pipeline_objects_map_.clear();
		ParserObjectsEntry(objs_element->FirstChildElement());
		tinyxml2::XMLElement *element= doc.FirstChildElement("functions");
		if (element == nullptr) return false;
		ParserFunctionsEntry(element->FirstChildElement());
		element = doc.FirstChildElement("commands");
		if (element == nullptr) return false;
		commands_.clear();
		commands_map_.clear();
		ParserCommandsEntry(element->FirstChildElement());
		element = doc.FirstChildElement("sorts");
		if (element != nullptr) {
			const tinyxml2::XMLAttribute *attr = element->FindAttribute("parallel");
			std::string parallel;
			if(element->FindAttribute("parallel"))
				parallel = element->Attribute("parallel");
			if (parallel != "true") sort_parallel_ = false;
			else sort_parallel_ = true;
			sorts_.clear();
			ParserSortsEntry(element->FirstChildElement());
		}
		element = doc.FirstChildElement("flowchart");
		if (element == nullptr) return false;
		flowchart_steps_.clear();
		flowchart_edges_.clear();
		flowchart_steps_map_.clear();
		ParserFlowchartEntry(element);
		return true;
	}

	void Pipeline::ResetSteps(){
		unsigned int count = steps_.size();
		for (unsigned int i = 0; i < count; i++)
			steps_[i].now_count = steps_[i].now_step_count = 0;
	}

	std::vector<Pipeline::Step> &Pipeline::GetSteps() {
		return steps_;
	}

	bool Pipeline::InitPipeline() {
		bool init_ok = true;
		unsigned int count = init_functions_.size();
		for (unsigned int i = 0; i < count; i++) {
			Object *object = init_functions_[i].object;
			init_ok=init_functions_[i].function_point(object->GetNativeObject());
			if (init_ok == false) break;
		}
		return init_ok;
	}

	bool Pipeline::LoopPipeline() {
		bool loop = true;
		unsigned int count = loop_functions_.size();
		for (unsigned int i = 0;i<count; i++) {
			Object *object = loop_functions_[i].object;
			loop = loop_functions_[i].function_point(object->GetNativeObject());
			if (loop==false) break;
		}
		return loop;
	}

	bool Pipeline::DestroyPipeline() {
		bool destroy_ok = true;
		unsigned int count = destroy_functions_.size();
		for (unsigned int i = 0; i < count; i++) {
			Object *object = destroy_functions_[i].object;
			destroy_ok = destroy_functions_[i].function_point(object->GetNativeObject());
			if (destroy_ok == false) break;
		}
		return destroy_ok;
	}

	void Pipeline::SetPipelineObjectProperty(const std::string &name, void *data) {
		GetPipelineObject()->SetProperty(name, data);
	}

	void Pipeline::SetApplication(Object *application_object) {
		application_object_ = application_object;
	}

	void Pipeline::SetResource(Object *resource_object) {
		resource_object_ = resource_object;
	}

	Object *Pipeline::GetApplication() {
		return application_object_;
	}

	Object *Pipeline::GetResource() {
		return resource_object_;
	}

	Object *Pipeline::GetPipelineObject() {
		return pipeline_object_;
	}

	Object *Pipeline::CreateObject(const std::string &module, const std::string &object_type) {
		std::vector<FunctionParameter> fp;
		std::vector<FunctionReturn> fr;
		fp.resize(2);
		fp[0].string_value = module;
		fp[1].string_value = object_type;
		GetResource()->CallFunction("CreateObject", fp, fr);
		return (Object *)fr[0].point_value;
	}

	void *Pipeline::GetFunction(const std::string &module,const std::string &function) {
		std::vector<FunctionParameter> fp;
		std::vector<FunctionReturn> fr;
		fp.resize(2);
		fp[0].string_value = module;
		fp[1].string_value = function;
		GetResource()->CallFunction("GetFunction", fp, fr);
		return fr[0].point_value;
	}

	Pipeline::Step::Step() {

	}

	Pipeline::Step::Step(const Pipeline::Step &step) {
		to_steps = step.to_steps;
		start_count = step.start_count.load();
		now_count = step.now_count.load();
		step_count = step.start_count.load();
		now_step_count = step.now_step_count.load();
		function_point = step.function_point;
		handles = step.handles;
		parallel = step.parallel;
	}

	Pipeline::Pipeline() {
		pipeline_object_ = nullptr;
		application_object_ = nullptr;
		resource_object_ = nullptr;
		init_function_point = nullptr;
		destroy_function_point = nullptr;
		memset(sort_function_map_, 0, sizeof(sort_function_map_));
	}

	Pipeline::~Pipeline() {

	}
}
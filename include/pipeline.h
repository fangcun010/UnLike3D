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
#ifndef _PIPELINE_H_INCLUDED
#define _PIPELINE_H_INCLUDED

#include <tinyxml2.h>
#include <string>
#include <objecttype.h>
#include <atomic>

#define			MAX_SORT_LIST_COUNT						32

namespace unlike3d {
	class Pipeline {
	public:
		struct ObjectEntry {
			std::string name;
			std::string module;
			std::string object_type;
			Object *object;
		};
		struct FunctionEntry {
			Object *object;
			ukBoolFuncPoint function_point;
		};
		struct SceneFunctionEntry {
			Object *object;
			ukBoolFuncPointPoint function_point;
		};
		struct CommandEntry {
			std::string name;
			std::string module;
			std::string function;
			Object *ref_object;
			unsigned int thread_id;
			unsigned int handle;
			ukBoolFunc3Point function_point;
		};
		struct SortEntry {
			unsigned int handle;
			std::string module;
			std::string function;
			ukIntFuncPointPointPoint function_point;
		};
		struct FlowchartStepEntry {
			std::string name;
			std::string command;
			bool parallel;
		};
		struct FlowchartEdgeEntry {
			std::string from;
			unsigned int from_step_index;
			std::string to;
			unsigned int to_step_index;
		};
		struct Step {
			std::vector<int> to_steps;
			std::atomic_uint32_t start_count;
			std::atomic_uint32_t now_count;
			std::atomic_uint32_t step_count;
			std::atomic_uint32_t now_step_count;
			int thread_id;
			Object *ref_object;
			ukBoolFunc3Point function_point;
			std::vector<unsigned int> handles;
			bool parallel;
			Step();
			Step(const Step &step);
		};
	private:
		std::vector<CommandEntry> commands_;
		std::vector<SortEntry> sorts_;
		bool sort_parallel_;
		std::vector<FlowchartStepEntry> flowchart_steps_;
		std::vector<FlowchartEdgeEntry> flowchart_edges_;
		std::map<std::string, unsigned int> commands_map_;
		std::map<std::string, unsigned int> flowchart_steps_map_;
		std::vector<Step> steps_;
		unsigned int sort_function_map_[MAX_SORT_LIST_COUNT];
	private:
		Object *application_object_;
		Object *resource_object_;
		Object *pipeline_object_;//to delete
		std::vector<ObjectEntry> pipeline_objects_;
		std::map<std::string, unsigned int> pipeline_objects_map_;
		void ParserObjectsEntry(tinyxml2::XMLElement *element);
		void ParserSortsEntry(tinyxml2::XMLElement *element);
		void ParserCommandsEntry(tinyxml2::XMLElement *element);
		void ParserStepsEntry(tinyxml2::XMLElement *element);
		void ParserEdgesEntry(tinyxml2::XMLElement *element);
		void ParserFlowchartEntry(tinyxml2::XMLElement *element);
		void ParserFunctionsEntry(tinyxml2::XMLElement *element);
		void ParserPropertiesEntry(Object *object,tinyxml2::XMLElement *element);
		ukBoolFuncPoint init_function_point;//to delete
		ukBoolFuncPoint loop_function_point;//to delete
		ukBoolFuncPoint destroy_function_point;//to delete
		std::vector<FunctionEntry> init_functions_;
		std::vector<FunctionEntry> loop_functions_;
		std::vector<FunctionEntry> destroy_functions_;
		std::vector<SceneFunctionEntry> loadscene_functions_[MAX_SORT_LIST_COUNT+1];
		std::vector<SceneFunctionEntry> destroyscene_functions_[MAX_SORT_LIST_COUNT+1];
		virtual Object *CreateObject(const std::string &module, const std::string &object_type);
		virtual void *GetFunction(const std::string &module,const std::string &function);
	public:
		virtual const SortEntry *GetSortEntry(unsigned int bit_index);
		virtual std::vector<Step> &GetSteps();
		virtual void ResetSteps();

		virtual bool LoadFromXMLFile(const std::string &name);
		virtual void SetApplication(Object *application_object);
		virtual void SetResource(Object *resource_object);
		virtual Object *GetApplication();
		virtual Object *GetResource();
		virtual Object *GetPipelineObject();
		
		virtual std::vector<SceneFunctionEntry> &GetLoadSceneFunctionEntry(unsigned int index);
		virtual bool InitPipeline();
		virtual bool LoopPipeline();
		virtual bool DestroyPipeline();
		virtual void SetPipelineObjectProperty(const std::string &name, void *data);//to delete
		
		Pipeline();
		virtual ~Pipeline();
	};
}

#endif
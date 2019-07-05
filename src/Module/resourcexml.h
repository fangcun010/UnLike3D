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
#ifndef _RESOURCEXML_H_INCLUDED
#define _RESOURCEXML_H_INCLUDED

#include <string>
#include <map>
#include <vector>

#include <tinyxml2.h>

namespace unlike3d {
	class ResourceXML {
	public:
		struct BinaryEntry {
			std::string name;
			std::string src;
		};

		struct KeySetEntry {
			std::string name;
			std::string src;
		};

		struct LibraryEntry {
			std::string name;
			std::string module;
			std::string object_type;
		};

		struct NodeSetEntry {
			std::string name;
			std::string src;
		};

		struct PipelineEntry {
			std::string name;
			std::string src;
		};

		struct SceneEntry {
			std::string name;
			std::string src;
		};

		struct ScriptEntry {
			std::string name;
			std::string src;
		};

		struct ShaderEntry {
			std::string name;
			std::string src;
		};

		struct ModuleObjectTypeEntry {
			std::string module;
			std::string object_type;
			std::vector<unsigned int> requires;
		};
	private:
		std::string resource_xml_file_;
		std::string binaries_path_;
		std::string keys_path_;
		std::string libraries_path_;
		std::string nodes_path_;
		std::string pipelines_path_;
		std::string scenes_path_;
		std::string scripts_path_;
		std::string shaders_path_;
		std::string base_path_;
		std::map<std::string, unsigned int> binaries_map_;
		std::map<std::string, unsigned int> keys_map_;
		std::map<std::string, unsigned int> libraries_map_;
		std::map<std::string, unsigned int> nodes_map_;
		std::map<std::string, unsigned int> pipelines_map_;
		std::map<std::string, unsigned int> scenes_map_;
		std::map<std::string, unsigned int> scripts_map_;
		std::map<std::string, unsigned int> shaders_map_;
		std::vector<BinaryEntry> binary_list_;
		std::vector<KeySetEntry> key_set_list_;
		std::vector<LibraryEntry> library_list_;
		std::vector<NodeSetEntry> node_set_list_;
		std::vector<PipelineEntry> pipeline_list_;
		std::vector<SceneEntry> scene_list_;
		std::vector<ScriptEntry> script_list_;
		std::vector<ShaderEntry> shader_list_;

		std::map <std::string, std::map<std::string, unsigned int> > module_obj_type_map_;
		std::vector<ModuleObjectTypeEntry> module_obj_type_list_;
	private:
		void ParserBinaryEntry(tinyxml2::XMLElement *element, const std::string &base_name);
		void ParserKeySetEntry(tinyxml2::XMLElement *element, const std::string &base_name);
		void ParserLibraryEntry(tinyxml2::XMLElement *element, const std::string &base_name);
		void ParserNodeSetEntry(tinyxml2::XMLElement *element, const std::string &base_name);
		void ParserPipelineEntry(tinyxml2::XMLElement *element, const std::string &base_name);
		void ParserSceneEntry(tinyxml2::XMLElement *element, const std::string &base_name);
		void ParserScriptEntry(tinyxml2::XMLElement *element, const std::string &base_name);
		void ParserShaderEntry(tinyxml2::XMLElement *element, const std::string &base_name);
		void LoadBinaryList();
		void LoadKeySetList();
		void LoadLibraryList();
		void LoadNodeSetList();
		void LoadPipelineList();
		void LoadSceneList();
		void LoadScriptList();
		void LoadShaderList();
	public:
		virtual bool LoadResourceXML(const char *strName);
		virtual const std::string &GetResourceXMLFile() const;
		virtual const std::string &GetBinariesPath() const;
		virtual const std::string &GetKeysPath() const;
		virtual const std::string &GetLibrariesPath() const;
		virtual const std::string &GetNodesPath() const;
		virtual const std::string &GetPipelinesPath() const;
		virtual const std::string &GetScenesPath() const;
		virtual const std::string &GetScriptsPath() const;
		virtual const std::string &GetShadersPath() const;
		virtual const std::string &GetBasePath() const;

		virtual const std::vector<BinaryEntry> &GetBinaryList() const;
		virtual const std::vector<KeySetEntry> &GetKeySetList() const;
		virtual const std::vector<LibraryEntry> &GetLibraryList() const;
		virtual const std::vector<NodeSetEntry> &GetNodeSetList() const;
		virtual const std::vector<PipelineEntry> &GetPipelineList() const;
		virtual const std::vector<SceneEntry> &GetSceneList() const;
		virtual const std::vector<ScriptEntry> &GetScriptList() const;
		virtual const std::vector<ShaderEntry> &GetShaderList() const;
		virtual const std::vector<ModuleObjectTypeEntry> &GetModuleObjectTypeList() const;

		virtual const BinaryEntry *GetBinaryEntry(const std::string &name);
		virtual const KeySetEntry *GetKeySetEntry(const std::string &name);
		virtual const LibraryEntry *GetLibraryEntry(const std::string &name);
		virtual const NodeSetEntry *GetNodeSetEntry(const std::string &name);
		virtual const PipelineEntry *GetPipelineEntry(const std::string &name);
		virtual const SceneEntry *GetSceneEntry(const std::string &name);
		virtual const ScriptEntry *GetScriptEntry(const std::string &name);
		virtual const ShaderEntry *GetShaderEntry(const std::string &name);
		virtual const ModuleObjectTypeEntry *GetModuleObjectTypeEntry(
			const std::string module,
			const std::string object_type);

		ResourceXML();
		virtual ~ResourceXML();
	};
}

#endif
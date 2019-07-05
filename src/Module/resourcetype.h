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
#ifndef _RESOURCETYPE_H_INCLUDED
#define _RESOURCETYPE_H_INCLUDED

#include <objecttype.h>
#include "resourcexml.h"
#include <modulemanager.h>

#define			OBJECT_TYPE_RESOURCE							"Resource"

namespace unlike3d {
	class ResourceType {
	public:
		struct NodeEntry {
			std::string name;
			std::string module;
			std::string object_type;
		};
		std::map<std::string, unsigned int> nodes_map_;
		std::vector<NodeEntry> node_list_;
	private:
		static ukObjectTypeInterface *object_type_interface_;
		ResourceXML resource_xml_;
		ModuleManager *module_manager_;
	private:
		static ukFunctionDefineListInterface *InitObjectTypeInterfaceFunctions();
		static ukPropertyDefineListInterface *InitObjectTypeInterfaceProperties();
		static ukObjectDefineListInterface *InitObjectTypeInterfaceObjects();
		bool ParserNodeEntry(tinyxml2::XMLElement *elemnt);
	public:
		virtual bool Init();
		virtual bool LoadResource(const char *strName);

		virtual bool LoadNodeSet(const char *strName);

		virtual void SetModuleManager(ModuleManager *module_manager);
		virtual ModuleHandle LoadModule(const char *module_name);
		virtual void RemoveModule(const char *module_name);
		virtual ObjectType *GetObjectType(const char *module_name, const char* obj_type);
		virtual Object *CreateObject(ObjectType *obj_type);
		virtual Object *CreateObject(const char *module_name, const char *obj_type);
		virtual ukBoolFuncPoint GetFunction(const char *module_name, const char *function_name);
		virtual bool DestroyObject(Object *object);

		virtual bool LoadModuleObjectType(const std::string &module,const std::string &object_type);

		virtual std::string GetBinarySource(const std::string &name);

		virtual std::string GetSceneSource(const std::string &name);
		virtual ObjectType *GetSceneNodeObjectType(const std::string &name);
		virtual Object *CreateSceneNode(const std::string &name);

		virtual std::string GetNodeSetSource(const std::string &name);

		virtual NodeEntry *GetNodeEntry(const std::string &name);
		virtual const ResourceXML::ModuleObjectTypeEntry *GetModuleObjectTypeEntry(
													const std::string &name,
													const std::string &object_type);

		virtual std::string GetPipelineSource(const std::string &name);
		
		static void InitObjectTypeInterface();
		static ukObjectTypeInterface *GetObjectTypeInterface();

		ResourceType();
		virtual ~ResourceType();
	};
}

#endif
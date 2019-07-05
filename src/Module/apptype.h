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
#ifndef _APPTYPE_H_INCLUDED
#define _APPTYPE_H_INCLUDED

#include <modulemanager.h>
#include <objecttype.h>

#define			OBJECT_TYPE_APP							"App"

namespace unlike3d {
	class AppType {
	private:
		static ukObjectTypeInterface *object_type_interface_;
		ObjectType *resource_object_type_;
		ObjectType *viewer_object_type_;
		Object *resource_object_;
		Object *viewer_object_;
		ukObject *native_object_;
		ModuleManager *module_manager_;
		std::string resource_xml_file_;
	private:
		static ukFunctionDefineListInterface *InitObjectTypeInterfaceFunctions();
		static ukPropertyDefineListInterface *InitObjectTypeInterfaceProperties();
		static ukObjectDefineListInterface *InitObjectTypeInterfaceObjects();
		virtual void SetResourceObject();
		virtual void SetViewerObject();
	public:
		virtual bool Init();
		virtual bool Loop();

		Object *GetResource();
		Object *GetViewer();

		virtual void SetModuleManager(ModuleManager *module_manager);

		virtual void SetResourceXMLFile(const char *strName);
		
		virtual void SetNativeObject(ukObject *native_object);
		virtual ukObject *GetNativeObject();

		virtual unsigned int GetThreadCount();
		virtual void AddThreadTask(int thread_id, ukVoidFuncPoint function_point,void *parameter);
		virtual void WaitAllThreadTaskEnd();

		static void InitObjectTypeInterface();
		static ukObjectTypeInterface *GetObjectTypeInterface();

		AppType();
		virtual ~AppType();
	};
}

#endif
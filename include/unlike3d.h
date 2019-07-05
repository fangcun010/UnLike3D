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
#ifndef _UNLIKE3D_H_INCLUDED
#define _UNLIKE3D_H_INCLUDED

#include <appxml.h>
#include <modulemanager.h>
#include <objecttype.h>
#include <script.h>

namespace unlike3d{
    class Engine{
	private:
		AppXML app_xml_;
		Object *app_object_;
		std::vector<Object *> objects_;
		std::string script_file_;
		Script *script_;
		ModuleManager module_manager_;
	public:
        virtual void Init();
        virtual bool LoadApp(const char *strApp);
        virtual bool Run();
        virtual void Destroy();
		virtual ModuleHandle LoadModule(const char *strPath);
		virtual void RemoveModule(const char *strPath);
		virtual ObjectType *GetObjectType(const char *module_path, const char *obj_type);
		virtual Object *CreateObject(ObjectType *obj_type);
		virtual bool DestroyObject(Object *object);
    };
}

#endif
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
#ifndef _MODULEMANAGER_H_INCLUDED
#define _MODULEMANAGER_H_INCLUDED

#include <map>
#include <module.h>
#include <objecttype.h>

namespace unlike3d {
	class ModuleManager {
	private:
		std::map<std::string, ModuleHandle> modules_;
		std::map <std::pair<std::string, std::string>, ukBoolFuncPoint> functions_;
		std::map<std::pair<std::string, std::string>,ObjectType> object_types_;
	public:
		ModuleHandle LoadModule(const char *module_name);
		ukBoolFuncPoint GetFunction(const char *module_name, const char *function_name);
		ObjectType *GetObjectType(const char *module_name, const char *object_type_name);
		Object *CreateObject(ObjectType *obj_type);
		bool IsLoaded(const char *module_name, const char *object_type);
		bool DestroyObject(Object *object);
		void RemoveObjectType(const char *module_path, const char *object_type_name);
		void RemoveModule(const char *module_name);
		void RemoveAllModule();
	};
}

#endif
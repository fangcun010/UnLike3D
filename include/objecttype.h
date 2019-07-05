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
#ifndef _OBJECTTYPE_H_INCLUDED
#define _OBJECTTYPE_H_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <objecttypeinterface.h>
#include <module.h>

namespace unlike3d {
	struct PropertyDefine {
		enum PropertyType {
			NORMAL = 1,
			ENGINE=(1<<1),
			EDITOR=(1<<2),
			USER=(1<<3)
		};
		enum VarType {
			INTEGER, FLOAT, STRING, POINT
		};
		PropertyType type;
		VarType var_type;
		std::string name;
	};
	struct FunctionParameter {
		int integer_value;
		float float_value;
		std::string string_value;
		void *point_value;
	};
	struct FunctionReturn {
		int integer_value;
		float float_value;
		std::string string_value;
		void *point_value;
	};
	struct FunctionParameterDefine {
		enum VarType {
			INTEGER, FLOAT, STRING, POINT
		};
		std::string name;
		VarType type;
	};
	struct FunctionReturnDefine {
		enum VarType {
			INTEGER, FLOAT, STRING, POINT
		};
		std::string name;
		VarType type;
	};
	struct FunctionDefine {
		enum FunctionType {
			NORMAL = 1,
			ENGINE = (1 << 1),
			EDITOR = (1 << 2),
			USER = (1 << 3)
		};
		FunctionType type;
		std::string name;
		std::vector<FunctionParameterDefine> parameter_list;
		std::vector<FunctionReturnDefine> return_list;
		ukVoidFuncObjPointPoint function_point;
	};
	struct ObjectDefine {
		enum ObjectType {
			NORMAL = 1,
			ENGINE = (1 << 1),
			EDITOR = (1 << 2),
			USER = (1 << 3)
		};
		ObjectType type;
		std::string module_name;
		std::string object_type_name;
		std::string name;
	};

	class Object {
	private:
		friend class ObjectType;
		friend class ModuleManager;
		ukObject *native_object_;
		unsigned int object_count_;
		Object **objects_;
		ObjectType *object_type_;
		std::string name_;
	public:
		void SetNativeObject(ukObject *native_object);
		void SetObjectType(ObjectType *object_type);

		void SetName(const char *name);
		const std::string &GetName();

		int GetIntegerProperty(const std::string &name);
		float GetFloatProperty(const std::string &name);
		std::string GetStringProperty(const std::string &name);
		void *GetProperty(const std::string &name);
		
		void SetIntegerProperty(const std::string &name,int value);
		void SetFloatProperty(const std::string &name,float value);
		void SetStringProperty(const std::string &name, const std::string &value);
		void SetProperty(const std::string &name, void *value);

		void CallFunction(const std::string &name, void *parameter_list, void *return_list);
		void CallFunction(const std::string &name, const std::vector<FunctionParameter> &fp,
													std::vector<FunctionReturn> &fr);

		ukVoidFuncObjPointPoint GetFunctionAddress(const std::string &name);
		FunctionDefine *GetFunctionDefine(const std::string &name);
		PropertyDefine *GetPropertyDefine(const std::string &name);
		ObjectDefine *GetObjectDefine(const std::string &name);
		
		const std::vector<std::string> &GetFunctionList();
		const std::vector<std::string> &GetPropertyList();
		const std::vector<std::string> &GetObjectList();

		ukObject *GetNativeObject();
		ObjectType *GetObjectType();
		Object **GetObjects();
	};

	class ObjectType {
	private:
		ukObjectTypeInterface *object_type_interface_;
		ukPointFuncVoid create_object_;
		ukBoolFuncPoint destroy_object_;
		std::string name_;
		std::vector<std::string> functions_;
		std::vector<std::string> properties_;
		std::vector<std::string> objects_;
		std::map<std::string,FunctionDefine> function_defines_;
		std::map<std::string, PropertyDefine> property_defines_;
		std::map<std::string, ObjectDefine> object_defines_;
		ukVoidFuncObjPointPoint set_property_;
		ukPointFuncObjPoint get_property_;
	public:
		friend class Object;
		friend class ModuleManager;
		const std::string &GetName();
		Object *CreateObject();
		bool DestroyObject(Object *object);
		bool DestroyObjectType();
		void LoadFromObjectTypeInterface(ukObjectTypeInterface *obj_type_interface);
		void LoadFromModule(ModuleHandle *handle, const char *obj_type_name);
	};
}

#endif
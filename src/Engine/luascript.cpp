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
#include <luascript.h>
#include <iostream>

#define			ukLUA_PRINT_FUNCTION_NAME				"Print"
#define			ukLUA_PRINTLN_FUNCTION_NAME				"PrintLn"

namespace unlike3d {
	static int ukLuaScriptPrintFunction(lua_State *vm_) {
		unsigned int count = lua_gettop(vm_);
		for (unsigned int i = 0; i < count; i++) {
			switch (lua_type(vm_, i + 1)) {
			case LUA_TBOOLEAN:
				std::cout << lua_toboolean(vm_, i + 1);
				break;
			case LUA_TNUMBER:
				std::cout << lua_tonumber(vm_, i + 1);
				break;
			case LUA_TSTRING:
				std::cout << lua_tostring(vm_, i + 1);
				break;
			case LUA_TNIL:
				std::cout << "nil";
				break;
			case LUA_TTABLE:
				std::cout << "table";
				break;
			case LUA_TFUNCTION:
				std::cout << "function";
				break;
			default:
				std::cout << "unknow!";
				break;
			}
		}
		return 0;
	}

	static int ukLuaScriptPrintLnFunction(lua_State *vm_) {
		unsigned int count = lua_gettop(vm_);
		if (count == 0) {
			std::cout << std::endl;
			return 0;
		}
		for (unsigned int i = 0; i < count; i++) {
			switch (lua_type(vm_, i + 1)) {
			case LUA_TBOOLEAN:
				std::cout << lua_toboolean(vm_, i + 1) << std::endl;
				break;
			case LUA_TNUMBER:
				std::cout << lua_tonumber(vm_, i + 1)<<std::endl;
				break;
			case LUA_TSTRING:
				std::cout << lua_tostring(vm_, i + 1)<<std::endl;
				break;
			case LUA_TNIL:
				std::cout << "nil" << std::endl;
				break;
			case LUA_TTABLE:
				std::cout << "table" << std::endl;
				break;
			case LUA_TFUNCTION:
				std::cout << "function" << std::endl;
				break;
			default:
				std::cout << "unknow!" << std::endl;
				break;
			}
		}
		return 0;
	}

	static int ukLuaScriptFunction(lua_State *vm_) {
		unsigned int parameter_count=lua_gettop(vm_);
		Object *object = (Object *)lua_topointer(vm_, lua_upvalueindex(1));
		std::string name = lua_tostring(vm_, lua_upvalueindex(2));
		FunctionDefine *func_def = (FunctionDefine *)lua_topointer(vm_, lua_upvalueindex(3));
		if (parameter_count != func_def->parameter_list.size()) {
			lua_pushstring(vm_, "error parameter to call function!");
			lua_error(vm_);
		}
		std::vector<FunctionParameter> fp;
		const std::vector<FunctionParameterDefine> &param_list= func_def->parameter_list;
		unsigned int count = param_list.size();
		fp.resize(count);
		for (unsigned int i = 0; i < count; i++) {
			switch (param_list[i].type) {
			case FunctionParameterDefine::INTEGER:
				fp[i].integer_value = (int)lua_tointeger(vm_, i + 1);
				break;
			case FunctionParameterDefine::FLOAT:
				fp[i].float_value = (float)lua_tonumber(vm_, i + 1);
				break;
			case FunctionParameterDefine::STRING:
				fp[i].string_value = lua_tostring(vm_, i + 1);
				break;
			}
		}
		std::vector<FunctionReturn> fr;
		object->CallFunction(name, fp, fr);
		const std::vector<FunctionReturnDefine> &return_list = func_def->return_list;
		count = fr.size();
		for (unsigned int i = 0; i < count; i++) {
			switch (return_list[i].type) {
			case FunctionReturnDefine::INTEGER:
				lua_pushinteger(vm_, fr[i].integer_value);
				break;
			case FunctionReturnDefine::FLOAT:
				lua_pushnumber(vm_, fr[i].float_value);
				break;
			case FunctionReturnDefine::STRING:
				lua_pushstring(vm_, fr[i].string_value.c_str());
				break;
			}
		}
		return count; 
	}
	
	static int ukLuaScriptGetProperty(lua_State *vm_) {
		unsigned int parameter_count = lua_gettop(vm_);
		if (parameter_count != 0) {
			lua_pushstring(vm_, "error parameter for set property!");
			lua_error(vm_);
			return 1;
		}
		Object *object = (Object *)lua_topointer(vm_, lua_upvalueindex(1));
		const std::string name = lua_tostring(vm_, lua_upvalueindex(2));
		PropertyDefine *property_def = (PropertyDefine *)lua_topointer(vm_, lua_upvalueindex(3));
		switch (property_def->var_type) {
		case PropertyDefine::INTEGER:
			lua_pushinteger(vm_,object->GetIntegerProperty(name));
			return 1;
			break;
		case PropertyDefine::FLOAT:
			lua_pushnumber(vm_,object->GetFloatProperty(name));
			return 1;
			break;
		case PropertyDefine::STRING:
			lua_pushstring(vm_, object->GetStringProperty(name).c_str());
			return 1;
			break;
		}
		lua_pushstring(vm_, "error get property!");
		lua_error(vm_);
		return 1;
	}

	static int ukLuaScriptSetProperty(lua_State *vm_) {
		unsigned int parameter_count = lua_gettop(vm_);
		Object *object = (Object *)lua_topointer(vm_, lua_upvalueindex(1));
		std::string name = lua_tostring(vm_, lua_upvalueindex(2));
		PropertyDefine *property_def = (PropertyDefine *)lua_topointer(vm_, lua_upvalueindex(3));
		if (parameter_count!= 1) {
			lua_pushstring(vm_, "error parameter for set property!");
			lua_error(vm_);
			return 1;
		}
		int int_value;
		float float_value;
		std::string str_value;
		switch (property_def->var_type) {
		case PropertyDefine::INTEGER:
			int_value = (int)lua_tointeger(vm_, 1);
			object->SetIntegerProperty(name, int_value);
			return 0;
			break;
		case PropertyDefine::FLOAT:
			float_value = (float)lua_tonumber(vm_, 1);
			object->SetFloatProperty(name, float_value);
			return 0;
			break;
		case PropertyDefine::STRING:
			str_value = lua_tostring(vm_, 1);
			object->SetStringProperty(name, str_value);
			return 0;
			break;
		}
		lua_pushstring(vm_, "error set property!");
		lua_error(vm_);
		return 1;
	}

	void LuaScript::AddObject(Object *object) {
		lua_newtable(vm_);
		const std::vector<std::string> &func_list = object->GetFunctionList();
		unsigned int count = func_list.size();
		for (unsigned int i = 0; i < count; i++) {
			const std::string &name = func_list[i];
			FunctionDefine *func_def = object->GetFunctionDefine(name);
			if ((func_def->type&ukSCRIPT) == 0) continue;
			lua_pushlightuserdata(vm_, object);
			lua_pushstring(vm_, name.c_str());
			lua_pushlightuserdata(vm_, func_def);
			lua_pushcclosure(vm_, ukLuaScriptFunction, 3);
			lua_setfield(vm_, -2, name.c_str());
		}
		const std::vector<std::string> &property_list = object->GetPropertyList();
		count = property_list.size();
		for (unsigned int i = 0; i < count; i++) {
			std::string name = property_list[i];
			std::string get_name = "Get" + name;
			std::string set_name = "Set" + name;
			PropertyDefine *property_def = object->GetPropertyDefine(name);
			if ((property_def->type&ukSCRIPT) == 0) continue;
			lua_pushlightuserdata(vm_, object);
			lua_pushstring(vm_, name.c_str());
			lua_pushlightuserdata(vm_, property_def);
			lua_pushcclosure(vm_, ukLuaScriptGetProperty, 3);
			lua_setfield(vm_, -2, get_name.c_str());
			lua_pushlightuserdata(vm_, object);
			lua_pushstring(vm_, name.c_str());
			lua_pushlightuserdata(vm_, property_def);
			lua_pushcclosure(vm_, ukLuaScriptSetProperty, 3);
			lua_setfield(vm_, -2, set_name.c_str());
		}
		const std::vector<std::string> &object_list = object->GetObjectList();
		count = object_list.size();
		for (unsigned int i = 0; i < count; i++) {
			Object **objs=object->GetObjects();
			AddObject(objs[i]);
			lua_setfield(vm_, -2, objs[i]->GetName().c_str());
		}
	}

	void LuaScript::MakeEnvironment() {
		ScriptInterface &script_interface = GetScriptInterface();
		vm_ = luaL_newstate();
		lua_pushcclosure(vm_, ukLuaScriptPrintFunction, 0);
		lua_setglobal(vm_, ukLUA_PRINT_FUNCTION_NAME);
		lua_pushcclosure(vm_, ukLuaScriptPrintLnFunction, 0);
		lua_setglobal(vm_, ukLUA_PRINTLN_FUNCTION_NAME);
		const std::vector<Object *> &objects = script_interface.GetObjects();
		unsigned int count = objects.size();
		for (unsigned int i = 0; i < count; i++) {
			AddObject(objects[i]);
			lua_setglobal(vm_, objects[i]->GetName().c_str());
		}
	}

	void LuaScript::DestroyEnvironment() {
		lua_close(vm_);
	}

	bool LuaScript::RunScript(const char *script_file) {
		if (luaL_dofile(vm_, script_file)) {
			std::cout << lua_tostring(vm_, -1) << std::endl;
			return false;
		}
		return true;
	}

	LuaScript::~LuaScript() {

	}
}
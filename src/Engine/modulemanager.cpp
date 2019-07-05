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
#include <modulemanager.h>

#define			UK_MODULE_INIT_FUNCTION_NAME				"ukInit"

namespace unlike3d {
	ModuleHandle ModuleManager::LoadModule(const char *module_path) {
		std::string module_name = CrossPlatform::GetFileName(module_path);
		std::map<std::string, ModuleHandle>::iterator itor = modules_.find(module_name);
		if (itor != modules_.end())
			return itor->second;
		ModuleHandle handle = Module::LoadModule(module_path);
		if (handle.handle == nullptr)return handle;
		ukBoolFuncPoint init_func=(ukBoolFuncPoint)Module::GetProcAddress(&handle, UK_MODULE_INIT_FUNCTION_NAME);
		if (init_func == nullptr) {
			handle.handle = nullptr;
			return handle;
		}
		if (init_func(nullptr) == false) {
			handle.handle = nullptr;
			return handle;
		}
		modules_[module_name] = handle;
		return handle;
	}

	bool ModuleManager::IsLoaded(const char *module_name, const char *object_type) {
		if (modules_.find(module_name) == modules_.end())
			return false;
		if (object_types_.find(std::make_pair(module_name, object_type)) == object_types_.end())
			return false;
		return true;
	}

	ukBoolFuncPoint ModuleManager::GetFunction(const char *module_path, const char *function_name) {
		std::string module_name = CrossPlatform::GetFileName(module_path);
		std::pair<std::string, std::string> module_function = std::make_pair(module_name, function_name);
		if (functions_.find(module_function) != functions_.end()) {
			return functions_[module_function];
		}
		ModuleHandle handle = LoadModule(module_path);
		if (handle.handle == nullptr) return nullptr;
		ukBoolFuncPoint &function_point = functions_[module_function];
		function_point = (ukBoolFuncPoint)CrossPlatform::GetProcAddress(handle.handle, function_name);
		return function_point;
	}

	ObjectType *ModuleManager::GetObjectType(const char *module_path, const char *object_type_name) {
		std::string module_name = CrossPlatform::GetFileName(module_path);
		std::pair<std::string, std::string> module_object = std::make_pair(module_name,object_type_name);
		if (object_types_.find(module_object) != object_types_.end()) {
			return &object_types_[module_object];
		}
		ModuleHandle handle = LoadModule(module_path);
		if (handle.handle == nullptr) return nullptr;
		ObjectType &obj_type=object_types_[module_object];
		obj_type.LoadFromModule(&handle, object_type_name);
		return &obj_type;
	}

	Object *ModuleManager::CreateObject(ObjectType *obj_type) {
		Object *obj = obj_type->CreateObject();
		if (obj == nullptr) return nullptr;
		obj->object_count_ = obj_type->object_type_interface_->object_list->count;
		obj->objects_ = new Object *[obj->object_count_];
		obj->native_object_->object_count= obj_type->object_type_interface_->object_list->count;
		obj->native_object_->objects = new ukObject *[obj->object_count_];
		int index = 0;
		for (std::map<std::string, ObjectDefine>::iterator itor = obj_type->object_defines_.begin();
			itor != obj_type->object_defines_.end();itor++) {
			ObjectDefine &def = itor->second;
			ObjectType *sub_obj_type = GetObjectType(def.module_name.c_str(), def.object_type_name.c_str());
			if (sub_obj_type == nullptr) return nullptr;
			Object *sub_obj = CreateObject(sub_obj_type);
			if (sub_obj == nullptr) return nullptr;
			sub_obj->SetName(def.name.c_str());
			obj->objects_[index] = sub_obj;
			obj->native_object_->objects[index] = sub_obj->native_object_;
			index++;
		}
		return obj;
	}

	bool ModuleManager::DestroyObject(Object *object) {
		ObjectType *obj_type=object->GetObjectType();
		return obj_type->DestroyObject(object);
	}

	void ModuleManager::RemoveObjectType(const char *module_path, const char *object_type_name) {
		std::string module_name = CrossPlatform::GetFileName(module_path);
		std::pair<std::string, std::string> module_object = std::make_pair(module_name, object_type_name);
		std::map<std::pair<std::string, std::string>, ObjectType>::iterator itor = object_types_.find(module_object);
		if (itor == object_types_.end()) return;
		ObjectType &obj_type = object_types_[module_object];
		obj_type.DestroyObjectType();
		object_types_.erase(itor);
	}

	void ModuleManager::RemoveModule(const char *module_path) {
		std::string module_name = CrossPlatform::GetFileName(module_path);
		std::map<std::string, ModuleHandle>::iterator itor = modules_.find(module_name);
		if (itor != modules_.end()) {
			Module::DestroyModule(&itor->second);
			modules_.erase(itor);
		}
	}

	void ModuleManager::RemoveAllModule() {
		/*for (std::map<std::pair<std::string, std::string>, ObjectType>::iterator itor = object_types_.begin();
			itor != object_types_.end(); itor++) {
			itor->second.DestroyObjectType();
		}*/
		object_types_.clear();
		for (std::map<std::string, ModuleHandle>::iterator itor = modules_.begin();
				itor != modules_.end(); itor++) {
			Module::DestroyModule(&itor->second);
		}
		modules_.clear();
	}
}

#undef UK_MODULE_INIT_FUNCTION_NAME
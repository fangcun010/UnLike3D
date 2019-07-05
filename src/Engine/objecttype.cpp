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
#include <objecttype.h>

namespace unlike3d{
	void Object::SetName(const char *name) {
		name_ = name;
	}

	const std::string &Object::GetName() {
		return name_;
	}

	void Object::SetNativeObject(ukObject *native_object) {
		native_object_ = native_object;
	}

	void Object::SetObjectType(ObjectType *object_type) {
		object_type_ = object_type;
	}

	int Object::GetIntegerProperty(const std::string &name) {
		ukPointFuncObjPoint get_property = object_type_->get_property_;
		int ret = *(int *)get_property(native_object_, (void *)name.c_str());
		return ret;
	}

	float Object::GetFloatProperty(const std::string &name) {
		ukPointFuncObjPoint get_property = object_type_->get_property_;
		float ret = *(float *)get_property(native_object_, (void *)name.c_str());
		return ret;
	}

	std::string Object::GetStringProperty(const std::string &name) {
		ukPointFuncObjPoint get_property = object_type_->get_property_;
		std::string ret=(const char *)get_property(native_object_, (void *)name.c_str());
		return ret;
	}

	void *Object::GetProperty(const std::string &name) {
		ukPointFuncObjPoint get_property = object_type_->get_property_;
		return get_property(native_object_, (void *)name.c_str());
	}

	void Object::SetIntegerProperty(const std::string &name, int value) {
		ukVoidFuncObjPointPoint set_property = object_type_->set_property_;
		set_property(native_object_, (void *)name.c_str(), &value);
	}

	void Object::SetFloatProperty(const std::string &name, float value) {
		ukVoidFuncObjPointPoint set_property = object_type_->set_property_;
		set_property(native_object_, (void *)name.c_str(), &value);
	}

	void Object::SetStringProperty(const std::string &name, const std::string &value) {
		ukVoidFuncObjPointPoint set_property = object_type_->set_property_;
		set_property(native_object_,(void *)name.c_str(), (void *)value.c_str());
	}

	void Object::SetProperty(const std::string &name, void *value) {
		ukVoidFuncObjPointPoint set_property = object_type_->set_property_;
		set_property(native_object_, (void *)name.c_str(), value);
	}

	void Object::CallFunction(const std::string &name, void *parameter_list, void *return_list) {
		FunctionDefine *func_def = GetFunctionDefine(name);
		ukVoidFuncObjPointPoint func = func_def->function_point;
		func(native_object_, parameter_list, return_list);
	}

	void Object::CallFunction(const std::string &name, const std::vector<FunctionParameter> &fp,
		std::vector<FunctionReturn> &fr) {
		FunctionDefine *func_def = GetFunctionDefine(name);
		ukVoidFuncObjPointPoint func = func_def->function_point;
		std::vector<FunctionParameterDefine> &param_def = func_def->parameter_list;
		ukFunctionParameterListInterface *param_list = ukMakeFunctionParameterListInterface(fp.size());
		for (unsigned int i = 0; i < fp.size(); i++) {
			FunctionParameterDefine &def = param_def[i];
			ukVarInterface &var=param_list->parameter_point[i];
			const FunctionParameter &param = fp[i];
			var.type = (ukVarInterface::VarType)def.type;
			switch (def.type) {
			case FunctionParameterDefine::INTEGER:
				var.integer_value = param.integer_value;
				break;
			case FunctionParameterDefine::FLOAT:
				var.float_value = param.float_value;
				break;
			case FunctionParameterDefine::STRING:
				ukStringCopy(&var.string_value, param.string_value.c_str());
				break;
			case FunctionParameterDefine::POINT:
				var.point_value = param.point_value;
				break;
			}
		}
		ukFunctionReturnListInterface *return_list = ukMakeFunctionReturnListInterface(func_def->return_list.size());
		func(native_object_, param_list, return_list);
		fr.resize(return_list->count);
		std::vector<FunctionReturnDefine> &return_def = func_def->return_list;
		for (unsigned int i = 0; i < return_list->count; i++) {
			FunctionReturnDefine &def = return_def[i];
			ukVarInterface &var = return_list->return_point[i];
			FunctionReturn &return_var = fr[i];
			switch (def.type) {
			case FunctionReturnDefine::INTEGER:
				return_var.integer_value = var.integer_value;
				break;
			case FunctionReturnDefine::FLOAT:
				return_var.float_value = var.float_value;
				break;
			case FunctionReturnDefine::STRING:
				return_var.string_value = var.string_value.buffer;
				break;
			case FunctionReturnDefine::POINT:
				return_var.point_value = var.point_value;
				break;
			}
		}
		ukDestroyFunctionParameterListInterface(param_list);
		ukDestroyFunctionReturnListInterface(return_list);
	}

	ukVoidFuncObjPointPoint Object::GetFunctionAddress(const std::string &name) {
		return object_type_->function_defines_[name].function_point;
	}

	FunctionDefine *Object::GetFunctionDefine(const std::string &name) {
		return &object_type_->function_defines_[name];
	}

	PropertyDefine *Object::GetPropertyDefine(const std::string &name) {
		return &object_type_->property_defines_[name];
	}

	ObjectDefine *Object::GetObjectDefine(const std::string &name) {
		return &object_type_->object_defines_[name];
	}

	const std::vector<std::string> &Object::GetFunctionList() {
		return object_type_->functions_;
	}

	const std::vector<std::string> &Object::GetPropertyList() {
		return object_type_->properties_;
	}

	const std::vector<std::string> &Object::GetObjectList() {
		return object_type_->objects_;
	}

	ukObject *Object::GetNativeObject() {
		return native_object_;
	}

	ObjectType *Object::GetObjectType() {
		return object_type_;
	}

	Object **Object::GetObjects() {
		return objects_;
	}

	const std::string &ObjectType::GetName() {
		return name_;
	}

	Object *ObjectType::CreateObject() {
		ukObject *native_object=(ukObject *)create_object_();
		if (native_object == nullptr) return nullptr;
		Object *object = new Object();
		if (object == nullptr) {
			destroy_object_(native_object);
			return nullptr;
		}
		native_object->object_count = 0;
		native_object->objects = nullptr;
		native_object->object_type_interface = object_type_interface_;
		object->native_object_ = native_object;
		object->object_type_ = this;
		object->object_count_ = 0;
		native_object->data2.point = object;
		return object;
	}

	bool ObjectType::DestroyObject(Object *object) {
		for (unsigned int i = 0; i < object->object_count_; i++) {
			ObjectType *obj_type = object->objects_[i]->GetObjectType();
			if (obj_type == nullptr) return false;
			if (obj_type->DestroyObject(object->objects_[i]) == false)
				return false;
		}
		if(object->object_count_) delete []object->objects_;
		if (object->native_object_->object_count) delete[]object->native_object_->objects;
		bool ret = destroy_object_(object->native_object_);
		delete object;
		return ret;
	}

	bool ObjectType::DestroyObjectType() {
		return ukDestroyObjectTypeInterface(object_type_interface_);
	}

	void ObjectType::LoadFromObjectTypeInterface(ukObjectTypeInterface *obj_type_interface) {
		object_type_interface_ = obj_type_interface;
		create_object_ = obj_type_interface->create_object;
		destroy_object_ = obj_type_interface->destroy_object;
		set_property_ = obj_type_interface->set_property;
		get_property_ = obj_type_interface->get_property;
		name_ = obj_type_interface->name;
		ukFunctionDefineInterface *funcs = obj_type_interface->function_list->function_point;
		functions_.clear();
		function_defines_.clear();
		for (unsigned int i = 0; i < obj_type_interface->function_list->count; i++) {
			functions_.push_back(funcs[i].name);
			FunctionDefine &func_def = function_defines_[funcs[i].name];
			func_def.name = funcs[i].name;
			func_def.type = (FunctionDefine::FunctionType)funcs[i].type;
			func_def.parameter_list.resize(funcs[i].parameter_list->count);
			ukFunctionParameterDefineInterface *param_def_interface = funcs[i].parameter_list->parameter_point;
			for (unsigned int j = 0; j < funcs[i].parameter_list->count; j++) {
				FunctionParameterDefine &param_def = func_def.parameter_list[j];
				param_def.name = param_def_interface[j].name;
				param_def.type = (FunctionParameterDefine::VarType)param_def_interface[j].type;
			}
			func_def.return_list.resize(funcs[i].return_list->count);
			ukFunctionReturnDefineInterface *return_def_interface = funcs[i].return_list->return_point;
			for (unsigned int j = 0; j < funcs[i].return_list->count; j++) {
				FunctionReturnDefine &return_def = func_def.return_list[j];
				return_def.name = return_def_interface[j].name;
				return_def.type = (FunctionReturnDefine::VarType)return_def_interface[j].type;
			}
			func_def.function_point = funcs[i].function_point;
		}
		properties_.clear();
		property_defines_.clear();
		ukPropertyDefineInterface *properties = obj_type_interface->property_list->property_point;
		for (unsigned int i = 0; i < obj_type_interface->property_list->count;i++) {
			properties_.push_back(properties[i].name);
			PropertyDefine &prop_def = property_defines_[properties[i].name];
			prop_def.name = properties[i].name;
			prop_def.type = (PropertyDefine::PropertyType)properties[i].type;
			prop_def.var_type = (PropertyDefine::VarType)properties[i].var_type;
		}
		objects_.clear();
		object_defines_.clear();
		ukObjectDefineInterface *objects = obj_type_interface->object_list->object_point;
		for (unsigned int i = 0; i < obj_type_interface->object_list->count; i++) {
			objects_.push_back(objects[i].name);
			ObjectDefine &obj_def = object_defines_[objects[i].name];
			obj_def.name = objects[i].name;
			obj_def.type = (ObjectDefine::ObjectType)objects[i].type;
			obj_def.module_name = objects[i].module_name;
			obj_def.object_type_name = objects[i].object_type_name;
		}
	}

	void ObjectType::LoadFromModule(ModuleHandle *handle, const char *obj_type_name) {
		ukObjectTypeInterface *obj_type_interface = Module::GetObjectType(handle, obj_type_name);
		LoadFromObjectTypeInterface(obj_type_interface);
	}
}
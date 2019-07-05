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
#include <string.h>
#include <objecttypeinterface.h>

#define DLL_EXPORT extern "C" __declspec(dllexport)

DLL_EXPORT ukFunctionParameterListInterface *ukMakeFunctionParameterListInterface(unsigned int count) {
	ukFunctionParameterListInterface *list = new ukFunctionParameterListInterface();
	if (list == nullptr) return nullptr;
	list->count = count;
	list->parameter_point = new ukVarInterface[count];
	memset(list->parameter_point, 0, sizeof(ukVarInterface)*count);
	return list;
}

DLL_EXPORT ukFunctionReturnListInterface *ukMakeFunctionReturnListInterface(unsigned int count) {
	ukFunctionReturnListInterface *list = new ukFunctionReturnListInterface();
	if (list == nullptr) return nullptr;
	list->count = count;
	list->return_point = new ukVarInterface[count];
	memset(list->return_point, 0, sizeof(ukVarInterface)*count);
	return list;
}

DLL_EXPORT bool ukDestroyFunctionParameterListInterface(ukFunctionParameterListInterface *list) {
	for (unsigned int i = 0; i < list->count; i++) {
		ukVarInterface &var = list->parameter_point[i];
		if (var.type == ukVarInterface::VarType::STRING) {
			if (var.string_value.buffer)
				delete []var.string_value.buffer;
		}
	}
	delete []list->parameter_point;
	delete list;
	return true;
}

DLL_EXPORT bool ukDestroyFunctionReturnListInterface(ukFunctionReturnListInterface *list) {
	for (unsigned int i = 0; i < list->count; i++) {
		ukVarInterface &var = list->return_point[i];
		if (var.type == ukVarInterface::VarType::STRING) {
			if (var.string_value.buffer)
				delete[]var.string_value.buffer;
		}
	}
	delete[]list->return_point;
	delete list;
	return true;
}

DLL_EXPORT ukPropertyDefineListInterface *ukMakePropertyDefineListInterface(unsigned int count) {
	ukPropertyDefineListInterface *list = new ukPropertyDefineListInterface();
	if (list == nullptr) return nullptr;
	list->count = count;
	if(list->count)
		list->property_point = new ukPropertyDefineInterface[count];
	return list;
}

DLL_EXPORT ukFunctionDefineListInterface *ukMakeFunctionDefineListInterface(unsigned int count) {
	ukFunctionDefineListInterface *list = new ukFunctionDefineListInterface();
	if (list == nullptr) return nullptr;
	list->count = count;
	if(list->count)
		list->function_point = new ukFunctionDefineInterface[count];
	return list;
}

DLL_EXPORT ukObjectDefineListInterface *ukMakeObjectDefineListInterface(unsigned int count) {
	ukObjectDefineListInterface *list = new ukObjectDefineListInterface();
	if (list == nullptr) return nullptr;
	list->count = count;
	list->object_point = new ukObjectDefineInterface[count];
	return list;
}

DLL_EXPORT bool ukDestroyPropertyDefineListInterface(ukPropertyDefineListInterface *property_list) {
	if(property_list->count)
		delete []property_list->property_point;
	delete property_list;
	return true;
}

DLL_EXPORT bool ukDestroyFunctionDefineListInterface(ukFunctionDefineListInterface *function_list) {
	for (unsigned int i = 0; i < function_list->count; i++) {
		ukFunctionDefineInterface &func_def = function_list->function_point[i];
		ukDestroyFunctionParameterDefineListInterface(func_def.parameter_list);
		ukDestroyFunctionReturnDefineListInterface(func_def.return_list);
	}
	if(function_list->count)
		delete []function_list->function_point;
	delete function_list->function_point;
	return true;
}

DLL_EXPORT bool ukDestroyObjectDefineListInterface(ukObjectDefineListInterface *object_list) {
	if(object_list->count)
		delete[]object_list->object_point;
	delete object_list;
	return true;
}

DLL_EXPORT ukFunctionParameterDefineListInterface *ukMakeFunctionParameterDefineListInterface(unsigned int count) {
	ukFunctionParameterDefineListInterface *list = new ukFunctionParameterDefineListInterface();
	if (list == nullptr) return nullptr;
	list->count = count;
	if(list->count)
		list->parameter_point = new ukFunctionParameterDefineInterface[count];
	return list;
}

DLL_EXPORT ukFunctionReturnDefineListInterface *ukMakeFunctionReturnDefineListInterface(unsigned int count) {
	ukFunctionReturnDefineListInterface *list = new ukFunctionReturnDefineListInterface[count];
	if (list == nullptr) return nullptr;
	list->count = count;
	if(list->count)
		list->return_point = new ukFunctionReturnDefineInterface[count];
	return list;
}

DLL_EXPORT bool ukDestroyFunctionParameterDefineListInterface(ukFunctionParameterDefineListInterface *list) {
	if(list->count)
		delete []list->parameter_point;
	delete list;
	return true;
}

DLL_EXPORT bool ukDestroyFunctionReturnDefineListInterface(ukFunctionReturnDefineListInterface *list) {
	if(list->count)
		delete[]list->return_point;
	delete list;
	return true;
}

DLL_EXPORT void ukSetFunctionReturnDefineInterface(ukFunctionReturnDefineListInterface *list,
	unsigned int index,
	const char *name, ukVarType type){
	list->return_point[index].name = name;
	list->return_point[index].type = type;
}

DLL_EXPORT void ukSetFunctionParameterDefineInterface(ukFunctionParameterDefineListInterface *list,
	unsigned int index,
	const char *name, ukVarType type) {
	list->parameter_point[index].name = name;
	list->parameter_point[index].type = type;
}

DLL_EXPORT void ukSetFunctionDefineInterface(ukFunctionDefineListInterface *function_list, unsigned int index,
	const char *name,ukType type,
	ukFunctionParameterDefineListInterface *parameter_list,
	ukFunctionReturnDefineListInterface *return_list,
	ukVoidFuncObjPointPoint function_point) {
	function_list->function_point[index].type = type;
	function_list->function_point[index].name = name;
	function_list->function_point[index].parameter_list=parameter_list;
	function_list->function_point[index].return_list = return_list;
	function_list->function_point[index].function_point = function_point;
}

DLL_EXPORT void ukSetPropertyDefineInterface(ukPropertyDefineListInterface *property_list, unsigned int index,
	const char *name,
	ukType type,
	ukVarType var_type) {
	ukPropertyDefineInterface &def = property_list->property_point[index];
	def.name = name;
	def.type = type;
	def.var_type = var_type;
}

DLL_EXPORT void ukSetObjectDefineInterface(ukObjectDefineListInterface *object_list, unsigned int index,
	const char *name,
	ukType type,
	const char *module_name,
	const char *object_type_name) {
	ukObjectDefineInterface &def = object_list->object_point[index];
	def.type = type;
	def.name = name;
	def.module_name = module_name;
	def.object_type_name = object_type_name;
}

DLL_EXPORT ukObjectTypeInterface *ukMakeObjectTypeInterface(const char *name,ukPointFuncVoid create_object,
	ukBoolFuncPoint destroy_object,
	ukPointFuncObjPoint get_property,
	ukVoidFuncObjPointPoint set_property,
	ukFunctionDefineListInterface *function_list,
	ukPropertyDefineListInterface *property_list,
	ukObjectDefineListInterface *object_list) {
	ukObjectTypeInterface *obj_type_interface = new ukObjectTypeInterface();

	if (obj_type_interface == nullptr) return nullptr;

	obj_type_interface->name = name;
	obj_type_interface->create_object = create_object;
	obj_type_interface->destroy_object = destroy_object;
	obj_type_interface->get_property = get_property;
	obj_type_interface->set_property = set_property;
	obj_type_interface->function_list =function_list;
	obj_type_interface->property_list = property_list;
	obj_type_interface->object_list = object_list;

	return obj_type_interface;
}

DLL_EXPORT bool ukDestroyObjectTypeInterface(ukObjectTypeInterface *obj_type_interface) {
	ukDestroyFunctionDefineListInterface(obj_type_interface->function_list);
	ukDestroyPropertyDefineListInterface(obj_type_interface->property_list);
	ukDestroyObjectDefineListInterface(obj_type_interface->object_list);
	delete obj_type_interface;
	return true;
}

DLL_EXPORT void ukStringCopy(ukStringInterface *string, const char *value) {
	unsigned int len = strlen(value);
	if(string->buffer_size<=len){
		delete []string->buffer;
		string->buffer = new char[len+1];
		string->buffer_size = len + 1;
	}
	memcpy(string->buffer, value, len);
	string->buffer[len] = '\0';
	string->length = len;
}

#undef DLL_EXPORT
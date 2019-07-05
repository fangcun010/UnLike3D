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
#ifndef _OBJECTTYPEINTERFACE_H_INCLUDED
#define _OBJECTTYPEINTERFACE_H_INCLUDED

#define DLL_EXPORT extern "C" __declspec(dllexport)

typedef void(*ukVoidFuncPoint) (void *);
typedef void *(*ukPointFuncObjPoint) (void *, void *);
typedef void (*ukVoidFuncObjPointPoint)(void *, void *, void *);
typedef void *(*ukPointFuncVoid) ();
typedef void *(*ukPointFuncPoint)(void *);
typedef bool (*ukBoolFuncPoint) (void *);
typedef bool(*ukBoolFuncPointPoint) (void *, void *);
typedef int (*ukIntFuncPointPoint) (void *,void *);
typedef int(*ukIntFuncPointPointPoint) (void *, void *, void *);
typedef void *(*ukPointFuncPointPoint) (void *, void *);
typedef bool(*ukBoolFunc3Point) (void *, void *, void *);
typedef void(*ukVoidFunc5Point) (void *, void *, void *, void *, void *);

enum ukType {
	ukNORMAL = 1,
	ukENGINE = (1 << 1),
	ukEDITOR = (1 << 2),
	ukSCRIPT=(1<<3),
	ukUSER = (1 << 4)
};

enum ukVarType {
	ukINTEGER, ukFLOAT, ukSTRING,ukPOINT
};

struct ukStringInterface {
	unsigned int length;
	unsigned int buffer_size;
	char *buffer;
};

struct ukVarInterface {
	enum VarType {
		INTEGER, FLOAT, STRING,POINT
	};
	VarType type;
	struct {
		int integer_value;
		float float_value;
		ukStringInterface string_value;
		void *point_value;
	};
};

struct ukFunctionParameterDefineInterface {
	const char *name;
	ukVarType type;
};

struct ukFunctionParameterDefineListInterface {
	unsigned int count;
	ukFunctionParameterDefineInterface *parameter_point;
};

struct ukFunctionReturnDefineInterface {
	const char *name;
	ukVarType type;
};

struct ukFunctionReturnDefineListInterface {
	unsigned int count;
	ukFunctionReturnDefineInterface *return_point;
};

struct ukFunctionDefineInterface {
	ukType type;
	const char *name;
	ukFunctionParameterDefineListInterface *parameter_list;
	ukFunctionReturnDefineListInterface *return_list;
	ukVoidFuncObjPointPoint function_point;
};

struct ukFunctionDefineListInterface{
	unsigned int count;
	ukFunctionDefineInterface *function_point;
};

struct ukFunctionParameterListInterface {
	unsigned int count;
	ukVarInterface *parameter_point;
};

struct ukFunctionReturnListInterface {
	unsigned int count;
	ukVarInterface *return_point;
};

struct ukPropertyDefineInterface {
	ukType type;
	const char *name;
	ukVarType var_type;
};

struct ukPropertyDefineListInterface {
	unsigned int count;
	ukPropertyDefineInterface *property_point;
};

struct ukObjectTypeInterface;

struct ukObjectDefineInterface {
	ukType type;
	const char *name;
	const char *module_name;
	const char *object_type_name;
};

struct ukObjectDefineListInterface {
	unsigned int count;
	ukObjectDefineInterface *object_point;
};

struct ukObjectData {
	void *point;
};

struct ukObject {
	unsigned int object_count;
	ukObject **objects;
	ukObjectTypeInterface *object_type_interface;
	ukObjectData data1;
	ukObjectData data2;
	ukObjectData data3;
	ukObjectData data4;
	ukObjectData data5;
};

struct ukObjectTypeInterface {
	const char *name;
	ukPointFuncObjPoint get_property;
	ukVoidFuncObjPointPoint set_property;
	ukPointFuncVoid create_object;
	ukBoolFuncPoint destroy_object;
	ukFunctionDefineListInterface *function_list;
	ukPropertyDefineListInterface *property_list;
	ukObjectDefineListInterface *object_list;
};

struct ukSceneNode;

struct ukSceneNodeList {
	unsigned int count;
	ukSceneNode **list;
};

struct ukSceneNode {
	unsigned int handled;
	bool dirty;
	bool new_thread;
	ukSceneNodeList children;
	ukSceneNode *parent;
	ukIntFuncPointPoint in_frustum;
	ukPointFuncPointPoint get_children;
	ukPointFuncPoint get_parent;
	ukPointFuncPoint mark_dirty;
	ukPointFuncPoint get_boundingbox;
	ukObjectData data1;
	ukObjectData data2;
	ukObjectData data3;
	ukObjectData data4;
	ukObjectData data5;
};

DLL_EXPORT ukFunctionParameterListInterface *ukMakeFunctionParameterListInterface(unsigned int count);
DLL_EXPORT ukFunctionReturnListInterface *ukMakeFunctionReturnListInterface(unsigned int count);

DLL_EXPORT ukPropertyDefineListInterface *ukMakePropertyDefineListInterface(unsigned int count);
DLL_EXPORT ukFunctionDefineListInterface *ukMakeFunctionDefineListInterface(unsigned int count);
DLL_EXPORT ukObjectDefineListInterface *ukMakeObjectDefineListInterface(unsigned int count);

DLL_EXPORT void ukSetPropertyDefineInterface(ukPropertyDefineListInterface *property_list, unsigned int index,
														const char *name,
														ukType type,
														ukVarType var_type);
DLL_EXPORT void ukSetObjectDefineInterface(ukObjectDefineListInterface *object_list, unsigned int index,
														const char *name,
														ukType type,
														const char *module_name,
														const char *object_type_name);

DLL_EXPORT bool ukDestroyPropertyDefineListInterface(ukPropertyDefineListInterface *property_list);
DLL_EXPORT bool ukDestroyFunctionDefineListInterface(ukFunctionDefineListInterface *function_list);
DLL_EXPORT bool ukDestroyObjectDefineListInterface(ukObjectDefineListInterface *object_list);

DLL_EXPORT ukFunctionParameterDefineListInterface *ukMakeFunctionParameterDefineListInterface(unsigned int count);
DLL_EXPORT ukFunctionReturnDefineListInterface *ukMakeFunctionReturnDefineListInterface(unsigned int count);

DLL_EXPORT bool ukDestroyFunctionParameterDefineListInterface(ukFunctionParameterDefineListInterface *list);
DLL_EXPORT bool ukDestroyFunctionReturnDefineListInterface(ukFunctionReturnDefineListInterface *list);

DLL_EXPORT void ukSetFunctionParameterDefineInterface(ukFunctionParameterDefineListInterface *parameter_list, unsigned int index,
														const char *name,ukVarType type);
DLL_EXPORT void ukSetFunctionReturnDefineInterface(ukFunctionReturnDefineListInterface *return_list,unsigned int index,
														const char *name, ukVarType type);

DLL_EXPORT void ukSetFunctionDefineInterface(ukFunctionDefineListInterface *function_list, unsigned int index,
	const char *name,
	ukType type,
	ukFunctionParameterDefineListInterface *parameter_list,
	ukFunctionReturnDefineListInterface *return_list,
	ukVoidFuncObjPointPoint function_point);

DLL_EXPORT ukFunctionParameterListInterface *ukMakeFunctionParameterListInterface(unsigned int count);
DLL_EXPORT ukFunctionReturnListInterface *ukMakeFunctionReturnListInterface(unsigned int count);

DLL_EXPORT bool ukDestroyFunctionParameterListInterface(ukFunctionParameterListInterface *list);
DLL_EXPORT bool ukDestroyFunctionReturnListInterface(ukFunctionReturnListInterface *list);

DLL_EXPORT ukObjectTypeInterface *ukMakeObjectTypeInterface(const char *name, ukPointFuncVoid create_object,
	ukBoolFuncPoint destroy_object,
	ukPointFuncObjPoint get_property,
	ukVoidFuncObjPointPoint set_property,
	ukFunctionDefineListInterface *function_list,
	ukPropertyDefineListInterface *property_list,
	ukObjectDefineListInterface *object_list);

DLL_EXPORT bool ukDestroyObjectTypeInterface(ukObjectTypeInterface *obj_type_interface);

DLL_EXPORT void ukStringCopy(ukStringInterface *string, const char *value);

#undef DLL_EXPORT

#endif
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
#include "boxnodetype.h"
#include "boxnode.h"

#include <iostream>

namespace unlike3d {
	ukObjectTypeInterface *BoxNodeType::object_type_interface_ = nullptr;

	bool BoxNodeType::Init() {
		return true;
	}


	ukFunctionDefineListInterface *BoxNodeType::InitObjectTypeInterfaceFunctions() {
		ukFunctionDefineListInterface *function_list = ukMakeFunctionDefineListInterface(1);
		ukFunctionReturnDefineListInterface *return_list=ukMakeFunctionReturnDefineListInterface(1);
		ukSetFunctionReturnDefineInterface(return_list, 0, "result", ukINTEGER);
		ukSetFunctionDefineInterface(function_list, 0, "Init",ukSCRIPT,
			ukMakeFunctionParameterDefineListInterface(0),
			return_list,
			BoxNodeTypeInitFunction);
		return function_list;
	}

	ukPropertyDefineListInterface *BoxNodeType::InitObjectTypeInterfaceProperties() {
		ukPropertyDefineListInterface *property_list = ukMakePropertyDefineListInterface(0);
		return property_list;
	}

	ukObjectDefineListInterface *BoxNodeType::InitObjectTypeInterfaceObjects() {
		ukObjectDefineListInterface *object_list = ukMakeObjectDefineListInterface(0);
		return object_list;
	}

	void BoxNodeType::InitObjectTypeInterface() {
		ukFunctionDefineListInterface *func_list = InitObjectTypeInterfaceFunctions();
		ukPropertyDefineListInterface *prop_list = InitObjectTypeInterfaceProperties();
		ukObjectDefineListInterface *obj_list = InitObjectTypeInterfaceObjects();
		object_type_interface_ = ukMakeObjectTypeInterface(OBJECT_TYPE_BOXNODE, BoxNodeTypeCreateObject,
			BoxNodeTypeDestroyObject,
			BoxNodeTypeGetProperty,
			BoxNodeTypeSetProperty,
			func_list,
			prop_list,
			obj_list);
	}

	ukObjectTypeInterface *BoxNodeType::GetObjectTypeInterface() {
		if (object_type_interface_ == nullptr) {
			InitObjectTypeInterface();
		}
		return object_type_interface_;
	}

	BoxNodeType::BoxNodeType() {

	}

	BoxNodeType::~BoxNodeType() {

	}
}

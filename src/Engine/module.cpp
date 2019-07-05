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
#include <module.h>

#define			ukMODULE_GET_OBJECT_TYPE_NAME				"ukGetObjectType"

namespace unlike3d {
	ModuleHandle Module::LoadModule(const char *strModule) {
		ModuleHandle handle;
		handle.handle=CrossPlatform::LoadLibrary(strModule);
		return handle;
	}
	void *Module::GetProcAddress(ModuleHandle *handle, const char *strName) {
		return CrossPlatform::GetProcAddress(handle->handle, strName);
	}

	ukObjectTypeInterface *Module::GetObjectType(ModuleHandle *handle,
											const char *strName) {
		ukPointFuncPoint get_obj_type_function = (ukPointFuncPoint)CrossPlatform::GetProcAddress(
														handle->handle,ukMODULE_GET_OBJECT_TYPE_NAME);
		if (get_obj_type_function == nullptr) return nullptr;
		return (ukObjectTypeInterface *)get_obj_type_function((void *)strName);
	}

	bool Module::DestroyModule(ModuleHandle *handle) {
		return CrossPlatform::DestroyLibrary(handle->handle);
	}
}

#undef ukMODULE_GET_OBJECT_TYPE_NAME
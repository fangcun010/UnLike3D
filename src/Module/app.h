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
#ifndef _APP_H_INCLUDED
#define _APP_H_INCLUDED

#include <objecttypeinterface.h>

#define DLL_EXPORT extern "C" __declspec(dllexport)

DLL_EXPORT void *AppTypeCreateObject();

DLL_EXPORT bool AppTypeDestroyObject(void *object);

DLL_EXPORT void *AppTypeGetProperty(void *object, void *name);

DLL_EXPORT void AppTypeSetProperty(void *object, void *name, void *value);

DLL_EXPORT void AppTypeInitFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void AppTypeSetResourceXMLFileFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void AppTypeSetModuleManagerFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void AppTypeLoopFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void AppTypeGetThreadCountFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void AppTypeAddThreadTaskFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void AppTypeWaitAllThreadTaskEndFunction(void *object, void *parameter_list, void *return_list);

void AppTypeInit();

DLL_EXPORT bool ukInit(void *data);

DLL_EXPORT ukObjectTypeInterface *ukGetObjectType(const char *strName);

#undef DLL_EXPORT

#endif
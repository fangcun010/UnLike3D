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
#ifndef _VIEWER_H_INCLUDED
#define _VIEWER_H_INCLUDED

#include <objecttypeinterface.h>

#define DLL_EXPORT extern "C" __declspec(dllexport)

DLL_EXPORT void *ViewerTypeCreateObject();

DLL_EXPORT bool ViewerTypeDestroyObject(void *object);

DLL_EXPORT void *ViewerTypeGetProperty(void *object, void *name);

DLL_EXPORT void ViewerTypeSetProperty(void *object, void *name, void *value);

DLL_EXPORT void ViewerTypeLoadSceneFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void ViewerTypeSetResourceFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void ViewerTypeSetApplicationFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void ViewerTypeGetThreadCountFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void ViewerTypeAddThreadTaskFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void ViewerTypeWaitAllThreadTaskEndFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void ViewerTypeInitThreadPoolFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void ViewerTypeInitThreadPoolWithRecommendThreadCountFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void ViewerTypeLoadPipelineFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void ViewerTypeInitPipelineFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void ViewerTypeDestroyPipelineFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void ViewerTypeLookFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void ViewerTypeLoopFunction(void *object, void *parameter_list, void *return_list);

DLL_EXPORT void ViewerTypeInitFunction(void *object, void *parameter_list, void *return_list);

void ViewerTypeInit();

DLL_EXPORT bool ukInit(void *data);

DLL_EXPORT ukObjectTypeInterface *ukGetObjectType(const char *strName);

#undef DLL_EXPORT

#endif
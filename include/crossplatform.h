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
#ifndef _CROSSPLATFORM_H_INCLUDED
#define _CROSSPLATFORM_H_INCLUDED

#include <string>

#ifdef _MSC_VER

#include <windows.h>

#else

#error unsupport platform!

#endif

namespace unlike3d {
	class CrossPlatform {
	public:
		static HINSTANCE LoadLibrary(const char *strLibrary);
		static FARPROC GetProcAddress(HINSTANCE hInstance, const char *strName);
		static bool DestroyLibrary(HINSTANCE hInstance);
		static void CrossPlatform::SetCurrentDir(const char *strDir);
		static std::string CrossPlatform::GetCurrentDir();
		static std::string GetFileDir(const char *strFileName);
		static std::string GetFileName(const char *strFileName);
	};
}

#endif
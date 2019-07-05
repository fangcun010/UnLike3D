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
#include <crossplatform.h>

#define			ukMAX_PATH_LENGTH				256

namespace unlike3d {
	HINSTANCE CrossPlatform::LoadLibrary(const char *strLibrary) {
		return ::LoadLibrary(strLibrary);
	}

	FARPROC CrossPlatform::GetProcAddress(HINSTANCE hInstance, const char *strName) {
		return ::GetProcAddress(hInstance, strName);
	}

	bool CrossPlatform::DestroyLibrary(HINSTANCE hInstance) {
		return ::FreeLibrary(hInstance);
	}

	void CrossPlatform::SetCurrentDir(const char *strDir) {
		::SetCurrentDirectory(strDir);
	}

	std::string CrossPlatform::GetCurrentDir() {
		std::string ret;
		char path[ukMAX_PATH_LENGTH];
		if (GetCurrentDirectory(ukMAX_PATH_LENGTH,path)) {
			ret = path;
		}
		return ret;
	}

	std::string CrossPlatform::GetFileName(const char *strFilePath) {
		std::string ret;
		int len = strlen(strFilePath);
		ret = "";
		int spos = len - 1;
		while (spos>=0 && strFilePath[spos]!='\\' && strFilePath[spos]!='/') spos--;
		int epos = len - 1;
		while (epos >= 0 && strFilePath[epos] != '.') epos--;
		if(epos<0)
			ret = &strFilePath[spos + 1];
		else {
			ret = "";
			for (int i = spos + 1; i < epos; i++)
				ret += strFilePath[i];
		}
		return ret;
	}

	std::string CrossPlatform::GetFileDir(const char *strFileName) {
		std::string strFileDir;
		int len = strlen(strFileName);
		int end_pos = len-1;
		while (strFileName[end_pos]!='\\' && strFileName[end_pos]!='/') end_pos--;
		strFileDir = "";
		for (int i = 0; i <= end_pos; i++) strFileDir += strFileName[i];
		return strFileDir;
	}
}
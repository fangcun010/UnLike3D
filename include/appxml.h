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
#ifndef _APPXML_H_INCLUDED
#define _APPXML_H_INCLUDED

#include <vector>
#include <iostream>

namespace unlike3d {
	class AppXML {
	public:
		struct ModuleLoadEntry {
			std::string object_name;
			std::string module_name;
			std::string object_type;
		};
		std::vector<AppXML::ModuleLoadEntry> module_load_entries_;
	private:
		std::string app_xml_file_;
		std::string resource_xml_file_;
		std::string libraries_path_;
		std::string script_;
	public:
		AppXML();
		virtual bool LoadAppXML(const char *strName);
		virtual const std::string &GetAppXMLFile() const;
		virtual const std::string &GetLibrariesPath() const;
		virtual const std::string &GetResourceXMLFile() const;
		virtual const std::string &GetScriptName() const;
		virtual const std::vector<ModuleLoadEntry> &GetModuleLoadEntries() const;
		virtual ~AppXML();
	};
}

#endif
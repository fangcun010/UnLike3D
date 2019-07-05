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
#include <unlike3d.h>
#include <luascript.h>

namespace unlike3d {

	void Engine::Init() {
		script_ = new LuaScript();
	}

	bool Engine::LoadApp(const char *strApp) {
		app_xml_.LoadAppXML(strApp);
		std::string app_xml_dir = CrossPlatform::GetFileDir(strApp);
		std::string module_dir = app_xml_dir + app_xml_.GetLibrariesPath()+"/";
		const std::vector<AppXML::ModuleLoadEntry> &entries = app_xml_.GetModuleLoadEntries();
		for (unsigned int i = 0; i < entries.size(); i++) {
			const AppXML::ModuleLoadEntry &entry = entries[i];
			std::string module_path = module_dir + entry.module_name + UNLIKE3D_MODULE_SUFFIX;
			if (entry.object_type == "")
				LoadModule(module_path.c_str());
			else {
				ObjectType *obj_type=GetObjectType(module_path.c_str(), entry.object_type.c_str());
				if (entry.object_name == "") continue;
				Object *obj = CreateObject(obj_type);
				obj->SetName(entry.object_name.c_str());
				if (entry.object_name == "app") app_object_ = obj;
				objects_.push_back(obj);
			}
		}

		std::vector<FunctionParameter> fp;
		std::vector<FunctionReturn> fr;
		fp.resize(1);
		fp[0].string_value = app_xml_dir + app_xml_.GetResourceXMLFile();
		app_object_->CallFunction("SetResourceXMLFile", fp, fr);
		fp.resize(1);
		fp[0].point_value = &module_manager_;
		app_object_->CallFunction("SetModuleManager", fp, fr);
		script_file_ = app_xml_dir + app_xml_.GetScriptName();

		return true;
	}

	bool Engine::Run() {
		for (unsigned int i = 0; i < objects_.size(); i++)
			script_->GetScriptInterface().AddObject(objects_[i]);
		script_->MakeEnvironment();
		return script_->RunScript(script_file_.c_str());
	}

	void Engine::Destroy() {
		script_->DestroyEnvironment();
		for (unsigned int i = 0; i < objects_.size(); i++)
			DestroyObject(objects_[i]);
		objects_.clear();
		module_manager_.RemoveAllModule();
		delete script_;
	}

	ObjectType *Engine::GetObjectType(const char *module_path, const char *obj_type) {
		return module_manager_.GetObjectType(module_path, obj_type);
	}

	Object *Engine::CreateObject(ObjectType *obj_type) {
		return module_manager_.CreateObject(obj_type);
	}

	bool Engine::DestroyObject(Object *object) {
		ObjectType *obj_type= object->GetObjectType();
		return obj_type->DestroyObject(object);
	}

	ModuleHandle Engine::LoadModule(const char *strPath) {
		return module_manager_.LoadModule(strPath);
	}

	void Engine::RemoveModule(const char *strPath) {
		module_manager_.RemoveModule(strPath);
	}
}
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
#ifndef _SCENEXML_H_INCLUDED
#define _SCENEXML_H_INCLUDED

#include <string>

#include <tinyxml2.h>

#include <objecttype.h>

namespace unlike3d {
	class SceneXML {
	private:
		Object *resource_object_;
	private:
		void ParserSceneNodeEntry(tinyxml2::XMLElement *element,Object *parent_obj,ukSceneNodeList *node_list,
			ukSceneNode *parent);
	public:
		virtual ukSceneNodeList *LoadXMLFile(const std::string &name);

		virtual void SetResource(Object *resource_object);
		virtual Object *GetResource();

		SceneXML();
		virtual ~SceneXML();
	};
}

#endif
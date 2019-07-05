#include<iostream>

#include <unlike3d.h>

int main() {
	std::string app_xml_file;
	unlike3d::Engine engine;


	engine.Init();
#ifdef UNLIKE3D_ENV_DIRECTORY
	app_xml_file = UNLIKE3D_ENV_DIRECTORY;
#endif
	app_xml_file += "app.xml";

	engine.LoadApp(app_xml_file.c_str());
	if (!engine.Run()) {
		std::cout << "Error!" << std::endl;
	}
	engine.Destroy();

	return 0;
}
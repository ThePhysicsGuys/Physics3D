#include "core.h"

#include "saveDialog.h"

#include <iostream>

#include "serialization.h"

#include <Windows.h>
//#define GLFW_EXPOSE_NATIVE_WIN32
//#include "GLFW/glfw3native.h"

namespace Application {
void saveWorld(const PlayerWorld& world) {
	//glfwGetWin32Window(window);

	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "new.world";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
	//ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "World Files (*.world)\0*.world\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = "txt";

	if(GetSaveFileName(&ofn)) {
		world.syncReadOnlyOperation([szFileName, &world]() {
			WorldImportExport::saveWorld(szFileName, world);
		});
	}
}



};
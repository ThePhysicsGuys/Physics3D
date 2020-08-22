#include "core.h"

#include "saveDialog.h"

#include <iostream>
#include <stddef.h>
#include <string.h>

#include "serialization.h"

namespace P3D::Application {

static void saveWorld(const char* file, const PlayerWorld& world) {
	Log::info("Saving world to: %s", file);
	world.syncReadOnlyOperation([file, &world]() {
		WorldImportExport::saveWorld(file, world);
	});
}
static void openWorld(const char* file, PlayerWorld& world) {
	Log::info("Opening world: %s", file);
	world.syncModification([file, &world]() {
		std::vector<ExtendedPart*> partsToDelete;
		partsToDelete.reserve(world.getPartCount());
		for(ExtendedPart& p : world.iterParts()) {
			partsToDelete.push_back(&p);
		}
		world.clear();

		for(ExtendedPart* p : partsToDelete) {
			delete p;
		}

		WorldImportExport::loadWorld(file, world);
	});
}

#ifdef _WIN32
#include <Windows.h>

void saveWorld(const PlayerWorld& world) {
	//glfwGetWin32Window(window);

	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "new.world";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	//ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "World Files (*.world)\0*.world\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = "world";

	if(GetSaveFileName(&ofn)) {
		saveWorld(szFileName, world);
	}
}
void openWorld(PlayerWorld& world) {
	//glfwGetWin32Window(window);

	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	//ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "World Files (*.world)\0*.world\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofn.lpstrDefExt = "world";

	if(GetOpenFileName(&ofn)) {
		openWorld(szFileName, world);
	}
}
#else
static void stripFinalNewlines(char* fileName) {
	std::size_t lastChar = strlen(fileName) - 1;
	while(isspace(fileName[lastChar])){
		fileName[lastChar] = '\0';
		lastChar--;
	}
}
void saveWorld(const PlayerWorld& world) {
	FILE* fileDialog = popen("zenity --file-selection --save --confirm-overwrite --file-filter=*.world  2>/dev/null", "r");
	if(fileDialog != NULL) {
		char fileName[1024];
		fgets(fileName, sizeof(fileName), fileDialog);
		if(pclose(fileDialog) == 0) { // OK save file
			stripFinalNewlines(fileName);
			saveWorld(fileName, world);
		}
	} else {
		std::cout << "Save *.world to> ";
		std::string fileName;
		std::cin >> fileName;

		saveWorld(fileName.c_str(), world);
	}
}
void openWorld(PlayerWorld& world) {
	FILE* fileDialog = popen("zenity --file-selection --file-filter=*.world  2>/dev/null", "r");
	if(fileDialog != NULL) {
		char fileName[1024];
		fgets(fileName, sizeof(fileName), fileDialog);
		if(pclose(fileDialog) == 0) { // OK save file
			stripFinalNewlines(fileName);
			openWorld(fileName, world);
		}
	} else {
		std::cout << "Input world to open> ";
		std::string fileName;
		std::cin >> fileName;

		openWorld(fileName.c_str(), world);
	}
}
#endif
};

#include "core.h"

#include "saveDialog.h"

#include <iostream>
#include <stddef.h>
#include <string.h>

namespace P3D::Application {
#ifdef _WIN32
#include <Windows.h>

bool saveWorldDialog(char* worldFilePath) {
	//glfwGetWin32Window(window);

	OPENFILENAME ofn;
	strcpy(worldFilePath, "new.world");

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	//ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "World Files (*.world)\0*.world\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = worldFilePath;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = "world";

	return GetSaveFileName(&ofn); // saves the filename to input parameter worldFilePath, returns true if user confirmed
}
bool openWorldDialog(char* worldFilePath) {
	//glfwGetWin32Window(window);

	OPENFILENAME ofn;
	strcpy(worldFilePath, "");

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	//ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "World Files (*.world)\0*.world\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = worldFilePath;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofn.lpstrDefExt = "world";

	return GetOpenFileName(&ofn); // saves the filename to input parameter worldFilePath, returns true if user confirmed
}
#else
static void stripFinalNewlines(char* fileName) {
	std::size_t lastChar = strlen(fileName) - 1;
	while(isspace(fileName[lastChar])){
		fileName[lastChar] = '\0';
		lastChar--;
	}
}
bool saveWorldDialog(char* worldFilePath) {
	FILE* fileDialog = popen("zenity --file-selection --save --confirm-overwrite --file-filter=*.world  2>/dev/null", "r");
	if(fileDialog != NULL) {
		if(!fgets(worldFilePath, MAX_PATH_LENGTH, fileDialog)) throw "fgets error!";
		if(pclose(fileDialog) == 0) { // OK save file
			stripFinalNewlines(worldFilePath);
			return true;
		} else {
			return false;
		}
	} else {
		std::cout << "Save *.world to> ";
		std::string fileName;
		std::cin >> fileName;

		if(fileName.empty()) return false;
		strcpy(worldFilePath, fileName.c_str());
		return true;
	}
}
bool openWorldDialog(char* worldFilePath) {
	FILE* fileDialog = popen("zenity --file-selection --file-filter=*.world  2>/dev/null", "r");
	if(fileDialog != NULL) {
		if(!fgets(worldFilePath, MAX_PATH_LENGTH, fileDialog)) throw "fgets error!";
		if(pclose(fileDialog) == 0) { // OK save file
			stripFinalNewlines(worldFilePath);
			return true;
		} else {
			return false;
		}
	} else {
		std::cout << "Input world to open> ";
		std::string fileName;
		std::cin >> fileName;

		if(fileName.empty()) return false;
		strcpy(worldFilePath, fileName.c_str());
		return true;
	}
}
#endif
};

#pragma once

#ifndef HEADER_FILE_H
#define HEADER_FILE_H

#include <map>
#include <fstream>
#include "FileBrowser.h"
#include "Rendering.h"
#include <imgui.h>

namespace EngineUI {

	enum BrowserFocus {
		FileBrowser,
		FolderBrowser
	};
	
	void renderFolderTree(std::string parentPath, bool clickCaptured);
	std::map<std::string, bool> loopUI(bool& applicationOpen, Rendering* renderer, float &cameraSpeed, float &fov, bool& firstMouse);
	void initUI(Rendering* renderer);
	bool getIsNavigatingViewport();
	void openDirectory(std::string path, BrowserFocus focus);
	void prepareNewInspectorImage(std::string imagePath, Rendering* renderer);
	void switchNewInsectorImage(Rendering* renderer);
	void openPathInFileExplorer(std::string path);
	static int fileSearchCallback(ImGuiInputTextCallbackData* data);
	void renderLog();
	void showRightClickDirectoryOptions(FileBrowser::DirectoryItem item);
	float estimateTextWidth(const char* text);
      
}

#endif
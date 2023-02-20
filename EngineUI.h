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
	std::map<std::string, bool> loopUI(bool& applicationOpen, Rendering* renderer);
	void initUI(Rendering* renderer);
	void openDirectory(std::string path, BrowserFocus focus);
	void prepareNewInspectorImage(std::string imagePath, Rendering* renderer);
	void switchNewInsectorImage(Rendering* renderer);
	void openPathInFileExplorer(std::string path);
	static int fileSearchCallback(ImGuiInputTextCallbackData* data);

	void showRightClickDirectoryOptions(FileBrowser::DirectoryItem item);
      
}

#endif
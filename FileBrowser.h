#pragma once

#include <vector>
#include <fstream>
#include <filesystem>

namespace FileBrowser {

    struct DirectoryItem {
        std::string id;
        std::string name;
        std::string path;
        std::string parentPath;
        bool isFolder;
    };
    
    std::vector<DirectoryItem> getGameFiles(const std::string& pathToGameFiles);
    void printGameFiles(std::vector<DirectoryItem> gameFiles);
    int LaunchBlenderWithFBX(std::string fbxPath);

    bool renameFolder(std::string& path, const std::string& newName);

    std::string getParentDirectory(std::string path);

    bool deleteFileOrFolder(std::wstring path);

    bool createDirectory(const std::string& path);
   

}
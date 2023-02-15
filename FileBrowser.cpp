#include "FileBrowser.h"
#include <iostream>
#include <filesystem>

namespace FileBrowser {


    void createFolderTree(const std::string& path, Folder* folder) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_directory()) {
                auto childFolder = new Folder;
                childFolder->name = entry.path().filename().string();
                createFolderTree(entry.path().string(), childFolder);
                folder->children.push_back(childFolder);
            }
        }
    }

    std::string getFolderTree(Folder* folder, int depth = 0) {
        std::string result;
        if (folder != nullptr) {
            for (int i = 0; i < depth; i++) {
                result += "  ";
            }
            result += "+ " + folder->name + "\n";
            for (auto child : folder->children) {
                result += getFolderTree(child, depth + 1);
            }
        }
        return result;
    }

    int logRootFiles() {
        std::string rootPath = std::filesystem::current_path().string() + "\\content";;
        auto rootFolder = new Folder;
        rootFolder->name = rootPath;
        createFolderTree(rootPath, rootFolder);
        //printFolderTree(rootFolder);
        return 0;
    }

    Folder* getRootFolder() {
        std::string rootPath = std::filesystem::current_path().string() + "\\content";;
        auto rootFolder = new Folder;
        rootFolder->name = rootPath;
        createFolderTree(rootPath, rootFolder);
        return rootFolder;
    }




}
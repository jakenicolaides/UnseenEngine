#pragma once

#include <vector>
#include <fstream>
#include <filesystem>

namespace FileBrowser {

    struct Folder {
        std::string name;
        std::vector<Folder*> children;
    };

    struct Folder;

    void createFolderTree(const std::string& path, Folder* folder);

    std::string getFolderTree(Folder* folder, int depth );

    int logRootFiles();

    Folder* getRootFolder();

}
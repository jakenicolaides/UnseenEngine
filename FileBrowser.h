#pragma once

#include <vector>
#include <fstream>
#include <filesystem>

namespace FileBrowser {

    struct DirectoryData
    {
        std::string name;
        std::string type;
        bool isFile;
    };


    std::vector<DirectoryData> getDirectoryData(bool getCurrentDirectory, std::string directory, bool getFiles, bool getFolders);
    void logCurrentDirectory();

}
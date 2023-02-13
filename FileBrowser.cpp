#include "FileBrowser.h"
#include <iostream>

namespace FileBrowser {

    std::string currentDirectory = std::filesystem::current_path().string() + "\\content";

    std::vector<DirectoryData> getDirectoryData(bool getCurrentDirectory, std::string directory, bool getFiles, bool getFolders)
    {
        if (getCurrentDirectory) {
            directory = currentDirectory;
        }

        std::vector<DirectoryData> fileInformation;
        for (const auto& entry : std::filesystem::directory_iterator(directory))
        {
            if (entry.is_regular_file() && getFiles)
            {
                DirectoryData fileInfo;
                fileInfo.name = entry.path().filename().string();
                fileInfo.type = entry.path().extension().string();
                fileInfo.isFile = true;
                fileInformation.push_back(fileInfo);
            }
            else if (entry.is_directory() && getFolders)
            {
                DirectoryData fileInfo;
                fileInfo.name = entry.path().filename().string();
                fileInfo.type = "";
                fileInfo.isFile = false;
                fileInformation.push_back(fileInfo);
            }
        }
        return fileInformation;
    }

    void logCurrentDirectory() {

        
        std::vector<DirectoryData> FileInformation = getDirectoryData(true, "", true, true);

        std::cout << "Name\t\tType\t\n";
        for (int i = 0; i < FileInformation.size(); i++)
        {
            std::cout << FileInformation[i].name << "\t\t" << FileInformation[i].type << std::endl;
        }
        

    }

}
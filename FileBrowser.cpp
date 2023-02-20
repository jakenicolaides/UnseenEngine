#include "FileBrowser.h"
#include "Functions.h"
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <windows.h>
#include <vector>
#include <map>
#include <cstdlib>


namespace FileBrowser {


    std::vector<DirectoryItem> getGameFiles(const std::string& pathToGameFiles) {
        std::vector<DirectoryItem> gameFiles;

        //Add the root folder
        DirectoryItem root = {};
        root.id = "root"; // arbitrary ID for the root folder
        root.name = "Content";
        root.path = std::filesystem::current_path().string() + "\\content";
        root.parentPath = "";
        root.isFolder = true;

        gameFiles.push_back(root);

        // Create a new instance of recursive_directory_iterator
        for (const auto& entry : std::filesystem::recursive_directory_iterator(pathToGameFiles)) {
            DirectoryItem item = {};
            item.name = entry.path().filename().string();
            item.path = entry.path().string();
            item.parentPath = entry.path().parent_path().string();
            item.isFolder = entry.is_directory();

            // Get a unique ID for the file/folder using Windows API function GetFileInformationByHandle
            if (!entry.is_directory()) {
                HANDLE fileHandle = CreateFile(
                    entry.path().c_str(),
                    GENERIC_READ,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL
                );
                if (fileHandle != INVALID_HANDLE_VALUE) {
                    BY_HANDLE_FILE_INFORMATION fileInfo = {};
                    if (GetFileInformationByHandle(fileHandle, &fileInfo)) {
                        item.id = std::to_string(fileInfo.nFileIndexLow) + "_" + std::to_string(fileInfo.nFileIndexHigh);
                    }
                    CloseHandle(fileHandle);
                }
            }
            else {
                item.id = std::to_string(std::hash<std::string>{}(item.path));
            }

            gameFiles.push_back(item);
        }

        return gameFiles;
    }

    

    void printGameFiles(std::vector<DirectoryItem> gameFiles) {
        
        for (DirectoryItem& item : gameFiles) {
            std::cout << "Id: " << item.id << std::endl;
            std::cout << "Name: " << item.name << std::endl;
            std::cout << "Path: " << item.path << std::endl;
            std::cout << "Parent Path: " << item.parentPath<< std::endl;
            std::cout << "Is Folder: " << item.isFolder << std::endl;
            std::cout << " " << std::endl;
        }
    }

    int LaunchBlenderWithFBX(std::string fbxPath)
    {
        // Add the path to the Blender executable to the PATH environment variable
        std::string blenderPath = "C:\\Program Files\\Blender Foundation\\Blender 3.4";
        char* oldValue;
        size_t len;
        _dupenv_s(&oldValue, &len, "PATH");
        std::string pathEnvVar = blenderPath + ";" + oldValue;
        LPCWSTR widePath = Functions::ConvertToLPCWSTR(pathEnvVar.c_str());
        SetEnvironmentVariable(L"PATH", widePath);
        free(oldValue);

        // Construct the command string with the updated FBX path
        std::string command = "blender --python \"C:\\UnseenEngine\\auto_open.py\" -- \"" + fbxPath + "\"";
        LPWSTR const n = Functions::ConvertToLPWSTR(command.c_str());
        // Launch Blender in a new process
        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi;
       
        if (!CreateProcess(NULL,n, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            // Failed to create process
            return -1;
        }
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);

        return 0;
    }

    bool renameFolder(std::string& path, const std::string& newName) {


        // Construct the old and new folder paths using the folder ID and new name
        std::wstring oldFolderPath = Functions::ConvertToLPWSTR(path);
        std::string newPathString = getParentDirectory(path) + "\\" + newName;
        std::wstring newFolderPath = Functions::ConvertToLPWSTR(newPathString);

        // Rename the folder using the Windows API
        if (MoveFileEx(oldFolderPath.c_str(), newFolderPath.c_str(), MOVEFILE_REPLACE_EXISTING)) {
            return true;
        }

        // Handle errors
        DWORD error = GetLastError();
        LPWSTR buffer = nullptr;
        size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&buffer, 0, nullptr);
        if (size > 0) {
            std::wcerr << L"Error " << error << L": " << buffer << std::endl;
            LocalFree(buffer);
        }
        else {
            std::wcerr << L"Unknown error occurred." << std::endl;
        }

        return false;


    }

    std::string getParentDirectory(std::string path) {
        size_t pos = path.find_last_of("\\/");
        if (pos != std::string::npos) {
            return path.substr(0, pos);
        }
        return "";
    }

    bool deleteFileOrFolder(std::wstring path)
    {
        DWORD attributes = GetFileAttributesW(path.c_str());

        if (attributes == INVALID_FILE_ATTRIBUTES)
        {
            std::cerr << "Error: could not get file attributes for path " << std::string(path.begin(), path.end()) << std::endl;
            return false;
        }

        if (attributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // Delete all files and subfolders within the folder
            std::wstring searchPath = path + L"\\*";
            WIN32_FIND_DATAW findData;
            HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findData);
            if (hFind != INVALID_HANDLE_VALUE)
            {
                do
                {
                    if (wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0)
                    {
                        std::wstring filePath = path + L"\\" + findData.cFileName;
                        if (!deleteFileOrFolder(filePath))
                        {
                            FindClose(hFind);
                            return false;
                        }
                    }
                } while (FindNextFileW(hFind, &findData));

                FindClose(hFind);
            }

            // Delete the empty folder
            if (RemoveDirectoryW(path.c_str()))
            {
                return true;
            }
            else
            {
                std::cerr << "Error: could not remove directory at path " << std::string(path.begin(), path.end()) << std::endl;
                return false;
            }
        }
        else
        {
            if (DeleteFileW(path.c_str()))
            {
                return true;
            }
            else
            {
                std::cerr << "Error: could not delete file at path " << std::string(path.begin(), path.end()) << std::endl;
                return false;
            }
        }
    }


    bool createDirectory(const std::string& path)
    {
        if (CreateDirectory(Functions::ConvertToLPCWSTR(path.c_str()), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
        {
            return true;
        }
        else
        {
            DWORD errorCode = GetLastError();
            std::cout << "Error creating directory: " << path << std::endl;
            LPSTR messageBuffer = nullptr;
            size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
            std::string message(messageBuffer, size);
            std::cout << "Error code: " << errorCode << " - " << message << std::endl;
            LocalFree(messageBuffer);
            return false;
        }
    }

}
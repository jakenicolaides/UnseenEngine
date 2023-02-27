#include "Functions.h"
#include "Debug.h"
#include <algorithm>
#include <Windows.h>
#include <iomanip>

namespace Functions {
  
    std::string EscapeBackslashes(const std::string& path)
    {
        std::string escapedPath;
        for (char c : path) { if (c == '\\'){escapedPath += "\\\\";} else {escapedPath += c; }}
        return escapedPath;
    }

    LPCWSTR ConvertToLPCWSTR(const char* narrowString) {
        int length = MultiByteToWideChar(CP_UTF8, 0, narrowString, -1, NULL, 0);
        LPWSTR wideString = new WCHAR[length];
        MultiByteToWideChar(CP_UTF8, 0, narrowString, -1, wideString, length);
        return wideString;
    }

    LPWSTR ConvertToLPWSTR(const std::string& str) {
        int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
        LPWSTR buffer = new WCHAR[size];
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, size);
        return buffer;
    }

    std::string getFilePathExtension(std::string imagePath) {  return imagePath.substr(imagePath.find_last_of(".") + 1); }


    bool isSubstring(const std::string& mainstring, const std::string& substring, bool caseSensitive) {
        if (caseSensitive) {
            return mainstring.find(substring) != std::string::npos;
        }
        else {
            auto it = std::search(
                mainstring.begin(), mainstring.end(),
                substring.begin(), substring.end(),
                [](char a, char b) { return std::toupper(a) == std::toupper(b); }
            );
            return it != mainstring.end();
        }
    }

    std::string insertNewlines(const std::string& str, int line_length) {
        std::string result;
        for (int i = 0; i < str.length(); i += line_length) {
            result += str.substr(i, line_length) + "\n";
        }
        return result;
    }

    int countNewlines(std::string str) {
        int count = 0;
        for (char c : str) {
            if (c == '\n') {
                count++;
            }
        }
        return count;
    }

    std::string getFileType(std::string filePath) {


        std::string ext = getFilePathExtension(filePath);


        //returns the file type based on the extension
        if (ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "bmp" || ext == "gif") {
            return "image";
        }
        else if (ext == "fbx" || ext == "obj" || ext == "glb" || ext == "gltf") {
            return "model";
        }
        else if (ext == "mp4" || ext == "avi" || ext == "mov" || ext == "wmv" || ext == "flv" || ext == "mkv") {
			return "video";
		}
        else if (ext == "mp3" || ext == "wav" || ext == "flac" || ext == "aac" || ext == "ogg") {
			return "audio";
		}
        else if (ext == "txt" || ext == "doc" || ext == "docx" || ext == "pdf" || ext == "rtf" || ext == "odt") {
			return "document";
		}
        else if (ext == "zip" || ext == "rar" || ext == "7z" || ext == "tar" || ext == "gz" || ext == "xz") {
			return "archive";
		}
        else if (ext == "exe" || ext == "msi" || ext == "bat" || ext == "cmd" || ext == "sh" || ext == "py") {
			return "executable";
		}
        else if(ext == filePath){
			return "folder";
        }
        else {
            return "unknown";
        }

    }

    std::string getFileNameFromPath(const std::string& path)
    {
        // Find the last position of the directory separator character
        size_t lastSlashPos = path.find_last_of("/\\");

        // If no directory separator character is found, return the full path
        if (lastSlashPos == std::string::npos)
            return path;

        // Return the substring starting from the position after the last directory separator character
        return path.substr(lastSlashPos + 1);
    }

    std::string convertBytes(long long bytes, char choice) {

        double result;
        std::string unit;

        switch (choice) {
        case 'K':
            result = bytes / 1024.0;
            break;
        case 'M':
            result = bytes / (1024.0 * 1024.0);
            break;
        case 'G':
            result = bytes / (1024.0 * 1024.0 * 1024.0);
            break;
        case 'T':
            result = bytes / (1024.0 * 1024.0 * 1024.0 * 1024.0);
            break;
        }
        return std::to_string(result);

    }
}


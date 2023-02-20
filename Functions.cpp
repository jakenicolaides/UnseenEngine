#include "Functions.h"
#include <algorithm>
#include <Windows.h>

namespace Functions {
  
    std::string EscapeBackslashes(const std::string& path)
    {
        std::string escapedPath;
        for (char c : path)
        {
            if (c == '\\')
            {
                escapedPath += "\\\\";
            }
            else
            {
                escapedPath += c;
            }
        }
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

   

    
}


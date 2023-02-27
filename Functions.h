#pragma once
#include <Windows.h>
#include <iostream>
#include <typeinfo>

namespace Functions {

     std::string EscapeBackslashes(const std::string& path);
     LPCWSTR ConvertToLPCWSTR(const char* narrowString);
     LPWSTR ConvertToLPWSTR(const std::string & str);
     std::string getFilePathExtension(std::string imagePath);
     bool isSubstring(const std::string& mainstring, const std::string& substring, bool caseSensitive = false);

     std::string insertNewlines(const std::string& str, int line_length);

     int countNewlines(std::string str);

     std::string getFileType(std::string filePath);

     std::string getFileNameFromPath(const std::string& path);

     std::string convertBytes(long long bytes, char choice);
         
}
#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <typeinfo>

namespace Functions {

     std::string EscapeBackslashes(const std::string& path);
     LPCWSTR ConvertToLPCWSTR(const char* narrowString);
     LPWSTR ConvertToLPWSTR(const std::string & str);
     std::string getFilePathExtension(std::string imagePath);
     bool isSubstring(const std::string& mainstring, const std::string& substring, bool caseSensitive = false);

}
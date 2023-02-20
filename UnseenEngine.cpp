/*
* 
Engine Setup Instructions:

 - Download the repo and instal to the root of a hard disk volume
 - Download and install Vulkan to the root of the same volume
 - Current dependancies for opening files: Blender 3.4, Paint.Net (but you can change these if you like)
 - Hook up the paths to the exes for an dependant programs
 - In Blender: set splash screen to off
 - In Blender: delete default cube etc the go File > Defaults > Save Startup File (this is so that you don't get them appearing when opening 3d files in blender)



C++ naming conventions:

Structs, Classes, and Enumerated Types: These are named using CamelCase, starting with an uppercase letter. For example: struct EmployeeRecord
Member Functions and Variables: Member functions and variables in a class are named using camelCase, starting with a lowercase letter.
Global Variables: Global variables are named using camelCase, starting with a lowercase letter and with a suffix "_". For example: int globalVariable_;
Functions: Functions are named using CamelCase, starting with a lowercase letter. For example: void processData();
Constants: Constants are named using all uppercase letters, with words separated by underscores. For example: const int MAX_RECORDS = 100;

*/


#include "UnseenEngine.h"
#include "Rendering.h"
#include "FileBrowser.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

bool engineInitialised_ = false;

int main() {
    Rendering renderer;

    try {
        renderer.start();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


void engineLoop() {

}

void initEngine() {

    engineInitialised_ = true;
    std::cout << "Engine Initialised" << std::endl;
  
}
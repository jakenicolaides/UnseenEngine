#include "UnseenEngine.h"
#include "Rendering.h"
#include "FileBrowser.h"
#include "Debug.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

bool engineInitialised_ = false;

int main() {

    Rendering renderer;
    Entities entities;

    entities.id[0] = 29;

    debug.log(std::to_string(entities.id[0]));

    try {
        renderer.start(entities);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


void engineLoop() {
    //debug.log("loop");
}

void initEngine() {

    engineInitialised_ = true;
  
}
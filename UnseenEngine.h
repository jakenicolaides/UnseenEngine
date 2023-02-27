#pragma once

#include <vector>
#include <fstream>

const int maxNumberOfEntities = 100;

void engineLoop();
void initEngine();

struct Entities {
    int id[maxNumberOfEntities];
};
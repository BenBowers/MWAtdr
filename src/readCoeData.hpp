#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
//Reading the coeficent data function will only take a file name and return an intager array to the caller of the function.
std::vector<int16_t> readCoeData(char* fileName);
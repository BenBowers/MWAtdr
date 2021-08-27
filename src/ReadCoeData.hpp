#pragma once
#include <vector>
#include <complex>
#include <string>

//This Function will throw the following exceptions std::ios::failure 

//Reading the coeficent data function will only take a file name and return an intager array to the caller of the function.
std::vector<std::complex<float>> readCoeData(std::string fileName);
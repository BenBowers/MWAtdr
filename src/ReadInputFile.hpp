#pragma once

#include <vector>
#include <complex>
#include <string>
//take s a file name will read that file remove the data that is applicable for this run of the program and output a set containing the data
//file name shoud be observation start time _ signal start time
std::vector<std::complex<float>> readInputDataFile(std::string fileName,int antenaInput);

// takes in the file name this is a string will check to see if it is a standered pre defined size if this is found to be correct the function will return true if not it will return false
bool validateInputData(std::string fileName);

//Exception that will be thrown by readinputdatafile
class ReadInputDataException :public std::runtime_error {
public:
   ReadInputDataException(const std::string& msg) : std::runtime_error(msg) {}
};
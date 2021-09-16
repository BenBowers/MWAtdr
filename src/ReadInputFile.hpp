#pragma once

#include <vector>
#include <complex>
#include <string>
//take s a file name will read that file remove the data that is applicable for this run of the program and output a set containing the data
std::vector<std::vector<std::complex<float>>> readInputDataFile(std::string fileName,int antenaInput, char pol);

// takes in the file name this is a string will check to see if it is a standered pre defined size if this is found to be correct the function will return true if not it will return false
bool validateInputData(std::string fileName);

//Exception that will be thrown by readinputdatafile
class ReadInputDataException :public std::exception {
public:
   ReadInputDataException(const std::string& msg) : msg_(msg) {}
  ~ReadInputDataException() {}

   std::string getMessage() const {return(msg_);}
private:
   std::string msg_;
};
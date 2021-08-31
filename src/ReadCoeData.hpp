#pragma once
#include <vector>
#include <complex>
#include <string>

// This function will throw the following readCoeDataException upon error

// Reading the coefficient data function will only take a file name and return an integer array to the caller of the function.
std::vector<std::complex<float>> readCoeData(std::string fileName);

class readCoeDataException {
public:
   readCoeDataException(const std::string& msg) : msg_(msg) {}
  ~readCoeDataException() {}

   std::string getMessage() const {return(msg_);}
private:
   std::string msg_;
};

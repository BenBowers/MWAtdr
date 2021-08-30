#pragma once
#include <vector>
#include <complex>
#include <string>

//This Function will throw the following readCoeDataException upon error

//Reading the coeficent data function will only take a file name and return an intager array to the caller of the function.
std::vector<std::complex<float>> readCoeData(std::string fileName);

class readCoeDataException : public std::exception{
    public:
        const char * what() const throw(){
            return "Reading Coefficient data Error";
        }
};
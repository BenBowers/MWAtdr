#include "ReadCoeData.hpp"
#include <exception>
#include <iostream>
#include <fstream>
#include <vector>
#include <complex>
#include <array>
#include <string>
#include <cstdint>

//note c++ dose not allow for the return of an array from a function as such vecotrs will be used for simpicity.

//main function for reading in the coeficent data  will return a vector of those coeficents.

//takes a file name as its only input and will return an intager array of the coeficents
std::vector<std::complex<float>> readCoeData(std::string fileName){
    //final vector that will contain all of the coeficent data
    std::vector<std::complex<float>> result;
    //opening the file
    std::ifstream infile;
    infile.open(fileName);
    if(!infile){
        throw std::system_error(errno, std::generic_category(), "unable to open"+fileName);
    }
    std::clog << "Opened" << fileName <<std::endl;

   
   //obtaining the filter length
    std::uint8_t filterLength;
    infile.read(reinterpret_cast<char *>(&filterLength),sizeof(filterLength));//first read get the size of the filter exactly 8 bits 
    //TODO error handeling with the filter size infomation
    
    float buffer;
    while(!infile.eof()){
    infile.read(reinterpret_cast<char *>(&buffer),sizeof(buffer));//second read get the float data from the file
    result.push_back(buffer);

    }

    infile.close();
    return result;
}
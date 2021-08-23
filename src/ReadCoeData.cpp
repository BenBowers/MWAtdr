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
    std::clog << "Reading filter length" <<std::endl;    
    std::uint8_t filterLength;
    infile.read(reinterpret_cast<char *>(&filterLength),sizeof(filterLength));//first read get the size of the filter exactly 8 bits 
    //TODO error handeling with the filter size infomation
    std::clog << "Completed filter length reading" <<std::endl;    
    
    //creating buffer to hold coeficent data as it is read befor pushing it onto the vector
    float rbuffer;
    float ibuffer;
    std::clog << "Reading data values from file" <<std::endl;        
    while(!infile.eof()){
    infile.read(reinterpret_cast<char *>(&rbuffer),sizeof(rbuffer));//second read get the float data from the file
    infile.read(reinterpret_cast<char *>(&ibuffer),sizeof(ibuffer));
    result.push_back({rbuffer,ibuffer});

    }
    std::clog << "Completed reading data values total values read"+result.size() <<std::endl;        
    //closing the data file
    infile.close();
    //returning the full vector
    return result;
}
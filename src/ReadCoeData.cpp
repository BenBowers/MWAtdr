#include "Common.hpp"
#include "ReadCoeData.hpp"
#include <exception>
#include <iostream>
#include <fstream>
#include <vector>
#include <complex>
#include <array>
#include <string>
#include <cstdint>
#include <filesystem>

// note c++ does not allow for the return of an array from a function as such vectors will be used for simplicity.

// main function for reading in the coeficent data  will return a vector of those coefficients.

// takes a file name as it's only input and will return a float vector of the coefficients that will always be filter length * 256 long
std::vector<std::complex<float>> readCoeData(std::string fileName){
    // final vector that will contain all of the coefficient data
    std::vector<std::complex<float>> result;
    // creating file stream
    std::ifstream infile(fileName, std::ios::binary);
    // main error handling statement
    if(infile.is_open()){
        // filter length defined as the first 8 bits of the file or first float that is contained in the file that will represent how long the filter length is
        uint8_t filterLength;

        //obtaining the filter length
        infile.read(reinterpret_cast<char *>(&filterLength),sizeof(filterLength));//first read get the size of the filter exactly 8 bits
        //error checking to make sure the file is of the right size this is to validate that all the infomation inside atleast of the correct
        if(std::filesystem::file_size(fileName) != 1+filterSize*filterLength*sizeof(float)){
            throw ReadCoeDataException("Error File size is not expected");
        }

        // reading the data into the array
       float rbuffer;
       result.reserve(filterLength*filterSize);
       while(infile.read(reinterpret_cast<char*>(&rbuffer), sizeof(float)))

       result.push_back({rbuffer,0.0f});
    }
    else{
        throw ReadCoeDataException("Failed to open the file");
    }

    //returning the full vector holding the filter length by 256 coefficients
    return result;
}

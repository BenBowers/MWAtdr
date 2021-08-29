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

//note c++ dose not allow for the return of an array from a function as such vecotrs will be used for simpicity.

//main function for reading in the coeficent data  will return a vector of those coeficents.

//takes a file name as its only input and will return an float vector of the coeficents that will always be filter length * 256 long
std::vector<std::complex<float>> readCoeData(std::string fileName){
    //final vector that will contain all of the coeficent data
    std::vector<std::complex<float>> result;
    //filter length definded as the first 8 bits of the file or first float that is contained in the file that will represent low long long the filter length is
    uint8_t filterLength;
    //creating file stream
    std::ifstream infile(fileName, std::ios::binary);
    //main error handling statment
    if(infile.is_open()){
        //obtaining the filter length  
        infile.read(reinterpret_cast<char *>(&filterLength),sizeof(filterLength));//first read get the size of the filter exactly 8 bits         
        //error checking 
        if(std::filesystem::file_size(fileName) != 4+filterSize*filterLength*4){
            std::ios::failure("file was incoreect size based on filter length");
        }

        // buffer for holding the real data read in from the file
        float rbuffer;
        //it is garenteed that the there is 256 enterys for each int of a filter 
        for(int i =1; i<= 256*filterLength; i++){
            infile.read(reinterpret_cast<char *>(&rbuffer),sizeof(rbuffer));
            result.push_back({rbuffer,0.0f});
        }     
        //closing the data file
        infile.close();
    }
    else{
        throw std::ios::failure("Unable to Open file");
    }

    //returning the full vectore holding the filter length by 256 coefficents
    return result;
}
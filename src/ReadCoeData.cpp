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
    float filterLength;
    // buffer for holding the real data read in from the file
    float rbuffer;
    //creating file stream
    std::ifstream infile( "app\\coeficentdataFile.bin", std::ios::binary );
    //main error handling statment
    if(infile.is_open()){
        std::clog << "Opened" << fileName <<std::endl;
        //obtaining the filter length
        std::clog << "Reading filter length" <<std::endl;    
        infile.read(reinterpret_cast<char *>(&filterLength),sizeof(filterLength));//first read get the size of the filter exactly 8 bits 
        
        //TODO error handeling with the filter size infomation
        std::clog << "Completed filter length reading" <<std::endl;         
        
        //creating buffer to hold coeficent data as it is read befor pushing it onto the vector
        std::clog << "Reading data values from file" <<std::endl;        
        while(!infile.eof()){
            infile.read(reinterpret_cast<char *>(&rbuffer),sizeof(rbuffer));//second read get the float data from the file
            result.push_back({rbuffer,0});
            }
    
        std::clog << "Completed reading data values total values read"+result.size() <<std::endl;        
        //closing the data file
        infile.close();
    }
    else{
        throw std::ios::failure("Unable to Open file");
    }

    //returning the full vectore holding the filter length by 256 coefficents
    return result;
}
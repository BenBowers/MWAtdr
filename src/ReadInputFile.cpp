#include "ReadInputFile.hpp"
#include <complex>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>


//Main function for reading in of the data file takes the name of the directory that the data files are stored in
//will read all files for a specific calculation into 1 complex vector array for signal processing
std::vector<std::complex<float> > readInputDataFile(std::string fileName,int antenaInput){   
    std::vector<std::complex<float> > datavalues;
    std::ifstream datafile(fileName, std::ios::binary);
    // main error handling statement
    if(datafile.is_open()){
        //error checking to make sure the file is of the right size this is to validate that all the infomation inside atleast of the correct


        // reading the data into the array
       datavalues.reserve(65000*sizeof(std::int8_t));
       while(datafile.read(reinterpret_cast<char*>(&rbuffer), sizeof(float)))

       datavalues.push_back({rbuffer,0.0f});
    }
    else{
        throw ReadInputDataException("Failed to open the file");
    }

}


//function used to validate if the data file is the correct size and thus allowing the program to know if there is anything missing.
//The file size this program will be given is a constant as such its easy to validate if the file is correct or not
//break
bool validateInputData(std::string fileName){
    if(std::filesystem::file_size(fileName) != 1+filterSize*filterLength*sizeof(float)){
        throw ReadInputDataException("Error File size is not expected");
    }    
    else{
        return true;
    }
}
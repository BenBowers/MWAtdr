#include "ReadInputFile.hpp"
#include "Common.hpp"
#include <complex>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

//where dose this random ass number come from
//MWA documentation states this is the standered file size for a 128 tile dule polarisation file
long stdinputsize = 5275652096;

//Main function for reading in of the data file takes the name of the directory that the data files are stored in
//will read all files for a specific calculation into 1 complex vector array for signal processing
std::vector<std::complex<float> > readInputDataFile(std::string fileName,int antenaInput){   
    std::vector<std::complex<float> > datavalues;
    std::ifstream datafile(fileName, std::ios::binary);
    
    //this gets changed depending on what antean we want to read the data from
    int offset = 0;
    // main error handling statement
    if(datafile.is_open()){

        //error checking to make sure the file is of the right size this is to validate that all the infomation inside atleast of the correct

        //reading the data into the vector

        datavalues.reserve(64000*sizeof(std::int8_t));
        std::int8_t rbuffer;
        std::int8_t ibuffer;
        //alot of this is dependent on the meta data file reader numbers are subject to change once i figure out what to do
        //seeking to the start of the data portion of the file 
        //this will be antena 0 polarisation x sample 1 of 64000
        datafile.seekg(4096+32768000+offset, std::ios::beg);
        while(datafile.read(reinterpret_cast<char*>(&rbuffer), 64000*sizeof(std::int8_t)))
        datavalues.push_back({rbuffer,ibuffer});
    }
    else{
        throw ReadInputDataException("Failed to open the file");
    }

}


//function used to validate if the data file is the correct size and thus allowing the program to know if there is anything missing.
//The file size this program will be given is a constant as such its easy to validate if the file is correct or not
//break
bool validateInputData(std::string fileName){
    if(std::filesystem::file_size(fileName) != stdinputsize){
        throw ReadInputDataException("Error File size is not expected");
    }    
    else{
        return true;
    }
}
#include "ReadInputFile.hpp"
#include "Common.hpp"
#include <complex>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

//where dose this random ass number come from
//MWA documentation states this is the standered file size for a 128 tile dual polarisation file
const long long stdinputsize = 5275652096;

//Main function for reading in of the data file takes the name of the file it is to read from
//will read all files for a specific calculation into 1 complex vector array for signal processing
std::vector<std::vector<std::complex<float>>> readInputDataFile(std::string fileName,int antenaInput){    
    fileName = "1294797712_1294797712";
    
    //this needs to be moved to a diff function as for each file it could be a different number
    long long Ninputs = 256;
    long long DELAYDATALENGTH = 128000*Ninputs;

    //checking to see what challels are avaliable in the data files provided for that observation
    //mapping channels and filenames into usable variables
    std::vector<std::string> allfiles;
    std::vector<int> channels;
    for(int l = 1; l <=256; l++){
        std::string curpath = "mnt/" + fileName + "_" + std::to_string(l) +".sub";
        if(std::filesystem::exists(std::filesystem::path(curpath))){
            allfiles.push_back(curpath);
            channels.push_back(l);
        }    
    }
    
    std::vector<std::vector<std::complex<float>>> datavalues;
    for (int k = 0; k < channels.size(); k++){
        std::vector<std::complex<float>> channelvals;
        //Opening the first data filestream this changes each interation of the loop to pass thru all files
        std::ifstream datafile(allfiles.at(k), std::ios::binary);   
        // main error handling statement
        if(datafile.is_open()){
            //error checking to make sure the file is of the right size this is to validate that all the infomation inside atleast of the correct
            validateInputData(allfiles.at(k));
        
            //this gets changed depending on what antean we want to read the data from
            //per antena per polarisation there is a 64000 bytes of data        
            long long offset;
            if(antenaInput != 0){   
                offset = 64000*antenaInput;
            }
            else{
                offset = 0;
            }        
            //reading the data into the vector
            //known size of data file enteries as per file specification pre allocation to save time later
            datavalues.reserve(64000*sizeof(std::complex<float>));        
            //alot of this is dependent on the meta data file reader numbers are subject to change once i figure out what to do
            //seeking to the start of the data portion of the file 
            //this will be antena 0 polarisation x and y sample 1 of 64000
            datafile.seekg(4096+offset, std::ios::beg);
            for(int i = 1; i <= 160;i++){
                datafile.seekg(32768000, std::ios::cur);
                for(int j = 1; j<=64000;j++){
                    signed char rbuffer;
                    signed char ibuffer;
                    datafile.read(reinterpret_cast<char*>(&rbuffer),sizeof(signed char));
                    datafile.read(reinterpret_cast<char*>(&ibuffer),sizeof(signed char));
                    channelvals.push_back({rbuffer,ibuffer});
                }
            }
            datavalues.push_back(channelvals);    
        }
        else{
            //if file was unable to be opened an exception will be thrown
            throw ReadInputDataException("Failed to open the file");
        }
    }
    
    std::cout.precision(2);
    std::cout << "total enteries in data values" << std::endl;    
    std::cout << std::to_string(datavalues.size()) << std::endl;
    std::cout << std::to_string(datavalues[0].size()) << std::endl;
    std::cout << std::to_string(datavalues[1].size()) << std::endl;
    std::cout << std::to_string(datavalues[2].size()) << std::endl;
    std::cout << std::to_string(datavalues[3].size()) << std::endl;
    /*
    for (int i = 0; i < datavalues.size(); i++) {
    std::cout<< std::fixed << datavalues.at(i) << ' ';
    }
*/
    return datavalues;
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
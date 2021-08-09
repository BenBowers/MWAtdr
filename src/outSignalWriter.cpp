#include <vector>
#include <fstream>
#include "common.hpp"
#include "outSignalWriter.hpp"

void outSignalWriter(std::int16_t inputData[], AppConfig observation){
    

    //typecasting intagers to strings for the purpose of creating the output file name
    //TODO include an output directory
    std::string sObsID = std::to_string(observation.observationID);
    std::string sStartTime = std::to_string(observation.signalStartTime);

    //creating the output file with the correct name 
    std::ofstream myfile(sObsID + "_" + sStartTime + "_signalchain.bin",std::ios::out | std::ios::binary);
    //if the file is unable to be created and opened then an error is made will be handled later
    if(!myfile){
        //TODO error hanndling
        std::cout << "cannot create file" << std::endl;
    }
    //input of the data into the file for tsting only.
    for(int i = 0; i<3; i++)
        //TODO Actual data wrighting 
        //as of now only test will be writen to the file to confirm that file creation is working correctly
        myfile.write("test\n",sizeof(char[5]));
        myfile.close();   
}
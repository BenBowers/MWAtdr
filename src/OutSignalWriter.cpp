#include <vector>
#include <fstream>
#include <cstdint>
#include <string>
#include <iostream>
#include <filesystem>
#include "Common.hpp"
#include "OutSignalWriter.hpp"

void outSignalWriter(const std::vector<std::int16_t> &inputData, const AppConfig &observation, const AntennaInputPhysID &physID){
    

    //typecasting intagers to strings for the purpose of creating the output file name
    //could possibly all be done inside the ofstream but for ease of use has been done outside
    std::string sObsID = std::to_string(observation.observationID);
    std::string sStartTime = std::to_string(observation.signalStartTime);
    std::string sPhysID = std::to_string(physID.tile);
    
    //error checking to make sure the file dosnt already exist
    if(std::filesystem::exists(observation.outputDirectoryPath + "/" + sObsID + "_" + sStartTime + "_" + sPhysID + "_signalchain.bin") == true){
        throw std::ios::failure("file already exists");
    }
    //creating the output file with the correct name and in the correct directory
    std::ofstream outfile(observation.outputDirectoryPath + "/" + sObsID + "_" + sStartTime + "_" + sPhysID + "_signalchain.bin",std::ios::out | std::ios::binary);
    
    if(outfile.is_open()){
        //dump the whole vector into a file
        outfile.write(reinterpret_cast<const char*>(inputData.data()), sizeof(std::int16_t) * inputData.size());
        outfile.close();        
    }
    else{
        throw std::ios::failure("Error creating file");
    }



}
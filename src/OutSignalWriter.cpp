#include <vector>
#include <fstream>
#include <cstdint>
#include <string>
#include <iostream>
#include <filesystem>
#include "Common.hpp"
#include "OutSignalWriter.hpp"
std::filesystem::path generateFilePath(const AppConfig &observation, const AntennaInputPhysID &physID);
void outSignalWriter(const std::vector<std::int16_t> &inputData, const AppConfig &observation, const AntennaInputPhysID &physID){

    std::filesystem::path newpath = generateFilePath(observation,physID);
    
    //error checking to make sure the file dosnt already exist
    if(std::filesystem::exists(newpath) == true){
        throw std::ios::failure("file already exists");
    }
    //creating the output file with the correct name and in the correct directory
    std::ofstream outfile(newpath,std::ios::out | std::ios::binary);
    
    if(outfile.is_open()){
        //dump the whole vector into a file
        outfile.write(reinterpret_cast<const char*>(inputData.data()), sizeof(std::int16_t) * inputData.size());
        outfile.close();        
    }
    else{
        throw std::ios::failure("Error creating file");
    }


}

std::filesystem::path generateFilePath(const AppConfig &observation, const AntennaInputPhysID &physID){
    std::string sObsID = std::to_string(observation.observationID);
    std::string sStartTime = std::to_string(observation.signalStartTime);
    std::string sPhysID = std::to_string(physID.tile);

    if(observation.outputDirectoryPath.empty() == false){
    std::filesystem::path dir (observation.outputDirectoryPath);
    std::filesystem::path obsID (sObsID+"_");
    std::filesystem::path obstime (sStartTime+"_");
    std::filesystem::path fphysID (sPhysID);
    std::filesystem::path file ("_signalchain.bin");
    std::filesystem::path full_path = dir / obsID += obstime += fphysID += file;

    std::cout << full_path << std::endl;

    return full_path;
    }
    else{
        throw std::ios::failure("Error in creating file name");
    }
}
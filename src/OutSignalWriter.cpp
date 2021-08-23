#include <vector>
#include <fstream>
#include <cstdint>
#include <string>
#include <iostream>
#include "Common.hpp"
#include "OutSignalWriter.hpp"

void outSignalWriter(const std::vector<std::int16_t> &inputData, const AppConfig &observation, const AntennaInputPhysID &physID){
    

    //typecasting intagers to strings for the purpose of creating the output file name
    //TODO include an output directory
    std::string sObsID = std::to_string(observation.observationID);
    std::string sStartTime = std::to_string(observation.signalStartTime);
    std::string sPhysID = std::to_string(physID.tile);
    std::string outDir = observation.outputDirectoryPath;

    //creating the output file with the correct name and in the correct directory
    std::ofstream myfile(outDir + sObsID + "_" + sStartTime + + "_signalchain.bin",std::ios::out | std::ios::binary);
    //if the file is unable to be created and opened then an error is made will be handled later
    if(!myfile){
        //TODO error hanndling
        std::cout << "cannot create file" << std::endl;
    }

    //dump the whole vector into a file
    myfile.write(reinterpret_cast<const char*>(inputData.data()), sizeof(std::int16_t) * inputData.size());
    myfile.close();


}
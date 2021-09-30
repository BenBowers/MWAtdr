#include "../../src/ReadInputFile.hpp"
#include "ReadInputFileTest.hpp"
#include "../TestHelper.hpp"
#include "../../src/Common.hpp"
#include <complex>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <random>

class ReadInputFileTest : public TestModule::Impl {
public:
    ReadInputFileTest();

    virtual std::vector<TestCase> getTestCases() override;
};

std::vector<std::complex<float>> testdata;
ReadInputFileTest::ReadInputFileTest(){ 
    std::string str = "HDR_SIZE 4096\nPOPULATED 1\nOBS_ID 1294797712\nSUBOBS_ID 1294797712\nMODE VOLTAGE_START\nUTC_START 2021-01-16-02:01:34\nOBS_OFFSET 0\nNBIT 8\nNPOL 2\nNTIMESAMPLES 64000\nNINPUTS 256\nNINPUTS_XGPU 256\nAPPLY_PATH_WEIGHTS 0\nAPPLY_PATH_DELAYS 0\nINT_TIME_MSEC 500\nFSCRUNCH_FACTOR 50\nAPPLY_VIS_WEIGHTS 0\nTRANSFER_SIZE 5275648000\nPROJ_ID G0034\nEXPOSURE_SECS 304\nCOARSE_CHANNEL 118\nCORR_COARSE_CHANNEL 10\nSECS_PER_SUBOBS 8\nUNIXTIME 1610762494\nUNIXTIME_MSEC 0\nFINE_CHAN_WIDTH_HZ 10000\nNFINE_CHAN 128\nBANDWIDTH_HZ 1280000\nSAMPLE_RATE 1280000\nMC_IP 0.0.0.0\nMC_PORT 0\nMC_SRC_IP 0.0.0.0\n";
    std::string invalidstr = "HDR_SIZE 4096\nPOPULATED 1\nOBS_ID 1294797712\nSUBOBS_ID 1294797712\nMODE VOLTAGE_START\nUTC_START 2021-01-16-02:01:34\nOBS_OFFSET 0\nNBIT 8\nNPOL 3\nNTIMESAMPLES 64000\nNINPUTS 256\nNINPUTS_XGPU 256\nAPPLY_PATH_WEIGHTS 0\nAPPLY_PATH_DELAYS 0\nINT_TIME_MSEC 500\nFSCRUNCH_FACTOR 50\nAPPLY_VIS_WEIGHTS 0\nTRANSFER_SIZE 5275648000\nPROJ_ID G0034\nEXPOSURE_SECS 304\nCOARSE_CHANNEL 118\nCORR_COARSE_CHANNEL 10\nSECS_PER_SUBOBS 8\nUNIXTIME 1610762494\nUNIXTIME_MSEC 0\nFINE_CHAN_WIDTH_HZ 10000\nNFINE_CHAN 128\nBANDWIDTH_HZ 1280000\nSAMPLE_RATE 1280000\nMC_IP 0.0.0.0\nMC_PORT 0\nMC_SRC_IP 0.0.0.0\n";

    //known good file
    std::ofstream myfile("/tmp/1294797712_1294797717_118.sub",std::ios::out | std::ios::binary);
    if(myfile.is_open()){
        myfile << str;
        for(int i =1; i<= 3533;i++){
            myfile <<(std::int8_t)0;
        }
        for(long i = 1; i <= 32768000; i++){
            myfile << (std::int8_t)0;
        }
        //num tiles
        long j = 1;
        for(long i = 1; i<=2621440000; i++){
            signed char realrand = (std::int8_t) rand();
            signed char irand = (std::int8_t) rand();
            if(i <= 64000){
                testdata.push_back({realrand,irand});
                myfile << realrand;
                myfile << irand;
            }
            if(i == 16384000*j+64000){
                    j++;    
            }
            else if(i > 16384000*j && i <= 16384000*j+64000){                
                testdata.push_back({realrand,irand});
                myfile << realrand;
                myfile << irand;
            }
            else{
                myfile << realrand;
                myfile << irand;
            }    
        }
    }

    std::ofstream invalidfile("/tmp/1294797712_1294797718_118.sub",std::ios::out | std::ios::binary);
    if(invalidfile.is_open()){
        invalidfile << invalidstr;
        for(int i =1; i<= 3533;i++){
            invalidfile <<(std::int8_t)0;
        }
        for(long i = 1; i <= 32768000; i++){
            invalidfile << (std::int8_t)0;
        }
        //num tiles
        for(long i = 1; i<=2621440000; i++){
            std::int8_t sample[2] = {(std::int8_t) rand(),(std::int8_t) rand()};
            invalidfile.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));   
        }  
    }
    std::ofstream wrongsize("/tmp/1294797712_1294797719_118.sub",std::ios::out | std::ios::binary);
    if(wrongsize.is_open()){
        wrongsize << str;
        for(int i =1; i<= 3533;i++){
            wrongsize <<(std::int8_t)0;
        }
        for(long i = 1; i <= 32768000; i++){
            wrongsize << (std::int8_t)0;
        }
        //num tiles
        for(long i = 1; i<=2621440001; i++){
            std::int8_t sample[2] = {(std::int8_t) rand(),(std::int8_t) rand()};
            wrongsize.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));   
        }  
    }    
}


std::vector<TestCase> ReadInputFileTest::getTestCases() {
    return {        
        {"Single Valid Data file(Checking the first 64000 elements are the same)", []() {
            try{
                std::vector<std::complex<float>> data = readInputDataFile("/tmp/1294797712_1294797717_118.sub",0);              
                testAssert(data == testdata);                                                           
            }
            catch(ReadInputDataException const& e){}
        }},
        {"Single Valid Data file(Assert Vector Size is expeted) for all antenas", []() {
            try{
                for(int i = 0; i <= 255; i++){
                    std::vector<std::complex<float>> data = readInputDataFile("/tmp/1294797712_1294797717_118.sub",i);              
                    testAssert(data.size() == 10240000);
                }
                std::filesystem::remove("/tmp/1294797712_1294797717_118.sub");                                                            
            }
            catch(ReadInputDataException const& e){}
        }},        
        {"Single invalid Data file (invalid file size)", []() {
            try{
                std::vector<std::complex<float>> data = readInputDataFile("/mnt/test_input/1294797712_1294797719_118.sub",0);              
                failTest();                                                            
            }
            catch(ReadInputDataException const& e){
                std::filesystem::remove("/tmp/1294797712_1294797719_118.sub");
            }
        }},
        {"Invalid File Name", []() {
            try{
                std::vector<std::complex<float>> data = readInputDataFile("123456789",0);              
                failTest();                                                            
            }
            catch(ReadInputDataException const& e){}
        }},
        {"Correct Data File Invalid Meta Data", []() {
            try{
                std::vector<std::complex<float>> data = readInputDataFile("/tmp/1294797712_1294797718_118.sub",0);              
                failTest();                                                            
            }
            catch(ReadInputDataException const& e){
                std::filesystem::remove("/tmp/1294797712_1294797718_118.sub");
            }
        }},                                                                             
    };
}


TestModule readInputFileTest() {
    return {
        "Input File reader test",
        []() { return std::make_unique<ReadInputFileTest>(); }
    };
}

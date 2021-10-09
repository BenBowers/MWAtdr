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

// code taken from https://noobtuts.com/cpp/compare-float-values
//for comaping floats and doubles
bool cmpf(float x, float y, float epsilon = 0.00000005f)
{
    if(fabs(x - y) < epsilon){
      return true; //they are same
    }
    else{
        return false; //they are not same
    }
}
ReadInputFileTest::ReadInputFileTest(){ 
    std::string str = "HDR_SIZE 4096\nPOPULATED 1\nOBS_ID 1294797712\nSUBOBS_ID 1294797712\nMODE VOLTAGE_START\nUTC_START 2021-01-16-02:01:34\nOBS_OFFSET 0\nNBIT 8\nNPOL 2\nNTIMESAMPLES 64000\nNINPUTS 256\nNINPUTS_XGPU 256\nAPPLY_PATH_WEIGHTS 0\nAPPLY_PATH_DELAYS 0\nINT_TIME_MSEC 500\nFSCRUNCH_FACTOR 50\nAPPLY_VIS_WEIGHTS 0\nTRANSFER_SIZE 5275648000\nPROJ_ID G0034\nEXPOSURE_SECS 304\nCOARSE_CHANNEL 118\nCORR_COARSE_CHANNEL 10\nSECS_PER_SUBOBS 8\nUNIXTIME 1610762494\nUNIXTIME_MSEC 0\nFINE_CHAN_WIDTH_HZ 10000\nNFINE_CHAN 128\nBANDWIDTH_HZ 1280000\nSAMPLE_RATE 1280000\nMC_IP 0.0.0.0\nMC_PORT 0\nMC_SRC_IP 0.0.0.0\n";
    std::string invalidstr = "HDR_SIZE 4096\nPOPULATED 1\nOBS_ID 1294797712\nSUBOBS_ID 1294797712\nMODE VOLTAGE_START\nUTC_START 2021-01-16-02:01:34\nOBS_OFFSET 0\nNBIT 8\nNPOL 3\nNTIMESAMPLES 64000\nNINPUTS 256\nNINPUTS_XGPU 256\nAPPLY_PATH_WEIGHTS 0\nAPPLY_PATH_DELAYS 0\nINT_TIME_MSEC 500\nFSCRUNCH_FACTOR 50\nAPPLY_VIS_WEIGHTS 0\nTRANSFER_SIZE 5275648000\nPROJ_ID G0034\nEXPOSURE_SECS 304\nCOARSE_CHANNEL 118\nCORR_COARSE_CHANNEL 10\nSECS_PER_SUBOBS 8\nUNIXTIME 1610762494\nUNIXTIME_MSEC 0\nFINE_CHAN_WIDTH_HZ 10000\nNFINE_CHAN 128\nBANDWIDTH_HZ 1280000\nSAMPLE_RATE 1280000\nMC_IP 0.0.0.0\nMC_PORT 0\nMC_SRC_IP 0.0.0.0\n";

    //known good file
    std::ofstream myfile("/tmp/1294797712_1294797717_118.sub",std::ios::out | std::ios::binary);
    if(myfile.is_open()){
        myfile << str;
        for(int i =1; i<= 3533;i++){
            std::int8_t sample[1] = {0};
            myfile.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));
        }
        for(long i = 1; i <= 32768000; i++){
            std::int8_t sample[1] = {0};
            myfile.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));
        }
        //counters for file validation writing
        std::vector<std::int8_t> blockValues;
        blockValues.reserve(256 * 64000);  // Reserve important for performance
        for (std::size_t block = 0; block < 160; ++block) {
            blockValues.clear();    // Make vector empty
            for (std::size_t antennaInput = 0; antennaInput < 256; ++antennaInput) {
                for (std::size_t sample = 0; sample < 64000; ++sample) {
                // Put in some math formula here to generate effectively random data.
                std::int8_t real = 7 * block * antennaInput * sample - 8 * block;
                std::int8_t imaj = block - antennaInput + 3 * sample;
                blockValues.push_back(real);
                blockValues.push_back(imaj);
                }
            }
            myfile.write(reinterpret_cast<char const*>(blockValues.data()),blockValues.size() * sizeof(std::int8_t));
        }   
    }
    std::ofstream invalidfile("/tmp/1294797712_1294797718_118.sub",std::ios::out | std::ios::binary);
    if(invalidfile.is_open()){
        invalidfile << invalidstr;
        for(int i =1; i<= 3533;i++){
            std::int8_t sample[1] = {0};
            invalidfile.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));
        }
        for(long i = 1; i <= 32768000; i++){
            std::int8_t sample[1] = {0};
            invalidfile.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));
        }
        std::vector<std::int8_t> blockValues;
        blockValues.reserve(256 * 64000);  // Reserve important for performance
        for (std::size_t block = 0; block < 160; ++block) {
            blockValues.clear();    // Make vector empty
            for (std::size_t antennaInput = 0; antennaInput < 256; ++antennaInput) {
                for (std::size_t sample = 0; sample < 64000; ++sample) {
                // Put in some math formula here to generate effectively random data.
                std::int8_t real = 0;
                std::int8_t imaj = 0;
                blockValues.push_back(real);
                blockValues.push_back(imaj);
                }
            }
            invalidfile.write(reinterpret_cast<char const*>(blockValues.data()),blockValues.size() * sizeof(std::int8_t));
        } 
    }
    std::ofstream wrongsize("/tmp/1294797712_1294797719_118.sub",std::ios::out | std::ios::binary);
    if(wrongsize.is_open()){
        wrongsize << str;
        for(int i =1; i<= 3533;i++){
            std::int8_t sample[1] = {0};
            wrongsize.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));
        }
        for(long i = 1; i <= 32768000; i++){
            std::int8_t sample[1] = {0};
            wrongsize.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));
        }
        std::vector<std::int8_t> blockValues;
        blockValues.reserve(256 * 64000);  // Reserve important for performance
        for (std::size_t block = 0; block < 160; ++block) {
            blockValues.clear();    // Make vector empty
            for (std::size_t antennaInput = 0; antennaInput < 256; ++antennaInput) {
                for (std::size_t sample = 0; sample < 64000; ++sample) {
                // Put in some math formula here to generate effectively random data.
                std::int8_t real = 0;
                std::int8_t imaj = 0;
                blockValues.push_back(real);
                blockValues.push_back(imaj);
                }
            }
            wrongsize.write(reinterpret_cast<char const*>(blockValues.data()),blockValues.size() * sizeof(std::int8_t));
        }  
    }
}

std::vector<TestCase> ReadInputFileTest::getTestCases(){
    return {              
        {"Signal Data file Validation(Checking all elements are expected)", []() {
            for(int i =0; i < 256; i++){
                std::vector<std::complex<float>> actualSignalData = readInputDataFile("/tmp/1294797712_1294797717_118.sub",i,256);              
                std::uint8_t antennaInput = i;
                std::size_t index = 0;
                for (std::size_t block = 0; block < 160; ++block) {
                    for (std::size_t sample = 0; sample < 64000; ++sample) {
                    // Put in the same formula as for generating the samples
                    std::int8_t expectedReal = 7 * block * antennaInput * sample - 8 * block;
                    std::int8_t expectedImaj = block - antennaInput + 3 * sample;
                    std::complex<float> actualValue = actualSignalData.at(index);
                    testAssert(cmpf(actualValue.real(),static_cast<float>(expectedReal)) == true);
                    testAssert(cmpf(actualValue.imag(),static_cast<float>(expectedImaj)) == true);
                    ++index;
                    }
                }
            }                                                            
        }},                                    
        {"Single invalid Data file (invalid file size)", []() {
            try{
                std::vector<std::complex<float>> data = readInputDataFile("/mnt/test_input/1294797712_1294797719_118.sub",0,256);              
                failTest();                                                            
            }
            catch(ReadInputDataException const& e){
                std::filesystem::remove("/tmp/1294797712_1294797719_118.sub");
            }
        }},
        {"Invalid File Name", []() {
            try{
                std::vector<std::complex<float>> data = readInputDataFile("123456789",0,256);              
                failTest();                                                            
            }
            catch(ReadInputDataException const& e){}
        }},
        {"Correct Data File Invalid Meta Data", []() {
            try{
                std::vector<std::complex<float>> data = readInputDataFile("/tmp/1294797712_1294797718_118.sub",0,256);              
                failTest();                                                            
            }
            catch(ReadInputDataException const& e){
            }
        }},
        {"Metafits and File metadata not the same", []() {
            try{
                std::vector<std::complex<float>> data = readInputDataFile("/tmp/1294797712_1294797718_118.sub",0,253);              
                failTest();                                                            
            }
            catch(ReadInputDataException const& e){
                std::filesystem::remove("/tmp/1294797712_1294797718_118.sub");
            }
        }},
        {"Validate file function Test valid input", []() {            
                testAssert(validateInputData("/tmp/1294797712_1294797717_118.sub",256) == true);                                                           

        }},
        {"Validate file function Test(Fail) metafits and file dont match", []() {            
            testAssert(validateInputData("/tmp/1294797712_1294797717_118.sub",220) == false);
            std::filesystem::remove("/tmp/1294797712_1294797717_118.sub");                                                            
        }},                                                                                                            
    };
}


TestModule readInputFileTest() {
    return {
        "Input File reader test",
        []() { return std::make_unique<ReadInputFileTest>(); }
    };
}

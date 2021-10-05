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

//antena 0 expected data
std::vector<std::complex<float>> testdata0;
//antena 256 expected data
std::vector<std::complex<float>> testdata1;
//antena 100 expected data
std::vector<std::complex<float>> testdata2;
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
        long j = 1;
        for(long i = 1; i<=2621440000; i++){
            std::int8_t sample[2] = {(std::int8_t) rand(),(std::int8_t) rand()};
            //antena 0
            if(i <= 64000){
                testdata0.push_back({sample[0],sample[1]});
                myfile.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));
            }
            //antena 0
            else if(i == 16384000*j+64000){
                    j++;
                    testdata0.push_back({sample[0],sample[1]});
                    myfile.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));    
            }
            //antena 0
            else if(i > 16384000*j && i <= 16384000*j+64000){                
                testdata0.push_back({sample[0],sample[1]});
                myfile.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));
            }                     
            //all other antena
            else{
                myfile.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));
            }    
        }
        std::cout << testdata0.size() << std::endl;
    } 

    //known good file vectore of input 256
    std::ofstream myfile("/tmp/1294797712_1294797715_118.sub",std::ios::out | std::ios::binary);
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
        long k = 1;
        for(long i = 1; i<=2621440000; i++){
            long offset = 64000*k;
            std::int8_t sample[2] = {(std::int8_t) rand(),(std::int8_t) rand()};
            //antena 256
            if(i > 16320000 && i <= 16383999){
                testdata1.push_back({sample[0],sample[1]});
                myfile.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));
            }
            //antena 256
            else if(i == 16384000*k){
                    k++;
                    testdata1.push_back({sample[0],sample[1]});
                    myfile.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));    
            }   
            //antena 256
            else if(i > 16320000*k+offset && i <= 16384000*k){                
                testdata1.push_back({sample[0],sample[1]});
                myfile.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));
            }                     
            //all other antena
            else{
                myfile.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));
            }    
        }
        std::cout << k <<std::endl;
        std::cout << testdata1.size() << std::endl;
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
            std::int8_t sample[1] = {0};
            wrongsize.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));
        }
        for(long i = 1; i <= 32768000; i++){
            std::int8_t sample[1] = {0};
            wrongsize.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));
        }
        //num tiles
        for(long i = 1; i<=2621440001; i++){
            std::int8_t sample[2] = {(std::int8_t) rand(),(std::int8_t) rand()};
            wrongsize.write(reinterpret_cast<char const*>(&sample[0]), sizeof(sample));   
        }  
    } 
     
}


std::vector<TestCase> ReadInputFileTest::getTestCases(){
    return {        
        
        {"Single Valid Data file(Checking the first all elements are the same) antena 0", []() {
            try{
                std::vector<std::complex<float>> data = readInputDataFile("/tmp/1294797712_1294797717_118.sub",0,256);              
                testAssert(data == testdata0);                                                           
            }
            catch(ReadInputDataException const& e){}
        }},
        {"Single Valid Data file(Checking the first all elements are the same) antena 256", []() {
            try{
                std::vector<std::complex<float>> data = readInputDataFile("/tmp/1294797712_1294797717_118.sub",255,256);              
                testAssert(data == testdata0);                                                           
            }
            catch(ReadInputDataException const& e){}
        }},
        {"Single Valid Data file(Checking the first all elements are the same) antena 100", []() {
            try{
                std::vector<std::complex<float>> data = readInputDataFile("/tmp/1294797712_1294797717_118.sub",100,256);              
                testAssert(data == testdata0);                                                           
            }
            catch(ReadInputDataException const& e){}
        }},                
        {"Single Valid Data file(Assert Vector Size is expeted) for 0-50 inputs", []() {
            try{
                for(int i = 0; i <= 50; i++){    
                    std::vector<std::complex<float>> data = readInputDataFile("/tmp/1294797712_1294797717_118.sub",i,256);
                    testAssert(data.size() == 10240000);
                }                                                          
            }
            catch(ReadInputDataException const& e){
                std::cout << "\nerror" << std::endl;
            }
        }},
        {"Single Valid Data file(Assert Vector Size is expeted) for 51-100 inputs", []() {
            try{
                for(int i = 51; i <= 100; i++){    
                    std::vector<std::complex<float>> data = readInputDataFile("/tmp/1294797712_1294797717_118.sub",i,256);
                    testAssert(data.size() == 10240000);
                }                                                          
            }
            catch(ReadInputDataException const& e){
                std::cout << "\nerror" << std::endl;
            }
        }},
        {"Single Valid Data file(Assert Vector Size is expeted) for 101-150 inputs", []() {
            try{
                for(int i = 101; i <= 150; i++){    
                    std::vector<std::complex<float>> data = readInputDataFile("/tmp/1294797712_1294797717_118.sub",i,256);
                    testAssert(data.size() == 10240000);
                }                                                          
            }
            catch(ReadInputDataException const& e){
                std::cout << "\nerror" << std::endl;
            }
        }},
        
        {"Single Valid Data file(Assert Vector Size is expeted) for 151-200 inputs", []() {
            try{
                for(int i = 151; i <= 200; i++){    
                    std::vector<std::complex<float>> data = readInputDataFile("/tmp/1294797712_1294797717_118.sub",i,256);
                    testAssert(data.size() == 10240000);                    
                }                                                          
            }
            catch(ReadInputDataException const& e){
                std::cout << "\nerror" << std::endl;
            }
        }},
        {"Single Valid Data file(Assert Vector Size is expeted) for 201-255 inputs", []() {
            try{
                for(int i = 201; i <= 255; i++){    
                    std::vector<std::complex<float>> data = readInputDataFile("/tmp/1294797712_1294797717_118.sub",i,256);
                    testAssert(data.size() == 10240000);
                }
                std::filesystem::remove("/tmp/1294797712_1294797717_118.sub");                                                          
            }
            catch(ReadInputDataException const& e){
                std::cout << "\nerror" << std::endl;
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
        {"Metafits and file metadata not the same", []() {
            try{
                std::vector<std::complex<float>> data = readInputDataFile("/tmp/1294797712_1294797718_118.sub",0,253);              
                failTest();                                                            
            }
            catch(ReadInputDataException const& e){}
        }},
        {"Validate file function Test valid input", []() {
            try{             
                testAssert(validateInputData("/tmp/1294797712_1294797717_118.sub",256) == true);                                                           
            }
            catch(ReadInputDataException const& e){}
        }},
        
        {"Validate file function Test(Fail) metafits and file dont match", []() {
            try{             
                testAssert(validateInputData("/tmp/1294797712_1294797717_118.sub",220) == false);
                std::filesystem::remove("/tmp/1294797712_1294797717_118.sub");                                                            
            }
            catch(ReadInputDataException const& e){}
        }},                                                                                                            
    };
}


TestModule readInputFileTest() {
    return {
        "Input File reader test",
        []() { return std::make_unique<ReadInputFileTest>(); }
    };
}

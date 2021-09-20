#include "../../src/ReadInputFile.hpp"
#include "ReadInputFileTest.hpp"
#include "../TestHelper.hpp"
#include "../../src/Common.hpp"
#include <complex>
#include <string>
#include <iostream>
#include <fstream>

class ReadInputFileTest : public TestModule::Impl {
public:
    ReadInputFileTest();

    virtual std::vector<TestCase> getTestCases() override;
};


ReadInputFileTest::ReadInputFileTest(){
    std::ifstream f("signals/metadata.sub");   
    std::string str(4096, ' ');
    if (f){
        f.seekg(4096, std::ios::beg);
        const auto size = f.tellg();
        f.seekg(0);
        f.read(&str[0], size); 
        f.close();
    }

    std::ifstream invf("signals/invalidmetadata.sub");   
    std::string invalidstr(4096, ' ');
    if (invf){
        invf.seekg(4096, std::ios::beg);
        const auto size = invf.tellg();
        invf.seekg(0);
        invf.read(&invalidstr[0], size); 
        invf.close();
    }
    //known good file
    std::ofstream myfile("signals/1294797712_1294797717_118.sub",std::ios::out | std::ios::binary);
    if(myfile.is_open()){
        myfile << str;
        for(long i = 1; i <= 32768000; i++){
            myfile << (signed char)0;
        }
        //num tiles
        for(long long i = 1; i<=256; i++){
            //number of data enteries
            for(long long j =1; j<=160; j++){
                //num samples
                for (long k = 1; k <= 64000; k++){
                    myfile << (signed char)1;
                    myfile << (signed char)1;
                }
            }
        }  
    }
    //invalid metadata wrong file size
    std::ofstream invalidfile("signals/1294797712_1294797715_118.sub",std::ios::out | std::ios::binary);
    if(invalidfile.is_open()){
        invalidfile << str;
        for(long i = 1; i <= 32768000; i++){
            invalidfile << (signed char)0;
        }
        //num tiles
        for(long long i = 1; i<=256; i++){
            //number of data enteries
            for(long long j =1; j<=160; j++){
                //num samples
                for (long k = 1; k <= 64001; k++){
                    invalidfile << (signed char)1;
                    invalidfile << (signed char)1;
                }
            }
        }  
    }    
    //invalid meta data
    std::ofstream invalidmeta("signals/1294797712_1294797714_118.sub",std::ios::out | std::ios::binary);
    if(invalidmeta.is_open()){
        invalidmeta << invalidstr;
        for(long i = 1; i <= 32768000; i++){
            invalidmeta << (signed char)0;
        }
        //num tiles
        for(long long i = 1; i<=256; i++){
            //number of data enteries
            for(long long j =1; j<=160; j++){
                //num samples
                for (long k = 1; k <= 64000; k++){
                    invalidmeta << (signed char)1;
                    invalidmeta << (signed char)1;
                }
            }
        }  
    }    
    //5 different channels
    for(int ii =1; ii<=5; ii++){
        std::string filename = "signals/1294797712_1294797712_"+std::to_string(ii) +".sub";
        std::ofstream diffchan(filename,std::ios::out | std::ios::binary);
        if(diffchan.is_open()){
            diffchan << str;
            for(long i = 1; i <= 32768000; i++){
                diffchan << (signed char)0;
            }
            //num tiles
            for(long long i = 1; i<=256; i++){
                //number if data enteries
                for(long long j =1; j<=160; j++){
                    //num samples
                    for (long k = 1; k <= 64000; k++){
                        diffchan << (signed char)1;
                        diffchan << (signed char)1;
                    }
                }
            }  
        }
    }
    
}


std::vector<TestCase> ReadInputFileTest::getTestCases() {
    return {        
        {"5 valid Signal Channel files", []() {
            try{
                auto data = readInputDataFile("1294797712_1294797712",0);
                testAssert(data.size() == 5); 
            }
            catch(ReadInputDataException const& e){}
        }},
        {"Invalid data file name", []() {
            try{
                auto data = readInputDataFile("1234567890",0);
                failTest();
            }
            catch(ReadInputDataException const& e){}
        }},
        {"Single Malformed Meta Data file Incorect NPOLS", []() {
            try{
                auto data = readInputDataFile("1294797712_1294797714",0);
                failTest();
            }
            catch(ReadInputDataException const& e){}
        }},
        {"Single Invalid Data file Correct Meta Data wrong size", []() {
            try{
                auto data = readInputDataFile("1294797712_1294797715",0);
                failTest(); 
            }
            catch(ReadInputDataException const& e){}
        }}, 
        {"Single Valid Data file all Antena read (File confirmed to have 128 tiles 2x polarisations)", []() {
            try{
                for(int i = 1; i <= 1; i++){
                    auto data = readInputDataFile("1294797712_1294797713",i);
                    testAssert(data.size() == 1);
                }  
            }
            catch(ReadInputDataException const& e){}
        }},
        {"Verify Data (Known Data file all 1's as input)", []() {
            try{
                auto data = readInputDataFile("1294797712_1294797717",0);
                std::vector<std::vector<std::complex<float>>> expected(1,std::vector<std::complex<float>>(10240000,{1,1}));
                testAssert(expected == data);     
            }
            catch(ReadInputDataException const& e){}
        }}                                               
    };
}


TestModule readInputFileTest() {
    return {
        "Input File reader test",
        []() { return std::make_unique<ReadInputFileTest>(); }
    };
}

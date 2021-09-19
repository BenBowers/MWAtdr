#include "../../src/ReadInputFile.hpp"
#include "ReadInputFileTest.hpp"
#include "../TestHelper.hpp"
#include "../../src/Common.hpp"
#include <chrono>
#include <iostream>

class ReadInputFileTest : public TestModule::Impl {
public:
    ReadInputFileTest();

    virtual std::vector<TestCase> getTestCases() override;
};


ReadInputFileTest::ReadInputFileTest() {

}


std::vector<TestCase> ReadInputFileTest::getTestCases() {
    return {        
        {"5 valid Signal Channel files", []() {
            try{
                auto tstart = std::chrono::high_resolution_clock::now();
                auto data = readInputDataFile("1294797712_1294797712",0);
                testAssert(data.size() == 5);
                auto tend = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(tend-tstart);
                std::cout << duration.count() << std::endl; 
            }
            catch(ReadInputDataException const& e){}
        }},
        {"Single Valid Data file", []() {
            try{
                auto data = readInputDataFile("1294797712_1294797713",0);
                testAssert(data.size() == 1); 
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
                auto tstart = std::chrono::high_resolution_clock::now(); 
                for(int i = 1; i <= 1; i++){
                    auto data = readInputDataFile("1294797712_1294797713",i);
                    testAssert(data.size() == 1);
                }
                auto tend = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(tend-tstart);
                std::cout << duration.count() << std::endl;   
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

#include "../../src/ReadInputFile.hpp"
#include "ReadInputFileTest.hpp"
#include "../TestHelper.hpp"
#include "../../src/Common.hpp"
#include <complex>
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
                auto data = readInputDataFile("1294797712_1294797712",0);
                testAssert(data.size() == 5); 
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
                for(int i = 1; i <= 256; i++){
                    auto data = readInputDataFile("1294797712_1294797713",i);
                    testAssert(data.size() == 1);
                }  
            }
            catch(ReadInputDataException const& e){}
        }},
        {"Verify Data (Known Data file all 1's as input)", []() {
            try{
                auto data = readInputDataFile("1294797712_1294797716",0);
                std::vector<std::vector<std::complex<float>>> expected(1,std::vector<std::complex<float>>(10240000,{1,1}));
                testAssert(data == expected);     
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

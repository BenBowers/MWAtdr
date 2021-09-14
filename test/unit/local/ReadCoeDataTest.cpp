#include "ReadCoeData.hpp"
#include "ReadCoeDataTest.hpp"
#include "TestHelper.hpp"
#include "Common.hpp"

#include <stdexcept>
#include <cstdint>
#include <vector>
#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>


std::vector<std::complex<float>> readCoeData(std::string fileName);
const std::string FILENAME = "/tmp/coefficientdataFile.bin";
const std::string INCORRECTDATAFILE = "/tmp/badDatacoefficientdataFile";
const std::string MULTIDATAFILE = "/tmp/multiNumber.bin";

ReadCoeDataTest::ReadCoeDataTest() : TestModule{"Read Coefficient data Test", {
    
    {"Valid InputFile", []() {
       std::vector<std::complex<float>> actual = readCoeData(FILENAME);
        testAssert(actual.size() % filterSize == 0);
	}},

    {"Invalid InputFile(wrong size of data inside the file)", []() {
       try{
           readCoeData(INCORRECTDATAFILE);
           failTest();
       }
       catch(ReadCoeDataException const& e){}
	}},
    
    {"Valid InputFile(Data Integrity Check)", []() {
       std::vector<std::complex<float>> actual = readCoeData(FILENAME);
       testAssert(std::adjacent_find(actual.begin(), actual.end(), std::not_equal_to<>() ) == actual.end() == true);
	}},

    {"Valid InputFile(Data Integrity Check Different Coefficients)", []() {
       std::vector<std::complex<float>> actual = readCoeData(MULTIDATAFILE);
       std::vector<std::complex<float>> expected; 
       
       float coeficent =1.0;
       for(int i = 0; i < filterSize*128; i++){
            expected.push_back({coeficent+i,0.0f});
        }        
        
        testAssert(actual == expected); 

	}},        

    {"No Data File", []() {
       try{
           readCoeData("11");
           failTest();
       }
       catch(ReadCoeDataException const& e){}
	}}

}} {}

void buildTestData(){
    
    uint8_t filterlength = 128;
    float testcoeficent = 1.0;

    std::ofstream myfile(FILENAME,std::ios::out | std::ios::binary);
    if(myfile.is_open()){

        myfile.write(reinterpret_cast<const char*>(&filterlength),sizeof(uint8_t));

        for(int i = 0; i < filterSize*128; i++){
            myfile.write(reinterpret_cast<const char*>(&testcoeficent),sizeof(float));
        }   
    }

    std::ofstream badfile(INCORRECTDATAFILE,std::ios::out | std::ios::binary);
    if(badfile.is_open()){

        badfile.write(reinterpret_cast<const char*>(&filterlength),sizeof(uint8_t));

        for(int i = 0; i < filterSize*30; i++){
            badfile.write(reinterpret_cast<const char*>(&testcoeficent),sizeof(float));
        }   
    }

    std::ofstream multiNumber(MULTIDATAFILE,std::ios::out | std::ios::binary);
    if(multiNumber.is_open()){

        multiNumber.write(reinterpret_cast<const char*>(&filterlength),sizeof(uint8_t));

        for(int i = 0; i < filterSize*128; i++){
            float newtest = testcoeficent+(float)i;
            multiNumber.write(reinterpret_cast<const char*>(&newtest),sizeof(float));
        }   
    }     
    
}
#include "../../src/ReadCoeData.hpp"
#include "ReadCoeDataTest.hpp"
#include "../TestHelper.hpp"
#include "../../src/Common.hpp"

#include <stdexcept>
#include <cstdint>
#include <vector>
#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>


std::vector<std::complex<float>> readCoeData(std::string fileName);
std::string filename = "coeficentdataFile.bin";
std::string incorectDataFile = "badDatacoeficentdataFile";

ReadCoeDataTest::ReadCoeDataTest() : TestModule{"Read Coeficent data Test", {
    
    {"Valid InputFile", []() {
       std::vector<std::complex<float>> actual = readCoeData("coeficentdataFile.bin");
        testAssert(actual.size() % filterSize == 0);
	}},

    {"Invalid InputFile(wrong size of data inside the file)", []() {
       try{
           readCoeData(incorectDataFile);
           failTest();
       }
       catch(readCoeDataException const&){}
	}},
    
    {"Valid InputFile(Data Integrity Check)", []() {
       std::vector<std::complex<float>> actual = readCoeData("coeficentdataFile.bin");
       testAssert(std::adjacent_find(actual.begin(), actual.end(), std::not_equal_to<>() ) == actual.end() == true);
	}},

    {"Valid InputFile(Data Integrity Check Different coeficents)", []() {
       std::vector<std::complex<float>> actual = readCoeData("multiNumber.bin");
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
       catch(readCoeDataException const&){}
	}}

}} {}

void buildTestData(){
    
    uint8_t filterlength = 128;
    float testcoeficent = 1.0;

    std::ofstream myfile("coeficentdataFile.bin",std::ios::out | std::ios::binary);
    if(myfile.is_open()){

        myfile.write(reinterpret_cast<const char*>(&filterlength),sizeof(uint8_t));

        for(int i = 0; i < filterSize*128; i++){
            myfile.write(reinterpret_cast<const char*>(&testcoeficent),sizeof(float));
        }   
    }

    std::ofstream badfile("badDatacoeficentdataFile.bin",std::ios::out | std::ios::binary);
    if(badfile.is_open()){

        badfile.write(reinterpret_cast<const char*>(&filterlength),sizeof(uint8_t));

        for(int i = 0; i < filterSize*30; i++){
            badfile.write(reinterpret_cast<const char*>(&testcoeficent),sizeof(float));
        }   
    }

    std::ofstream multiNumber("multiNumber.bin",std::ios::out | std::ios::binary);
    if(multiNumber.is_open()){

        multiNumber.write(reinterpret_cast<const char*>(&filterlength),sizeof(uint8_t));

        for(int i = 0; i < filterSize*128; i++){
            float newtest = testcoeficent+(float)i;
            multiNumber.write(reinterpret_cast<const char*>(&newtest),sizeof(float));
        }   
    }     
    
}
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

std::string filename = "coeficentdataFile";

ReadCoeDataTest::ReadCoeDataTest() : TestModule{"Read Coeficent data Test", {
    
    {"Valid InputFile", []() {
       auto actual = readCoeData(filename);

        testAssert(actual.size() % 256 == 0);
	}},

    {"Invalid InputFile(wring size of data inside the file)", []() {
       try{
           readCoeData(filename);
           failTest();
       }
       catch(std::ios::failure const&){}
	}}    


}} {}
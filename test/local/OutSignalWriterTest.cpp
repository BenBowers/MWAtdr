#include "../../src/OutSignalWriter.hpp"
#include "OutSignalWriterTest.hpp"
#include "../TestHelper.hpp"
#include "../../src/Common.hpp"

#include <stdexcept>
#include <cstdint>
#include <vector>


std::vector<std::int16_t> testData = {1,2,3,4,5,6,7,8,9};
//input path, obs id, signal start time, poly path, output dir
AppConfig invalidTestConfig = {"123456789",123456789,123456789,"",""};
AppConfig validTestConfig = {"",123456789,123456789,"",""}; //input and output directories are listed 
//Antena input Physical ID the two components of that struct is the first physical id the second is the char representing what signal chain that id is for 
AntennaInputPhysID testAntenaPhysID = {1,'x'};


OutSignalWriterTest::OutSignalWriterTest() : TestModule{"Output File Writer unit test", {
    
    {"Verify output Data", []() {

		try {
			outSignalWriter(testData,validTestConfig,testAntenaPhysID);

		    failTest();

		}

		catch (std::invalid_argument const&) {}

	}},

    {"Invalid inputs (incorrect output directory)", []() {

		try {
			outSignalWriter(testData,invalidTestConfig,testAntenaPhysID);

			failTest();
		}

		catch (std::invalid_argument const&) {}

	}},
    
    {"Invalid ",[](){
        
        try {
            outSignalWriter();
            failTest();
        }
        catch(const std::exception& e){}

    }}    

}} {}
#include "../../src/OutSignalWriter.hpp"
#include "OutSignalWriterTest.hpp"
#include "../TestHelper.hpp"
#include "../../src/Common.hpp"

#include <stdexcept>
#include <cstdint>
#include <vector>


std::vector<std::int16_t> testData;
AppConfig invalidTestConfig = {"123456789",123456789,123456789,"",""};
AppConfig validTestConfig = {"",123456789,123456789,"/","/"}; //input and output directories are listed 


OutSignalWriterTest::OutSignalWriterTest() : TestModule{"Output File Writer unit test", {
    
    {"Verify output Data", []() {

		try {
			outSignalWriter(testData,invalidTestConfig,);

		    failTest();

		}

		catch (std::invalid_argument const&) {}

	}},

    {"Invalid inputs (incorect formatting)", []() {

		try {
			outSignalWriter();

			failTest();
		}

		catch (std::invalid_argument const&) {}

	}},
    
    {"Invalid output directory",[](){
        
        try {
            outSignalWriter();
            failTest();
        }
        catch(const std::exception& e){}

    }}    

}} {}
#include "../../src/OutSignalWriter.hpp"
#include "../TestHelper.hpp"

#include <stdexcept>
#include <vector>

OutSignalWriterTest::OutSignalWriterTest() : TestModule{"Output File Writer unit test",{
    
    {"Invalid Inputs (Unexpected Data types)", []() {

		try {

			outSignalWriter(0,0,0);

		    failTest();

		}

		catch (std::invalid_argument const&) {}

	}},

    {"Invalid inputs (incorect formatting)", []() {

		try {
			outSignalWriter(2,0,0,);

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
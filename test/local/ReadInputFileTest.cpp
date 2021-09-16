#include "../../src/ReadInputFile.hpp"
#include "ReadInputFileTest.hpp"
#include "../TestHelper.hpp"
#include "../../src/Common.hpp"

class ReadInputFileTest : public TestModule::Impl {
public:
    ReadInputFileTest();

    virtual std::vector<TestCase> getTestCases() override;
};


ReadInputFileTest::ReadInputFileTest() {

}


std::vector<TestCase> ReadInputFileTest::getTestCases() {
    return {        
        {"Valid Data File", []() {
            try{
                auto data = readInputDataFile("1294797712_1294797712",0);
                testAssert(data.size() == 5); 
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

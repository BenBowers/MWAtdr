#include "../TestHelper.hpp"
#include "ReadCoeDataTest.hpp"
#include <vector>
#include <complex>

std::vector<std::complex<float>> readCoeData(std::string fileName);

int main() {
    
    std::vector<std::complex<float>> actual = readCoeData("coeficentdataFile.bin");
    /*
    runTests({
        ReadCoeDataTest{}
    });
*/
}
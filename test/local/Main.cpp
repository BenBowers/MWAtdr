#include "../TestHelper.hpp"
#include "ReadCoeDataTest.hpp"
#include <vector>
#include <complex>
#include <fstream>
#include <iostream>

int main(){
    float filterlength = 128.0;
    float testcoeficent = 1.0;

    std::ofstream myfile("coeficentdataFile.bin",std::ios::out | std::ios::binary);
    if(myfile.is_open()){

        myfile.write(reinterpret_cast<const char*>(&filterlength),sizeof(float));

        for(int i = 1; i <= 256*128; i++){
            myfile.write(reinterpret_cast<const char*>(&testcoeficent),sizeof(float));
        }
        myfile.close();    
    }

    std::ofstream badfile("badDatacoeficentdataFile.bin",std::ios::out | std::ios::binary);
    if(badfile.is_open()){

        badfile.write(reinterpret_cast<const char*>(&filterlength),sizeof(float));

        for(int i = 1; i <= 256*30; i++){
            badfile.write(reinterpret_cast<const char*>(&testcoeficent),sizeof(float));
        }
        badfile.close();    
    }    
    runTests({
        ReadCoeDataTest{}
    });

}
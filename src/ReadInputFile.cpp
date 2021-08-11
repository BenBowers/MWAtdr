#include "ReadInputFile.hpp"
#include <complex>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


//Main function for reading in of the data file takes the name of the directory that the data files are stored in
//will read all files for a specific calculation into 1 complex vector array for signal processing
std::vector<std::complex<float> > readInputDataFileFile(std::string fileName,int antenaInput){   
    //declaration of variables used to construct the complex number. In future will be set by reading in the file
    double real;
    double imaginary;

    //creating the vector/array of complex numbers represending
    std::vector<std::complex<float> > datavalues;
    
    //test values will read this from the file later
    real = 1;
    imaginary = 1;
    //filling the vectore with 65000 complex numbers with 1 being the real and imag part as a place holder
    for(int i = 0; i<= 65000;i++){
        //TODO FIle IO component will be included here
        std::complex<float> temp(real, imaginary);
        //depending on data files this might need to be chaged to push_fowrd
        datavalues.push_back(temp);
    }
    return datavalues;
}


//function used to validate if the data file is the correct size and thus allowing the program to know if there is anything missing.
//all data files will be 1 specific size so it will be assumed that any file that is not of the correct size is malfored and not usable for calculations.
//it is going to be assumed for testing purposes that all files are correct.
//TODO Actual file validation
bool validateInputData(std::string fileName){
    return true;
}
#include <iostream>
#include <fstream>
#include <string>
//pre declaration of output singnal wrigter function
void outSignalWriter(std::int16_t inputData[], std::int16_t obsID, std::int16_t startTime, char* outFileDirectory);

int main() {

    int test[5] = {1,1,1,1,1};
    outSignalWriter(test,123456789,123456789,"/local_test");

}


void outSignalWriter(std::int16_t inputData[], std::int16_t obsID, std::int16_t startTime, char* outFileDirectory){
    

    //typecasting intagers to strings for the purpose of creating the output file name
    //TODO include an output directory
    std::string sObsID = std::to_string(obsID);
    std::string sStartTime = std::to_string(startTime);

    //creating the output file with the correct name 
    std::ofstream myfile(sObsID + "_" + sStartTime + "_signalchain.bin",std::ios::out | std::ios::binary);
    //if the file is unable to be created and opened then an error is made will be handled later
    if(!myfile){
        //TODO error hanndling
        std::cout << "cannot create file" << std::endl;
    }
    //input of the data into the file for tsting only.
    for(int i = 0; i<3; i++)
        //TODO Actual data wrighting 
        //as of now only test will be writen to the file to confirm that file creation is working correctly
        myfile.write("test\n",sizeof(char[5]));
        myfile.close();   
}

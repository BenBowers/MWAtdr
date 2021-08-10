#include "readCoeData.hpp"
//note c++ dose not allow for the return of an array from a function as such vecotrs will be used for simpicity.

//main function for reading in the coeficent data  will return a vector of those coeficents.

//takes a file name as its only input and will return an intager array of the coeficents
std::vector<int16_t> readCoeData(std::string fileName){
    
    //realy messay way of reading a file most likly to change the internals of this function in the future.
    fileName = "test.txt";
    std::ifstream infile(fileName);
    std::vector<int16_t> result;
    //checking to see if the file is open
   
    if(infile.is_open()){
        while(true){
            int data;
            infile >> data;
            //dont like now this exits right now will be changed in the future ugly use of a break
            if(infile.eof()){
                break;
            }
            result.push_back(data);
        }
        infile.close();
    }
    else{
        //TODO pass errors to the error handler
        //remove cosole log
        std::cout << "unable to open file";
    }
    //closing the file once all the reading is compleated.
    infile.close();
    //creating and filling an array of 1's for testing and produce knowen resaults

    for (std::int16_t i = 0; i = 256;i++){
        result.push_back(1);
    }

    return result;
}
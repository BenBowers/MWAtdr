/* WARNING MOST OF THIS MODULE CAN AND WILL BREAK IF THE MWA DATA FILE FORMAT CHANGES IN A DRASTIC WAY
    Some things to note that i have tryed to make this module as effiecent as it can be
    Due to having to read into complex numbers greatly makes this module super in efficent
    Having to read the meta data inside of each file is also inefficent.

*/
#include "ReadInputFile.hpp"
#include "Common.hpp"
#include <complex>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

//Meta data inside each file readers
std::string getMetaDataString(std::string fileName);
unsigned int getMetaDataSize(std::string fileName);
int getNInputs(std::string fileName);
int getNPols(std::string fileName);
int getNSamples(std::string fileName);

//Main function for reading in of the data file takes the name of the file it is to read from
//will read all files for a specific calculation into 1 complex vector array for signal processing
std::vector<std::complex<float>> readInputDataFile(std::string fileName,int antenaInput){    

    std::vector<std::complex<float>> datavalues;
        
    std::string metadata = getMetaDataString(fileName);
    unsigned int metadatasize = getMetaDataSize(metadata);
    //This is the number of samples per 50ms time slice in the data files this is subject to change based on the MWA wiki
    int NUMSAMPLES = getNSamples(metadata);
    //This needs to be moved to a diff function as for each file it could be a different number
    long long NUMTILES = getNInputs(metadata) * getNPols(metadata);
    //This is how large the delay meta data block is inside of the file is is dependent on how many tiles are in the observation
    long long DELAYDATALENGTH = NUMTILES*NUMSAMPLES*2;

    //error checking to make sure the file is of the right size this is to validate that all the infomation inside atleast of the correct
    validateInputData(fileName);
    //Opening the first data filestream this changes each interation of the loop to pass thru all files
    std::ifstream datafile(fileName, std::ios::binary);   
    // main error handling statement
    if(datafile.is_open()){
        //this gets changed depending on what antean we want to read the data from
        //per antena per polarisation there is a 64000 bytes of data        
        long long offset;
        if(antenaInput != 0){   
            offset = NUMSAMPLES*antenaInput;  
        }
        else{
            offset = 0;     
        }        
        //reading the data into the vector
        //known size of data file enteries as per file specification pre allocation to save time later
        datavalues.reserve(NUMSAMPLES*160);        
        //alot of this is dependent on the meta data file reader numbers are subject to change once i figure out what to do
        //seeking to the start of the data portion of the file 
        //this will be antena 0 polarisation x and y sample 1 of 64000
        datafile.seekg(offset+metadatasize, std::ios::beg);
        for(int i = 1; i <= 160;i++){
            datafile.seekg(DELAYDATALENGTH, std::ios::cur);
            for(int j = 1; j<=NUMSAMPLES;j++){
                signed char rbuffer;
                signed char ibuffer;

                datafile.read(reinterpret_cast<char*>(&rbuffer),sizeof(std::int8_t));
                datafile.read(reinterpret_cast<char*>(&ibuffer),sizeof(std::int8_t));
                if(datafile.fail()){
                    throw ReadInputDataException("Failed to read data byte from file");
                }
                datavalues.push_back({rbuffer,ibuffer});
            }
        }   
    }
    else{
        //if file was unable to be opened an exception will be thrown
        throw ReadInputDataException("Failed to open the file");
    }
    return datavalues;
}


//function used to validate if the data file is the correct size and thus allowing the program to know if there is anything missing.
//The file size this program will be given is a constant as such its easy to validate if the file is correct or not
//break
void validateInputData(std::string fileName){
    //MWA documentation states this is the standered file size for a 128 tile dual polarisation file
    //4096+161*32768000
    //meta data 160 volatage data blocks + the single delay block before the data * the size of the delay block what is 
    //Number of tiles *2 polarisations * 128000 bytes 64000 1 byte samples for each real and imag part
    std::string metadata = getMetaDataString(fileName);    
    //2 bytes per sample
    long samplebytesize = getNSamples(metadata)*2;
    long delaydata = getNInputs(metadata) * getNPols(metadata) * samplebytesize;
    long metadatasize = getMetaDataSize(metadata);
    long long expectedInputSize = metadatasize + delaydata * 161;
    if(std::filesystem::file_size(fileName) != expectedInputSize){
        throw ReadInputDataException("Error File size is not expected");
    }
}

std::string getMetaDataString(std::string fileName){
    std::ifstream f(fileName);   
    if (f){
        f.seekg(4096, std::ios::beg);
        const auto size = f.tellg();
        std::string str(size, ' ');
        f.seekg(0);
        f.read(&str[0], size); 
        f.close();
        return str;
    }
    else{
        throw ReadInputDataException("Error Reading meta data from file");
    }    
}

int getNPols(std::string str){
    int nPols;
    int pos = str.find("NPOL");
    int delim = str.find("\n",pos);
    std::string sNpols = str.substr(pos+5,delim); 
    nPols = std::stoi(sNpols);
    return nPols;
}

//returns the number of samples per 50ms time slice in the data file 
int getNSamples(std::string str){
    int nSamples;
    int pos = str.find("NTIMESAMPLES");
    int delim = str.find("\n",pos);
    std::string snSamples = str.substr(pos+13,delim); 
    nSamples = std::stoi(snSamples);  
    return nSamples;
}

//returns the humber of fine channels that the meta data expects in the file
int getNInputs(std::string str){
    int nInputs;
    int pos = str.find("NFINE_CHAN");
    int delim = str.find("\n",pos);        
    std::string snInputs = str.substr(pos+10,delim); 
    nInputs = std::stoi(snInputs); 
    return nInputs;
}

//will return the size of the meta data header block described in the first line of the file
unsigned int getMetaDataSize(std::string str){
    unsigned int mSize;
    int pos = str.find("HDR_SIZE");
    int delim = str.find("\n",pos);
    std::string sHRDsize = str.substr(pos+8,delim); 
    mSize = std::stoi(sHRDsize);
    return mSize;
}

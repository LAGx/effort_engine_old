#include "service/fileServing.h"
#include "service/log.h"
#include <sys/stat.h>
#include <fstream>
#include "service/json.hpp"


using namespace eff;
using namespace eff_low;
using json = nlohmann::json;


SafeFileName::SafeFileName(const string& fileName):string(fileName){
    struct stat buffer;    
    if (stat (fileName.c_str(), &buffer) != 0)
        throw Log::Exception("not exist file: " + fileName , true);
}


SafeFileName::SafeFileName(const char* fileName):string(fileName){
    struct stat buffer;    
    if (stat (fileName, &buffer) != 0)
        throw Log::Exception(string("not exist file: ") + fileName , true);
}


SafeFileName::operator FileName(){
    return dynamic_cast<FileName&> (*this);
}


CreateFileName::CreateFileName(const string& fileName){

    struct stat buffer; 
       
    if (stat (fileName.c_str(), &buffer) != 0){

 #ifdef DEBUG_MODE
        Log::WriteTo("log.log").log("not exist file: " + fileName + "\n\t create new...", true);
 #endif
        ofstream file(fileName, ios_base::trunc);
        file.close();
    }

}


CreateFileName::CreateFileName(const char* fileName){

    struct stat buffer; 
       
    if (stat (fileName, &buffer) != 0){

 #ifdef DEBUG_MODE
        Log::WriteTo("log.log").log(string("not exist file: ") + fileName + "\n\t create new...", true);
 #endif
        ofstream file(fileName, ios_base::trunc);
        file.close();
    }

}



CreateFileName::operator FileName(){
    return dynamic_cast<FileName&> (*this);
}


json JsonFile_fast::getFile(const Json_FileName& fileName){
    ifstream file(fileName);
    json j;
    file >> j;
    file.close();
    return j;
}


void JsonFile_fast::saveFile(const Json_FileName& fileName, const json& j){
    ofstream file(fileName, ios_base::trunc);
    file << j;
    file.close();
}

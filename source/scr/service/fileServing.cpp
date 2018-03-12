#include "service/fileServing.h"
#include "service/log.h"
#include <sys/stat.h>
#include <fstream>

using namespace eff;
using namespace eff_low;



SafeFileName::SafeFileName(const string& fileName){
    struct stat buffer;    
    if (stat (name.c_str(), &buffer) != 0)
        throw Log::Exception("not exist file: " + fileName , true);
}


SafeFileName::operator FileName(){
    return dynamic_cast<FileName> (*this);
}


CreateFileName::CreateFileName(const string& fileName){

    struct stat buffer; 
       
    if (stat (name.c_str(), &buffer) != 0){

 #ifdef DEBUG_MODE
        Log::WriteTo("log.log").log("not exist file: " + fileName + "\n\t create new...", true);
 #endif
        ofstream file(fileName, ios_base::trunc);
        file.close();
    }

}


CreateFileName::operator FileName(){
    return dynamic_cast<FileName> (*this);
}
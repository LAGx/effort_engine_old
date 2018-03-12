#pragma once

#include "service/json.hpp"
#include <string>


using namespace std;
using json = nlohmann::json;


namespace eff{
namespace eff_low{


typedef string FileName;


/*!
    @brief check for file exist
    @exeption Log::Exeption if file not exist
    @warning low performance
*/ 
class SafeFileName:public FileName{
public:
    SafeFileName(const string& fileName);
    operator FileName();
};


/*!
    @brief if file not exist this will create a new one
    @warning write log that file was created in DEBUG_MODE
    @warning low performance
*/
class CreateFileName: public FileName{
public:
    CreateFileName(const string& fileName);
    operator FileName();
};


};


typedef eff_low::FileName JsonSettings_FileName;
typedef eff_low::FileName JsonData_FileName;
typedef eff_low::FileName Json_FileName;
typedef eff_low::FileName Data_ScriptName;
typedef eff_low::FileName Settings_ScriptName;
typedef eff_low::FileName Usual_ScriptName;

typedef eff_low::SafeFileName JsonSettings_SafeFileName;
typedef eff_low::SafeFileName JsonData_SafeFileName;
typedef eff_low::SafeFileName Json_SafeFileName;
typedef eff_low::SafeFileName Data_SafeScriptName;
typedef eff_low::SafeFileName Settings_SafeScriptName;
typedef eff_low::SafeFileName Usual_SafeScriptName;

typedef eff_low::CreateFileName JsonSettings_CreateFileName;
typedef eff_low::CreateFileName JsonData_CreateFileName;
typedef eff_low::CreateFileName Json_CreateFileName;
typedef eff_low::CreateFileName Data_CreateScriptName;
typedef eff_low::CreateFileName Settings_CreateScriptName;
typedef eff_low::CreateFileName Usual_CreateScriptName;


/*!
    @brief class for working with json file
    @warning low performance
*/
class JsonFile_safe{

    /// download json file into json 
    static json getFile(const Json_SafeFileName&);
    
    //upload json to file
    static void saveFile(const Json_CreateFileName&, const json&);
};


/*!
    @brief class for working with json file
    @warning can be unsafe
*/
class JsonFile_fast{

    /// download json file into json 
    static json getFile(const Json_FileName&);
    
    //upload json to file
    static void saveFile(const Json_FileName&, const json&);
};



//TODO 
/*

/*!
    @brief class for working with lua files
*//*
class ScriptFile{

    /// download lua file into lua class
    static LuaScript getFile(const Usual_SafeScriptName&);
    
    //upload lua to file
    static void saveFile(const Usual_CreateScriptName&, const LuaScript&);
};
*/
//

};
#pragma once
#include <string>
#include <memory>
#include <fstream>
#include "localTime.h"

namespace eff{

/*!
@brief class for singlethread logging in file
@example 
Log::WriteTo().warning("hello world", true); //will write warning to the last or defoult file
Log::WriteTo("texas.txt").log("MAU MAU!!"); //will write log "MAU MAU!!" without time in file (prefix_namefile+"texas.txt"). And set this file as default
@throw runtime_error -File can not be open. it`s impossible to write log.
*/
class Log{
public:

    /*!
    @brief static function to call all class. Remember last file 
    to write and change default.
    @warning all log will move in folder "logs". it can be change in prefix_namefile
    @param[in] log_file_name {filename to write a log}
    \throw std::runtime_error if unable open current file
    */
    static Log& WriteTo(std::string log_filename = "");


    ///write log
    void log(const std::string& log_text, bool show_time = false);


    ///write warning
	void warning(const std::string& warning_text, bool show_time = false);


    ///clear current file
    void clear();


    ~Log();

    /// simple Exeption with writing error to last log file
    class Exception{
    public:
        Exception(const std::string& error, bool show_time = false);
    };

private:

#ifdef WIN_OS
    std::string prefix_namefile = "logs\\";
#else  //LINUX_OS or MAC_OS
    std::string prefix_namefile = "logs/";
#endif

    Log(); 

    ///can use only for Exeption class
    void error(const std::string& error_text, bool show_time = false);

    static std::unique_ptr<Log> log_ptr;    //< ptr of class
    std::string current_log_file_name = ""; //< name of last file
    std::ofstream file_toWrite;             //< object for writing in file
    eff::LocalTime localTime;               //< local time variable
};


};
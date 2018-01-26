#pragma once
#include <string>
#include <memory>
#include <fstream>

namespace eff{



class Log{
public:

    /*!
    @brief static function to call all class. Remember last file 
    to write and change default to last file for writing.
    @param[in] log_file_name {filename to write a log}
    \throw std::runtime_error if unable open current file
    */
    static Log& WriteTo(std::string log_file_name = "default_log.txt");


    ///write log
    void log(std::string log_text, bool show_time = false);


    ///write warning
	void warning(std::string warning_text, bool show_time = false);


    ///clear current file
    void clear();

    ~Log();

    /// simple Exeption with writing error to last log file
    class Exception{
    public:
        Exception(std::string error, bool show_time = false);
    };

private:

    Log();

    ///can use only for Exeption class
    void error(std::string error_text, bool show_time = false);

    static std::unique_ptr<Log> log_ptr;    //< ptr of class
    std::string current_log_file_name = ""; //< name of last file
    std::ofstream file_toWrite;             //< object for writing in file
    
};


};
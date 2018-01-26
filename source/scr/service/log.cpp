#include "service/log.h"
#include <string>
#include <memory>
#include <stdexcept>

using namespace eff;
using namespace std;


std::unique_ptr<Log> Log::log_ptr;

Log& Log::WriteTo(string log_filename){
    
    if(log_ptr == nullptr){
        log_ptr = unique_ptr<Log>(new Log());
        if(log_filename == ""){
            log_ptr->current_log_file_name = "log.txt";
            log_ptr->file_toWrite.open(log_ptr->prefix_namefile + log_ptr->current_log_file_name);
            goto pass_default_mechanism;
        }
    }

    if(!( (log_filename == log_ptr->current_log_file_name) || (log_filename == "") )){
        if(log_ptr->file_toWrite.is_open())
            log_ptr->file_toWrite.close();
        log_ptr->current_log_file_name = log_filename;
        log_ptr->file_toWrite.open(log_ptr->prefix_namefile + log_ptr->current_log_file_name);
    }

    pass_default_mechanism:;
    if(!log_ptr->file_toWrite.is_open())
        throw runtime_error("file can not be open. it`s impossible to write log.");

    return *log_ptr;
}


Log::~Log(){
    if(log_ptr != nullptr)
        log_ptr->file_toWrite.close();
}

Log::Log(){
}


void Log::log(const string& log_text, bool show_time){
    if(show_time){
        file_toWrite << "LOG|" << localTime(LocalTime::TimeType::Hour) <<
                            ":" << localTime(LocalTime::TimeType::Minute) << 
                            ":" << localTime(LocalTime::TimeType::Hour) << 
                           "|> " << log_text << endl;
    }else{
        file_toWrite << "LOG|> " << log_text << endl;
    }
}

void Log::warning(const string& log_text, bool show_time){
    if(show_time){
        file_toWrite << "WARNING|" << localTime(LocalTime::TimeType::Hour) <<
                            ":" << localTime(LocalTime::TimeType::Minute) << 
                            ":" << localTime(LocalTime::TimeType::Hour) << 
                           "|> " << log_text << endl;
    }else{
        file_toWrite << "WARNING|> " << log_text << endl;
    }
}

void Log::error(const string& log_text, bool show_time){
    if(show_time){
        file_toWrite << "ERROR|" << localTime(LocalTime::TimeType::Hour) <<
                            ":" << localTime(LocalTime::TimeType::Minute) << 
                            ":" << localTime(LocalTime::TimeType::Hour) << 
                           "|> " << log_text << endl;
    }else{
        file_toWrite << "ERROR|> " << log_text << endl;
    }
}


void Log::clear(){
    file_toWrite.close();
    file_toWrite.open(log_ptr->prefix_namefile + log_ptr->current_log_file_name, std::ios_base::trunc);
}

Log::Exception::Exception(const string& error_text, bool show_time){    
    Log::WriteTo("error.txt").error(error_text, show_time);
}
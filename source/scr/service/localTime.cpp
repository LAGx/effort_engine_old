#include "service/localTime.h"
#include <string>
#include <ctime>
#include <stdexcept>

using namespace std;
using namespace eff;



string LocalTime::operator()(LocalTime::TimeType _timeType){
    
    rawtime = time(0);
    timeinfo = localtime(&rawtime);
    
    int str_time;

    switch(_timeType){
        case (TimeType::Second): 
            str_time = timeinfo->tm_sec;
            break;
        case (TimeType::Minute): 
            str_time = timeinfo->tm_min;
            break;
        case (TimeType::Hour): 
            str_time = timeinfo->tm_hour;
            break;
        case (TimeType::Day): 
            str_time = timeinfo->tm_mday;
            break;
        case (TimeType::Month): 
            str_time = timeinfo->tm_mon+1;
            break;
        case (TimeType::Year): 
            str_time = timeinfo->tm_year+1900;
            break;
        default:
            throw runtime_error("no such TimeType");
    }

    return to_string(str_time);
}
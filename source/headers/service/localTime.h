#include <string>
#include <ctime>

namespace eff{

/*!
@brief functor, that return time of local machine
@warning low performance. 
*/
class LocalTime{
public:

    enum class TimeType{
        Second = 0, //< seconds [0, 60]
        Minute,     //< minutes [0, 59]
        Hour,       //< hours   [0, 23]
        Day,        //< days    [1, 31]
        Month,      //< month   [1, 12]          -m
        Year        //< year    [in xxxx format] -m
    };

    /*!
    @param[in] timeType {what to return}
    @param[out] time in normal format
    */
    std::string operator()(LocalTime::TimeType timeType);

private:

    std::time_t rawtime;
    std::tm timeinfo;

};



};
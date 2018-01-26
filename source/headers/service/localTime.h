#include <string>
#include <ctime>

namespace eff{

/*!
@brief functor, that return time of local machine
@warning low performance. 
@example 
LocalTime loct;
loct(TimeType::Year); //will return 2018
*/
class LocalTime{
public:

    enum class TimeType{
        Second = 0, //< seconds [0, 60]
        Minute,     //< minutes [0, 59]
        Hour,       //< hours   [0, 23]
        Day,        //< days    [1, 31]
        Month,      //< month   [1, 12]          
        Year        //< year    [in xxxx format] 
    };

    /*!
    @param[in] timeType {what to return}
    @return time in normal format
    */
    std::string operator()(LocalTime::TimeType timeType);

private:

    std::time_t rawtime;
    std::tm* timeinfo;

};



};
#include <iostream>
#include <string>
#include <cmath>
#include "service/localTime.h"

using namespace std;
using namespace eff;

int main(int argc, char **argv) {
    
    LocalTime time;

    time(LocalTime::TimeType::Day);

    return 0;
}
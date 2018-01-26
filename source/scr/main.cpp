#include <iostream>
#include <string>
#include <cmath>
#include "service/log.h"

using namespace std;
using namespace eff;

int main() {
    
    Log::WriteTo("log1.txt").log("hello ", true);
    Log::WriteTo().log("hello  1", true);
    Log::WriteTo("new_loging.txt").warning("hello  2", true);

    return 0;
}
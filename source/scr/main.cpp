#include <iostream>
#include <string>
#include "log.h"

using namespace std;
using namespace eff;


int main(){

    
    cout << "hello world 245" << endl;
    double NOT_A = 12.85;

#ifdef DEBUG_MODE
    cout << "DEBUG_MODE" << endl;
#endif

#ifdef RELEASE_MODE
    cout << "RELEASE_MODE" << endl;
#endif

#ifdef WIN_OS
    cout << "WIN_OS" << endl;
#endif

#ifdef LINUX_OS
    cout << "LINUX_OS" << endl;
#endif

#ifdef MAC_OS
    cout << "MAC_OS" << endl;
#endif


    return 0;
}
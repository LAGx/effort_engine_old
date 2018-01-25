#include <iostream>

using namespace std;


int main(){

    int a = 92;
    cout << "hello world 17" << endl;
    

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
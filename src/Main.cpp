#include <iostream>
#include <vector>
#include <memory>
using namespace std;

#include "Engine.h"

int main(int argc, char *argv[])
{
#ifdef _MSC_VER
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF|_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF));
#endif
#endif

    // build argument list
    vector<string> args;
    for(int i=1; i<argc; i++)
        args.push_back(argv[i]);

    // create driver
    unique_ptr<Engine> pEngine(new Engine(args));
    if(!pEngine || pEngine->hasError()) {
        std::cout << "Engine failed to load." << endl;
        return 1;
    }
    
    // start driver
    return pEngine->run() ? 0 : 1;
}


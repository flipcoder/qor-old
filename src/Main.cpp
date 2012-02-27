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

    // init basic logging and settings systems
    Settings::get(new Settings("settings.ini"));
    Log::get(new Log());
    Log::get().setStdOut(Settings::get().getProperty("Console", "StandardOut", "true")=="true");
    Log::get().write("Logging system initialized.");

    // create driver
    try{
        Engine::get(new Engine(args));
    }catch(const IFallible::Failure& f) {
        if(Engine::ptr())
            Log::get().error(Engine::get().getError());
        return 1;
    }
    
    // start driver
    return Engine::get().run() ? 0 : 1;
}


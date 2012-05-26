#ifndef _SCRIPTABLE_H
#define _SCRIPTABLE_H

#include <string>
#include "IConfig.h"

class IScriptable: public IConfig
{
    private:
        
    public:
        IScriptable(const std::string& fn):
            IConfig(fn)
        {
            open(fn);
        }
        bool open(const std::string& fn) {
            
        }
        virtual ~IScriptable() {}
};

#endif


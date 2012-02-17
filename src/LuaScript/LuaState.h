#ifndef _LUASCRIPT_H
#define _LUASCRIPT_H

#include <lua.hpp>

//extern "C" {
//#include <lua.h>
//#include <lualib.h>
//#include <lauxlib.h>
//}

class LuaState
{
    private:

        lua_State* m_pState;
        bool m_bError;

    public:

        LuaState() {
            m_bError = false;
            m_pState = lua_open();
            luaopen_base(m_pState);
            luaopen_table(m_pState);
            //luaopen_io(m_pState);
            luaopen_string(m_pState);
            luaopen_math(m_pState);
            luaopen_debug(m_pState);
        }

        virtual ~LuaState(){
            if(m_pState)
                lua_close(m_pState);
        }

        inline operator lua_State*() {
            return m_pState;
        }

        bool run(const std::string& fn){
            return luaL_dofile(m_pState, fn.c_str()) == 0;
        }

        bool error() const { return m_bError; }
        void error(bool b) { m_bError = b; }
};

#endif


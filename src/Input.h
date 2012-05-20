#ifndef _INPUT_H
#define _INPUT_H
#pragma once

// This is a really old, basic Input system

#include <string>
#include <memory>
#include "GfxAPI.h"
#include "Settings.h"
#include "IRealtime.h"

class Input : public IRealtime
{
    //protected:
    public:
        Input();
        virtual ~Input() {}
        
        enum Button
        {
            UP,
            DOWN,
            LEFT,
            RIGHT,
            ACTION,
            MAX_BUTTONS
        };

        // TODO: We need string representation of all of the keys, for parsing cfgs and possibly outputing key names
        // TODO: Add enumeration of input keys to hide SDLK and make scriptable/configurable (?)
        
    private:
    
        /*enum eKeyMode
        {
            kmNormal,
            kmUnicode
        } m_keyMode;*/
        
        static const int MAX_INPUT_CONTROLLERS = 1;
        struct InputController
        {
            InputController() {
                for(unsigned int i=0;i<MAX_BUTTONS;i++)
                    btn[i] = 0;
            }
            unsigned int btn[MAX_BUTTONS];
        };
        InputController inputController[MAX_INPUT_CONTROLLERS];
    
        static const unsigned int MAX_KEYS = 512;
        bool m_key[MAX_KEYS];

        std::string m_tempString;
        //std::string *m_destString;
        std::weak_ptr<std::string> m_wpDestString;
        bool m_bGrabString;
        
        int mouse_x;
        int mouse_y;
        int mouse_rel_x;
        int mouse_rel_y;
        
        bool m_bQuit;

        bool m_bMouseHidden;
        bool m_bMouseLeft;
        bool m_bMouseLeftFrame;
        bool m_bMouseRightFrame;
        bool m_bMouseMiddleFrame;
        bool m_bMouseLeftRelease;
        bool m_bMouseRightRelease;
        bool m_bMouseMiddleRelease;
        bool m_bMouseRight;
        bool m_bMouseMiddle;
        bool m_bMouseWheelUp;
        bool m_bMouseWheelDown;
        float m_fMouseSens;
        
        // Add joystick flags here
        void backspaceString();
        void grabChar(int);
        
    public:
    
        //int loadConfig();
        //void saveConfig();
        
        int keyRepeat(int delay = SDL_DEFAULT_REPEAT_DELAY,
            int interval = SDL_DEFAULT_REPEAT_INTERVAL);
    
        int getMouseX() const { return (m_bMouseHidden?-1:mouse_x); }
        int getMouseY() const { return (m_bMouseHidden?-1:mouse_y); }
        float getMouseRelX() const { return mouse_rel_x * m_fMouseSens; }
        float getMouseRelY() const { return mouse_rel_y * m_fMouseSens; }
        bool isMouseHidden() const { return m_bMouseHidden; }
        void toggleHideMouse() {
            hideMouse(!m_bMouseHidden);
        }
        void hideMouse(bool b);
        //VideoRect getMouseHotSpot();
    
        bool key(int key) const;
        bool keyd(int key) { bool b = m_key[key]; m_key[key]=false; return b; }
        void disableKey(int key) { m_key[key]=false; }
        virtual void logic(unsigned int t);
        void clearKeys();
        
        //void getString(std::string* str, std::string app = "");
        void getString(std::shared_ptr<std::string>& str, std::string app = "");
        bool isEnteringString() const;
        
        void endStringGrab();
        void clearStringGrab();
        
        bool mouseLeftClick() { return m_bMouseLeft; }
        bool mouseRightClick() { return m_bMouseRight; }
        bool mouseMiddleClick() { return m_bMouseMiddle; }

        bool mouseAnyf() const {
            return m_bMouseLeftFrame || m_bMouseRightFrame || m_bMouseMiddleFrame;
        }
        bool mouseLeftClickd() {
            bool b = m_bMouseLeft;
            m_bMouseLeft = false;
            return b;
        }
        bool mouseLeftClickf() const {
            return m_bMouseLeftFrame;
        }
        bool mouesLeftRelease() const{
            return m_bMouseLeftRelease;
        }
        bool mouseRightClickd() {
            bool b = m_bMouseRight;
            m_bMouseRight = false;
            return b;
        }
        bool mouseRightClickf() const {
            return m_bMouseRightFrame;
        }
        bool mouseRightRelease() const {
            return m_bMouseRightRelease;
        }
        bool mouseMiddleClickd() {
            bool b = m_bMouseMiddle;
            m_bMouseMiddle = false;
            return b;
        }
        bool mouseMiddleRelease() const{
            return m_bMouseMiddleRelease;
        }
        bool mouseMiddleClickf() const {
            return m_bMouseMiddleFrame;
        }
        bool mouseWheelUpd() {
            bool b = m_bMouseWheelUp;
            m_bMouseWheelUp=false;
            return b;
        }
        bool mouseWheelDownd() {
            bool b = m_bMouseWheelDown;
            m_bMouseWheelDown=false;
            return b;
        }

        bool quit() const { return m_bQuit; }

        InputController* getControl(int idx) { return &inputController[idx]; }
        
        bool btn(int controllerID, Button btnidx) const {
            return (m_key[inputController[controllerID].btn[btnidx]]);
        }

};

#endif

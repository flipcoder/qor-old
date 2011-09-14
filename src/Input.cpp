#include "GfxAPI.h"
#include "Input.h"

#include <iostream>
using namespace std;

Input :: Input()
{
    clearKeys();
    //m_keyMode = kmNormal;
    m_tempString = "";
    //m_destString = NULL;
    
    m_bGrabString = false;
    m_bMouseHidden = false;
    m_bMouseLeft = false;
	m_bMouseLeftFrame = false; // reset every frame
	m_bMouseRightFrame = false; // reset every frame
	m_bMouseMiddleFrame = false; // reset every frame
	m_bMouseLeftRelease = false; // reset every frame
	m_bMouseRightRelease = false; // reset every frame
	m_bMouseMiddleRelease = false; // reset every frame
    m_bMouseRight = false;
    m_bMouseMiddle = false;
    m_bMouseWheelUp = false;
    m_bMouseWheelDown = false;
    
	//SDL_WarpMouse(0,0);
	//SDL_WM_GrabInput( SDL_GRAB_ON );
    mouse_x = 0;
    mouse_y = 0;
    mouse_rel_x=0;
    mouse_rel_y=0;
	m_fMouseSens = 1.0f;
    
	// setup some default binds
    inputController[0].btn[LEFT] = SDLK_w;
    inputController[0].btn[RIGHT] = SDLK_d;
    inputController[0].btn[DOWN] = SDLK_s;
    inputController[0].btn[UP] = SDLK_w;
    inputController[0].btn[ACTION] = SDLK_SPACE;

	SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);

	keyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
}

/*VideoRect Input :: getMouseHotSpot()
{
    return cVideo::makeRect(mouse_x,mouse_y,2,2);
}*/

void Input :: hideMouse(bool b)
{
	SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    m_bMouseHidden = b;
    SDL_ShowCursor(!b);
	if(b)
		SDL_WM_GrabInput(SDL_GRAB_ON);
	else
	    SDL_WM_GrabInput(SDL_GRAB_OFF);
	SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
}

int Input :: keyRepeat(int delay, int interval)
{
    return SDL_EnableKeyRepeat(delay,interval);
}

void Input :: clearKeys()
{
    for(int i=0;i<MAX_KEYS;i++)
        m_key[i] = false;
}

bool Input :: key(int key) const
{
	// TODO: add gamepad and other buttons
	return m_key[key];
}

int Input :: logic()
{
    SDL_Event ev;
    
    mouse_rel_x = 0;
    mouse_rel_y = 0;
	m_bMouseLeftFrame = false;
	m_bMouseRightFrame = false;
	m_bMouseMiddleFrame = false;
	m_bMouseLeftRelease = false;
	m_bMouseMiddleRelease = false;
	m_bMouseRightRelease = false;

    while (SDL_PollEvent(&ev))
    {
        switch (ev.type)
        {
            case SDL_QUIT:
                return 1;
                break;

            case SDL_KEYDOWN:
            {
                m_key[ev.key.keysym.sym] = true;
                
                if(isEnteringString())
                {
                    grabChar(ev.key.keysym.sym);
                    switch(ev.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            clearStringGrab();
                            m_key[ev.key.keysym.sym] = true;
                            break;
                        case SDLK_RETURN:
                            endStringGrab();
                            m_key[ev.key.keysym.sym] = true;
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    if (ev.key.keysym.sym == SDLK_ESCAPE)
                        return 1;
                }
                
                break;
            }
            case SDL_KEYUP:
            {
                m_key[ev.key.keysym.sym] = false;
                break;
            }
            case SDL_MOUSEMOTION:
                mouse_x = ev.motion.x;
                mouse_y = ev.motion.y;
				
				if(m_bMouseHidden)
				{
					// incrementing here (zeroing outside while() loop) because:
					// PollEvent might send multiple events, resulting in lost movement
					mouse_rel_x += ev.motion.xrel;
					mouse_rel_y += ev.motion.yrel;
				}
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                if(ev.button.button==SDL_BUTTON_LEFT)
                    m_bMouseLeft = m_bMouseLeftFrame = true;
                if(ev.button.button==SDL_BUTTON_RIGHT)
                    m_bMouseRight = m_bMouseRightFrame = true;
                if(ev.button.button==SDL_BUTTON_MIDDLE)
                    m_bMouseMiddle = m_bMouseMiddleFrame = true;
                    
                if(ev.button.button==SDL_BUTTON_WHEELUP)
                    m_bMouseWheelUp = true;
                if(ev.button.button==SDL_BUTTON_WHEELDOWN)
                    m_bMouseWheelDown = true;
                break;
            case SDL_MOUSEBUTTONUP:
                if(ev.button.button==SDL_BUTTON_LEFT) {
                    m_bMouseLeft = false;
					m_bMouseLeftRelease = true;
				}
                if(ev.button.button==SDL_BUTTON_RIGHT) {
                    m_bMouseRight = false;
					m_bMouseRightRelease = true;
				}
                if(ev.button.button==SDL_BUTTON_MIDDLE) {
                    m_bMouseMiddle = false;
					m_bMouseMiddleRelease = true;
				}
                
                /*if(ev.button.button==SDL_BUTTON_WHEELUP)
                    m_bMouseWheelUp = false;
                if(ev.button.button==SDL_BUTTON_WHEELDOWN)
                    m_bMouseWheelDown = false;*/
                break;
        }
    }
    
    /*if(m_bGrabString)
        if(m_key[SDLK_BACKSPACE])
        {
            backspaceString();
        }*/

    return 0;
}

//void Input :: getString(string* str, string app)
//{
//    if(!str) return;
    
//    m_destString = str;
//    m_tempString += app;
//    m_bGrabString = true;
//}

void Input :: getString(shared_ptr<string>& str, string app)
{
	if(!str) return;
	
	//m_destString = str.get();
	m_wpDestString = str;
    m_tempString += app;
    m_bGrabString = true;
}

bool Input :: isEnteringString() const
{
	return !m_wpDestString.expired();
}

void Input :: endStringGrab()
{
    //*m_destString = m_tempString;
	shared_ptr<string> dest_string(m_wpDestString);
	if(dest_string)
		*dest_string = m_tempString;
		
	m_wpDestString.reset();
    m_tempString="";
}

void Input :: clearStringGrab()
{
	m_tempString = "";
	
	shared_ptr<string> dest_string(m_wpDestString);
	if(dest_string)
		*dest_string = "";
	
	m_wpDestString.reset();
}


void Input :: backspaceString()
{
    if(!m_tempString.empty())
        m_tempString.erase(m_tempString.length()-1);
}

void Input :: grabChar(int keycode)
{
    // String entering mode
    if(isEnteringString())
    {
        if(keycode==SDLK_BACKSPACE)
        {
            if(!m_tempString.empty())
                m_tempString.erase(m_tempString.length()-1);
        }
        else
        {
            bool caps_flag = false;
            
            if((keycode!=SDLK_RETURN)
                &&(keycode!=SDLK_BACKSPACE)
                &&(keycode!=SDLK_ESCAPE)
                &&(keycode!=SDLK_RSHIFT)
                &&(keycode!=SDLK_LSHIFT)
                &&(keycode!=SDLK_CAPSLOCK)
                &&(keycode!=SDLK_RCTRL)
                &&(keycode!=SDLK_LCTRL)
                &&(keycode!=SDLK_RALT)
                &&(keycode!=SDLK_LALT)
                &&(keycode!=SDLK_RSUPER)
                &&(keycode!=SDLK_LSUPER)
                &&(keycode!=SDLK_RMETA)
                &&(keycode!=SDLK_LMETA)
                &&(keycode!=SDLK_TAB))
            {
                
                if( key(SDLK_RSHIFT) || key(SDLK_LSHIFT) )
                {
                    if((keycode>='a')&&(keycode<='z'))
                    //if(keycode>32)
                    {
                        keycode-=32; // caps
                        caps_flag=true;
                    }
                    
                    if(keycode==SDLK_SLASH)
                    {
                        m_tempString+='?';
                        disableKey(keycode);
                        return;
                    }
                    if(keycode==SDLK_MINUS)
                    {
                        m_tempString+='_';
                        disableKey(keycode);
                        return;
                    }
                }
                
                // translate numpad into numbers
                if(keycode>=SDLK_KP0 && keycode<=SDLK_KP9)
                {
                    int kc = keycode - SDLK_KP0 + SDLK_0;
                    m_tempString += (char)kc;
                    disableKey(keycode);
                    return;
                }
                
                m_tempString += (char)keycode;
                disableKey(caps_flag?(keycode+32):keycode);
                
            }
        }
    }
}


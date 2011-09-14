// Author: Grady O'Connell
// URL: http://www.prolificaxis.com
// SDL Timer wrapper with interpolation

#include "Freq.h"
#include "GfxAPI.h" // for SDL_GetTicks()

void Freq :: set(int logicTickSpeed)
{
    if(logicTickSpeed<=0)
        return;
    
    m_iLogicTickSpeed = logicTickSpeed;
    m_dTimeBetweenTicks = 1000.00 / m_iLogicTickSpeed;
    m_ulStartTime = SDL_GetTicks();
    
    m_ulTicks = 0L;
}

unsigned long Freq :: getElapsedTime() const
{
    return (SDL_GetTicks() - m_ulStartTime);
}

double Freq :: getElapsedSeconds() const
{
    return (((double)getElapsedTime()) / 1000.00);
}

bool Freq :: tick()
{
    
    if ( getElapsedTime() / m_dTimeBetweenTicks > m_ulTicks )
    {
        ++m_ulTicks;
		return true;
    }
    
    return false;
}

float Freq :: interp() const
{
	float erp = (getElapsedTime() - ((m_ulTicks-1)*(float)m_dTimeBetweenTicks))/(float)m_dTimeBetweenTicks;
	
	if(erp<0.0000)
		return 0.0f;
	if(erp>1.0000)
		return 1.0f;
	return erp;
}

//void Freq :: pause()
//{
//    m_bPaused = true;
//}

//void Freq :: unpause()
//{
//    //m_ulTicks = (getElapsedTime() / m_dTimeBetweenTicks - 1) * 1.0;
//    m_bPaused = false;
//}


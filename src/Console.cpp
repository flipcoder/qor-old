#include "Console.h"
#include "Version.h"
#include "Util.h"

#include <algorithm>
using namespace std;

Console :: Console()
{
    nullify();

    m_spFont.reset(new FTGLPixmapFont("data/base/fonts/liberation/LiberationMono-Regular.ttf"));
    if( !m_spFont.get() ||
         m_spFont->Error() ||
        !m_spFont->FaceSize(FONT_HEIGHT))
    {
        setError("Failed to load font.");
    }


    //m_pFont = new Font();
    //if(!m_pFont->load("data/fonts/lucida12b.glf")) {
    //    Log::get().error("Could not load console font.");
    //}
}

Console :: ~Console()
{
}

void Console :: nullify()
{
    m_bAutoScroll = true;
    m_bVisible = false;
    m_bError = false;
    m_fDrop = 0.0f;
    m_Scroll = 0;
}

int Console ::logic(unsigned int a, Input* input)
{
    const float DROP_SPEED = 0.01f;

    if(input->keyd(SDLK_BACKQUOTE))
        m_bVisible = !m_bVisible;
    
    if(m_bVisible)
    {
        if(m_fDrop < 1.0f)
            m_fDrop += DROP_SPEED * a;
        if(m_fDrop > 1.0f)
            m_fDrop = 1.0f;

        if(input->keyd(SDLK_UP))
            scroll(-1);
        if(input->keyd(SDLK_DOWN))
            scroll(1);
    }
    else
    {
        if(m_fDrop > 0.0f)
            m_fDrop -= DROP_SPEED * a;
        if(m_fDrop < 0.0f)
            m_fDrop = 0.0f;
    }

    return 0;
}

void Console :: render() const
{
    unsigned int w = Renderer::get().width();
    unsigned int h = Renderer::get().height();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE); // screen?

    // bottom left "origin" coord
    int y_origin = h - round_int(m_fDrop*h/2.0f);

    glBegin(GL_QUADS);
        glColor4f(0.0f, 0.0f, 0.9f, 1.0f);
        glVertex2i(0, y_origin + FONT_HEIGHT);
        glVertex2i(w, y_origin + FONT_HEIGHT);
        glColor4f(0.0f, 0.0f, 0.2f, 1.0f);
        glVertex2i(w, y_origin + h/2);
        glVertex2i(0, y_origin + h/2);
    glEnd();

    glBegin(GL_QUADS);
        glColor4f(0.0f, 0.0f, 0.8f, 1.0f);//0.5
        glVertex2i(0, y_origin);
        glVertex2i(w, y_origin);
        glColor4f(0.0f, 0.0f, 0.2f, 1.0f);//0.1
        glVertex2i(w, y_origin + FONT_HEIGHT);
        glVertex2i(0, y_origin + FONT_HEIGHT);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    //glDisable(GL_BLEND);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    if(m_fDrop > 0.0f)
    {
        // iterate through log

        //const int SPACING = 18;
        int i = 0;
        
        //for(auto itr = m_pLog->getBuffer()->begin();
        //        itr != m_pLog->getBuffer()->end();
        //        i += SPACING, itr++)
        for(unsigned int i = 0;
            i < h / 2 / max(FONT_HEIGHT - 1, 1);
            i++)
        {
            m_spFont->Render(
                Log::get().message(scroll() - h/2/FONT_HEIGHT + i + 1).c_str(),
                -1,
                FTPoint(
                        0.0f,
                        y_origin  + ((float)h/2.0f) - (i*FONT_HEIGHT)*1.0f
                    )
                );
        }

        glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
        //m_pFont->writeStringRight(
        //    PROGRAM_NAME + " " + PROGRAM_VERSION,
        //    w*1.0f, y_origin + FONT_HEIGHT*2.0f);
    }

    glDisable(GL_BLEND);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

int Console :: scroll() const
{
    if(m_Scroll > size())
       m_Scroll = size();
    else if(m_bAutoScroll)
       m_Scroll = size();
    return m_Scroll;
}

void Console :: scroll(int v)
{
    int new_scroll;
    if(v<0 && -v>m_Scroll)
        new_scroll = 0;
    else
        new_scroll = m_Scroll + v;

    if(v < 0)
        m_bAutoScroll = false;

    if(new_scroll < 0)
        new_scroll = 0;
    else if(new_scroll > size())
        new_scroll = size();

    m_Scroll = new_scroll;

    if(m_Scroll == size())
        m_bAutoScroll = true;
}


#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <FTGL/ftgl.h>

#include "Log.h"
#include "Renderer.h"
#include "Input.h"
#include "IFallible.h"

class Console : public IFallible
{
    static const int FONT_HEIGHT = 18;

    //Font* m_pFont;
    std::shared_ptr<FTGLPixmapFont> m_spFont;
    bool m_bVisible;
    bool m_bAutoScroll;
    mutable int m_Scroll;
    bool m_bError;

    // how far down console is (0-1) in drop transitions
    float m_fDrop;

public:

    Console();
    virtual ~Console();

    void nullify();

    int logic(unsigned int a, Input* input);
    void render() const;

    void visible(bool b) { m_bVisible=b; }
    bool visible() { return m_bVisible; }

    void scroll(int v);
    int scroll() const;

    int size() const {
        return Log::get().getBuffer_c()
            ?Log::get().getBuffer_c()->size()
            :0;
    }
};

#endif


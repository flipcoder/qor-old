#ifndef _TRANSITIONINFO_H
#define _TRANSITIONINFO_H

class TransitionInfo
{
    public:
        
        TransitionInfo() {
            m_fProgress = 0.0f;
            m_bDone = false;
            m_bKillFlag = false;
        }
        
        // this is not thread-side, its to tell the thread to stop
        void kill(bool b){
           m_bKillFlag = b;
        }
        bool kill() const {
           return m_bKillFlag;
        }
        
        void progress(float f) {
            m_fProgress = f;
        }
        
        float progress() const {
            return m_fProgress;
        }
        
        bool done() const {
            return m_bDone;
        }
        
        void done(bool b) {
            m_bDone = b;
        }
    
    private:
        float m_fProgress;
        bool m_bDone;
        bool m_bKillFlag;
};

#endif


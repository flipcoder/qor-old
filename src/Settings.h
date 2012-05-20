#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <string>
#include <list>
#include "PropertyList.h"
#include "IStaticInstance.h"

class Settings : public IStaticInstance<Settings>
{
public:

    class Listener
    {
        private:

            Settings* m_pSettings;
            std::string m_sGroup;

        public:
            
            Listener() {
                m_pSettings = NULL;
                m_sGroup = "";
            }
            virtual ~Listener() {}
            void listen(Settings* settings, std::string group) {
                m_pSettings = settings;
                m_sGroup = group;
            }
    };

private:
    
    PropertyList m_Properties;
    std::string m_FileName;
    bool m_bError;

    std::multimap<std::string, Listener*> m_Listeners;

public:

    Settings(std::string fn = "");
    virtual ~Settings();

    void nullify();
    bool open(std::string fn);
    bool save();

    bool error() { return m_bError; }

    void update();

    //void addListener(Listener* n) {
    //    //if(m_Listeners.find(n) != m_Listeners.end() && n)
    //    m_Listeners.push_back(n);
    //}
    //void clearListeners() {
    //    m_Listeners.clear();
    //}

    std::string getProperty(std::string c, std::string p, std::string def);
    float getProperty(std::string c, std::string p, float def);
    int getProperty(std::string c, std::string p, int def);
    void setProperty(std::string c, std::string p, std::string val);
    void setProperty(std::string c, std::string p, int val);
    void setProperty(std::string c, std::string p, float val);
};

#endif


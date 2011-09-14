#include "Settings.h"
#include "Util.h"
//#include <boost/property_tree/ini_parser.hpp>

Settings :: Settings(std::string fn)
{
	nullify();
	m_FileName = fn;

	if(!open(fn))
		m_bError = true;
}

Settings :: ~Settings()
{

}


void Settings :: nullify()
{
	m_bError = false;
}

void Settings :: update()
{
	//foreach(Listener* listener, m_Listeners)
	//    listener->update();
}

bool Settings :: open(std::string fn)
{
	m_FileName = fn;
	return m_Properties.open(m_FileName.c_str());
}

bool Settings :: save()
{
	m_Properties.save(m_FileName.c_str());
	return true;
}

std::string Settings :: getProperty(std::string c, std::string p, std::string def)
{
	std::string v;
	if(!m_Properties.getStringValue(c.c_str(), p.c_str(), v))
	{
		m_Properties.setStringValue(c.c_str(), p.c_str(), def);
		return def;
	}
	return v;
}

int Settings :: getProperty(std::string c, std::string p, int def)
{
	int v;
	if(!m_Properties.getIntValue(c.c_str(), p.c_str(), v))
	{
		m_Properties.setIntValue(c.c_str(), p.c_str(), def);
		return def;
	}
	return v;
}

float Settings :: getProperty(std::string c, std::string p, float def)
{
	float v;
	if(!m_Properties.getFloatValue(c.c_str(), p.c_str(), v))
	{
		m_Properties.setFloatValue(c.c_str(), p.c_str(), def);
		return def;
	}
	return v;
}


void Settings :: setProperty(std::string c, std::string p, std::string val)
{
	m_Properties.setStringValue(c.c_str(), p.c_str(), val);
}

void Settings :: setProperty(std::string c, std::string p, int val)
{
	m_Properties.setIntValue(c.c_str(), p.c_str(), val);
}

void Settings :: setProperty(std::string c, std::string p, float val)
{
	m_Properties.setFloatValue(c.c_str(), p.c_str(), val);
}


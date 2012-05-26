#ifndef _DYNAMICTYPING_H
#define _DYNAMICTYPING_H

// Provides dynamic classification (attributes, flags, inheritence) of objects for
//  scripting.  Users can define types and flags and tag them to existing scriptable
//  objects for better user game logic.

#include <vector>
#include <map>
#include <algorithm>
#include <boost/dynamic_bitset.hpp>
#include "IScriptable.h"

//  rules->addType("rocket")
//  rules->classSpec("rocket")->inherit("ammo")
//  inventory->addItem("rocket", 5);
//  if (inventory->anyOfType("ammo")) {
//    // etc...
//  }

class DynamicType
{
    public:

        DynamicType(const std::string& name):
            m_sName(name)
        {}
        virtual ~DynamicType() {}

        bool inherit(unsigned int id) {
            if(std::find(m_Types.begin(), m_Types.end(), id) != m_Types.end())
                return false;
            m_Types.push_back(id);
            return true;
        }
        unsigned int id() const { return m_ID; }
        const std::string& name() const { return m_sName; }

    private:

        unsigned int m_ID; // ID of type, also index in DynamicContext' class member m_Types

        std::string m_sName; // name for scripting and lookup
        std::vector<unsigned int> m_Types; // inherited type info

        // User data (varies depending on type)
        std::map<unsigned int, std::vector<unsigned int>> m_Tags; // type -> user tags for type
        std::map<unsigned int, std::string> m_TagNames;

        boost::dynamic_bitset<> m_Flags;
        std::map<unsigned int, std::string> m_FlagNames;
};

class DynamicContext : public IScriptable
{
    private:
        
    public:

        std::vector<DynamicType> m_Types; // indexed by type id (array position)

        DynamicContext(std::string fn):
            IScriptable(fn)
        {
            m_Types.push_back(DynamicType("object"));
        }
        virtual ~DynamicContext() {}

        unsigned int addType(DynamicType spec) {
            m_Types.push_back(spec);
            return m_Types.size()-1;
        }

        unsigned int type(std::string name) {
            for(auto itr = m_Types.begin();
                itr != m_Types.end();
                ++itr)
            if(itr->name() == name)
                return 0;
            else
                return itr->id();
        }

        // makes a new type that inherits from other types
        unsigned int inherit(std::string name, std::vector<unsigned int> types);

        // deep lookup if a type matches another type
        //  warning: one-way relationship possible here
        //  square is rectangle, rectangle is not always square, etc.
        bool isType(unsigned int type, unsigned int type_cmp);
};

#endif


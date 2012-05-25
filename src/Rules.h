#ifndef _RULES_H
#define _RULES_H

#include <vector>
#include <boost/dynamic_bitset.hpp>
// Rules
//  Lays out the game mechanics, items, weapons, and provides scriptable interface to them

// eventually stuff like this (less stringly tho, using ID system preferably):
//  rules->addType("rocket")
//  rules->classSpec("rocket")->inherit("ammo")
//  inventory->addItem("rocket", 5);
//  if (inventory->anyOfType("ammo")) {
//    // etc...
//  }

class ClassSpec
{
    public:

        ClassSpec(const std::string& name):
            m_sName(name)
        {}
        virtual ~ClassSpec() {}

        bool inherit(unsigned int id) {
            if(m_Types.find(id) != m_Types.end())
                return false;
            m_Types.push_back(id);
                return true;
        }
        unsigned int id() const { return m_ID; }
        const std::string& name() const { return m_sName; }

    private:

        unsigned int m_ID; // ID of type, also index in Rules' class member m_Types

        std::string m_sName; // name for scripting and lookup
        std::vector<unsigned int> m_Types; // inherited type info

        // User data (varies depending on type)
        std::map<unsigned int, std::vector<unsigned int>> m_Tags; // type -> user tags for type
        std::map<unsigned int, std::string> m_TagNames;

        boost::dynamic_bitset<> m_Flags;
        std::map<unsigned int, std::string> m_FlagNames;
}

class Rules : public IScriptable
{
    private:
        
    public:

        std::vector<ClassSpec> m_Types; // indexed by type id (array position)

        Rules(std::string fn):
            IScriptable(fn)
        {
            m_Types.push_back(ClassSpec("object"));
        }
        virtual ~Rules() {}

        unsigned int addType(ClassSpec spec) {
            m_Types.push_back(spec);
            return m_Types.size()-1;
        }

        unsigned int type(std::string name) {
            for(auto itr = m_Types.begin();
                itr != m_Types.end();
                ++itr)
            if(spec.name() == name)
                return 0;
            else
                return itr->id();
        }

        // makes a new type that inherits from other types
        unsigned int inherit(std::string name, std::vector<unsigned int> types) {
            // TODO: 
            return 0;
        }

        // deep lookup if a type matches another type
        //  warning: one-way relationship possible here
        //  square is rectangle, rectangle is not always square, etc.
        bool isType(unsigned int type) const {
            
        }
};

#endif


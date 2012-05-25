#ifndef _INVENTORY_H
#define _INVENTORY_H

#include <vector>

class TypeSpec;
class Inventory
{
    private:

        // item primary type, quantity
        std::map<unsigned int, unsigned int> m_Contents;

    public:

        Inventory(unsigned int id) {}
        virtual ~Inventory() {}

        bool has(unsigned int global_id) const {
            return m_Contents.find(global_id) != m_Contents.end();
        }

        // adding directly does not follow rules
        bool add(unsigned int type, unsigned int quantity = 1) {
            if(m_Contents.find(type) == m_Contents.end())
                return false;
            m_Contents.push_back(type);
            return true;
        }

        bool remove(unsigned int type) {
            auto itr = m_Contents.find(type);
            if(itr == m_Contents.end())
                return false;
            m_Contents.erase(itr);
            return true;
        }

        bool anyOfType(const TypeSpec& type) const {
            // TODO: quick pass to check for primary classes of items,
            // then pass again for deep inspection
            return false;
        }

        unsigned int numOfType(const TypeSpec& type) const {
            // TODO: Do a deep type inspection for each contents entry
            //   and return all that match with type
            return 0;
        }

        std::vector<unsigned int> getMatches(const TypeSpec& type) const {
            // TODO: get a list of each object matching the type
            //  do deep inspection
            return std::vector<unsigned int>();
        }

        bool empty() const { return m_Contents.empty(); }
};

#endif


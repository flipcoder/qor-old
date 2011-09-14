#ifndef _EDITOR_SELECTION_H
#define _EDITOR_SELECTION_H

#include "SelectorNode.h"
#include <memory>

class Selection
{
	public:
		
		bool add(Node* target) {
			if(!target || selected(target))
				return false;
			
			// TODO: if target type is SUBENVIRONMENT, redirect action to superparent
			// TODO: change of idea, add superParent() method to Node class
			std::shared_ptr<Node> sp(new SelectorNode());
			target->add(sp);
			m_SelectedNodes.push_back(std::weak_ptr<Node>(sp));
			return true;
		}
		
		bool selected(Node* target) {
			if(!target)
				return false;
			for(auto itr = m_SelectedNodes.begin();
				itr != m_SelectedNodes.end();
				)
			{
				shared_ptr<Node> sp;
				if((sp = itr->lock()))
				{
					if(sp.get() == target)
						return true;
				}
				else
				{
					itr = m_SelectedNodes.erase(itr);
					continue;
				}
				++itr;
			}
		}
		
		bool remove(Node* target) {
			if(!target)
				return false;
			
			for(auto itr = m_SelectedNodes.begin();
				itr != m_SelectedNodes.end();
				)
			{
				//if((*itr)->expired) {
				//    itr = m_SelectedNodes.erase(itr);
				//    continue;
				//}
					
				shared_ptr<Node> sp;
				if((sp = itr->lock()))
				{
					if(sp.get() == target)
					{
						itr = m_SelectedNodes.erase(itr);
						return true;
					}
				}
				else
					itr = m_SelectedNodes.erase(itr);
					
				++itr;
			}
			return false;
		}
		
		void toggle(Node* target) {
			if(!remove(target))
				add(target);
		}
		
		void clear() {
			for(auto itr = m_SelectedNodes.begin();
				itr != m_SelectedNodes.end();
				++itr)
			{
				shared_ptr<Node> sp;
				if(sp = itr->lock())
					sp->remove();
			}
			m_SelectedNodes.clear();
		}
		
		void update()
		{
			for(auto itr = m_SelectedNodes.begin();
				itr != m_SelectedNodes.end();
				)
			{
				if(itr->expired())
					itr = m_SelectedNodes.erase(itr);
				else
					++itr;
			}
		}
		
		size_t size() const { return m_SelectedNodes.size(); }
		bool empty() const { return m_SelectedNodes.empty(); }
		
		
		enum eModifyFlags
		{
			MOD_NONE = 0,
			MOD_WORLD = BIT(0),
			MOD_POST_MULTIPLY = BIT(1)
		};
		void modify(const glm::mat4& mod, unsigned int flags = 0)
		{
			for(auto itr = m_SelectedNodes.begin();
				itr != m_SelectedNodes.end();
				)
			{
				shared_ptr<Node> sp;
				if(sp = itr->lock())
				{
					glm::vec3 trans;
					
					glm::mat4* m = sp->getParent()->matrix();

					if(!flags & MOD_WORLD)
					{
						trans = Matrix::translation(*m) + Matrix::translation(mod);
						Matrix::resetTranslation(*m);
					}
					
					if(flags & MOD_POST_MULTIPLY)
						*m *= mod;
					else
						*m = mod * *m;
					
					if(!flags & MOD_WORLD)
						Matrix::translation(*m, trans);
				}
				else
				{
					itr = m_SelectedNodes.erase(itr);
					continue;
				}
				++itr;
			}
		}
		
		void move(const glm::vec3& trans)
		{
			for(auto itr = m_SelectedNodes.begin();
				itr != m_SelectedNodes.end();
				)
			{
				shared_ptr<Node> sp;
				if(sp = itr->lock())
				{
					glm::mat4* m = sp->getParent()->matrix();
					*m = glm::translate(*m, trans);
				}
				else
				{
					itr = m_SelectedNodes.erase(itr);
					continue;
				}
				++itr;
			}
		}
		void destroy()
		{
			for(auto itr = m_SelectedNodes.begin();
				itr != m_SelectedNodes.end();
				++itr)
			{
				shared_ptr<Node> sp;
				if(sp = itr->lock())
				{
					sp->remove();
					sp->getParent()->remove();
				}
			}
			m_SelectedNodes.clear();
		}
	
	private:
		std::list<std::weak_ptr<Node>> m_SelectedNodes;
};

#endif


#ifndef _SUPERPARENT_H
#define _SUPERPARENT_H

#include <memory>

class ISuperParent
{
public:
	const Node* superParent() const {
		return m_wpSuperParent.lock().get();
	}
	Node* superParent() {
		return m_wpSuperParent.lock().get();
	}
	void superParent(std::shared_ptr<Node>& n) {
		m_wpSuperParent = n;
	}
private:
	std::weak_ptr<Node> m_wpSuperParent;
};

#endif


#include "DummyPartitioner.h"

void DummyPartitioner :: nodeTreeToList(Node* n, std::vector<Node*>& node_list)
{
    node_list.push_back(n);
    for(auto itr = n->children()->begin();
        itr != n->children()->end();
        ++itr)
    {
        nodeTreeToList(itr->get(), node_list);
    }
}


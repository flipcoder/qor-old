#include "GfxAPI.h"
#include "Node.h"
#include "Util.h"
#include "Log.h"
#include "Scene.h"
#include "math/common.h"
#include "NodeFactory.h"
#include "Partitioner.h"

#include <iostream>
using namespace std;
using namespace rapidxml;

Node :: Node(unsigned int flags, unsigned int layer)
{
    m_pParent = NULL;
    m_Flags = flags;
    m_Layer = layer;
    m_PendingCache = PC_MASK; // all caches are pending
    m_SortBias = 0;
}

Node::~Node()
{
    removeAll();
}

void Node :: getParents(queue<const Node*>& q, bool include_self) const
{
    const Node* parent = m_pParent;
    if(include_self)
        q.push(this);
    while(parent != NULL)
    {
        q.push(parent);
        parent = parent->getParent_c();
    }
}

void Node :: getParents(stack<const Node*>& s, bool include_self) const
{
    const Node* parent = m_pParent;
    if(include_self)
        s.push(this);
    while(parent != NULL)
    {
        s.push(parent);
        parent = parent->getParent_c();
    }
}

void Node :: getParents(queue<Node*>& q, bool include_self)
{
    Node* parent = m_pParent;
    if(include_self)
        q.push(this);
    while(parent != NULL)
    {
        q.push(parent);
        parent = parent->getParent();
    }
}

void Node :: getParents(stack<Node*>& s, bool include_self)
{
    Node* parent = m_pParent;
    if(include_self)
        s.push(this);
    while(parent != NULL)
    {
        s.push(parent);
        parent = parent->getParent();
    }
}

const glm::mat4* Node :: matrix_c(Node::Space s) const
{
    //m.clear(glm::mat4::IDENTITY);

    ASSERT(s != Node::S_LOCAL);
    if(s == Node::S_LOCAL)
        return NULL;

    if(s == Node::S_PARENT)
        return matrix_c();

    // TODO
    // assuming world space...
    
    if(isWorldMatrixPendingCache())
    {    
        const Node* parent = getParent_c();
        if(parent && parent->getParent_c()) // 2nd check assumes root has identity matrix at all times
            m_WorldMatrixCached = *getParent_c()->matrix_c(S_WORLD) * *matrix_c();
        else
            m_WorldMatrixCached = *matrix_c();
        
        m_PendingCache &= ~PC_WORLD_MATRIX;
    }
    
    return &m_WorldMatrixCached;
    //stack<const Node*> parents;
    //getParents(parents, INCLUDE_SELF);
    //Node* parent = m_pParent;
    //glm::mat4 matrix;
    //while(parent != NULL)
    //{
    //    foreach(const Node* n, *parents)
            
    //}
}


glm::vec3 Node :: position(Node::Space s) const
{
    ASSERT(s != Node::S_LOCAL); // alert, you probably meant PARENT

    if(s == Node::S_PARENT)
        return Matrix::translation(m_Matrix);
    else if (s == Node::S_WORLD)
    {
        return Matrix::translation(*matrix_c(Node::S_WORLD));

        //// TODO: Move this local-to-world crap to an actual Node method
        ////       and fix it because it looks broken
        
        ////glm::vec3 v(0.0, 0.0, 0.0); // [Note] shouldn't this be pos?
        //queue<const Node*> parents;
        //getParents(parents);

        ////glm::mat4 m(glm::mat4::INVERSE, *matrix_c());
        //glm::mat4 m = glm::inverse(*matrix_c());
        //// Now go through parents in order from local to world
        //const Node* parent = NULL;
        //while(!parents.empty())
        //{
        //    parent = parents.front();
        //    parents.pop();

        //    // multiply by inverse of each parent
        //    //m.clear(glm::mat4::INVERSE, *parent->matrix_c());
        //    m *= glm::inverse(*parent->matrix_c());
        //}
        ////v *= m;
        //return Matrix::mult(glm::vec3(), m);
        ////matrix()->translate(v); <- to move in world space, do this
    }
    ASSERT(false);
    return glm::vec3();
}

void Node :: position(const glm::vec3& v, Node::Space s)
{
    ASSERT(s != S_LOCAL); // didn't you mean parent?
    ASSERT(s != S_WORLD); // not yet implemented
    Matrix::translation(m_Matrix, v);
    pendWorldMatrix();
}

void Node :: move(const glm::vec3& v, Node::Space s)
{
    ASSERT(s != S_LOCAL); // didn't you mean parent?
    ASSERT(s != S_WORLD); // not yet implemented
    Matrix::translate(m_Matrix, v);
    pendWorldMatrix();
}

void Node :: render(Partitioner* partitioner, unsigned int flags) const
{
    //glPushMatrix();
    //if(isWorldMatrixPendingCache())
    //{
    //    //glMultMatrixf(glm::value_ptr(*matrix_c()));
    //    //cacheModelMatrix();
    //}
    //glLoadMatrixf(glm::value_ptr(m_WorldMatrixCached));
    //if(flags & RENDER_USE_STACK)
    //    glMultMatrixf(glm::value_ptr(*matrix_c()));
    //else
    //{
        glm::mat4 modelview = *Renderer::get().getViewMatrix() * *matrix_c(S_WORLD);
        glLoadMatrixf(glm::value_ptr(modelview));
    //}
    //glMultMatrixf(glm::value_ptr(*matrix_c()));

    // render self
    if(visible() && (!hasAttribute(NodeAttributes::SIZE) || inView(partitioner)))
        renderSelf(partitioner, flags);

    // render children
    if(!(flags & RENDER_SELF_ONLY))
    {
        for(list<shared_ptr<Node>>::const_iterator itr = m_Children.cbegin();
            itr != m_Children.cend();
            ++itr)
        {
            (*itr)->render(partitioner, flags);
        }
    }

    //glPopMatrix();
}

void Node :: logic(unsigned int a)
{
    //if(!(m_Flags & NodeFlags::PAUSED))
    //{
        logicSelf(a);

        for(list<shared_ptr<Node>>::const_iterator itr = m_Children.cbegin();
                itr != m_Children.cend();
                ++itr)
        //foreach(Node* n, m_Children)
            (*itr)->logic(a);
    //}
}

void Node :: renderSelf(Partitioner* partitioner, unsigned int flags) const
{
    // Nothing to render
}

void Node :: renderFrom(Partitioner* partitioner, unsigned int flags) const
{
    // Grab queue of parents connected this node
    queue<const Node*> parents;
    getParents(parents);

    // Multiply GL matrix by inverse of self matrix
    //glm::mat4 m(glm::mat4::INVERSE, *matrix_c());
    glm::mat4 m = glm::inverse(*matrix_c());
    glMultMatrixf(glm::value_ptr(m));

    // Now go through parents in order from this node (camera) to world space
    const Node* parent = NULL;
    while(!parents.empty())
    {
        parent = parents.front();
        parents.pop();

        // multiply by inverse of each parent
        //m.clear(glm::mat4::INVERSE, *parent->matrix_c());
        m = glm::inverse(*parent->matrix_c());
        glMultMatrixf(glm::value_ptr(m));
    }
    
    //if(partitioner)
    //    partitioner->refreshFrustum();
}

void Node :: write(std::ofstream& file, int indent) const
{
    if(m_Layer == NodeLayer::TEMPORARY || m_Layer == NodeLayer::SUB_ENVIRONMENT)
        return;
    

    string s_indent = repeat("\t", max(indent,0));
    string attrs = attributeString();
    string type = nodeTypeString();

    
    //if(m_pParent)
    //{
        file << s_indent << "<" << type << (attrs.empty()?"":" ") << attrs << ">" << endl;
        writeSelf(file, indent+1);
    //}

    if(m_Layer)
    {
        for(list<shared_ptr<Node>>::const_iterator itr = m_Children.cbegin();
            itr != m_Children.cend();
            ++itr)
        {
            (*itr)->write(file, indent + 1);//+ (m_pParent!=NULL?1:0));
        }
    }

    //if(m_pParent)
        file << s_indent << "</"<< type <<">" << endl;
}

void Node :: writeSelf(std::ofstream& file, int indent) const
{
    string s_indent = repeat("\t", max(indent,0));
    if(!Matrix::isIdentity(*matrix_c()))
    {
        file << s_indent << "<matrix>" << endl;
        const float* matrix_array = glm::value_ptr(*matrix_c());
        file << s_indent << "\t";
        for(int i=0; i<16; i++)
            file << matrix_array[i] << (i==15?"":",");
        file << endl << s_indent << "</matrix>" << endl;
    }
}

bool Node :: read(Scene* scene, rapidxml::xml_node<>* cur_node)
{
    ASSERT(cur_node);
    ASSERT(scene);
    
    Log::get().write("Loading Node: " + str(cur_node->name()));
    
    if(!readSelf(cur_node))
        return false;
    
    xml_node<>* node = NULL;
    
    if(getParent_c() && (node = cur_node->first_node("matrix")))
    {
        Log::get().write("has matrix");
        if(!readMatrix(node)) // disallow root matrix changes
            return false;
    }
    
    if((node = cur_node->first_node()) != NULL)
    {
        do{
            if(string(node->name()) == "matrix")
                continue;
            
            // attempt to get filename attribute of node and pass to constructor
            string fn = "";
            rapidxml::xml_attribute<>* fn_attr = node->first_attribute("filename");
            if(fn_attr)
                fn = fn_attr->value();
            Log::get().write("Filename: " + fn);
            
            Node* scene_node = NodeFactory::create(scene, getParent(), node->name(), fn);
            Log::get().write(str(node->name()) + " " + (scene_node?"created!":"failed"));
            if(scene_node)
            {
                if(!scene_node->read(scene, node))
                    return false;
            }
        }while((node = node->next_sibling()));
    }
    return true;
}

bool Node :: readSelf(rapidxml::xml_node<>* cur_node)
{
    cout << "Node name: " << cur_node->name() << endl;
    
    string s = cur_node->value();
    if(!s.empty())
        cout << "Node value: " << s << endl;
    
    for(xml_attribute<> *attr = cur_node->first_attribute();
        attr; attr = attr->next_attribute())
    {
        cout << "\tAttr " << attr->name() << " = " << attr->value() << endl;
    }
    
    return true;
}

bool Node :: readMatrix(rapidxml::xml_node<>* matrix_node)
{
    ASSERT(matrix_node);
    
    Log::get().write("Matrix: " +str(matrix_node->value()));
    
    vector<string> values = explode(',', matrix_node->value());
    if(values.size() != 16)
        return false;
    
    float* m = glm::value_ptr(*matrix());
    for(int i=0;i<16;i++)
        m[i] = from_str<float>(values[i]);

    return true;
}


const Node* Node :: find_c(const Node* n) const
{
    ASSERT(n);
    if(n->getParent_c() == this)
        return n;

    for(auto itr = m_Children.cbegin();
        itr != m_Children.cend();
        ++itr)
    {
        const Node* temp = (*itr)->find_c(n);
        if(temp)
            return temp;
    }
    
    //foreach(const Node* child, m_Children)
    //    child->find_c(n);

    return NULL;
}

Node* Node :: find(Node* n)
{
    ASSERT(n);
    if(n->getParent_c() == this)
        return n;

    for(auto itr = m_Children.begin();
        itr != m_Children.end();
        ++itr)
    {
        Node* temp = (*itr)->find(n);
        if(temp)
            return temp;
    }

    //foreach(Node* child, m_Children)
    //    child->find(n);
    return NULL;
}

Node* Node ::add(Node* n)
{
    shared_ptr<Node> np(n);
    return add(np);
}

Node* Node ::add(shared_ptr<Node> n)
{
    ASSERT(n);
    ASSERT(this != n.get());
    ASSERT(n->getParent() == NULL); // node we're trying to add has no existing parent

    // do similar checks in case assertions are disabled
    if(this == n.get())
        return NULL; // can't add node to self
    //if(find(n))
    //    return false; // node already connected
    //if(m_pParent != NULL)
//        return false;

    //n->addReference();
    n->_setParent(this);
    //if(ops & ADD_FRONT)
    //    m_Children.push_front(n);
    //else
        m_Children.push_back(n);
    //m_Children.push_back(shared_ptr<Node>(n, NO_DEALLOCATOR));
    _onAdd(n.get());
    return n.get();
}

bool Node :: remove(Node* n, unsigned int flags)
{
    //ASSERT(flags & PRESERVE); // deprecated: use preserve() instead

    for(list<shared_ptr<Node>>::iterator itr = m_Children.begin();
        itr != m_Children.end();
        ++itr)
    {
        if(itr->get() == n)
        {
            //if(!(flags & PRESERVE))
                (*itr)->removeAll();
            
            _onRemove(itr->get());
            //Node* delete_me = itr->get();
            m_Children.erase(itr);

            //if(!(flags & PRESERVE))
            //    delete delete_me;

            return true;
        }
        else if(flags & SEARCH_SUBNODES)
        {
            // recursively search subnodes for node to remove
            if((*itr)->remove(n,flags))
                return true;
        }
    }

    return false;
}

shared_ptr<Node> Node :: preserve(Node* n, unsigned int flags)
{
    shared_ptr<Node> preserved_node;

    for(list<shared_ptr<Node>>::iterator itr = m_Children.begin();
        itr != m_Children.end();
        ++itr)
    {
        if(itr->get() == n)
        {
            (*itr)->removeAll(); // remove children
            _onRemove(itr->get());
            (*itr)->_setParent(NULL);
            preserved_node = *itr;
            m_Children.erase(itr);
            return preserved_node;
        }
        else if(flags & SEARCH_SUBNODES)
        {
            // recursively search subnodes for node to remove
            if(preserved_node = (*itr)->preserve(n))
                return preserved_node;
        }
    }

    return preserved_node; // should be NULL at this point
}

//unsigned int Node :: remove(Node* n, unsigned int flags)
//{
//    // keep track of # of removed nodes
//    int removed = 0;

//    for(list<Node*>::iterator itr = m_Children.begin();
//        itr != m_Children.end();
//        )
//    {
//        // SEARCH_SUBNODES flag: search subnodes for node to remove
//        if(*itr != n)
//        {
//            if(flags & SEARCH_SUBNODES)
//                removed += (*itr)->remove(n,flags);
//        }
//        else
//        {
//            (*itr)->removeAll();
//            onRemove(*itr);
//            //(*itr)->removeReference();
//            itr = m_Children.erase(itr);
//            removed++;
//
//            continue; // avoid itr++
//        }

//        ++itr; // increment here to avoid erase() itr invalidation
//    }

//    return removed;
//}

void Node :: removeAll(unsigned int flags)
{
   //ASSERT(! (flags & PRESERVE));

   for(auto itr = m_Children.begin();
       itr != m_Children.end();
       )
   {
       _onRemove(itr->get());
       //Node* delete_me = itr->get();
       itr = m_Children.erase(itr);
       //delete delete_me;
   }

   m_Children.clear();
}

//void Node :: removeAll(list<Node*>& removed_nodes, unsigned int flags)
//{
//    ASSERT(flags & PRESERVE);

//    for(auto itr = m_Children.begin();
//        itr != m_Children.end();
//        )
//    {
//        _onRemove(itr->get());
//        removed_nodes.push_back(itr->get()); // preserve (still contains subnodes!)
//        itr = m_Children.erase(itr);
//    }

//    m_Children.clear();
//}


//void Node :: removeAll(list<shared_ptr<Node>>& removed_nodes, unsigned int flags)
//{
//    ASSERT(flags & PRESERVE);

//    for(auto itr = m_Children.begin();
//        itr != m_Children.end();
//        )
//    {
//        _onRemove(itr->get());
//        removed_nodes.push_back(itr); // preserve (still contains subnodes!)
//        itr = m_Children.erase(itr);
//    }

//    m_Children.clear();
//}


void Node :: collapse(Space s, unsigned int flags)
{
    ASSERT(s != S_LOCAL);

    if(s == S_PARENT)
    {
        if(!getParent()) // if this node is root, exit
            return;
        if(!getParent()->getParent()) // if parent is root, exit
            return;

        // dettach self from parent
        //getParent()->remove(this, PRESERVE);
        shared_ptr<Node> preserve_me = getParent()->preserve(this);
        // Combine transformations
        m_Matrix = *getParent()->matrix() * m_Matrix;

        // reassign parent
        getParent()->getParent()->add(preserve_me);
        
        //_setParent(getParent()->getParent()); // bad

        ASSERT(m_pParent); // if this node becomes root, we've done something wrong
    }
    else if(s == S_WORLD)
    {
        if(!getParent())
            return;

        while(getParent()->getParent())
            collapse(S_PARENT);
    }
}

bool Node :: remove()
{
    if(m_pParent)
    {
        m_pParent->remove(this);
        return true;
    }
    return false;
}

string Node :: attributeString() const
{
    string attr;
    if(!filename().empty())
        attr += "filename=\""+filename()+"\" ";
    return trim(attr);
}

Node* Node ::pointTest(glm::vec3 point, MatrixStack& matrixstack)
{
    AABB* bbox = box();
    matrixstack.push(*matrix_c());
    //glm::vec3 point_world = point * *matrixstack.top();
    //glm::vec3 point_world; // incomplete!
    glm::vec3 point_world = Matrix::mult(point, *matrixstack.top());
    //glm::vec3 point_world = glm::vec3(point_world4.x, point_world4.y, point_world4.z);
    if(bbox && bbox->valid() && bbox->inside(point_world))
        return this;
    
    Node* c = NULL;
    for(auto itr = m_Children.begin();
        itr != m_Children.end();
        ++itr)
    {
        if((c = (*itr)->pointTest(point, matrixstack)))
            return c;
    }
    
    matrixstack.pop();
    return NULL;
}


//Node* Node :: vLineTest(glm::vec2 point2d, MatrixStack& matrixstack)
//{
//    AABB* bbox = box();
//    //if(bbox && bbox->valid() && bbox->lineInside(point2d))
//        //return this;
//    //glm::vec3 point3d(point.x, 0.0f, point.z);
//    //point3d = point2d * matrix_c();

//    matrixstack.push_inverse(matrix_c());
    
//    Node* c = NULL;
//    for(auto itr = m_Children.begin();
//        itr != m_Children.end();
//        ++itr)
//    {
//        if(c = (*itr)->vLineTest(point2d, matrixstack))
//            return c;
//    }
    
//    matrixstack.pop();
    
//    if(bbox && bbox->valid() && bbox->lineInside(point2d))
//        return this;

//    return NULL;
//}

Node* Node :: vLineTest(glm::vec3 point, MatrixStack& matrixstack, unsigned int flags)
{
    AABB* bbox = box();
    matrixstack.push(*matrix_c());
    MatrixStack::ScopedPop scopedpop(matrixstack);

    Node* c = NULL;
    for(auto itr = m_Children.begin();
        itr != m_Children.end();
        ++itr)
    {
        if((c = (*itr)->vLineTest(point, matrixstack, flags)))
        {
            if(flags & USE_SUPERPARENT)
            {
                return c->superParent();
            }
            else
                return c;
        }
    }
    
    if(!(flags & TEST_ONLY_VISIBLE) || visible())
        if(bbox && bbox->valid())
        {
            glm::vec3 point_local = transformOut(glm::vec3(point.x, 0.0f, point.z));
            //glm::vec3 point_local = Matrix::mult(*matrixstack.top(), glm::vec3(point.x, 0.0f, point.z));
            glm::vec2 point_local2d(point_local.x, point_local.z);
            
            if(bbox->vLineInside(point_local2d))
            {
                if(flags & USE_SUPERPARENT)
                    return this->superParent();
                else
                    return this;
            }
        }
    
    return NULL;
}

bool Node :: vLineTest(std::list<Node*>& nodes, glm::vec3 point, MatrixStack& matrixstack, unsigned int flags)
{
    AABB* bbox = box();
    matrixstack.push(*matrix_c());
    MatrixStack::ScopedPop scopedpop(matrixstack);

    Node* c = NULL;
    for(auto itr = m_Children.begin();
        itr != m_Children.end();
        ++itr)
    {
        (*itr)->vLineTest(nodes, point, matrixstack, flags);
    }
    
    if(!(flags & TEST_ONLY_VISIBLE) || visible())
        if(bbox && bbox->valid())
        {
            glm::vec3 point_local = transformOut(glm::vec3(point.x, 0.0f, point.z));
            //glm::vec3 point_local = Matrix::mult(*matrixstack.top(), glm::vec3(point.x, 0.0f, point.z));
            glm::vec2 point_local2d(point_local.x, point_local.z);
            
            if(bbox->vLineInside(point_local2d))
            {
                if(flags & USE_SUPERPARENT)
                {
                    Node* superparent = this->superParent();
                    //ASSERT(superparent == this->superParent());
                    push_back_unique(nodes, superparent);
                    //push_back_unique(nodes, this->superParent());
                }
                else
                    push_back_unique(nodes, this);
            }
                
        }
    
    return !nodes.empty();
}


glm::vec3 Node :: transformIn(glm::vec3 point) const
{
    std::stack<const Node*> parents;
    getParents(parents, true); // include self
    while(!parents.empty())
    {
        point = Matrix::mult(point, *parents.top()->matrix_c());
        parents.pop();
    }
    
    return point;
}

glm::vec3 Node :: transformOut(glm::vec3 point) const
{
    std::queue<const Node*> parents;
    getParents(parents, true); // include self
    
    while(!parents.empty())
    {
        point = Matrix::mult(glm::inverse(*parents.front()->matrix_c()), point);
        parents.pop();
    }
    
    //const Node* parent = m_pParent;
    //while(parent != NULL)
    //{
    //    point *= parent->matrix_c();
    //    parent = parent->getParent_c();
    //}
    return point;
}

void Node :: cacheModelMatrix() const
{
    if(m_PendingCache & PC_WORLD_MATRIX)
    {
        // make sure parent is not pending cache (debug)
        //ASSERT(!getParent_c() || !getParent_c()->isWorldMatrixPendingCache());
        
        //if(getParent_c() && getParent_c()->isWorldMatrixPendingCache())
        //    m_WorldMatrixCached = 
        // ugh.. I need the model matrix, not modelview >:(
        matrix_c(S_WORLD);
        //Renderer::get().getModelView(m_WorldMatrixCached);
        
        // pend children
        for(auto itr = m_Children.cbegin();
            itr != m_Children.cend();
            ++itr)
        {
            const_cast<Node*>(itr->get())->pendWorldMatrix();
        }
        // unpend self
        m_PendingCache &= ~PC_WORLD_MATRIX;
    }
}


/*
 * Node.h
 *
* - Connects to a parent to store hierarchal transformations
* - Handles children using a simple list (no smart ordering or partioning)
* - Handles position and transformation through a 4x4 Matrix.
* - Transform can be changed (to Euler or Quat, perhaps) by overloading matrix[_c]() accessors
*/

#ifndef _NODE_H
#define _NODE_H

//#include "math/vector3.h"
//#include "math/matrix.h"
#include "math/matrixstack.h"
#include "math/matrixops.h"
#include <glm/glm.hpp>
#include <list>
#include <queue>
#include <stack>
#include <memory>
#include <boost/optional.hpp>
#include <rapidxml.hpp>

#include "Util.h"
#include "NodeAttributes.h"
#include "Physics.h"
#include "Frustum.h"
#include "Graphics.h"
#include "IPartitioner.h"

class Scene;

class Node : public IRealtime, public std::enable_shared_from_this<Node>
{

protected:

    // mutable?: Derived classes may do matrix computation on-the-fly, during an accessor call.
    //   Since some accessors may be const, making this mutable makes perfect sense.
    mutable glm::mat4 m_Matrix;
    mutable glm::mat4 m_WorldMatrixCached;
    unsigned int m_Flags;
    unsigned int m_Layer; // "Node Layer" refers to scene node categories
    int m_SortBias; // Sort order (normal objects default to 0)
    
    enum {
        PC_WORLD_MATRIX = BIT(0),
        PC_MASK = MASK(1)
    };
    mutable unsigned int m_PendingCache; // changes pending chache

    std::list<std::shared_ptr<Node>> m_Children;
    Node* m_pParent;
    
    //unsigned int m_ChangedAttributes;
    //Freq::Time m_tLastChange; // unused

public:

    enum Space
    {
        S_LOCAL,
        S_PARENT,
        S_WORLD
    };
    
    //class ScopedMatrixMod
    //{
    //private:
    //    std::weak_ptr<Node> m_wpNode;
    //    Node* m_pNode;
    //    bool m_bShared;
    //public:
    //    ScopedMatrixMod(shared_ptr<Node>& n):
    //        m_pNode(NULL),
    //        m_wpNode(n),
    //        m_bShared(true)
    //    {
    //    }
    //    ScopedMatrixMod(Node* n):
    //        m_pNode(n),
    //        m_bShared(false)
    //    {
    //        ASSERT(n);
    //    }
    //    ~ScopedMatrixMod()
    //    {
    //        if(m_bShared)
    //        {
    //            std::shared_ptr<Node> sp;
    //            if((sp = m_wpNode.lock()))
    //                sp->pendWorldMatrix();
    //        }
    //        else
    //        {
    //            m_pNode->pendWorldMatrix();
    //        }
    //    }
    //};
    
    Node(
        unsigned int flags = Node::defaultFlags(),
        unsigned int layer = 0);
    virtual ~Node();
    //Node(const Node& n);

    // Callbacks for parent when child nodes are added/removed
    virtual void _onAdd(Node* n) {}
    virtual void _onRemove(Node* n) {}

    void visible(bool v) {
        if((m_Flags & NodeFlags::VISIBLE) != v)
            m_Flags ^= NodeFlags::VISIBLE;
    }
    bool visible() const { return m_Flags & NodeFlags::VISIBLE; }
    //void pause(bool p = true) {
    //    if((m_Flags & NodeFlags::PAUSED) != p)
    //        m_Flags ^= NodeFlags::PAUSED;
    //}
    //void unpause() {
    //    m_Flags &= ~NodeFlags::PAUSED;
    //}
    //bool pause() const { return m_Flags & NodeFlags::PAUSED; }
    void _setParent(Node* p) { m_pParent = p; }
    Node* getParent() { return m_pParent; }
    const Node* getParent_c() const { return m_pParent; }
    void getParents(std::queue<const Node*>& q, bool include_self = false) const;
    void getParents(std::stack<const Node*>& s, bool include_self = false) const;
    void getParents(std::queue<Node*>& q, bool include_self = false);
    void getParents(std::stack<Node*>& s, bool include_self = false);

    virtual bool hasAttribute(unsigned int attr) const {
        if(attr == NodeAttributes::PHYSICS)
            return false;
        return const_cast<Node*>(this)->Node::getAttribute(attr)!=NULL;
    }
    virtual void* getAttribute(unsigned int attr) {
        if(attr == NodeAttributes::CHILDREN)
            return (void*)&m_Children;
        return NULL;
    }

    unsigned int flags() { return m_Flags; }
    void flags(unsigned int f) { m_Flags = f; }
    void setFlags(unsigned int f) { m_Flags = f; }
    bool hasFlags(unsigned int f) { return m_Flags & f; }
    void addFlags(unsigned int f) { m_Flags |= f; }
    void layer(unsigned int layer) {m_Layer = layer; }
    unsigned int layer() const { return m_Layer; }
    void sortBias(int sb) { m_SortBias = sb; }
    int sortBias() const { return m_SortBias; }
    
    static unsigned int defaultFlags() { return NodeFlags::VISIBLE; }
    
    //virtual bool updateAttribute (unsigned int attr, void* data) {
    //    return false;
    //}

    //void nullify();
    
    void cacheModelMatrix() const;
    
    bool isWorldMatrixPendingCache() const
    {
        return m_PendingCache & PC_WORLD_MATRIX;
    }

    virtual glm::mat4* matrix() { return &m_Matrix; }
    virtual const glm::mat4* matrix_c() const { return &m_Matrix; }
    virtual const glm::mat4* matrix_c(Node::Space s) const;
    //virtual glm::mat4 matrix(Node::Space s = Node::S_PARENT) const;

    // btMotionState overloads
    //virtual void setWorldTransform(const btTransform& worldTrans) {
    //    // TODO: assumes world space
    //    worldTrans.getOpenGLMatrix((btScalar*)Matrix::ptr(*matrix()));
    //    pendWorldMatrix();
    //}
    //virtual void getWorldTransform(btTransform& worldTrans) const {
    //    worldTrans.setFromOpenGLMatrix(Matrix::ptr(*matrix_c()));
    //}
    //virtual void setKinematicPos(btTransform &currentPos) {}

    virtual void pendWorldMatrix() const {
        m_PendingCache |= PC_WORLD_MATRIX;
        for(auto itr = m_Children.cbegin();
            itr != m_Children.cend();
            ++itr)
        {
            const_cast<Node*>(itr->get())->pendWorldMatrix();
        }
        //m_WorldMatrixCache = m_Matrix;
    }
    virtual void resetOrientation() {
        Matrix::resetOrientation(*matrix());
        pendWorldMatrix();
    }

    virtual glm::vec3 heading() const { return Matrix::heading(*matrix_c()); }
    virtual glm::vec3 position(Node::Space s = Node::S_PARENT) const;
    virtual void position(const glm::vec3& v, Node::Space s = Node::S_PARENT);
    virtual void move(const glm::vec3& v, Node::Space s = Node::S_PARENT);

    // find searches subnodes automatically, to not search subnodes,
    // (p->getParent_c()==c)?c:NULL is equivalent
    virtual Node* find(Node* n);
    virtual const Node* find_c(const Node* n) const;

    virtual void logic(unsigned int advance);
    virtual void logicSelf(unsigned int advance) {}
    enum eRenderFlags
    {
        RENDER_USE_STACK=BIT(0),
        RENDER_SELF_ONLY=BIT(1),
        RENDER_AMBIENT_PASS=BIT(2),
        RENDER_INDICATORS=BIT(3)
    };
    virtual void renderSelf(IPartitioner* partitioner = NULL, unsigned int flags = 0) const;
    virtual void render(IPartitioner* partitioner = NULL, unsigned int flags = 0) const;
    virtual void renderFrom(IPartitioner* partitioner = NULL, unsigned int flags = 0) const;
    //virtual void 

    // NOTE: should always return true if view is NULL
    virtual bool inView(IPartitioner* partitioner = NULL) const {
        //if(!partitioner)
        //    return true;
        return true;
        //if(!view || !box)
        //    return true;
        //glm::vec3 bbox(
            
        //);
        //view->
        //return view->containsBox(bbox.x, bbox.y, bbox.z, r);
    }

    enum {
        ADD_FRONT = BIT(0)
    };
    virtual Node* add(Node* n);
    virtual Node* add(std::shared_ptr<Node> n);

    enum {
        SEARCH_SUBNODES=BIT(1), // search subnodes for node to be removed?
    };

    virtual bool remove();
    virtual bool remove(Node* n, unsigned int flags = 0);
    virtual std::shared_ptr<Node> preserve(Node* n, unsigned int flags = 0);
    virtual void removeAll(unsigned int flags = 0);

    //virtual std::weak_ptr<Node> weakPtr() {
    //    std::weak_ptr<Node> wp;
    //    if(getParent())
    //        getParent()->find(this);
    //    // not done
    //}

    //virtual void removeAll(std::list<Node*>& removed_nodes, unsigned int flags = 0);
    //virtual void removeAll(std::list<shared_ptr<Node>>& removed_nodes, unsigned int flags = 0);

    virtual void collapse(Space s = S_PARENT, unsigned int flags = 0);

    virtual unsigned int numChildren() { return m_Children.size(); }
    
    virtual std::list<std::shared_ptr<Node>>* children() {
        return &m_Children;
    }

    virtual void write(std::ofstream& file, int indent) const;
    virtual void writeSelf(std::ofstream& file, int indent) const;
    virtual bool read(Scene* scene, rapidxml::xml_node<>* cur_node);
    virtual bool readSelf(rapidxml::xml_node<>* cur_node);
    virtual bool readMatrix(rapidxml::xml_node<>* matrix_node);

    virtual std::string attributeString() const;
    virtual std::string filename() const { return ""; }
    virtual std::string name() const { return ""; }
    virtual void filename(std::string s) { ASSERT(false); }
    virtual const AABB* box() const { return const_cast<Node*>(this)->box(); }
    virtual AABB* box() { return NULL; }
    virtual void recalculate() {}
    
    virtual SCOPED_ENUM_TYPE(NodeType) nodeType() const { return NodeType::NODE; }
    virtual std::string nodeTypeString() const { return "node"; }

    enum {
        TEST_ONLY_VISIBLE = BIT(0),
        USE_SUPERPARENT = BIT(1)
    };
    
    Node* pointTest(glm::vec3 point, MatrixStack& matrixstack);
    bool vLineTest(std::list<Node*>& nodes, glm::vec3 point, MatrixStack& matrixstack, unsigned int flags = 0);
    Node* vLineTest(glm::vec3 point, MatrixStack& matrixstack, unsigned int flags = 0);
    bool vLineTest(std::list<Node*>& nodes, glm::vec3 point, unsigned int flags = 0) {
        MatrixStack matrixstack;
        return vLineTest(nodes, point, matrixstack, flags);
    }
    Node* vLineTest(glm::vec3 point, unsigned int flags = 0) {
        MatrixStack matrixstack;
        return vLineTest(point, matrixstack, flags);
    }
    
    // transforms between world and local space
    virtual glm::vec3 transformIn(glm::vec3 point) const;
    virtual glm::vec3 transformOut(glm::vec3 point) const;
    
    virtual const Node* superParent() const { return this; }
    virtual Node* superParent() { return const_cast<Node*>(static_cast<const Node*>(this)->superParent()); }
    
};

#endif


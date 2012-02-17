#include "math/common.h"
#include "Shadow.h"
#include "Log.h"
#include "Node.h"
#include "Mesh.h"
#include "Light.h"
using namespace std;

//Shadow :: Shadow(Node* mesh_node, Mesh& mesh_ref, const Light& light_ref)
Shadow :: Shadow(weak_ptr<Node>& mesh_node_ref, weak_ptr<Mesh>& mesh_ref, weak_ptr<Light>& light_ref)
{
    m_bValid = false;
    
    try
    {
        // set members
        m_wpNode = mesh_node_ref;
        m_wpMesh = mesh_ref;
        m_wpLight = light_ref;
        
        // acquire shared_ptrs from weak_ptrs
        SharedData shared_data(m_wpNode, m_wpMesh, m_wpLight);
        if(!shared_data.valid())
            throw false;
        
        // Get matricies so we can get all the vectors into world space
        glm::mat4 node_matrix = *shared_data.node->matrix_c(Node::S_WORLD);
        glm::mat4 light_matrix = *shared_data.light->matrix_c(Node::S_WORLD);
        
        // now we go through the mesh and detect edges
        if(!generateEdges(shared_data))
            throw false;
    }
    catch(bool v)
    {
        m_bValid = v;
    }
    catch(...)
    {
        m_bValid = false;
    }
}

bool Shadow :: generateEdges(Shadow::SharedData& shared_data)
{
    return false;
}


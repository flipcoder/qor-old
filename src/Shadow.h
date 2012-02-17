#ifndef _SHADOW_H
#define _SHADOW_H

#include <memory>
#include "Node.h"
#include "Mesh.h"
#include "Light.h"

class Mesh;
class Light;

class Shadow
{
public:
    
    class SharedData
    {
    public:
        SharedData(
                std::weak_ptr<Node> n,
                std::weak_ptr<Mesh> m,
                std::weak_ptr<Light> l
            ):
            node(n),
            mesh(m),
            light(l)
        {}
        std::shared_ptr<Node> node;
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Light> light;
        bool valid() {return node && mesh && light;}
    };
    
    Shadow(std::weak_ptr<Node>& mesh_node, std::weak_ptr<Mesh>& mesh_ref, std::weak_ptr<Light>& light_ref);
    //~Shadow();

    bool generateEdges(SharedData& shared_data);
    bool good() { return m_bValid; }
    
private:
    std::vector<glm::vec3> m_vEdges;
    
    std::weak_ptr<Node> m_wpNode;
    std::weak_ptr<Mesh> m_wpMesh;
    std::weak_ptr<Light> m_wpLight;
    
    bool m_bValid;
};

#endif


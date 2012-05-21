#include <memory>
using namespace std;

#include "Entity.h"

Entity::Entity(unsigned int entflags)
{
    //setMeshOwnership(true); // tell IMeshContainer to clean up the meshes it allocates    
    m_Flags = entflags;
}

Entity::Entity(const shared_ptr<Mesh>& m, unsigned int entflags)
{
    //setMeshOwnership(true);
    addMesh(m);
    m_Flags = entflags;
}

Entity::Entity(const shared_ptr<Mesh>& m, glm::vec3 pos, unsigned int entflags)
{
    //setMeshOwnership(true);
    addMesh(m);
    position(pos);
    m_Flags = entflags;
}

Entity::Entity(std::string fn, ResourceCache<Mesh>& mesh_map, ResourceCache<Texture>& tex_map, glm::vec3 pos, unsigned int entflags/*, std::string override_name, float minlod*/)
{
    shared_ptr<Mesh> mesh_ptr = mesh_map.cache(fn);
    if(!mesh_ptr->good())
        mesh_ptr->load(fn, tex_map);
    
    addMesh(mesh_ptr);
    position(pos);
    m_Flags = entflags;
}


Entity::~Entity()
{
}

void Entity :: renderSelf(IPartitioner* partitioner, unsigned int flags) const
{
    //if(flags & F_ONTOP){
    //}
    
    foreach(auto m, *getMeshes_c())
        m->render();

    //for(auto& mesh: *getMeshes_c())
    //    mesh->render();

    //for(list<shared_ptr<Mesh>>::const_iterator itr = getMeshes_c()->begin();
    //    itr != getMeshes_c()->end();
    //    ++itr)
    //{
    //    (*itr)->render();
    //}
    // [TODO] convert the above code to use const shared_ptr's

    //if(flags & F_ONTOP) {
    //}
}


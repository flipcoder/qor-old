#ifndef _IMESHCONTAINER_H
#define _IMESHCONTAINER_H

#include <list>
#include "Mesh.h"

class IMeshContainer
{
protected:
    std::list<std::shared_ptr<Mesh>> m_Meshes;
    //bool m_bMeshAlloc;
    
public:
    IMeshContainer() {}
        //m_bMeshAlloc(false) {}

    virtual ~IMeshContainer() {
        //if(m_bMeshAlloc)
        //{
        //    for(auto itr = getMeshes()->begin();
        //        itr != getMeshes()->end();
        //        ++itr)
        //    {
        //        delete *itr;
        //    }
        //}
    }

    virtual std::list<std::shared_ptr<Mesh>>* getMeshes() { return &m_Meshes; }
    virtual const std::list<std::shared_ptr<Mesh>>* getMeshes_c() const { return &m_Meshes; }
    
    bool addMesh(const std::shared_ptr<Mesh>& spm) {
        if(!spm)
            return false;
        m_Meshes.push_back(spm);
        return true;
    }
    
    void logic(unsigned int advance)
    {
        for(auto itr = m_Meshes.cbegin();
           itr != m_Meshes.cend();
           ++itr)
        {
           (*itr)->logic(advance);
        }        
        
        //for(auto& mesh: m_Meshes)
        //    mesh->logic(advance);
    }

    //bool addMesh(Mesh* m) {
    //    return addMesh(shared_ptr<Mesh>(m));
    //}

    unsigned int numMeshes() {
        return m_Meshes.size();
    }

    //void setMeshOwnership(bool b) {
    //    m_bMeshAlloc = b;
    //}
};

#endif


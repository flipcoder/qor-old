#ifndef _MESH_H
#define _MESH_H

#include <vector>
#include <list>
#include <memory>
//#include <map>
#include <unordered_map>
#include "math/common.h"
#include "Material.h"
#include "ResourceMap.h"
#include <assimp/assimp.hpp>
#include <assimp/aiScene.h>
#include <assimp/aiPostProcess.h>
#include "Shadow.h"
#include "Light.h"
class Shadow;
class Light;

class Mesh
{

public:

    struct Face
    {
        unsigned int indices[3];
    };

    struct Edge
    {
        unsigned int indices[2];
    };

    std::string name;
    std::vector<glm::vec3> bitangents;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> vertices;
    //std::vector<Bone> bones;
    std::vector<Face> faces;
    std::vector<std::shared_ptr<Material>> materials;
    std::vector<glm::vec3> normals;
    std::vector<std::vector<glm::vec3>> UVs;

    std::vector<std::shared_ptr<Mesh>> LOD;
    std::map<std::pair<std::weak_ptr<Node>, std::weak_ptr<Light>>, Shadow> shadows;

    float minimum_lod;
    bool error;
    //bool bMaterialAlloc;

    enum {
        F_DOUBLE_SIDED = BIT(0)
    };
    unsigned int flags;
    //unsigned int refs; // currently unused

    Mesh(std::string n = "", float minlod = 0.1f);
    Mesh(std::string fn, ResourceMap<Texture>& tex_map, std::string override_name = "", float minlod = 0.1f);
    virtual ~Mesh();

    //Shadow generateShadow();
    
    Mesh* getLOD(float quality = 1.0f) const;

    virtual bool load(std::string fn, ResourceMap<Texture>& tex_map);

    enum {
        RENDER_LOD = BIT(0)
    };
    void logic(unsigned int advance);
    void render(unsigned int flags = 0, float quality = 1.0f) const;
    Shadow* getShadow(Node* node, Light* light){
        
    }

    bool loadAIMesh(const aiMesh* aimesh);
    
    bool good() const { return !vertices.empty(); } // this could be improved, but it works for now
    
    // TODO: add Mesh::allocate like Material::allocate for usage with ResourceMaps
};

#endif


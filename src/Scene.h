#ifndef _SCENE_H
#define _SCENE_H
#pragma once

#include <string>
#include <map>
#include <assimp/assimp.hpp>
#include <assimp/aiScene.h>
#include <assimp/aiPostProcess.h>
#include <rapidxml.hpp>
#include "IPartitioner.h"
#include "math/common.h"

#include "Log.h"
#include "Actor.h"
#include "Material.h"
#include "Log.h"
#include "Node.h"
#include "Texture.h"
#include "Input.h"
#include "Shader.h"
#include "EnvironmentNode.h"
#include "ResourceMap.h"
#include "IFallible.h"

class Entity;

class Scene : public IFallible, public IRealtime
{
public:

    enum eFlags { // constructor flags
        F_PHYSICS = BIT(0),
        F_CLEAR_ENV = BIT(1),
        F_CLEAR_ALL = BIT(2)
    };

private:

    std::shared_ptr<Node> m_spRoot;
    //std::string m_sError;
    unsigned int m_Flags;

    std::string m_sName;
    std::string m_sVersion;
    std::string m_sDescription;
    std::string m_sAuthor;

    std::string m_sFilename;

    Color m_SkyColor;

    struct TempData {
        std::vector<std::shared_ptr<Material>> materials;
        std::vector<std::shared_ptr<Mesh>> meshes;
    };

    //std::map<std::string, Texture*> m_Textures;
    //std::map<std::string, Mesh*> m_MeshPool;

    ResourceMap<Mesh> m_Meshes;
    //ResourceMap<Material> m_Materials;
    ResourceMap<Texture> m_Textures;
    
    //Camera* m_pCamera;
    std::unique_ptr<Physics> m_spPhysics;
    std::unique_ptr<IPartitioner> m_spPartitioner;

    void nullify();
    void initInternals();

    bool loadFMLInfo(rapidxml::xml_node<>* info_node);

    //void parseAIVector(Vector3& v, const aiVector3D& aiv);

public:

    Scene(unsigned int flags = 0);
    Scene(std::string fn, unsigned int flags = 0);
    virtual ~Scene();

    // 
    bool load(std::string fn);

    // Assimp format to get bash Scene meshes
    Node* loadAI(std::string fn, glm::vec3 pos = glm::vec3(0.0f), unsigned int flags = Node::defaultFlags(), Node* parent = NULL);
    Entity* loadEntity(std::string fn, glm::vec3 pos = glm::vec3(0.0f), unsigned int ent_flags = 0, unsigned int node_flags = Node::defaultFlags(), Node* parent = NULL);

    bool loadFML(std::string fn);
    //bool saveFML() const;
    bool saveFML(std::string fn) const;

    void clear();
    void clearEnvironment();

    virtual void logic(unsigned int a);
    //void renderNode(const aiScene* scene, const aiNode* node) const;
    void render(Node* from = NULL) const;
    void renderOverview(const glm::vec3& pos, float scale) const;

    void printDetails(const aiScene* ais);
    void compile();

    bool loadAINode(EnvironmentNode* node, const aiNode* ainode, TempData* tempdata);
    bool loadAIMeshData(const std::string& scene_fn, aiScene* aiscene, aiScene* aiscene_lmap, TempData* tempdata);

    //std::shared_ptr<Material> allocMaterial(std::string fn, ResourceMap<Texture>& rmap);

    bool add(Node* node, /*glm::vec3 pos = glm::vec3(0.0f), */Node* parent = NULL);

    //Camera* camera() { return m_pCamera; }

    //bool error() const { return (m_sError!=""); }
    //std::string getError() const { return m_sError; }

    ResourceMap<Mesh>* getMeshResources() { return &m_Meshes; }
    //ResourceMap<Material>* getMaterialResources() { return &m_Materials; }
    ResourceMap<Texture>* getTextureResources() { return &m_Textures; }

    std::string name() const { return m_sName; }
    std::string filename() const { return m_sFilename; }

    void info(std::string name,
        std::string version = "",
        std::string author = "",
        std::string desc = "")
    {
        m_sName = name;
        m_sAuthor = author;
        m_sDescription = desc;
    }

    Node* root() { return m_spRoot.get(); }
};

#endif

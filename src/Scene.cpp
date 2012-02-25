#include "Scene.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <list>
#include <sstream>
#include <cassert>
#include <vector>
using namespace std;

//#include <pugixml.hpp>
#include <rapidxml.hpp>
using namespace rapidxml;

#include "GfxAPI.h"
#include "Log.h"
#include "Util.h"
#include "EnvironmentNode.h"
#include <boost/algorithm/string.hpp>
#include "AITypeParser.h"
#include "Entity.h"
#include "ParticleSystem.h"
#include "Graphics.h"
#include "Shader.h"
#include "Entity.h"
#include "DumbyPartitioner.h"

//#define AI_SCALE 1.0f
//0.05f

Scene::Scene(unsigned int flags)
{
    nullify();
    m_Flags = flags;
    
    initInternals();
    m_spRoot.reset(new Node(Node::defaultFlags(), NodeLayer::ROOT));
}

Scene::Scene(string fn, unsigned int flags)
{
    nullify();
    m_Flags = flags;
    
    initInternals();

    load(fn);
    if(flags & F_PHYSICS)
        m_spPhysics.reset(new Physics());
    
    //ps = new ParticleSystem();
    //ps->setTextureOwnership(true);
    //particle.size = 60.0f;
    //particle.life = 2.0f;
    //particle.color.white();
    //particle.color *= 0.5f;
    ////particle.color.a = 0.1f;
    //particle.vel.set(0.0f,1.0f,0.0f);
    //ps->init(new Texture("data/particles/particle.png", Texture::MIPMAPPED), particle, ParticleSystem::BLEND_COLOR, 0.5f, 25.0f);
    //ps->matrix()->translate(glm::vec3(0.0f, 1.0f, 0.0f));
    //m_pRoot->add(ps);
}

void Scene :: initInternals()
{
    m_spPartitioner.reset(new DumbyPartitioner());
}

bool Scene :: load(string fn)
{
    clear();

    if(stringEndsWith<string>(boost::to_upper_copy(fn), ".OBJ"))
        return loadAI(fn);
    if(stringEndsWith<string>(boost::to_upper_copy(fn), ".FML"))
        return loadFML(fn);

    setError("Unrecognized scene file type.");
    return false;
}

bool Scene :: loadFML(string fn)
{
    clear();
    ifstream file(fn.c_str());
    if(!file.good())
       return false;
    vector<char> data((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    data.push_back('\0');
    m_sFilename = fn;
    xml_document<> doc;
    doc.parse<parse_declaration_node | parse_no_data_nodes>(&data[0]);
    
    xml_node<>* fml_node = doc.first_node("fml");
    xml_node<>* info_node = NULL;
    if(fml_node)
        info_node = fml_node->first_node("info");
    if(info_node)
        loadFMLInfo(info_node);
    return m_spRoot->read(this, fml_node->first_node("node"));
}

bool Scene :: loadFMLInfo(xml_node<>* info_node)
{
    return true;
}

//bool Scene :: saveFML() const
//{
//    if(m_sFilename.empty())
//        return false;
//    return saveFML(m_sFilename);
//}

bool Scene :: saveFML(string fn) const
{
    ofstream file(fn);
    if(!file.good())
        return false;

    // write header
    file << "<?xml version=\"1.0\"?>" << endl;
    file << "<fml version=\"0.1\">" << endl;
    file << endl;

    // write info
    file << "\t<info>" << endl;
    if(!m_sName.empty())
        file << "\t\t<name>" << m_sName << "</name>" << endl;
    if(!m_sVersion.empty())
        file << "\t\t<version>" << m_sVersion << "</version>" << endl;
    if(!m_sDescription.empty())
        file << "\t\t<description>" << m_sDescription << "</description>" << endl;
    if(!m_sAuthor.empty())
        file << "\t\t<author>" << m_sAuthor << "</author>" << endl;
    
    file << "\t</info>" << endl << endl;
    
    // write scenegraph data
    m_spRoot->write(file, 1);

    file << endl << "</fml>" << endl;

    return true;
}

Node* Scene :: loadAI(std::string fn, glm::vec3 pos, unsigned int flags, Node* parent)
{
    TempData tempdata;
    size_t num_materials = 0;

    // get directory path and lightmap filename
    string path = "";
    string fn_lmap = "";

    size_t split_point;
    if((split_point = fn.find_last_of("\\/")) != string::npos)
    {
        path = fn.substr(0,split_point+1);
        fn = fn.substr(split_point+1);
    }
    if((split_point = fn.find_last_of(".")) != string::npos)
        fn_lmap = fn.substr(0,split_point) + "_lmap" + fn.substr(split_point);
    
    unique_ptr<Assimp::Importer> importer(new Assimp::Importer());
    unique_ptr<Assimp::Importer> importer_lmap(new Assimp::Importer());
    
    Node* base = NULL;

    //Log::get().write("fn: "+fn);
    //Log::get().write("fn_lmap: " +fn_lmap);
    //Log::get().write("path: "+path);

    try {
        // Assimp aiScene Loading
        unsigned int aiflags = aiProcess_Triangulate|
            aiProcess_CalcTangentSpace|
            aiProcess_GenSmoothNormals;
        
            /*aiProcess_ValidateDataStructure|
            aiProcess_Triangulate|
            aiProcess_JoinIdenticalVertices|
            aiProcess_SortByPType|
            aiProcess_GenSmoothNormals|
            aiProcess_SplitLargeMeshes|
            aiProcess_FindInstances;*/

        aiScene* aiscene = const_cast<aiScene*>(importer->ReadFile(path+fn,aiflags));
        string err = importer->GetErrorString();

        if(!aiscene || err != "" || 
            aiscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
            aiscene->mFlags & AI_SCENE_FLAGS_VALIDATED) {
            Log::get().error(err);
            throw exception();
        }
        //printDetails(aiscene);

        aiScene* aiscene_lmap = const_cast<aiScene*>(importer_lmap->ReadFile(path+fn_lmap, aiflags));
        err = importer_lmap->GetErrorString();
        if(aiscene_lmap) { // lightmap is optional
            if(err != "" || 
                aiscene_lmap->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
                aiscene_lmap->mFlags & AI_SCENE_FLAGS_VALIDATED){
                Log::get().error(err);
                throw exception();
            }
        }
        //printDetails(aiscene_lmap);

        // Load Scene Textures+Materials

        //vector<aiTextureType> textypes = {
        //    aiTextureType_NONE,
        //    aiTextureType_DIFFUSE,
        //    aiTextureType_SPECULAR,
        //    aiTextureType_AMBIENT,
        //    aiTextureType_EMISSIVE,
        //    aiTextureType_HEIGHT,
        //    aiTextureType_NORMALS,
        //    aiTextureType_SHININESS,
        //    aiTextureType_OPACITY,
        //    aiTextureType_DISPLACEMENT,
        //    aiTextureType_LIGHTMAP,
        //    aiTextureType_REFLECTION,
        //    aiTextureType_UNKNOWN
        //};

        // Load Textures
    
        num_materials += (size_t)aiscene->mNumMaterials;
        if(aiscene_lmap)
            num_materials += (size_t)aiscene_lmap->mNumMaterials;
        tempdata.materials.resize(num_materials);
        
        for(unsigned int i=0; i<aiscene->mNumMaterials; i++)
        {
            
            aiTextureType type = aiTextureType_DIFFUSE;
            unsigned int ntex = aiscene->mMaterials[i]->GetTextureCount(type);
            
            if(ntex)
            {
                // Grab individual texture
                aiString texpath;
                aiTextureMapping mapping;
                unsigned int uvindex;
                float blend;
                aiTextureOp op;
                aiTextureMapMode mapmode[3];
                aiscene->mMaterials[i]->GetTexture(
                    type,
                    0,
                    &texpath,
                    &mapping,
                    &uvindex,
                    &blend,
                    &op,
                    mapmode);

                assert(texpath.length);
                shared_ptr<Material> material = Material::allocate(path + string(texpath.data), m_Textures);
            
                if(!material)
                {
                    Log::get().error("Failed to load texture: " + path + str(texpath.data));
                    throw exception();
                }
                //    Log::get().write("Added material: " + (string)path + texpath.data + " (type: " + str(type) + ")");

                tempdata.materials[i] = material;
            }
            else
            {
                //tempdata.materials[i].reset();
            }
        }


        // load the lightmap materials to the end of the materials list (so, starting at offset)
        if(aiscene_lmap != NULL)
        {
            unsigned int material_offset = aiscene->mNumMaterials;
            for(unsigned int i=0; i<aiscene_lmap->mNumMaterials; i++)
            {
                
                aiTextureType type = aiTextureType_DIFFUSE;
                unsigned int ntex = aiscene_lmap->mMaterials[i]->GetTextureCount(type);
                
                if(ntex)
                {
                    // Grab individual texture
                    aiString texpath;
                    aiTextureMapping mapping;
                    unsigned int uvindex;
                    float blend;
                    aiTextureOp op;
                    aiTextureMapMode mapmode[3];
                    aiscene_lmap->mMaterials[i]->GetTexture(
                        type,
                        0,
                        &texpath,
                        &mapping,
                        &uvindex,
                        &blend,
                        &op,
                        mapmode);

                    assert(texpath.length);
                    shared_ptr<Material> material = Material::allocate(path + string(texpath.data), m_Textures);
                
                    if(!material)
                    {
                        Log::get().error("Failed to load texture: " + path + str(texpath.data));
                        throw exception();
                    }

                    tempdata.materials[material_offset+i] = material;
                }
                else
                {
                    //tempdata.materials[material_offset+i].reset();
                    //tempdata.materials[material_offset+i] = shared_ptr<Material>();
                }
            }
        }

        if(!loadAIMeshData(fn, aiscene, aiscene_lmap, &tempdata)) // aiscene_lmap might be null
            throw exception();

        // Load Scene Geometry
        //if(!m_spRoot || flags & F_CLEAR_ALL)
        //if(flags & F_CLEAR_ENV)
        if(!parent)
        {
            if(!m_spRoot)
                m_spRoot.reset(new Node(Node::defaultFlags(), NodeLayer::ROOT));
            parent = m_spRoot.get();
        }
        //    clearEnvironment();
        EnvironmentNode* scene_map = new EnvironmentNode(flags, fn);
        parent->add(scene_map);
        if(!loadAINode(scene_map, aiscene->mRootNode, &tempdata))
            throw exception();
        scene_map->recalculate();
        
        base = scene_map;

        // don't need these containers anymore
        
    }catch(...){
        setError("Unable to load Scene.");
        return  NULL;
    }
    
    Matrix::translate(*base->matrix(), pos);
    return base;
}

Entity* Scene::loadEntity(std::string fn, glm::vec3 pos, unsigned int ent_flags, unsigned int node_flags, Node* parent)
{
    Entity* base = new Entity(fn, *getMeshResources(), *getTextureResources(), pos, ent_flags);
    base->flags(node_flags);
    if(!add(base, parent))
    {
        delete base;
        return NULL;
    }
    return base;
}


void Scene::printDetails(const aiScene* ais)
{
    if(ais)
    {
        ostringstream oss;
        oss.str("");
        oss << "Animations: " << ais->mNumAnimations;
        Log::get().write(oss.str());

        oss.str("");
        oss << "Cameras: " << ais->mNumCameras;
        Log::get().write(oss.str());

        oss.str("");
        oss << "Lights: " << ais->mNumLights;
        Log::get().write(oss.str());

        oss.str("");
        oss << "Materials: " << ais->mNumMaterials;
        Log::get().write(oss.str());

        oss.str("");
        oss << "Meshes: " << ais->mNumMeshes;
        Log::get().write(oss.str());

        oss.str("");
        oss << "Textures: " << ais->mNumTextures;
        Log::get().write(oss.str());
    }
}

void Scene::nullify()
{
    m_SkyColor = Color(0.0f, 0.0f, 0.0f);
    //m_pImporter = NULL;
    // = NULL;
    //m_pRoot.reset();
    //m_sError = "";
    //m_bLightmaps = false;

    //m_pPlayer = NULL;
    //m_pPhysics = NULL;
}

Scene::~Scene()
{
    m_spRoot.reset(); // must reset root before deleting physics or segfault
    m_spPhysics.reset();

    //foreach(Mesh* mesh, tempdata.meshes)
    //    delete mesh;

    //// clear scene material list
    //foreach(Material* material, tempdata.materials)
    //    delete material;

    //for(auto itr = m_MeshPool.begin(); itr != m_MeshPool.end(); ++itr)
    //    delete itr->second;
    //for(auto itr = m_Textures.begin(); itr != m_Textures.end(); ++itr)
    //    delete itr->second;
}


void Scene::clear()
{
    m_spRoot.reset(new Node(Node::defaultFlags(), NodeLayer::ROOT));
}

void Scene::clearEnvironment()
{
    if(!m_spRoot)
    {
        clear();
        return;
    }
}

int Scene::logic(unsigned int a)
{
    m_spRoot->logic(a);
    if(m_spPhysics.get())
        m_spPhysics->logic(a, m_spRoot.get());
    return 0;
}

void Scene::render(Node* from) const
{
    if(!from)
        from = m_spRoot.get();
    
    ASSERT(from);

    Frustum f;
    from->renderFrom(m_spPartitioner.get());
    
    // sets the current modelview matrix to be the view matrix
    f.refresh();
    Renderer::get().setViewUniform();
    
    //m_pProgram->use();
    
    glClearColor(m_SkyColor.r, m_SkyColor.g, m_SkyColor.b, 1.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    // initial pass
    // TODO: bind an ambient shader here!
    glDisable(GL_BLEND);
    m_spRoot->render(m_spPartitioner.get());

    if(Renderer::get().lighting()) { // do multi-pass only if lighting is enabled
        // multi-pass light rendering utilizing partitioner
        std::list<Light*> light_list(std::move(m_spPartitioner->getViewableLights(m_spRoot.get())));
        std::list<Node*> node_list; // TODO: reserve some space here?
        unsigned int pass = 0;
        std::list<Node*> visible_nodes = std::move(m_spPartitioner->getViewableNodes(m_spRoot.get()));
        foreach(Light* light, light_list)
        {
            // TODO: Render initial ambient pass (no blend for pass 0 is fine for now,
            //  since we aren't doing light-object clipping yet)
            //if(pass == 0)
            //{
            //    glDisable(GL_BLEND);
            //}
            //else
            //{
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE);
            //}
            node_list = std::move(m_spPartitioner->getLitObjects(light, visible_nodes));
            if(!node_list.empty())
            {
                light->bind(0);
                foreach(Node* n, node_list)
                    n->render(m_spPartitioner.get(), Node::RENDER_SELF_ONLY);
            }
            //pass++;
        }
        glDisable(GL_BLEND);
    }
    //if(m_spRoot)
    //   m_spRoot->render(m_spPartitioner.get());
}

void Scene::renderOverview(const glm::vec3& pos, float scale) const
{
    if(floatcmp(scale, 0.0f))
        return;

    glm::mat4 inv = glm::scale(glm::mat4(), glm::vec3(scale));
    inv = glm::rotate(inv, 90.0f, Axis::X);
    //Matrix::translation(inv, glm::vec3(-pos.x, 0.0f, pos.z));
    Matrix::translation(inv, glm::vec3(-pos.x, pos.z, 0.0f));
    glMultMatrixf(glm::value_ptr(inv));
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    
    Renderer::get().setViewUniform();
    
    m_spRoot->render();
}

bool Scene :: loadAINode(EnvironmentNode* node, const aiNode* ainode, TempData* tempdata)
{
    assert(node && ainode);

    // load transformation from aiScene structure
    aiMatrix4x4 m = ainode->mTransformation;
    m.Transpose();
    *node->matrix() = Matrix::fromArray((float*)&m);

    // load geometry from aiScene structure
    for(unsigned int i=0; i<ainode->mNumMeshes; i++)
        node->addMesh(tempdata->meshes[ainode->mMeshes[i]]);

    // traverse and build children
    EnvironmentNode* child;
    for(unsigned int i=0; i<ainode->mNumChildren;i++)
    {
        child = new EnvironmentNode(Node::defaultFlags(), "", NodeLayer::SUB_ENVIRONMENT);

        if(node->add(child))
        {
            if(!loadAINode(child, ainode->mChildren[i], tempdata))
            {
                //delete child;
                Log::get().error("Failed to load EnvironmentNode into memory.");
            }
            child->recalculate();
        }
        else
        {
            //delete child;
            Log::get().error("Failed to add new node to Scene.");
            //return false; //error
        }
    }
    return true;
}

bool Scene :: loadAIMeshData(const std::string& scene_fn, aiScene* aiscene, aiScene* aiscene_lmap, TempData* tempdata)
{
    // this loading code loads meshes only, not embedded node transformations

    tempdata->meshes.resize(aiscene->mNumMeshes);
    for(unsigned int i=0; i<aiscene->mNumMeshes; i++)
    {
        aiMesh* aimesh = aiscene->mMeshes[i];
        aiMesh* aimesh_lmap = NULL;
        if(aiscene_lmap)
            aimesh_lmap = aiscene_lmap->mMeshes[i];

        //ASSERT(aimesh->mName.length);
        std::string mesh_name = aimesh->mName.data;
        if(mesh_name.empty())
            mesh_name = str(i); // use index-based named if no name is available

        //Mesh* mesh = new Mesh(aimesh->mName.data);
        Log::get().write("Loading mesh: " + scene_fn + ":" + mesh_name);
        std::shared_ptr<Mesh> mesh = m_Meshes.ensure_shared(scene_fn + ":" + mesh_name);
        tempdata->meshes[i] = mesh;
        if(!mesh->name.empty()){ // if name is empty, mesh was just created
            Log::get().write("Instanced mesh: " + scene_fn + ":" + mesh_name);
            continue; // Mesh already loaded!
        }

        //mesh->name = aimesh->mName.data;
        mesh->name = mesh_name;

        //if(aimesh->mMaterialIndex)
        //{
            // why doesn't this work:
            //mesh->materials.resize(1 + aimesh_lmap?1:0);
            // ugh...
            if(aimesh_lmap)
                mesh->materials.resize(2);
            else
                mesh->materials.resize(1);

            // diffuse:
            mesh->materials[0] = tempdata->materials[aimesh->mMaterialIndex];
            // lightmaps:
            if(aimesh_lmap)
                mesh->materials[1] = tempdata->materials[aiscene->mNumMaterials + aimesh_lmap->mMaterialIndex];
        //}

        if(aimesh->mNumVertices)
        {
            if(aimesh->HasTangentsAndBitangents())
            {
                mesh->bitangents.resize(aimesh->mNumVertices);
                for(unsigned int j=0; j<aimesh->mNumVertices; j++)
                    AITypeParser::parseVector(mesh->bitangents[j], aimesh->mBitangents[j]);

                mesh->tangents.resize(aimesh->mNumVertices);
                for(unsigned int j=0; j<aimesh->mNumVertices; j++)
                    AITypeParser::parseVector(mesh->tangents[j], aimesh->mTangents[j]);
            }

            if(aimesh->mNormals)
            {
                mesh->normals.resize(aimesh->mNumVertices);
                for(unsigned int j=0; j<aimesh->mNumVertices; j++)
                    AITypeParser::parseVector(mesh->normals[j], aimesh->mNormals[j]);
            }

            if(aimesh->HasTextureCoords(0))
            {
                if(aimesh_lmap)
                    mesh->UVs.resize(2);
                else
                    mesh->UVs.resize(1);
            
                // lightmaps
                //cout << aimesh_lmap->mNumVertices << endl;
                //cout << aimesh->mNumVertices << endl;
                if(aimesh_lmap)
                    if(aimesh_lmap->mNumVertices != aimesh->mNumVertices)
                    {
                        Log::get().error("Mesh vertex count and lightmap do not match.");
                        return false; // mesh and lightmap mesh num verts don't match
                    }

                mesh->UVs[0].resize(aimesh->mNumVertices);
                if(aimesh_lmap)
                    mesh->UVs[1].resize(aimesh_lmap->mNumVertices); // should be the same #verts
                for(unsigned int j=0; j<aimesh->mNumVertices; j++)
                {
                    AITypeParser::parseVector(mesh->UVs[0][j], aimesh->mTextureCoords[0][j]);
                    if(aimesh_lmap)
                        AITypeParser::parseVector(mesh->UVs[1][j], aimesh_lmap->mTextureCoords[0][j]);
                }
            }
        
            mesh->vertices.resize(aimesh->mNumVertices);
            for(unsigned int j=0; j<aimesh->mNumVertices; j++){
                AITypeParser::parseVector(mesh->vertices[j], aimesh->mVertices[j]);
                //mesh->vertices[j] *= AI_SCALE;
            }
        }
        
        if(aimesh->mNumFaces)
        {
            mesh->faces.resize(aimesh->mNumFaces);
            for(unsigned int j=0; j<aimesh->mNumFaces; j++)
            {
                if(aimesh->mFaces[j].mNumIndices > 3)
                {
                    Log::get().error("Mesh was not triangulated.  Face has " + str(aimesh->mFaces[j].mNumIndices) + " vertices.");
                    return false; // not triangulated
                }
                else if(aimesh->mFaces[j].mNumIndices < 3)
                {
                    Log::get().warning("Face has only " + str(aimesh->mFaces[j].mNumIndices) + " vertices.  Lines and points are ignored.");
                    //return false; // not triangulated
                }

                //mesh->faces[j].indices.resize(aimesh->mFaces[j].mNumIndices);
                for(unsigned int k=0; k<3; k++)
                    mesh->faces[j].indices[k] = aimesh->mFaces[j].mIndices[k];
            }
        }
    }
    return true;
}

bool Scene :: add(Node* node, Node* parent)
{
    if(!node)
        return false;
    if(!parent)
        parent = m_spRoot.get();

    parent->add(node);
    //if(m_Flags & F_PHYSICS)
    //    m_spPhysics->generate(node, Physics::GEN_RECURSIVE);
    return true;
}

void Scene :: compile()
{
    if(m_Flags & F_PHYSICS)
        m_spPhysics->generate(m_spRoot.get(), Physics::GEN_RECURSIVE);
}


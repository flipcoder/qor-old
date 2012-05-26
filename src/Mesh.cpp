#include "Mesh.h"
#include "Util.h"
#include "Texture.h"
#include <assimp/assimp.hpp>
#include <assimp/aiScene.h>
#include <assimp/aiPostProcess.h>
#include "MeshTypeParser.h"
#include "Util.h"

#include <iostream>
#include <memory>
#include <list>
using namespace std;

//#define MESH_SCALE 0.001f
//#define MESH_SCALE 1.0f

Mesh :: Mesh(std::string n, float minlod)
{
    name = n;
    minimum_lod = minlod;
    flags = 0;
    //bMaterialAlloc = false;
}

Mesh :: Mesh(std::string fn, ResourceCache<Texture>& tex_map, std::string override_name, float minlod)
{
    minimum_lod = minlod;
    flags = 0;
    //bMaterialAlloc = false;

    load(fn, tex_map);
    if(override_name != "")
        name = override_name; // override loaded name
}

Mesh :: ~Mesh()
{
    //if(bMaterialAlloc)
    //{
        //for(auto itr = materials.begin();
        //    itr != materials.end();
        //    ++itr)
        //{
        //    delete *itr;
        //}
    //}
}


bool Mesh :: load(std::string fn, ResourceCache<Texture>& rmap)
{
    //bMaterialAlloc = true;

    std::string path = "";
    size_t split_point;
    if((split_point = fn.find_last_of("\\/")) != string::npos)
    {
        path = fn.substr(0,split_point+1);
        fn = fn.substr(split_point+1);
    }

    auto_ptr<Assimp::Importer> importer(new Assimp::Importer());
    unsigned int flags = aiProcess_Triangulate|
        aiProcess_CalcTangentSpace|
        aiProcess_GenSmoothNormals;//GenNormals
        //aiProcess_GenSmoothNormals;

    const aiScene* aiscene = importer->ReadFile(path + fn, flags);
    std::string err = importer->GetErrorString();

    if(err != "" || 
        aiscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        aiscene->mFlags & AI_SCENE_FLAGS_VALIDATED)
    {
        return false;
    }
    
    ASSERT(aiscene->mNumMeshes <= 1);
    if(aiscene->mNumMeshes != 1)
        return false;
    
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

            ASSERT(texpath.length);
            
            shared_ptr<Material> material = Material::allocate(path + string(texpath.data), rmap);
            
            if(!material ||
                !material->good())
                return false;
            
            materials.push_back(material);
            break;
        }
    }

    for(unsigned int i = 0; i < aiscene->mNumMeshes; i++)
        loadModel(aiscene->mMeshes[i]);

    return true;
}


void Mesh :: logic(unsigned int advance)
{
    
}

void Mesh :: render(unsigned int flags, float quality) const
{
    // Level of Detail approximations
    if(flags & RENDER_LOD)
    {
        if(quality <= minimum_lod)
            return;

        Mesh* lod_mesh = getLOD(quality);
        if(lod_mesh)
        {
            lod_mesh->render(flags & (!RENDER_LOD));
            return;
        }
    }

    // Material failsafe
    unsigned int nMaterials = materials.size();
    if(nMaterials == 0)
        return;
    unsigned int i = 0;
    //for(unsigned int i = 0, layer = 0; i < nMaterials; ++i)
    //{
        if(!materials[i] ||
           !materials[i]->good())
        {
            return;
            //break;
        }

        materials[i]->bind(i);
    //}
    
    // OpenGL render code
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, &vertices[0].x);

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, &normals[0].x);

    for(unsigned int i = 0; i < nMaterials; ++i)
    {
        glClientActiveTexture(GL_TEXTURE0 + i);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(3, GL_FLOAT, 0, &UVs[i][0].x);
    }
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, true, 0, &tangents[0].x);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 3, GL_FLOAT, true, 0, &bitangents[0].x);

    glDrawElements(GL_TRIANGLES, faces.size()*3, GL_UNSIGNED_INT, &faces[0].indices);

    for(unsigned int i = 0; i < nMaterials; ++i)
    {
        glClientActiveTexture(GL_TEXTURE0 + i);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(6);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    Texture::unbind();

    // just for reference, here's some similar immediate-mode code (old)
    /*
    for(unsigned int t = 0; t < faces.size(); t++)
    {
        glBegin(GL_TRIANGLES);
        for(unsigned int i = 0; i < 3; i++) {
            unsigned int index = faces[t].indices[i];

            if(normals.size())
                glNormal3fv(&normals[index].x);
            
            // TODO: Add multitexture for lightmap here
            if(UVs.size() && UVs[0].size())
            {
                glMultiTexCoord2fv(GL_TEXTURE0, &UVs[0][index].x);
                glMultiTexCoord2fv(GL_TEXTURE1, &UVs[1][index].x);
            }

            glVertex3fv(&vertices[index].x);
        }

        glEnd();
    }*/
}

bool Mesh :: loadModel(const aiMesh* aimesh)
{
    name = aimesh->mName.data;

    if(aimesh->mNumVertices)
    {
        if(aimesh->HasTangentsAndBitangents())
        {
            bitangents.resize(aimesh->mNumVertices);
            for(unsigned int j=0; j<aimesh->mNumVertices; j++)
                MeshTypeParser::parseVector(bitangents[j], aimesh->mBitangents[j]);
        
            tangents.resize(aimesh->mNumVertices);
            for(unsigned int j=0; j<aimesh->mNumVertices; j++)
                MeshTypeParser::parseVector(tangents[j], aimesh->mTangents[j]);
        }

        if(aimesh->mNormals)
        {
            normals.resize(aimesh->mNumVertices);
            for(unsigned int j=0; j<aimesh->mNumVertices; j++)
                MeshTypeParser::parseVector(normals[j], aimesh->mNormals[j]);
        }

        if(aimesh->HasTextureCoords(0))
        {
            UVs.resize(1);
            UVs[0].resize(aimesh->mNumVertices);
            for(unsigned int j=0; j<aimesh->mNumVertices; j++)
            {
                MeshTypeParser::parseVector(UVs[0][j], aimesh->mTextureCoords[0][j]);
                //MeshTypeParser::parseVector(UVs[1][j], aimesh_lmap->mTextureCoords[0][j]);
            }
        }
    
        vertices.resize(aimesh->mNumVertices);
        for(unsigned int j=0; j<aimesh->mNumVertices; j++){
            MeshTypeParser::parseVector(vertices[j], aimesh->mVertices[j]);
            //vertices[j] *= MESH_SCALE;
        }
    }
    
    if(aimesh->mNumFaces)
    {
        faces.resize(aimesh->mNumFaces);
        for(unsigned int j=0; j<aimesh->mNumFaces; j++)
        {
            ASSERT(aimesh->mFaces[j].mNumIndices == 3);

            //faces[j].indices.resize(aimesh->mFaces[j].mNumIndices);
            for(unsigned int k=0; k<3; k++)
                faces[j].indices[k] = aimesh->mFaces[j].mIndices[k];
        }
    }

    return true;
}


//bool Mesh :: generateShadow(const Mesh& shadow)
//{
//    return false;
//}

Mesh* Mesh :: getLOD(float quality) const
{
    return NULL;
    /*unsigned int lod_idx = round_uint(LOD.size() * quality);

    if(lod_idx < LOD.size() && lod_idx >= 0)
    {
        ASSERT(LOD[lod_idx]);
        return LOD[lod_idx]->get();
    }*/
}


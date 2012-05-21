#include "Material.h"
#include <string>
#include <memory>
#include "Filesystem.h"
#include "Log.h"
using namespace std;

Material::~Material()
{

}

shared_ptr<Texture> Material :: allocateTex(string filename, string ext, ResourceCache<Texture>& rmap)
{
    shared_ptr<Texture> tex;
    
    size_t split_point;
    string fn = "";
    if((split_point = filename.find_last_of(".")) != string::npos)
        fn = filename.substr(0,split_point) + "_" + ext + filename.substr(split_point);
    
    if(!fn.empty()) {
        tex = rmap.cache(fn);
        assert(tex.get());
    }
    if(!tex->good())
        if(! tex->loadTex(fn, Texture::MIPMAPPED))
        {
            rmap.remove(fn);
            tex.reset();
            //tex = shared_ptr<Texture>(); // uncomment this if you remove the block below
            
            // this block below for testing purposes only, allows texs to be stored
            // in data/materials when diffuse texture is not
            // Otherwise, diffuse and tex need to be together
            fn = string("data/ne/materials/") + FS::getFileName(fn);
            
            //Log::get().write("(2) Attempting to load " + fn);
            
            tex = rmap.cache(fn);
            assert(tex.get());
            if(!tex->good())
            {
                if(! tex->loadTex(fn, Texture::MIPMAPPED))
                {
                    rmap.remove(fn);
                    tex = shared_ptr<Texture>();
                }
            }
        }
    return tex;
}

shared_ptr<Material> Material :: allocate(string fn, ResourceCache<Texture>& rmap)
{
    // is the texture 'fn' already loaded?  (Don't load textures twice)
    
    shared_ptr<Texture> tex = rmap.cache(fn);
    if(!tex->good())
        if(! tex->loadTex(fn, Texture::MIPMAPPED))
        {
            rmap.remove(fn);
            tex.reset();
            
            // change to default texture path
            fn = string("data/ne/materials/") + FS::getFileName(fn);
            tex = rmap.cache(fn);
            if(!tex->good())
            {
                if(! tex->loadTex(fn, Texture::MIPMAPPED))
                {
                    rmap.remove(fn);
                    return shared_ptr<Material>();
                }
            }
        }
    
    Log::get().write("Loaded Material " + fn);
            
    shared_ptr<Texture> bump = allocateTex(fn, "NRM", rmap);
    shared_ptr<Texture> disp = allocateTex(fn, "DISP", rmap);
    shared_ptr<Texture> spec = allocateTex(fn, "SPEC", rmap);
    shared_ptr<Texture> occ = allocateTex(fn, "OCC", rmap);
    
    return shared_ptr<Material>(new Material(tex, bump, disp, spec, occ));
}


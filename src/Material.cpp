#include "Material.h"
#include <string>
#include <memory>
#include "Filesystem.h"
#include "Log.h"
using namespace std;

Material::~Material()
{

}

shared_ptr<Texture> Material :: allocateTex(string filename, string ext, ResourceCache<Texture>& rcache)
{
    shared_ptr<Texture> tex;
    
    size_t split_point;
    string fn = "";
    if((split_point = filename.find_last_of(".")) != string::npos)
        fn = filename.substr(0,split_point) + "_" + ext + filename.substr(split_point);
    
    assert(!fn.empty());
    fn = FS::getFileName(fn);
    tex = rcache.cache(fn, ResourceCache<Texture>::F_PASS_NAME);
    if(!tex)
        Log::get().write("Failed to load material texture \"" + filename + "\"");
    //assert(tex.get() && tex->good());
    return tex;
}

shared_ptr<Material> Material :: allocate(string fn, ResourceCache<Texture>& rcache)
{
    // is the texture 'fn' already loaded?  (Don't load textures twice)
    
    fn = FS::getFileName(fn);
    shared_ptr<Texture> tex = rcache.cache(fn, ResourceCache<Texture>::F_PASS_NAME);
    assert(tex.get() && tex->good());
    
    assert(!fn.empty());
    
    Log::get().write("Loaded Material " + fn);
            
    shared_ptr<Texture> bump = allocateTex(fn, "NRM", rcache);
    shared_ptr<Texture> disp = allocateTex(fn, "DISP", rcache);
    shared_ptr<Texture> spec = allocateTex(fn, "SPEC", rcache);
    shared_ptr<Texture> occ = allocateTex(fn, "OCC", rcache);
    
    return shared_ptr<Material>(new Material(tex, bump, disp, spec, occ));
}


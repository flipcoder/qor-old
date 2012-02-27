#include "EnvironmentNode.h"
#include "Util.h"
#include "math/common.h"
#include "Log.h"

using namespace std;

void EnvironmentNode :: renderSelf(IPartitioner* partitioner, unsigned int flags) const
{
    //auto& meshes = *getMeshes_c();
    //foreach(const Mesh* m, meshes)
    //    m->render(flags);
    //
    for(list<shared_ptr<Mesh>>::const_iterator itr = getMeshes_c()->begin();
        itr != getMeshes_c()->end();
        itr++)
    {
        (*itr)->render(flags);
    }
}

void EnvironmentNode :: recalculate()
{
    for(list<shared_ptr<Mesh>>::const_iterator itr = getMeshes_c()->begin();
        itr != getMeshes_c()->end();
        ++itr)
    {
        for(vector<glm::vec3>::const_iterator jtr = (*itr)->vertices.begin();
            jtr != (*itr)->vertices.end();
            ++jtr)
        {
            for(int i=0; i<3; i++)
            {
                if((*jtr)[i] > m_Box.max[i])
                    m_Box.max[i] = (*jtr)[i];
                else if((*jtr)[i] < m_Box.min[i])
                    m_Box.min[i] = (*jtr)[i];
            }
        }
    }
}



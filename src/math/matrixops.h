#ifndef _MATRIXOPS_H
#define _MATRIXOPS_H

#include "common.h"
#include <glm/glm.hpp>
#include <glm/gtc/swizzle.hpp>

// extra Matrix functions that aren't easily provided by glm

namespace Matrix{
    inline void resetOrientation(glm::mat4& m) {
        float* f = glm::value_ptr(m);
        f[1]=f[2]=f[4]=f[5]=f[8]=f[9]=0.0f;
        f[0]=f[5]=f[10]=1.0f;
    }
    inline glm::vec3 translation(const glm::mat4& m){
        const float* f = glm::value_ptr(m);
        return glm::vec3(f[12], f[13], f[14]);
    }
    //inline glm::vec3 check(const glm::mat4& m) {
    //    const float* f = glm::value_ptr(m);
    //    assert(floatcmp(f[3],0.0f));
    //    assert(floatcmp(f[7],0.0f));
    //    assert(floatcmp(f[11],0.0f));
    //    assert(floatcmp(f[15],1.0f));
    //}
    inline glm::vec3 translation(glm::mat4& m, const glm::vec3& t){
        float* f = glm::value_ptr(m);
        f[12] = t[0];
        f[13] = t[1];
        f[14] = t[2];
    }
    inline glm::vec3 translate(glm::mat4& m, const glm::vec3& t) {
        float* f = glm::value_ptr(m);
        f[12] += t[0];
        f[13] += t[1];
        f[14] += t[2];
    }
    inline glm::vec3 resetTranslation(glm::mat4& m) {
        float* f = glm::value_ptr(m);
        f[12]=f[13]=f[14]=0.0f;
    }
    inline glm::vec3 up(const glm::mat4& m) {
        const float* f = glm::value_ptr(m);
        return glm::vec3(f[4], f[5], f[6]);
    }
    inline glm::vec3 heading(const glm::mat4& m)
    {
        const float* f = glm::value_ptr(m);
        return -glm::vec3(f[8], f[9], f[10]);
    }
    
    // remember to normalize after computing these
    inline glm::vec3 upXZ(const glm::mat4& m)
    {
        glm::vec3 r = up(m);
        r.y = 0.0f;
        return glm::normalize(r);
    }
    inline glm::vec3 headingXZ(const glm::mat4& m)
    {
        glm::vec3 r = heading(m);
        r.y = 0.0f;
        return glm::normalize(r);
    }
    
    inline bool isIdentity(const glm::mat4& m)
    {
        // glm takes care of floatcmp and epsilon value
        return m == glm::mat4();
    }
    
    inline bool isZero(const glm::mat4& m)
    {
        // glm takes care of floatcmp and epsilon value
        return m == glm::mat4(0.0f);
    }
    
    inline glm::vec3 mult(const glm::vec3& v, const glm::mat4& m)
    {
        return glm::swizzle<glm::X, glm::Y, glm::Z>(glm::vec4(v, 1.0f) * m);
    }
    inline glm::vec3 mult(const glm::mat4& m, const glm::vec3& v)
    {
        return glm::swizzle<glm::X, glm::Y, glm::Z>(m * glm::vec4(v, 1.0f));
    }
    
    inline glm::mat4 fromArray(const float* f)
    {
        glm::mat4 m;
        float* m_array = glm::value_ptr(m);
        for(int i=0;i<16;i++)
            m_array[i] = f[i];
        return m;
    }
}

#endif


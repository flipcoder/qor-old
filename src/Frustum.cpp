#include "Frustum.h"
#include "GfxAPI.h"

void Frustum :: normalize(Frustum::eSide side)
{
    float mag = sqrt((m_vFrustum[side].x * m_vFrustum[side].x) +
                     (m_vFrustum[side].y * m_vFrustum[side].y) +
                     (m_vFrustum[side].z * m_vFrustum[side].z) +
                     (m_fFrustumDist[side] * m_fFrustumDist[side]));

    m_vFrustum[side].x /= mag;
    m_vFrustum[side].y /= mag;
    m_vFrustum[side].z /= mag;
    m_fFrustumDist[side] /= mag;
}


void Frustum :: refresh()
{
    // get gl planes
    float projection[16];
    float modelview[16];
    float clipping[16] = {0.0f};
    glGetFloatv(GL_PROJECTION_MATRIX, projection);
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

    // construct clipping plane
    for(int i=0; i<16; i++)
        for(int j=0; j<4; j++)
            clipping[i] += modelview[(i/4*4)+j] * projection[(i%4)+(j*4)];

    // construct frustum planes
    for(int i=0,j=0; i<MAX_SIDES; i++)
    {
        for(j=0; j<3; j++)
            m_vFrustum[i][j] = clipping[(j*4) + 3] + (clipping[(j*4) + (i/2)] * ((i%2==0)||(i==0)?(-1.0f):(1.0f)));
        m_fFrustumDist[i] = clipping[(j*4) + 3] + (clipping[(j*4) + (i/2)]  * ((i%2==0)||(i==0)?(-1.0f):(1.0f)));
        normalize((eSide)i);
    }
}

bool Frustum :: containsPoint(float x, float y, float z)
{
    for(int i=0; i<MAX_SIDES; i++)
        if( m_vFrustum[i].x * x +
            m_vFrustum[i].y * y +
            m_vFrustum[i].z * z +
            m_fFrustumDist[i] <= 0 )
        {
                return false;
        }

    return true;
}

bool Frustum :: containsSphere(float x, float y, float z, float r)
{
    for(int i=0; i<MAX_SIDES; i++)
        if( m_vFrustum[i].x * x +
            m_vFrustum[i].y * y +
            m_vFrustum[i].z * z +
            m_fFrustumDist[i] <= -r )
        {
                return false;
        }

    return true;
}

bool Frustum :: containsBox(float x, float y, float z, float r)
{
	for(int i=0; i<MAX_SIDES; i++)
	{
		if(m_vFrustum[i].x * (x-r) + m_vFrustum[i].y * (y-r) + m_vFrustum[i].z * (z-r) + m_fFrustumDist[i] > 0)
			continue;
		if(m_vFrustum[i].x * (x+r) + m_vFrustum[i].y * (y-r) + m_vFrustum[i].z * (z-r) + m_fFrustumDist[i] > 0)
			continue;
		if(m_vFrustum[i].x * (x-r) + m_vFrustum[i].y * (y+r) + m_vFrustum[i].z * (z-r) + m_fFrustumDist[i] > 0)
			continue;
		if(m_vFrustum[i].x * (x+r) + m_vFrustum[i].y * (y+r) + m_vFrustum[i].z * (z-r) + m_fFrustumDist[i] > 0)
			continue;
		if(m_vFrustum[i].x * (x-r) + m_vFrustum[i].y * (y-r) + m_vFrustum[i].z * (z+r) + m_fFrustumDist[i] > 0)
			continue;
		if(m_vFrustum[i].x * (x+r) + m_vFrustum[i].y * (y-r) + m_vFrustum[i].z * (z+r) + m_fFrustumDist[i] > 0)
			continue;
		if(m_vFrustum[i].x * (x-r) + m_vFrustum[i].y * (y+r) + m_vFrustum[i].z * (z+r) + m_fFrustumDist[i] > 0)
			continue;
		if(m_vFrustum[i].x * (x+r) + m_vFrustum[i].y * (y+r) + m_vFrustum[i].z * (z+r) + m_fFrustumDist[i] > 0)
			continue;

		return false;
	}
	return true;
}


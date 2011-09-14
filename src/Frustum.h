
#ifndef _FRUSTUM_H
#define _FRUSTUM_H

#include <glm/glm.hpp>

class Frustum
{
	private:
		enum eSide
		{
			S_RIGHT,
			S_LEFT,
			S_TOP,
			S_BOTTOM,
			S_FAR,
			S_NEAR,
			MAX_SIDES
		};

		glm::vec3 m_vFrustum[MAX_SIDES];
		float m_fFrustumDist[MAX_SIDES];

		void normalize(eSide side);

	public:

		Frustum(){
			for(int i=0;i<MAX_SIDES;i++)
				m_fFrustumDist[i]=0.0f;
		}

		void refresh();

		bool containsPoint(float x, float y, float z);
		bool containsSphere(float x, float y, float z, float r);
		bool containsBox(float x, float y, float z, float r);
};

#endif

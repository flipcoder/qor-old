#ifndef _EULERNODE_H
#define _EULERNODE_H

#include "Node.h"
#include "math/common.h"
#include "math/angle.h"

class EulerNode : public Node
{
protected:

	// store in degrees
	Angle m_rot;
	Angle m_pitch;
	Angle m_tilt;

	bool m_bPitchClamp;
	float m_fPitchClamp;

	mutable bool m_bNeedsRefresh;

	//float wrap(float deg, float limit = 180.0f);

	void update() const;

public:

	EulerNode();
	virtual ~EulerNode();

	virtual glm::mat4* matrix();
	virtual const glm::mat4* matrix_c() const;
	virtual void resetOrientation() {
		m_rot = m_pitch = m_tilt = Angle(0.0f);
	}

	void clear();
	void setRot(float deg);
	void setPitch(float deg);
	void setTilt(float deg);
	void rot(float deg);
	void pitch(float deg);
	void tilt(float deg);
	float rot() const { return m_rot.degrees(); }
	float pitch() const { return m_pitch.degrees(); }
	float tilt() const { return m_tilt.degrees(); }

	void clampPitch(float limit) {
		m_fPitchClamp = limit;
		m_bPitchClamp = true;
	}
	void removeClamp() {
		m_fPitchClamp = 90.0f;
		m_bPitchClamp = false;
	}

	//glm::vec3 getHeading() const;
	glm::vec3 getRotationHeading() const;
};

#endif


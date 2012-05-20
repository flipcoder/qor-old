#include "EulerNode.h"
#include "math/common.h"
#include "math/matrixops.h"

EulerNode :: EulerNode()
{
    m_rot = m_pitch = m_tilt = Angle(0.0f);
    m_bNeedsRefresh = false;
    m_bPitchClamp = false;
    m_fPitchClamp = 90.0f;
}

EulerNode :: ~EulerNode()
{

}

void EulerNode :: update() const
{
    glm::vec3 pos = Matrix::translation(m_Matrix);
    //m_Matrix.clear(Matrix::ROTATE_Y, m_rot);
    m_Matrix = glm::rotate(glm::mat4(), m_rot.degrees(), Axis::Y);
    m_Matrix *= glm::rotate(glm::mat4(), m_pitch.degrees(), Axis::X);
    m_Matrix *= glm::rotate(glm::mat4(), m_tilt.degrees(), Axis::Z);
    //m_Matrix = m_Matrix * Matrix(Matrix::ROTATE_X, m_pitch);
    //m_Matrix = m_Matrix * Matrix(Matrix::ROTATE_Z, m_tilt);
    Matrix::translation(m_Matrix, pos);
    //m_Matrix.setTranslation(pos);
    m_bNeedsRefresh = false;
    pendWorldMatrix();
}

void EulerNode :: clear()
{
    m_rot = m_pitch = m_tilt = Angle(0.0f);
    m_bNeedsRefresh = true;
}

glm::mat4* EulerNode :: matrix()
{
    if(m_bNeedsRefresh)
        update();
    return &m_Matrix;
}

const glm::mat4* EulerNode :: matrix_c() const
{
    if(m_bNeedsRefresh)
        update();
    return &m_Matrix;
}


//float EulerNode :: wrap(float deg, float limit)
//{
//    while(deg >= limit)
//        deg -= limit*2.0f;
//    while(deg < -limit)
//        deg += limit*2.0f;
//    return deg;
//}

void EulerNode :: setRot(float deg)
{
    if(m_rot == deg)
        return;

    m_rot=deg;
    m_bNeedsRefresh = true;

    //if(!floatcmp(deg,0.0f))
    //{
    //    m_rot = wrap(m_rot);
    //    m_bNeedsRefresh=true;
    //}
}

void EulerNode :: setPitch(float deg)
{
    if(floatcmp(deg, 0.0f))
        return;

    m_pitch=deg;
    m_bNeedsRefresh=true;

    if(m_bPitchClamp)
    {
        if(m_pitch > m_fPitchClamp)
            m_pitch = m_fPitchClamp;
        else if(m_pitch < -m_fPitchClamp)
            m_pitch = -m_fPitchClamp;
    }

    //if(!floatcmp(deg,0.0f))
    //{
    //    m_pitch = wrap(m_pitch);
    //    m_bNeedsRefresh=true;
    //}
}

void EulerNode :: setTilt(float deg)
{
    if(m_tilt == deg)
        return;
    m_tilt = Angle(deg);
    m_bNeedsRefresh=true;
    
    //if(!floatcmp(deg,0.0f))
    //{
    //    m_tilt = wrap(m_tilt);
    //    m_bNeedsRefresh=true;
    //}
}

void EulerNode :: rot(float deg)
{
    if(floatcmp(deg, 0.0f))
        return;
    m_rot += Angle(deg);
    m_bNeedsRefresh=true;
}

void EulerNode :: pitch(float deg)
{
    if(floatcmp(deg,0.0f))
        return;

    m_pitch+=Angle(deg);

    if(m_bPitchClamp)
    {
        if(m_pitch > m_fPitchClamp)
            m_pitch = m_fPitchClamp;
        else if(m_pitch < -m_fPitchClamp)
            m_pitch = -m_fPitchClamp;
    }
    m_bNeedsRefresh = true;

    //if(!floatcmp(deg,0.0f))
    //{
    //    m_pitch = wrap(m_pitch);
    //    m_bNeedsRefresh=true;
    //}
}

void EulerNode :: tilt(float deg)
{
    if(floatcmp(deg, 0.0f))
        return;

    m_tilt+=Angle(deg);
    m_bNeedsRefresh=true;
}

glm::vec3 EulerNode :: getRotationHeading() const
{
    if(floatcmp(m_pitch.degrees(), 90.0f))
        return -Matrix::upXZ(*matrix_c());
    else if(floatcmp(m_pitch.degrees(), -90.0f))
        return Matrix::upXZ(*matrix_c());
    return Matrix::headingXZ(*matrix_c());
    //    return matrix_c()->up().XZ().unit();
    //return matrix_c()->heading().XZ().unit();
}


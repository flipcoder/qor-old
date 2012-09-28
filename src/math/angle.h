#ifndef _ANGLE_H
#define _ANGLE_H

#include "common.h"

class Angle
{

    float m_fDeg;

    public:
        
        enum Type{
            DEGREES,
            RADIANS
        };

        Angle():
            m_fDeg(0.0f)
        {
        }

        explicit Angle(float a, Type t = DEGREES)
        {
            m_fDeg = t==DEGREES ? a : RAD2DEGf(a);
            wrap();
        }
        
        static Angle degrees(float deg) {
            return Angle(deg, DEGREES);
        }
        static Angle radians(float rad) {
            return Angle(rad, RADIANS);
        }

        void wrap()
        {
            while(m_fDeg >= 180.0f)
                m_fDeg -= 360.0f;
            while(m_fDeg < -180.0f)
                m_fDeg += 360.0f;
        }

        glm::vec2 vector(float mag = 1.0f) {
            float rad = DEG2RADf(m_fDeg);
            return glm::vec2(
                mag * std::cos(rad),
                mag * std::sin(rad)
            );
        }

        float cos() const {
            return std::cos(DEG2RADf(m_fDeg));
        }
        float sin() const {
            return std::sin(DEG2RADf(m_fDeg));
        }

        float degrees() const { return m_fDeg; }
        float radians() const { return DEG2RADf(m_fDeg); }
        //void degrees(float deg) {
        //    m_fDeg = deg; 
        //    wrap();
        //}
        //void radians(float rad) {
        //    m_fDeg = RAD2DEGf(rad);
        //    wrap();
        //}

        Angle operator +(const Angle& a) const{
            return Angle(m_fDeg + a.degrees());
        }
        Angle operator +=(const Angle& a){
            return (*this = *this+a);
        }
        Angle operator -(const Angle& a) const{
            return Angle(m_fDeg - a.degrees());
        }
        Angle operator -() const {
            return Angle(-m_fDeg);
        }
        Angle operator *(const float& f) {
            return Angle(m_fDeg * f);
        }
        Angle operator*=(const float& f) {
            return (*this = *this*f);
        }
        bool operator==(const Angle& a) {
            return floatcmp(m_fDeg, a.degrees());
        }

        bool operator==(const float& f) {
            return floatcmp(m_fDeg, f);
        }
        bool operator!=(const float& f) {
            return !floatcmp(m_fDeg, f);
        }
        Angle operator=(const float& f) {
            m_fDeg = f;
            wrap();
            return *this;
        }

        bool operator>(const float& f) {
            return degrees() > f;
        }
        bool operator>=(const float& f) {
            return degrees() >= f;
        }
        bool operator<(const float& f) {
            return degrees() < f;
        }
        bool operator<=(const float& f) {
            return degrees() <= f;
        }

        bool operator>(const Angle& a) {
            return degrees() > a.degrees();
        }
        bool operator>=(const Angle& a) {
            return degrees() >= a.degrees();
        }
        bool operator<(const Angle& a) {
            return degrees() < a.degrees();
        }
        bool operator<=(const Angle& a) {
            return degrees() <= a.degrees();
        }
};

#endif


#ifndef _NAVIGATION_H
#define _NAVIGATION_H

#include <memory>
#include <functional>
#include <vector>
#include "Freq.h"
template<class T> class Navigation;

struct LinearInterpolation
{
    template<class T>
    T operator()(const T& a, const T& b, float t) const // where t ranges from 0..1
    {
        if(t <= 0.0f)
            return a;
        if(t >= 1.0f)
            return b;
        return a * (1.0f - t) + b * t;
    }
};

struct CosineInterpolation
{
    template<class T>
    T operator()(const T& a, const T& b, float t) const // where t ranges from 0..1
    {
        return T(); // TODO
    }
};

// TODO requirements: should work with vectors and orthogonal matrices
template<class T>
class Waypoint
{
    private:
        
        // TODO: execution callback?
        // TODO: expiry callback?
        // TODO: interpolative callback?
        T m_Position; 

        unsigned long m_ulStartTime; // transition start time
        unsigned long m_ulAlarmTime; // transition end time

        std::function<T (const T&, const T&, float)> m_Interpolation;

        Navigation<T>* m_Navigation;

    public:
        
        Waypoint(T position, Freq::Time time, Navigation<T>* nav):
            m_Position(position),
            m_ulStartTime(0L),
            m_ulAlarmTime(0L),
            m_Navigation(nav)
        {
            m_ulStartTime = Freq::get().getElapsedTime(); //ms
            m_ulAlarmTime = m_ulStartTime + time.get();
        }
        
        void poll() {
            // TODO: not necessary, but trigger callbacks if possible
        }

        bool elapsed() {
            poll();
            return Freq::get().getElapsedTime() >= m_ulAlarmTime;
        }

        // may be negative
        long remaining() {
            return m_ulAlarmTime - Freq::get().getElapsedTime();
        }

        void setTimer(Freq::Time time) {
            
        }

        T& position() {
            return m_Position;
        }
};

template<class T>
class Navigation
{
    private:

        std::vector<Waypoint<T>> m_Waypoints;
        T m_default;
        
    public:

        Navigation(
            Waypoint<T> initial,
            T default_value=T(),
            Freq::Accumulator* accum = Freq::get().accumulator()
        ):
            m_default(default_value)
        {
            m_Waypoints.push_back(std::move(initial));
        }

        virtual ~Navigation() {}

        void addWaypoint(T position, Freq::Time time) {
            m_Waypoints.push_back(Waypoint<T>(std::move(position), time, this));
        }

        void clear(T position) {
            m_Waypoints.clear();
            m_Waypoints.push_back(Waypoint<T>(position, Freq::Time(0)));
        }

        T get() {
            for(auto itr = m_Waypoints.begin();
                itr != m_Waypoints.end();
                )
            {
                if(itr->elapsed())
                    itr = itr->erase();
                else
                    ++itr;
            }
            return m_Waypoints.front().position();
        }

        bool empty() {
            return m_Waypoints.size() <= 1;
        }
};

#endif


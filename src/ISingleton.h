#ifndef _SINGLETON_H
#define _SINGLETON_H

template<typename T>
class Singleton
{
    public:
        static T& get()
        {
            static T si;
            return si;
        }
};

#endif


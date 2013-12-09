// Singleton.h

#ifndef __BRIANCHALLENGE_SINGLETON_H__
#define __BRIANCHALLENGE_SINGLETON_H__

#include "defines.h"

template <typename T> class Singleton
{
public:
    Singleton () {
        Q_ASSERT(!msSingleton);
        msSingleton = (T*)this;
    }
    ~Singleton () {
        Q_ASSERT(msSingleton);
        msSingleton = 0;
    }

    static T& GetSingleton () {
        Q_ASSERT(msSingleton);
        return (*msSingleton);
    }
    static T* GetSingletonPtr () {
        return msSingleton;
    }

private:
	//by wang liang, prevent some malfunction using
	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);



protected:
    static T* msSingleton;
};

template<typename T>
T *Singleton<T>::msSingleton=0;

#endif  // !defined(__BRIANCHALLENGE_SINGLETON_H__)

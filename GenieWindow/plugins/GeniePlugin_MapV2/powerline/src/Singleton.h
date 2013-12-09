// Singleton.h

#ifndef __BRIANCHALLENGE_SINGLETON_H__
#define __BRIANCHALLENGE_SINGLETON_H__

#define ASSERT(p) Q_ASSERT(p)

template <typename T> class Singleton
{
public:
    Singleton () {
        ASSERT(!msSingleton);
        msSingleton = (T*)this;
    }
    ~Singleton () {
        ASSERT(msSingleton);
        msSingleton = 0;
    }

    static T& GetSingleton () {
        ASSERT(msSingleton);
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

#ifndef TRANSACTIONWRAPPER_H
#define TRANSACTIONWRAPPER_H

template <typename T> class PointerTransfer
{
public:
    explicit PointerTransfer(T *pointer = 0): m_pointer(pointer){}

    PointerTransfer(const PointerTransfer &other):
        m_pointer(other.m_pointer)
    {
        PointerTransfer &mutableOther = const_cast<PointerTransfer &>(other);
        mutableOther.m_pointer = 0;
    }

    ~PointerTransfer(){delete m_pointer;}

public:
    PointerTransfer& operator = (PointerTransfer &left)
    {
        if(this != &left){
            m_pointer = left.m_pointer;
            left.m_pointer = 0;
        }

        return *this;
    }

    T* get() const {return m_pointer;}

private:
    T *m_pointer;
};

#endif // TRANSACTIONWRAPPER_H

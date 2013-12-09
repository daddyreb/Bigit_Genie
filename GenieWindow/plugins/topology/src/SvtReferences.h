/*****************************************************************
|
|   Neptune - References
|
| Copyright (c) 2002-2008, Axiomatic Systems, LLC.
| All rights reserved.
|
| Redistribution and use in source and binary forms, with or without
| modification, are permitted provided that the following conditions are met:
|     * Redistributions of source code must retain the above copyright
|       notice, this list of conditions and the following disclaimer.
|     * Redistributions in binary form must reproduce the above copyright
|       notice, this list of conditions and the following disclaimer in the
|       documentation and/or other materials provided with the distribution.
|     * Neither the name of Axiomatic Systems nor the
|       names of its contributors may be used to endorse or promote products
|       derived from this software without specific prior written permission.
|
| THIS SOFTWARE IS PROVIDED BY AXIOMATIC SYSTEMS ''AS IS'' AND ANY
| EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
| WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
| DISCLAIMED. IN NO EVENT SHALL AXIOMATIC SYSTEMS BE LIABLE FOR ANY
| DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
| (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
| LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
| ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
| (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
| SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
|
****************************************************************/

#ifndef _SVT_ReferenceS_H_
#define _SVT_ReferenceS_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/


template <typename T>
class SVT_Reference
{
public:
    // constructors and destructor
    SVT_Reference() : m_Object(NULL), m_Counter(NULL) {}
    explicit SVT_Reference(T* object) :
        m_Object(object), m_Counter(object?new quint32(1):NULL) {}

    SVT_Reference(const SVT_Reference<T>& ref) :
        m_Object(ref.m_Object), m_Counter(ref.m_Counter) {
        if (m_Counter) ++(*m_Counter);
    }

    // this methods should be private, but this causes a problem on some
    // compilers, because we need this function in order to implement
    // the cast operator operator SVT_Reference<U>() below, which would
    // have to be marked as a friend, and friend declarations with the 
    // same class name confuses some compilers
    SVT_Reference(T* object, quint32* counter) :
        m_Object(object), m_Counter(counter) {
        if (m_Counter) ++(*m_Counter);
    }

    ~SVT_Reference() {
        Release();
    }

    // overloaded operators
    SVT_Reference<T>& operator=(const SVT_Reference<T>& ref) {
        if (this != &ref) {
            Release();
            m_Object = ref.m_Object;
            m_Counter = ref.m_Counter;
            if (m_Counter) ++(*m_Counter);
        }
        return *this;
    }
    SVT_Reference<T>& operator=(T* object) {
        Release();
        m_Object = object;
        m_Counter = object?new quint32(1):NULL;
        return *this;
    }
    T& operator*() const { return *m_Object; }
    T* operator->() const { return m_Object; }

    bool operator==(const SVT_Reference<T>& ref) const {
        return m_Object == ref.m_Object;
    } 
    bool operator!=(const SVT_Reference<T>& ref) const {
        return m_Object != ref.m_Object;
    }

    bool sameContent(const SVT_Reference &ref)
    {
        return (
                    (this->isNull() && ref.isNull())
                ||(
                    (!this->isNull()&&!ref.isNull())
                    &&( *m_Object==*ref.m_Object)
                    )
                );
    }

    // overloaded cast operators
    template <typename U> operator SVT_Reference<U>() {
        return SVT_Reference<U>(m_Object, m_Counter);
    }

    // methods
    /**
     * Returns the naked pointer value.
     */
    T* asPointer() const { return m_Object; }
    
    /**
     * Returns the reference counter value.
     */
    quint32 getCounter() const { return *m_Counter; }
    
    /**
     * Returns whether this references a NULL object.
     */
    bool isNull()  const { return m_Object == NULL; }
    
    /**
     * Detach the reference from the shared object.
     * The reference count is decremented, but the object is not deleted if the
     * reference count becomes 0.
     * After the method returns, this reference does not point to any shared object.
     */
    void Detach() {
        if (m_Counter && --(*m_Counter) == 0) {
            delete m_Counter; 
        }
        m_Counter = NULL;
        m_Object  = NULL;
    }
    
private:
    // methods
    void Release() {
        if (m_Counter && --(*m_Counter) == 0) {
            delete m_Counter; m_Counter = NULL;
            delete m_Object;  m_Object  = NULL;
        }
    }

    // members
    T*            m_Object;
    quint32* m_Counter;
};

#endif // _SVT_ReferenceS_H_

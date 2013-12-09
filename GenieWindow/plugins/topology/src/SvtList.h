#ifndef SVT_LIST_H___
#define SVT_LIST_H___
template <typename T>
class _ListNode
{
public:
    _ListNode()
        :pNext(NULL)
        ,pLast(NULL)
    {};
    virtual ~_ListNode(){};

    _ListNode *pNext;
    _ListNode *pLast;
    T data;
};

template <typename T>
class SVT_List
{
public:
    SVT_List()
        :pFirst(NULL)
        ,pEnd(NULL)
        ,mCount(0)
    {

    };
    void clear()
    {
        while(pFirst)
        {
            pEnd=pFirst;
            pFirst=pFirst->pNext;
            delete pEnd;
        }
        pFirst=pEnd=NULL;
        mCount=0;
    }

    _ListNode<T> *find(const T &data)
    {
        _ListNode<T> *pLoop=pFirst;
        while(pLoop)
        {
            if(pLoop->data==data)
            {
                return pLoop;
            }
            pLoop=pLoop->pNext;
        }
        return NULL;
    }

    void remove(_ListNode<T> * node)
    {
        if(!node)
        {
            return;
        }
        if(node->pLast)
        {
            node->pLast->pNext=node->pNext;
        }
        else
        {
            pFirst=node->pNext;
        }
        if(node->pNext)
        {
            node->pNext->pLast=node->pLast;
        }
        else
        {
            pEnd=node->pLast;
        }
        delete node;
    }

    bool remove(const T &data)
    {
        _ListNode<T> *node=find(data);
        if(node)
        {
            remove(node);
            return true;
        }
        return false;
    }

    bool contains(const T &data)
    {
        _ListNode<T> *pLoop=pFirst;
        while(pLoop)
        {
            if(pLoop->data==data)
            {
                return true;
            }
            pLoop=pLoop->pNext;
        }
        return false;
    }

    ~SVT_List()
    {
        clear();
    }

    void append(const T &data)
    {
        _ListNode<T> *pData=new _ListNode<T>;
        pData->data=data;
        if(!pFirst)
        {
            Q_ASSERT(!pEnd);
            pFirst=pEnd=pData;
        }
        else
        {
            pEnd->pNext=pData;
            pData->pLast=pEnd;
            pEnd=pData;
        }
        mCount++;
    }
    quint32 count(){
        return mCount;
    }

    _ListNode<T> *pFirst;
    _ListNode<T> *pEnd;
    quint32 mCount;

};
#endif

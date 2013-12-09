#ifndef SVT_AVL_TREE
#define SVT_AVL_TREE

//============================================================================
// Name        : bitee.cpp
// Author      : taoyidong
// Version     :
// Copyright   : Your copyright notice
// Description : SVT_AvlTree in C++, Ansi-style
//============================================================================

#include <iostream>

#ifdef UNIT_TEST
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include<stdlib.h>
#define ASSERT(x) do{assert(x);}while(0)
#define UINIT_TEST_CHECK_TREE() do {checkvalid();}while(0)
#define UINIT_TEST_STACKSIZE 1024
#else
#define UINIT_TEST_CHECK_TREE()
#endif



template <typename T,typename CMP>
class SVT_AvlTree;

const int BT_LEFTTALLER=-1;
const int BT_BALANCE=0;
const int BT_RIGHTTALLER=1;

template <typename T,typename CMP>

#define MAX(x,y) (((x)>(y))?(x):(y))
class _TreeNode
{
public:
    _TreeNode(const T &_data):data(_data),pLChild(NULL),pRChild(NULL),pParent(NULL),pLSibling(NULL),pRSibling(NULL)
      ,depth(1){};
    virtual ~_TreeNode(){};
    T data;
    _TreeNode *pLChild;
    _TreeNode *pRChild;
    _TreeNode *pParent;
    _TreeNode *pLSibling;
    _TreeNode *pRSibling;
    int depth;
    inline int balance()
    {
        return (pRChild?(pRChild->depth):0)-(pLChild?(pLChild->depth):0);
    }
    inline int recount_depth()
    {
        //        int tempdepth=MAX((pLChild?(pLChild->depth):0),(pRChild?(pRChild->depth):0))+1;
        //        bool ret=(tempdepth==depth);
        depth=MAX((pLChild?(pLChild->depth):0),(pRChild?(pRChild->depth):0))+1;;
        return depth;
    }

    friend class SVT_AvlTree<T,CMP>;
    friend class SVT_NodePool;
};


template <typename T,typename CMP>
class SVT_AvlTree
{


public:
    SVT_AvlTree():pRoot(NULL),pFirst(NULL),pLast(NULL),mCount(0){};


    virtual ~SVT_AvlTree()
    {
        while(pFirst)
        {
            pLast=pFirst;
            pFirst=pFirst->pRSibling;
            delete pLast;

        }
    }

    const int &count(){
        return mCount;
    }

    bool insert(const T &data)
    {
        if(!pRoot)
        {
            pRoot=new _TreeNode<T,CMP>(data);
            pFirst=pLast=pRoot;
            mCount++;
            return true;
        }
        _TreeNode<T,CMP> *pLoopFront=pRoot;
        _TreeNode<T,CMP> *pLoopBack=NULL;
        int cp;
        while(pLoopFront)
        {
            pLoopBack=pLoopFront;
            cp=cmp(pLoopFront->data,data);
            if(cp<0)
                pLoopFront=pLoopFront->pRChild;
            else if(cp > 0)
                pLoopFront=pLoopFront->pLChild;
            else //cp ==0
                return false;
        }
        _TreeNode<T,CMP> *pInsertNode=new _TreeNode<T,CMP>(data);
        if(cp <0)//add to right
        {
            pLoopBack->pRChild=pInsertNode;
            pInsertNode->pParent=pLoopBack;

            if(pLoopBack->pRSibling)
                pLoopBack->pRSibling->pLSibling=pInsertNode;
            pInsertNode->pRSibling=pLoopBack->pRSibling;

            pInsertNode->pLSibling=pLoopBack;
            pLoopBack->pRSibling=pInsertNode;

            if(pLoopBack == pLast)
                pLast=pInsertNode;

        }
        else //add to left
        {
            pLoopBack->pLChild=pInsertNode;
            pInsertNode->pParent=pLoopBack;

            if(pLoopBack->pLSibling)
                pLoopBack->pLSibling->pRSibling=pInsertNode;
            pInsertNode->pLSibling=pLoopBack->pLSibling;

            pInsertNode->pRSibling=pLoopBack;
            pLoopBack->pLSibling=pInsertNode;

            if(pFirst == pLoopBack)
                pFirst=pInsertNode;

        }

        mCount++;
        _adaptBalance(pInsertNode->pParent);
        UINIT_TEST_CHECK_TREE();
        return true;

    };
    bool remove(_TreeNode<T,CMP> *pNode)
    {
        if(!pNode)
            return false;


        //        _TreeNode<T,CMP> *pNodeBalance=pNode->pParent;
        //        bool balanceLeft;

        //   modify   link
        if   (pNode   ==   pFirst)   {
            pFirst   =   pNode->pRSibling;
            if   (pFirst)   {
                pFirst->pLSibling   =   NULL;
            }
            else   {
                pFirst   =   pLast=   NULL;
            }
        }
        else   if   (pNode   ==   pLast)   {
            pLast=   pNode->pLSibling;
            pLast->pRSibling   =   NULL;
        }
        else   {
            pNode->pLSibling->pRSibling=pNode->pRSibling;
            pNode->pRSibling->pLSibling=pNode->pLSibling;
        }

        //   when   node   is   a   leaf
        if   (!pNode->pRChild   &&   !pNode->pLChild)   {
            if   (pNode->pParent)   {
                if   (pNode->pParent->pRChild==pNode)   {
                    pNode->pParent->pRChild=NULL;
                    //   pNode->pParent->balance--;
                    _adaptBalance(pNode->pParent);
                }
                else   {
                    pNode->pParent->pLChild =NULL;
                    //   pNode->pParent->balance++;
                    _adaptBalance(pNode->pParent);
                }
            }
            else   {
                pRoot   =   NULL;
            }
        }
        //   only   have   one   child
        else   if   ((pNode->pLChild && !pNode->pRChild)||(!pNode->pLChild&& pNode->pRChild)){
            if   (pNode->pParent)   {
                _TreeNode<T,CMP> *pNodeUsed=NULL;
                pNodeUsed=pNode->pLChild?pNode->pLChild:pNode->pRChild;
                pNodeUsed->pParent=pNode->pParent;

                if   (pNode->pParent->pLChild == pNode)   {   //   node   is   left   child
                    pNode->pParent->pLChild   =   pNodeUsed;
                    //    pNode->pParent->balance--;
                    _adaptBalance(pNode->pParent);
                }
                else   {   //   node   is   right   child
                    pNode->pParent->pRChild=pNodeUsed;
                    //   pNode->pParent->balance++;
                    _adaptBalance(pNode->pParent);
                }
            }
            else   {
                pRoot=pNode->pLChild?pNode->pLChild:pNode->pRChild;
                pRoot->pParent=NULL;
            }
        }
        //   have   two   children
        else   {

            //   not   thinking   balance   value
            //   prev_node   in   left   child   tree

            _TreeNode<T,CMP> *pNodePrev=pNode->pLSibling;
            pNodePrev->depth=pNode->depth;
            //            pNodeBalance=pNodePrev;

            //            if(pNodeParent)
            //                pNodeParent->balance++;

            if (pNodePrev->pParent == pNode)
            {
                pNodePrev->pRChild=pNode->pRChild;
                pNode->pRChild->pParent=pNodePrev;
                if(!pNode->pParent)
                {
                    pRoot=pNodePrev;
                    pRoot->pParent=NULL;
                    _adaptBalance(pRoot);
                }
                else{
                    (pNode->pParent->pLChild == pNode) ?
                                (pNode->pParent->pLChild=pNodePrev):(pNode->pParent->pRChild=pNodePrev);
                    pNodePrev->pParent=pNode->pParent;
                    //111111111111.pNodePrev 为 pNode的左节点，删除pNode后，pNodePrev替换了pNode的位置
                    _adaptBalance(pNodePrev);
                }

            }
            else {

                pNodePrev->pParent->pRChild=pNodePrev->pLChild;
                _TreeNode<T,CMP> *pNodeBalance=pNodePrev->pParent;

#if 0
                pNodePrev->pParent->pRChild=pNode;
                pNode->pParent

        #endif
                        if(pNodePrev->pLChild){
                    pNodePrev->pLChild->pParent=pNodePrev->pParent;
                }
                pNodePrev->pLChild=pNode->pLChild;
                pNodePrev->pLChild->pParent = pNodePrev;


                pNodePrev->pRChild=pNode->pRChild;
                pNodePrev->pRChild->pParent= pNodePrev;



                if   (pNode->pParent   ==   NULL)   {
                    pRoot   =   pNodePrev;
                    pNodePrev->pParent=NULL;
                }
                else   {
                    (pNode->pParent->pLChild ==pNode) ?
                                (pNode->pParent->pLChild=pNodePrev):(pNode->pParent->pRChild=pNodePrev);
                    pNodePrev->pParent=pNode->pParent;
                }
                _adaptBalance(pNodeBalance);
            }



        }

        delete   pNode;
        mCount--;
        UINIT_TEST_CHECK_TREE();
        return   true;
    }

    bool remove(const T &data)
    {
        _TreeNode<T,CMP> *pNode = find(data);

        return remove(pNode);
    };

    _TreeNode<T,CMP> * find(const T &data )
    {
        _TreeNode<T,CMP> *pLoop=pRoot;
        while(pLoop)
        {
            int cp=cmp(pLoop->data,data);
            if(0==cp)
                return pLoop;
            pLoop=(cp >0)?pLoop->pLChild:pLoop->pRChild;
        }
        return pLoop;
    };

#ifdef UNIT_TEST
    void dump() const
    {
        _TreeNode<T,CMP> * p=pFirst;
        while (p)
        {
            //   printf("%d ",p->data);
            p=p->pRSibling;
        }
        printf("\n");

    }

    bool checkvalid()
    {
        if(pRoot==NULL)
        {
            return true;
        }

        ASSERT(pFirst && pLast && pRoot);
        ASSERT(!pLast->pRSibling);
        ASSERT(!pFirst->pLSibling);

        dump();

        _TreeNode<T,CMP> *pCurNodeStack=pRoot;
        _TreeNode<T,CMP> *pCurNodeList=pFirst;
        _TreeNode<T,CMP> *nodeStack[UINIT_TEST_STACKSIZE]={0};
        int stackidx=0;
        int counter=0;

        while(pCurNodeStack)
        {
            ASSERT(stackidx < UINIT_TEST_STACKSIZE-1);
            nodeStack[stackidx++]=pCurNodeStack;
            pCurNodeStack=pCurNodeStack->pLChild;
        }

        while(stackidx >0 )
        {
            pCurNodeStack=nodeStack[--stackidx];


            ASSERT(!pCurNodeStack->pLChild || pCurNodeStack->pLChild->pParent==pCurNodeStack);
            ASSERT(!pCurNodeStack->pRChild || pCurNodeStack->pRChild->pParent==pCurNodeStack);
            ASSERT(!pCurNodeStack->pParent || pCurNodeStack->pParent->pLChild==pCurNodeStack || pCurNodeStack->pParent->pRChild==pCurNodeStack);
            ASSERT(!pCurNodeStack->pLSibling || pCurNodeStack->pLSibling->pRSibling==pCurNodeStack);
            ASSERT(!pCurNodeStack->pRSibling || pCurNodeStack->pRSibling->pLSibling==pCurNodeStack);


            ASSERT(pCurNodeStack==pCurNodeList);
            pCurNodeList=pCurNodeList->pRSibling;
            ASSERT(!pCurNodeList || cmp(pCurNodeList->pLSibling->data,pCurNodeList->data) <0);

            counter++;

            pCurNodeStack=pCurNodeStack->pRChild;
            while(pCurNodeStack)
            {
                ASSERT(stackidx < UINIT_TEST_STACKSIZE-1);
                nodeStack[stackidx++]=pCurNodeStack;
                pCurNodeStack=pCurNodeStack->pLChild;
            }
        }
        // ASSERT(pCurNodeStack==pCurNodeList);
        ASSERT(pCurNodeList==NULL);
        ASSERT(counter==mCount);

        checkbalance(pRoot);
        return true;

    }
    int checkbalance(_TreeNode<T,CMP> *pNode) const
    {
        int lb= pNode->pLChild?checkbalance(pNode->pLChild):0;
        int rb= pNode->pRChild?checkbalance(pNode->pRChild):0;
        int value=MAX(lb,rb)+1;
        ASSERT(value == pNode->depth);
        ASSERT(lb-rb>-2 && lb-rb<2);
        return value;
    }

#endif
public:
    void _adaptBalance(_TreeNode<T,CMP> *&pNode)
    {
        if(!pNode)
        {
            return;
        }
        _TreeNode<T,CMP> *pNodeChild=NULL;
        _TreeNode<T,CMP> *pNodeChildChild=NULL;
        _TreeNode<T,CMP> *pNodeLoop=pNode;
        _TreeNode<T,CMP> *pNodeLoopNext=NULL;
        bool leftlogic;
        bool loop=true;

        //     pNode->balance+=left?(-1):1;
        while(pNodeLoop && loop)
        {
            int saved_depth=pNodeLoop->depth;
            int temp_balance=pNodeLoop->balance();
            pNodeLoopNext=pNodeLoop->pParent;

            if(temp_balance<-1)
            {
                Q_ASSERT(pNodeLoop->pLChild);
                pNodeChild=pNodeLoop->pLChild;
                switch(pNodeChild->balance())
                {
                case BT_LEFTTALLER:
                    _turnright(pNodeLoop);
                    pNodeLoop->recount_depth();
                    if(pNodeChild->recount_depth()==saved_depth)
                    {
                        loop=false;
                    }
                    pNodeLoopNext=pNodeChild->pParent;
                    break;
                case BT_BALANCE:
                    _turnright(pNodeLoop);
                    pNodeLoop->recount_depth();
                    if(pNodeChild->recount_depth()==saved_depth)
                    {
                        loop=false;
                    }
                    pNodeLoopNext=pNodeChild->pParent;
                    break;
                case BT_RIGHTTALLER:
                    pNodeChildChild=pNodeChild->pRChild;
                    _turnleft(pNodeChild);
                    _turnright(pNodeLoop);
                    pNodeLoop->recount_depth();
                    pNodeChild->recount_depth();
                    if(pNodeChildChild->recount_depth()==saved_depth)
                    {
                        loop=false;
                    }
                    pNodeLoopNext=pNodeChildChild->pParent;
                    break;
                default:
                    Q_ASSERT(0);
                }

            }
            else if(temp_balance>1)
            {
                Q_ASSERT(pNodeLoop->pRChild);
                pNodeChild=pNodeLoop->pRChild;
                switch(pNodeChild->balance())
                {
                case BT_LEFTTALLER:
                    pNodeChildChild=pNodeChild->pLChild;
                    _turnright(pNodeChild);
                    _turnleft(pNodeLoop);
                    pNodeLoop->recount_depth();
                    pNodeChild->recount_depth();
                    if(pNodeChildChild->recount_depth()==saved_depth)
                    {
                        loop=false;
                    }
                    pNodeLoopNext=pNodeChildChild->pParent;
                    break;
                case BT_BALANCE:
                    _turnleft(pNodeLoop);
                    pNodeLoop->recount_depth();
                    if(pNodeChild->recount_depth()==saved_depth)
                    {
                        loop=false;
                    }
                    pNodeLoopNext=pNodeChild->pParent;
                    break;
                case BT_RIGHTTALLER:
                    _turnleft(pNodeLoop);
                    pNodeLoop->recount_depth();
                    if(pNodeChild->recount_depth()==saved_depth)
                    {
                        loop=false;
                    }

                    pNodeLoopNext=pNodeChild->pParent;
                    break;
                default:
                    Q_ASSERT(0);
                }
            }
            else
            {
                if(pNodeLoop->recount_depth()==saved_depth)
                {
                    loop=false;
                }
            }

            pNodeLoop=pNodeLoopNext;
        }//while


    }



    //    bool _TreeNode<T,CMP> *_rightbalance(_TreeNode<T,CMP> *pNode)

    //    {
    //        //if depth changed return true
    //        ASSERT(pNode);
    //        _TreeNode<T,CMP> * const pNodeChild=pNode->pRChild;
    //        _TreeNode<T,CMP> * pNodeChildChild;
    //        ASSERT(pNodeChild);

    //        int saved_depth=pNode->depth;
    //        int tempbalance=pNodeChild->balance();

    //        switch(pNodeChild->balance())
    //        {
    //        case BT_BALANCE:
    //            _turnleft(pNode);
    //            //22222 这时balance值可能变化，也有可能不变，
    //            //放到左右turn里面去更合理一些，貌似
    //            pNode->recount_depth();
    //            pNodeChild->recount_depth();
    //            return pNodeChild;
    //            break;
    //        case BT_LEFTTALLER:
    //            pNodeChildChild=pNodeChild->pLChild;
    //            _turnright(pNodeChild);
    //            _turnleft(pNode);
    //            pNodeChildChild->recount_depth();
    //            pNode->recount_depth();
    //            pNodeChild->recount_depth();
    //            return pNodeChildChild;
    //            break;
    //        case BT_RIGHTTALLER:
    //            _turnleft(pNode);
    //            pNode->recount_depth();
    //            pNodeChild->recount_depth();
    //            return pNodeChild;
    //            break;
    //        }
    //        return NULL;


    //    }



    //    bool _TreeNode<T,CMP> *_leftbalance(_TreeNode<T,CMP> *pNode)
    //    {
    //        ASSERT(pNode);
    //        _TreeNode<T,CMP> *const  pNodeChild =pNode->pLChild;
    //        _TreeNode<T,CMP> * pNodeChildChild;
    //        ASSERT(pNodeChild);
    //        switch(pNodeChild->balance())
    //        {
    //        case BT_BALANCE:
    //            _turnright(pNode);
    //            pNode->recount_depth();
    //            pNodeChild->recount_depth();
    //            return pNodeChild;
    //            break;
    //        case BT_LEFTTALLER:
    //            _turnright(pNode);
    //            pNode->recount_depth();
    //            pNodeChild->recount_depth();
    //            return pNodeChild;
    //            break;
    //        case BT_RIGHTTALLER:
    //            pNodeChildChild=pNodeChild->pRChild;
    //            _turnleft(pNodeChild);
    //            _turnright(pNode);
    //            pNodeChildChild->recount_depth();
    //            pNode->recount_depth();
    //            pNodeChild->recount_depth();

    //            return pNodeChildChild;
    //            break;
    //        }
    //        return NULL;
    //    }

    void _replace(_TreeNode<T,CMP> *pNode1,_TreeNode<T,CMP> *pNode2)
    {
        if(!pNode1->pParent)
        {
            pRoot=pNode2;
            pNode2->pParent=NULL;
        }
        else
        {
            if(pNode1->pParent->pLChild==pNode1)
            {
                pNode1->pParent->pLChild=pNode2;
                pNode2->pParent=pNode1->pParent;
            }
            else if(pNode1->pParent->pRChild==pNode1)
            {
                pNode1->pParent->pRChild=pNode2;
                pNode2->pParent=pNode1->pParent;
            }
            else
            {
                Q_ASSERT(0);
            }
        }
    };

    void _turnleft(_TreeNode<T,CMP> *pNode)
    {

        _TreeNode<T,CMP> *pNodeChild=pNode->pRChild;
        pNode->pRChild=pNodeChild->pLChild;
        if(pNodeChild->pLChild)
        {
            pNodeChild->pLChild->pParent=pNode;
        }
        _replace(pNode,pNodeChild);
        pNodeChild->pLChild=pNode;
        pNode->pParent=pNodeChild;

    }
    void _turnright(_TreeNode<T,CMP> *pNode)
    {
        _TreeNode<T,CMP> *pNodeChild=pNode->pLChild;
        pNode->pLChild=pNodeChild->pRChild;
        if(pNodeChild->pRChild)
        {
            pNodeChild->pRChild->pParent=pNode;
        }
        _replace(pNode,pNodeChild);
        pNodeChild->pRChild=pNode;
        pNode->pParent=pNodeChild;
    }

    _TreeNode<T,CMP> *pRoot;
    _TreeNode<T,CMP> *pFirst;
    _TreeNode<T,CMP> *pLast;
    CMP cmp;
    int mCount;
};


#ifdef UNIT_TEST
class Test
{
public:
    Test(){};
    ~Test(){};
    int operator()(int a,int b){/*printf("compare %d %d\n",a,b);*/return a-b;};
};

int main(int argc, void **argv) {
    printf("test");

    time_t ti;
    time(&ti);
    srand((unsigned int)ti);

    SVT_AvlTree<int,Test> btree;

#if 0
    struct _cmds{char type;int data;}cmds[]=
    {
        //	{'a',23538},{'a',30334},{'a',2261},{'a',25865},{'a',11054},{'a',19417},{'a',30785},{'d',19417},{'d',23538},
        //	{'a',4265},{'a',5389},{'a',26220},{'a',21084},{'a',15184},{'d',21084},
        //{'a',6871},{'a',30169},{'a',27749},{'a',21561},{'a',21446},{'a',11551},{'d',27749}
        //	{'a',30619},{'a',20523},{'a',31039},{'a',20777},{'a',19888},{'a',357},{'a',30944},{'d',30619},{'d',20777},{'a',4006},{'d',357},{'d',19888}
        //	{'a',23955},{'a',27019},{'a',32123},{'a',31953},{'a',25935},{'a',22598},{'d',31953},{'d',31953},{'d',31953},{'a',11939},{'d',32123},
        //	{'a',16253},{'a',10117},{'a',4058},{'a',14981},{'a',178},{'d',178},{'d',14981},{'a',16789},{'d',16253},{'a',20986},{'a',19682},{'d',10117},
    {'a',3},{'a',1},{'a',5},{'a',4},{'a',6},{'a',7},{'d',18323},{'d',32162},{'d',32162},{'a',30103},{'d',18426},
};
    for (int i=0;i<(sizeof(cmds)/sizeof(cmds[0]));i++)
    {
        if(cmds[i].type=='a')
            btree.insert(cmds[i].data);
        else if(cmds[i].type=='d')
            btree.remove(cmds[i].data);
    }
#else
    const int cLoopCount=9000;
    const int cDataCount=1200;
    // const int cDeleteRate=9;
    //        int *testdata=new int[cDataCount];
    int testloop=cLoopCount;
    struct _cmds{char type;int data;}cmds[cDataCount];

teststart:

    for(int i=0;i<cDataCount;i++)
    {
        cmds[i].type=(i<5)?'a':((rand()&0x1)?'a':'d');
        if(cmds[i].type=='a')
        {
            cmds[i].data=rand();
        }
        else
        {
            int itemp=rand()%i;
            cmds[i].data=cmds[itemp].data;
        }
    }
    for(int i=0;i<cDataCount;i++)
    {
        for(int j=0;j<=i;j++)
        {
            printf("{'%c',%d},",cmds[j].type,cmds[j].data);
        }
        printf("\n");
        if(cmds[i].type=='a')
            btree.insert(cmds[i].data);
        else if(cmds[i].type=='d')
            btree.remove(cmds[i].data);

    }
    for(int i=0;i<cDataCount;i++)
    {
        btree.remove(cmds[i].data);
    }
    ASSERT(0==btree.count());

    if(--testloop >0)
        goto teststart;
    //    delete testdata;
#endif
    return 0;
}

#endif //UNIT_TEST






#endif

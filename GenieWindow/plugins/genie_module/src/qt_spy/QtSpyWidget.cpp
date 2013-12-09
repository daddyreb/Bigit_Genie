#include "QtSpyWidget.h"
#include <QApplication>
#include <QtGui>

class _SpytHelper
{
public:
    _SpytHelper(QWidget *w)
    {
        mLabel=qobject_cast<QLabel *>(w);
        mButton=qobject_cast<QPushButton *>(w);
        mRdtButton=qobject_cast<QRadioButton *>(w);
        mChkBox=qobject_cast<QCheckBox *>(w);
        mTabBar=qobject_cast<QTabBar *>(w);
        mStack=qobject_cast<QStackedWidget *>(w);
        mFrame=qobject_cast<QFrame *>(w);
    }
    bool isOk()
    {
        return mLabel||mButton||mRdtButton||mChkBox||mTabBar;
    }
    void setText(const QString &text)
    {
        if(mLabel){mLabel->setText(text);}
        if(mButton){mButton->setText(text);}
        if(mRdtButton){mRdtButton->setText(text);}
        if(mChkBox){mChkBox->setText(text);}
        if(mTabBar){mTabBar->setTabText(mTabBar->currentIndex(),text);}
    }
    QString text() const
    {
        if(mLabel){return mLabel->text();}
        if(mButton){return mButton->text();}
        if(mRdtButton){return mRdtButton->text();}
        if(mChkBox){return mChkBox->text();}
        if(mTabBar){return mTabBar->tabText(mTabBar->currentIndex());}
        return QString();
    }
    Qt::GlobalColor getColor()
    {
        if(mLabel){return Qt::yellow;}
        if(mButton){return Qt::cyan;}
        if(mRdtButton){return Qt::magenta;}
        if(mChkBox){return Qt::white;}
        if(mTabBar){return Qt::black;}
        if(mStack){return Qt::blue;}
        if(mFrame){return Qt::green;}
        return Qt::red;

    }

protected:
    QLabel *mLabel;
    QPushButton *mButton;
    QRadioButton *mRdtButton;
    QCheckBox *mChkBox;
    QTabBar *mTabBar;
    QStackedWidget *mStack;
    QFrame *mFrame;
};

#define CUR_W() (mWStack.count()?mWStack.last():NULL)

QtSpyWidget::QtSpyWidget(QWidget *parent) :
        QWidget(parent)
        //        ,mCurW(NULL)
        ,mCount(0)
        ,mFlags(0)
        ,mEditor(NULL)
        ,mCanvasFocus(600,400,QImage::Format_RGB32)
{
}

QtSpyWidget::~QtSpyWidget()
{
    QMap<QWidget *,QPixmap *>::Iterator i=mPixmapCache.begin();
    while(i != mPixmapCache.end())
    {
        delete i.value();
        i++;
    }
    mPixmapCache.clear();
}



void QtSpyWidget::setEditType(int f)
{
    Q_ASSERT(0<=f && f<3);
    const quint32 cParas[]={F_EDIT_TEXT,F_EDIT_STYLESHEET,F_EDIT_STACK};
    const quint32 &cur=cParas[f];
    quint32 allF=0;
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        allF |= cParas[i];
    }
    mFlags&= ~allF;
    mFlags|=cur;

}

void QtSpyWidget::whenInstall(bool install)
{
    if(!install)
    {
        mWStack.clear();
        disconnect(qApp,SIGNAL(focusChanged(QWidget*,QWidget*)),this,SLOT(slot_FocusChanged(QWidget*,QWidget*)));

        QMap<QWidget *,QPixmap *>::Iterator i=mPixmapCache.begin();
        while(i != mPixmapCache.end())
        {
            delete i.value();
            i++;
        }
        mPixmapCache.clear();
    }
    else
    {
        connect(qApp,SIGNAL(focusChanged(QWidget*,QWidget*)),this,SLOT(slot_FocusChanged(QWidget*,QWidget*)));

       QWidget *focusW=qApp->focusWidget();
       QPainter p(&mCanvasFocus);
       p.fillRect(p.viewport(),Qt::SolidPattern);
       mFlags |= F_INPAINT;
       focusW->render(&mCanvasFocus);
       mFlags &= ~F_INPAINT;
    }
}

void QtSpyWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter p(this);
    //    p.drawText(40,40,mCurW?mCurW->objectName():"null");
    QWidget *curW=CUR_W();
    QString todraw=QString("objectName: %1\tclassName: %2\n")
            .arg(curW?curW->objectName():"null"
                      ,curW?QString(curW->metaObject()->className()):"null");
    if(curW)
    {
        QFontInfo finfo=curW->fontInfo();
        QFont font=curW->font();
        QString stemp;
        //        stemp+=QString("family: %1 %2\n").arg(font.family(),finfo.family());
        //        stemp+=QString("default family: %1\n").arg(font.defaultFamily());
        //        stemp+=QString("exactmatch: %1 %2\n").arg(font.exactMatch()?"true":"false"
        //                                                   ,finfo.exactMatch()?"true":"false");
        //        stemp+=QString("italic: %1 %2\n").arg(font.italic()?"true":"false",finfo.italic()?"true":"false");
        //        stemp+=QString("kerning: %1\n").arg(font.kerning()?"true":"false");
        //        stemp+=QString("fixed pitch: %1 %2\n").arg(font.fixedPitch()?"true":"false",finfo.fixedPitch()?"true":"false");
        stemp+=QString("key: %1 \n").arg(font.key())     ;
        todraw+=stemp;
        todraw+=QString("size %1 %2").arg(curW->width()).arg(curW->height());


    }
    const int cHeadLine=50;
    p.save();
    QRect rct=p.viewport();
    QRect rct_cur=rct;
    rct_cur.setHeight(cHeadLine);

    p.drawText(rct_cur,Qt::AlignHCenter|Qt::AlignVCenter
               ,todraw);
    rct_cur.adjust(0,cHeadLine,0,cHeadLine);
//    rct_cur.adjust(0,cHeadLine,-1,-1);
    p.setPen(Qt::blue);
    p.drawText(rct_cur,Qt::AlignHCenter|Qt::AlignVCenter|Qt::TextWordWrap
               ,curW?curW->styleSheet():"");


    rct_cur.adjust(0,cHeadLine,0,cHeadLine);
//    p.setClipRect(rct_cur);
    p.setBrush(Qt::black);
    p.drawImage(rct_cur,mCanvasFocus,QRect(0,0,rct_cur.width(),rct_cur.height()));

    p.setPen(Qt::red);

    p.restore();
//    p.setClipRect(rct);
    p.save();
    //paint stack
    const int cH=20;
    const int cW=9;
    const int cSpace=15;
    int c=mWStack.count();
    p.setBrush(Qt::gray);
    p.setPen(Qt::gray);
    p.drawRect(0,rct.height()-cH,rct.width(),cH);
    for(int i=0;i<c;i++)
    {
        Qt::GlobalColor color=_SpytHelper(mWStack[i]).getColor();
        p.setPen(color);
        p.setBrush(color);
        p.drawRect(cSpace*i,rct.height()-cH,cW,cH);
    }
    //paint stack end
    p.restore();

    p.setPen(Qt::red);
    p.drawRect(0,0,this->width()-1,this->height()-1);
    qDebug()<<"paint";
}

bool QtSpyWidget::eventFilter(QObject *o, QEvent *env)
{
    QWidget *curW=CUR_W();
    QEvent::Type type=env->type();
    if(type==QEvent::MouseButtonRelease
       ||type==QEvent::MouseButtonPress)
    {
        return true;
    }
    if(type!=QEvent::Paint
       &&type!=QEvent::Leave
       &&type!=QEvent::Enter
       &&type!=QEvent::MouseButtonDblClick
       &&type!=QEvent::FocusOut
       &&type!=QEvent::KeyRelease
       )
    {
        return false;
    }
    QWidget *w=qobject_cast<QWidget *>(o);
    Q_ASSERT(w);
    if(type==QEvent::Paint)
    {
        if((qApp->focusWidget()==w)  && !(mFlags&F_INPAINT))
        {
            QPainter p(w);
//                p.fillRect(rct_cur,);

            p.drawImage(0,0,mCanvasFocus);


        }
        if(
//#ifdef Q_OS_WIN32
                mWStack.contains(w)
//#else
//                curW==w
//#endif
                && !(mFlags&F_INPAINT) && !mEditor)
        {
            //            QPaintEvent *pe=static_cast<QPaintEvent *>(env);
            QPainter p(w);

            //        p.setPen(QColor(Qt::red));
            //        p.drawRect(0,0,width()-1,height()-1);
            //            const Qt::GlobalColor cParas[]={Qt::red,Qt::blue,Qt::green,Qt::yellow};

            p.setPen(_SpytHelper(w).getColor());
            QRect rct=p.viewport();
//#ifndef Q_OS_MACX
//            mFlags|=F_INPAINT;
//            QPixmap pixmap(w->size());
//            w->render(&pixmap);
//            p.drawPixmap(0,0,pixmap);
//            mFlags&=~F_INPAINT;
//#endif
            Q_ASSERT(mPixmapCache.contains(w));
            p.drawPixmap(0,0,*mPixmapCache[w]);



            p.drawRect(0,0,rct.width()-1,rct.height()-1);

            return true;
        }
        return false;
    }
    else if(type==QEvent::Enter )
    {
        if(!mEditor)
        {
            qDebug()<<"type==QEvent::Enter";
            mWStack.append(w);
            //Q_ASSERT(!mPixmapCache.contains(w));
            updatePixmapCache(w);
            if(!mPixmapCache.contains(w))
            {
                mPixmapCache.insert(w,new QPixmap(w->size()));
                mFlags |=F_INPAINT;
                w->render(mPixmapCache[w]);
                mFlags &=~F_INPAINT;
            }
            w->repaint();
            mCount++;
            this->repaint();
        }

    }
    else if(type==QEvent::Leave  )
    {
        if(!mEditor)
        {
            qDebug()<<"type==QEvent::Leave";
            if(mWStack.count())
            {
                QWidget *w=mWStack.back();
                mWStack.pop_back();
                if(mWStack.count(w)<=0)
                {
                    delete mPixmapCache[w];
                    mPixmapCache.remove(w);
                }
                mCount--;
            }
          //  Q_ASSERT(mPixmapCache.count()==mWStack.count());
            w->repaint();
            this->repaint();
        }
    }
    else if(type==QEvent::FocusOut)
    {
        if(mEditor==w && curW)
        {
            if(mFlags&F_EDIT_STYLESHEET)
            {
                curW->setStyleSheet(mEditor->toPlainText());
            }
            else
            {
                _SpytHelper(curW).setText(mEditor->toPlainText());
            }
            mEditor->deleteLater();;
            mEditor=NULL;
            updatePixmapCache(curW);
            curW->update();
        }
    }
    else if(type==QEvent::MouseButtonDblClick)
    {

        if(w==curW )
        {
            if(mEditor)
            {
                mEditor->deleteLater();
                mEditor=NULL;
            }
            if(mFlags&F_EDIT_STYLESHEET)
            {
                QTextEdit *edt=new QTextEdit(0);
                edt->setAcceptRichText(false);
                edt->installEventFilter(this);
                mEditor=edt;
                edt->setPlainText(w->styleSheet());
            }
            else if(mFlags &F_EDIT_TEXT)
            {
                _SpytHelper helper(w);
                if(helper.isOk())
                {
                    QTextEdit *edt=new QTextEdit(0);
                    edt->setAcceptRichText(false);
                    edt->installEventFilter(this);
                    mEditor=edt;
                    edt->setPlainText(helper.text());
                }
            }
            else if(mFlags &F_EDIT_STACK)
            {
                int wCount=mWStack.count();
                while(--wCount>=0)
                {
                    QStackedWidget *stack=qobject_cast<QStackedWidget *>(mWStack[wCount]);
                    if(stack)
                    {
                        int c=stack->count();
                        int i=stack->currentIndex();
                        stack->setCurrentIndex((i+1)%c);
                        stack->update();
                        break;
                    }
                }
            }

            if(mEditor)
            {
                mEditor->show();
                mEditor->setFocus();
            }

        }
    }
    //    else if(type==QEvent::KeyRelease)
    //    {
    //        QKeyEvent *kenv=static_cast<QKeyEvent *>(env);
    //        int kcode=kenv->key();
    //        if((kcode==Qt::Key_Left || kcode==Qt::Key_Right)&&(w==curW))
    //        {
    //            QStackedWidget *stack=qobject_cast<QStackedWidget *>(curW);
    //            if(stack)
    //            {
    //                int c=stack->count();
    //                int i=stack->currentIndex();
    //                i=(i+((kcode==Qt::Key_Left)?-1:1)+c)%c;
    //                stack->setCurrentIndex(i);
    //				qDebug()<<"stack changed to"<<i;
    //            }
    //        }
    //    }


    return false;
}

void QtSpyWidget::slot_FocusChanged(QWidget *old, QWidget *now)
{
	QPainter p(&mCanvasFocus);
    p.fillRect(p.viewport(),Qt::SolidPattern);
	p.end();
    if(now)
    {
        mFlags |=F_INPAINT;
        now->render(&mCanvasFocus);
        mFlags &=~F_INPAINT;
        QString text=now->objectName();
        QString text2=now->metaObject()->className();
        QWidget *parent=qobject_cast<QWidget*>(now->parent());
        if(parent)
        {
            text=parent->objectName()+" -> "+text;
            text2=QString(parent->metaObject()->className())+" -> "+text2;
			parent=qobject_cast<QWidget*>(parent->parent());
			if(parent)
			{
                text=parent->objectName()+" -> "+text;
                text2=QString(parent->metaObject()->className())+" -> "+text2;
			}
        }
		
		QPainter pp(&mCanvasFocus);
		pp.setPen(Qt::red);
        pp.drawText(0,30,text);
        pp.setPen(Qt::blue);
        pp.drawText(0,45,text2);

        update();
    }
}

void QtSpyWidget::updatePixmapCache(QWidget *w)
{
    if(!mPixmapCache.contains(w))
    {
        mPixmapCache.insert(w,new QPixmap(w->size()));
    }
    mFlags |=F_INPAINT;
    w->render(mPixmapCache[w]);
    mFlags &=~F_INPAINT;


}

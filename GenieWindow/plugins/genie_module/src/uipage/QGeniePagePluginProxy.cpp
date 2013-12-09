//#ifdef USE_PLUGIN_PROXY
#include "stable.h"

QGeniePagePluginProxy::QGeniePagePluginProxy(QWidget *parent)
    :QWidget(parent)
    ,mPluginInf(NULL)
  //  ,mRelatedBigButton(NULL)
  //  ,mIdx(-1)
    ,mState(PLST_GOOD)
{
    mUi.setupUi(this);
 //   setProperty(STYLE_PROPERTY_HINT_SUB,QVariant((int)SHSE_FRAME_PLUGIN_PROXY));
    resize(620,396); //this is the size of bg image
}

//void QGeniePagePluginProxy::setIdx(int i)
//{
//    ASSERT(mIdx ==-1);
//    mIdx=i;
//}

void QGeniePagePluginProxy::setPluginWidget(QWidget *w,IPluginInf *pi)
{
    mUi.verticalLayout->addWidget(w);
    mPluginInf=pi;
}


void QGeniePagePluginProxy::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    if(mState==PLST_WAITING)
    {
        QPainter painter(this);
        painter.drawRect(QRect(0,0,40,40));

    }
}

//#endif

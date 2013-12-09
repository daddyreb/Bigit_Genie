#include "stable.h"
//#ifdef PLATFORM_MACOSX
//#define BIG_BUT_CONFIG_FILE "./styles/bigbutton.qss"
//#else
#ifdef MIX_RESOURCE
#define BIG_BUT_CONFIG_FILE ":/styles/bigbutton.qss"
#else
#define BIG_BUT_CONFIG_FILE (GENIE_PATH+"../styles/bigbutton.qss")
#endif
//#endif






//const int cBigButtonText[UTC_NUM]={
//    L_MAIN_BIGBUT_GOOD_LEFT,
//    L_MAIN_BIGBUT_GOOD_RIGHT,
//    L_MAIN_BIGBUT_WARRING_LEFT,
//    L_MAIN_BIGBUT_WARRING_RIGHT,
//    L_MAIN_BIGBUT_ERROR_LEFT,
//    L_MAIN_BIGBUT_ERROR_RIGHT,
//    L_MAIN_BIGBUT_WAITING_LEFT,
//    L_MAIN_BIGBUT_WAITING_RIGHT,
//    L_MAIN_BIGBUT_WAITING_LEFT, //user left
//    L_MAIN_BIGBUT_WAITING_RIGHT, //user right
//    L_MAIN_BIGBUT_TEXT
//};

//void QGenieButtonBig::loadStyleSheet()
//{
//    //    QString typeimg=mType.replace(" ","_");
//    //    QString typestr=(QString(PLUGIN_DIR)+"but_type_%1.png").arg(typeimg);
//    QFile file(BIG_BUT_CONFIG_FILE);
//    file.open(QFile::ReadOnly);
//    QString stylesheet = QLatin1String(file.readAll());
//    //    stylesheet=stylesheet.arg(typestr);
//    setStyleSheet(stylesheet);
//}

void QGenieButtonBig::parseConfigString(const QString &s)
{
    //<showing string>:<priority>:<paraseting1>:<paraseting2>
    QStringList lst=s.split(":");

    for(int i=2;i<lst.size();i++)
    {
        QStringList parastring=lst.at(i).split("=");
        if(parastring.size()==2)
        {
            QString temppara=parastring.at(0).trimmed().toLower();
            QString tempvalue=parastring.at(1).trimmed();
            bool btemp;
            int ivalue=tempvalue.toInt(&btemp);


            if(temppara==BBT_PARA_LAN_IDX)
            {
                if(btemp)
                {
                    mLanIdx=ivalue;
                }
                else{
                    //mUi.label_error_text->setText(tempvalue);
                    //mUi.label_good_text->setText(tempvalue);
                    //mUi.label_waiting_text->setText(tempvalue);
                    //mUi.label_warring_text->setText(tempvalue);
                    //mUi.label_user_text->setText(tempvalue);
                    mUi.label_text->setText(tempvalue);
                    mLanIdx=-1;
                }
            }
        }
    }
    if(lst.size() >=2)
    {
        setType(lst.at(0));
#ifdef ENABLE_MULTI_LEFT_BUTTON
        this->mNotifyPara = lst[0];
#endif
    }
    changeText();
}

QGenieButtonBig::QGenieButtonBig(QWidget *parent)
    :QGenieButtonStackViewController(parent)
    //	,mState(BBS_GOOD)
    ////	,mType("")
    //        ,mUpdateFrequency(10000)
    //        ,mUpdateLastTime(-1)
    //   //     ,mTitleIdx(-1)
    //        ,mUpdatePolicy(SUP_NO_UPDATE)
    ,mLanIdx(-1)

{
    mUi.setupUi(this);
    //
    QGenieBigButtonManager *manager=QGenieBigButtonManager::GetSingletonPtr();
    ASSERT(manager);
    manager->registBigButton(this);
    //    setState(BBS_GOOD);
    //loadStyleSheet();
    mControllerType=CT_BIG_BUTTON;
    //    mUserTextCache.resize(UTC_NUM);
    //    mUserTextFlag.resize(UTC_NUM);
    //    mUserTextCache.fill(-1);
    //    mUserTextFlag.fill(false);
    //    for(int i=0;i<UTC_NUM;i++)
    //    {
    //        mUserTextCache[i]=cBigButtonText[i];
    //    }
    //   setSubState(EBSS_NONE);
    //connect(mUi.sub_but_good,SIGNAL(clicked()),this,SLOT(slot_NotifySubState()));
    // connect(mUi.sub_but_error,SIGNAL(clicked()),this,SLOT(slot_NotifySubState()));
    setBKG(BBT_CSS_GOOD);
}

//bool QGenieButtonBig::canUpdateStateNow(int msec) const
//{
//    return (mUpdatePolicy==SUP_BY_FRAME)&&((msec-mUpdateLastTime) > mUpdateLastTime);
//}


//void QGenieButtonBig::setState(BIG_BUTTON_STATUS st)
//{
////    mState=st;
////    mUi.label_state->setText(cConfigStateText[(int)st]);
////    mUi.frame_bg->setProperty(STYLE_HINT_BIGBUT_STATE_BG,st);
////    mUi.frame_head->setProperty(STYLE_HINT_BIGBUT_STATE_HEAD,st);
////    mUi.label_state->setProperty(STYLE_HINT_BIGBUT_STATE_TEXT,st);
////
////    loadStyleSheet();
//    mUi.stack->setCurrentIndex(st);
////    QGenieBigButtonManager *bigbutmanager=QGenieBigButtonManager::GetSingletonPtr();
////    ASSERT(bigbutmanager);
////    mUpdateLastTime=bigbutmanager->mTime.elapsed();
//}

void QGenieButtonBig::setSubState(ENUM_BIGBT_SUB_STATE st)
{
    //  GENIE_LOG(QString("QGenieButtonBig::setSubState %1").arg(st));
    //if(st==EBSS_NONE)
    //{
    //    //mUi.sub_but_good->hide();
    //    mUi.sub_but_error->hide();
    //}
    //else
    //{
    //    //mUi.sub_but_good->show();
    //    mUi.sub_but_error->show();

    //    const int state_num= mUi.sub_but_good->ui->stackedWidget->count();
    //    const int idx=st-EBSS_BUT;
    //    ASSERT(0<=idx && idx <state_num);
    //    mUi.sub_but_good->ui->stackedWidget->setCurrentIndex(idx);
    //    mUi.sub_but_error->ui->stackedWidget->setCurrentIndex(idx);
    //}
    //if(st==EBSS_REFRESH)
    //{
    //    mUi.sub_but_good->ui->widget_circle->startAnimation();
    //    mUi.sub_but_error->ui->widget_circle->startAnimation();
    //}
    //else{
    //    mUi.sub_but_good->ui->widget_circle->stopAnimation();
    //    mUi.sub_but_error->ui->widget_circle->stopAnimation();
    //}
    ASSERT(0);

}

void QGenieButtonBig::setBackGroundIco(const QString &ico)
{
    const char *cStyleSheet=
            "QFrame#frame_ico"
            "{"
            "background-image:url(%1);background-repeat: none; background-position: top center;"
            "}";
    QString stylesheet=QString(cStyleSheet);
    QString tp_noblank;
#if 0
    tp_noblank=QString("../styles/%1").arg(ico);
#else
    tp_noblank=QString(":/bbticon/%1").arg(ico);
#endif
    if(!QFile::exists(tp_noblank))
    {
        tp_noblank=":/bbticon/bbt_default.png";
    }
    stylesheet=stylesheet.arg(tp_noblank);

    //mUi.frame_type_good->setStyleSheet(stylesheet);
    //mUi.frame_type_warring->setStyleSheet(stylesheet);
    mUi.frame_ico->setStyleSheet(stylesheet);
    //mUi.frame_type_waiting->setStyleSheet(stylesheet);
    //mUi.frame_type_user->setStyleSheet(stylesheet);
    //  mUi.frame_type_user2->setStyleSheet(stylesheet);
}

void QGenieButtonBig::setType(const QString & tp)
{
    QString stemp=tp;
    stemp.replace(" ","_");
    setBackGroundIco(stemp+".png");
}

void QGenieButtonBig::click()
{
    qDebug()<<"QGenieButtonBig::click()";
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);

    QGenieButtonStackViewController::click();
}

void QGenieButtonBig::setUserStyle(const QString &s)
{
    qDebug()<<"QGenieButtonBig::setUserStyle";
    mUi.userText->setText(s);
}

void QGenieButtonBig::setBKG(const QString &s)
{
    // mUi.frame_butline->setStyleSheet(s);

#ifdef TEST_UI
    QString f("./testui.txt");
    QString stylesheet;
    if(QFile::exists(f))
    {
        QFile file(f);
        file.open(QFile::ReadOnly);
        stylesheet = QLatin1String(file.readAll());
        file.close();
        this->setStyleSheet(stylesheet);
    }
    else
#endif
    {
        this->setStyleSheet(s);

    }


}

void QGenieButtonBig::changeText()
{
    DEFINE_LANGUAGE_MANAGER

            QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);

    const plug_inf *p=proxymanager->getPluginInf(this);

    //    if(p && mUserTextFlag[UTC_GOOD_LEFT])
    //    {
    //        SET_LABEL_TEXT(mUi.label_status,mUserTextCache[UTC_GOOD_LEFT],p->uuid);
    //    }
    //    else
    //    {
    //        SET_TEXT(mUi.label_status,mUserTextCache[UTC_GOOD_LEFT]);
    //    }

    //    if(p && mUserTextFlag[UTC_GOOD_RIGHT])
    //    {
    //        SET_LABEL_TEXT(mUi.label_good,mUserTextCache[UTC_GOOD_RIGHT],p->uuid);
    //    }
    //    else
    //    {
    //        SET_TEXT(mUi.label_good,mUserTextCache[UTC_GOOD_RIGHT]);
    //    }

    //    if(p && mUserTextFlag[UTC_WARRING_LEFT])
    //    {
    //        SET_LABEL_TEXT(mUi.label_warring,mUserTextCache[UTC_WARRING_LEFT],p->uuid);
    //    }
    //    else
    //    {
    //        SET_TEXT(mUi.label_warring,mUserTextCache[UTC_WARRING_LEFT]);
    //    }

    //    if(p && mUserTextFlag[UTC_WARRING_RIGHT])
    //    {
    //        SET_LABEL_TEXT(mUi.label_warring_click,mUserTextCache[UTC_WARRING_RIGHT],p->uuid);
    //    }
    //    else
    //    {
    //        SET_TEXT(mUi.label_warring_click,mUserTextCache[UTC_WARRING_RIGHT]);
    //    }

    //    if(p && mUserTextFlag[UTC_ERROR_RIGHT])
    //    {
    //        SET_LABEL_TEXT(mUi.label_error_click,mUserTextCache[UTC_ERROR_RIGHT],p->uuid);
    //    }
    //    else
    //    {
    //        SET_TEXT(mUi.label_error_click,mUserTextCache[UTC_ERROR_RIGHT]);
    //    }

    //    if(p && mUserTextFlag[UTC_ERROR_LEFT])
    //    {
    //        SET_LABEL_TEXT(mUi.label_error,mUserTextCache[UTC_ERROR_LEFT],p->uuid);
    //    }
    //    else
    //    {
    //        SET_TEXT(mUi.label_error,mUserTextCache[UTC_ERROR_LEFT]);
    //    }

    //    if(p && mUserTextFlag[UTC_WAITING_LEFT])
    //    {
    //        SET_LABEL_TEXT(mUi.label_status_waiting,mUserTextCache[UTC_WAITING_LEFT],p->uuid);
    //    }
    //    else
    //    {
    //        SET_TEXT(mUi.label_status_waiting,mUserTextCache[UTC_WAITING_LEFT]);
    //    }

    //    if(p &&  mUserTextFlag[UTC_WAITING_RIGHT])
    //    {
    //        SET_LABEL_TEXT(mUi.label_waiting,mUserTextCache[UTC_WAITING_RIGHT],p->uuid);
    //    }
    //    else
    //    {
    //       SET_TEXT(mUi.label_waiting,mUserTextCache[UTC_WAITING_RIGHT]);
    //    }

    //    if(p && mUserTextFlag[UTC_USER_LEFT])
    //    {
    //        SET_LABEL_TEXT(mUi.label_status_user,mUserTextCache[UTC_USER_LEFT],p->uuid);
    //    }
    //    else
    //    {
    //        SET_TEXT(mUi.label_status_user,mUserTextCache[UTC_USER_LEFT]);
    //    }

    //    if(p && mUserTextFlag[UTC_USER_RIGHT])
    //    {
    //        SET_LABEL_TEXT(mUi.label_user,mUserTextCache[UTC_USER_RIGHT],p->uuid);
    //    }
    //    else
    //    {
    //        SET_TEXT(mUi.label_user,mUserTextCache[UTC_USER_RIGHT]);
    //    }

    //    int tempid=mUserTextCache[UTC_TEXT];

    if(p && mLanIdx >=0)
    {
        //        SET_LABEL_TEXT(mUi.label_error_text,mLanIdx,p->uuid);
        //        SET_LABEL_TEXT(mUi.label_good_text,mLanIdx,p->uuid);
        //        SET_LABEL_TEXT(mUi.label_waiting_text,mLanIdx,p->uuid);
        //        SET_LABEL_TEXT(mUi.label_warring_text,mLanIdx,p->uuid);
        //        SET_LABEL_TEXT(mUi.label_user_text,mLanIdx,p->uuid);
        SET_LABEL_TEXT(mUi.label_text,mLanIdx,p->uuid);
    }



}

void QGenieButtonBig::changeEvent(QEvent* event)
{
    if (event->type()==QEvent::LanguageChange)
    {
        QGenieInterfaceLanguageManager *languagemanager=QGenieInterfaceLanguageManager::GetSingletonPtr();
        ASSERT(languagemanager);

        changeText();

    }
    QPushButton::changeEvent(event);
}

void QGenieButtonBig::slot_NotifySubState()
{   
    ASSERT(0);
    //qDebug()<<"QGenieButtonBig::slot_NotifySubState()";

    //int curstate=EBSS_NONE;
    //if(!mUi.sub_but_good->isHidden())
    //{
    //    ASSERT(!mUi.sub_but_error->isHidden());
    //    curstate=mUi.sub_but_good->ui->stackedWidget->currentIndex()+EBSS_BUT;
    //    ASSERT(mUi.sub_but_error->ui->stackedWidget->currentIndex()+EBSS_BUT==curstate);
    //}
    //QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    //ASSERT(proxymanager);
    //const plug_inf *p = proxymanager->getPluginInf(this);
    //if(p && p->pi)
    //{
    //    p->pi->command(COMMAND_NOTIFY_SUBSTATE,curstate);
    //}

}

//void QGenieButtonBig::on_sub_but_error_clicked()
//{
//    qDebug()<<"QGenieButtonBig::on_sub_but_error_clicked();";
//}

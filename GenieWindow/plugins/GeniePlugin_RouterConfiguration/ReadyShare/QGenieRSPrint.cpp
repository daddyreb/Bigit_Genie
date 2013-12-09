#include "QGenieRSPrint.h"
#include <QEvent>
//#include <QFont>
#include "genie2_interface.h"
#include "language_RouterConfig.h"
#include "pcpform.h"
#include <QUiLoader>

QGenieRSPrint::QGenieRSPrint()
    :mW(NULL)
{

}

QWidget * QGenieRSPrint::getUi()
{

    if(!mW)
    {
        QUiLoader loader;
        QFile f(GENIE2_RES("ui/readyshareprint.ui"));
        bool btemp=f.open(QFile::ReadOnly);
        Q_ASSERT(btemp);
        QWidget *w=loader.load(&f, NULL);
        Q_ASSERT(w);
        mW=w;
        QPushButton *but_back=mW->findChild<QPushButton *>("but_back");
        Q_ASSERT(but_back);
        connect(but_back,SIGNAL(clicked()),this,SLOT(slot_ButBack()));

//        QList<QLabel *> labs=qFindChildren<QLabel *>(w);
//        c=labs.count();
//        for(int i=0;i<c;i++)
//        {
//            if(labs[i]->openExternalLinks())
//            {
//                connect(labs[i],SIGNAL(linkActivated(QString)),this,SLOT(slot_linkActivated(QString)));
//            }
//        }
        retranslateUi();

    }
    return mW;


}

void QGenieRSPrint::slot_ButBack()
{
    PCPForm *pcpform=PCPForm::GetSingletonPtr();
    Q_ASSERT(pcpform);
    pcpform->mStack->setCurrentWidget(pcpform->mManagerObj->getUi());
}

//void QGenieRSPrint::changeEvent(QEvent *e)
//{
//    switch (e->type()) {
//    case QEvent::LanguageChange:
//         retranslateUi();
//        break;
//    default:
//        break;
//    }
//}


void QGenieRSPrint::retranslateUi()
{

    if(!mW)
    {
        return;
    }
    const struct _{const char *objname;int lan;int type;const char *mode;}
    cPara[]={
        {"lab_title",L_READY_SHARE_PRINTING , 0,"<b>%1</b>"}
        ,{"lab_desc",L_READY_SHARE_PRINTING_DESC,0,NULL}
        ,{"lab_how",L_READY_SHARE_PRINTING_HOW_WORKS,0,NULL}
        ,{"lab_step1",L_READY_SHARE_PRINTING_STEPS_ONE,0,NULL}
        ,{"lab_step2",L_READY_SHARE_PRINTING_STEPS_TWO,0,NULL}
        ,{"lab_step3",L_READY_SHARE_PRINTING_STEPS_THREE,0,NULL}
        ,{"lab_step4",L_READY_SHARE_PRINTING_STEPS_FOUR,0,NULL}
        ,{"but_back",L_READY_SHARE_BACK,1,NULL}
    };

    for(int i=0;i<sizeof(cPara)/sizeof(cPara[0]);i++)
    {
        QString s=GENIE2_GET_TEXT(cPara[i].lan);
        if(cPara[i].mode)
        {
            s=QString(cPara[i].mode).arg(s);
        }
        if(0==cPara[i].type)
        {
            QLabel *lab=mW->findChild<QLabel*>(cPara[i].objname);
            Q_ASSERT(lab);
            lab->setText(s);
        }
        else if(1==cPara[i].type)
        {
            QPushButton *but=mW->findChild<QPushButton*>(cPara[i].objname);
            Q_ASSERT(but);
            but->setText(s);
        }

    }

//    lblReadyShare->setText("<b>" + GENIE2_GET_TEXT(L_READY_SHARE_PRINTING )+ "</b>");
//    lblDesc->setText(GENIE2_GET_TEXT(L_READY_SHARE_PRINTING_DESC ));
//    lblHow->setText("<b>"+GENIE2_GET_TEXT(L_READY_SHARE_PRINTING_HOW_WORKS )+"</b>");
//    lblStep1->setText(GENIE2_GET_TEXT(L_READY_SHARE_PRINTING_STEPS_ONE ));
//    lblStep2->setText(GENIE2_GET_TEXT(L_READY_SHARE_PRINTING_STEPS_TWO ));

//    QString strTextStep3=GENIE2_GET_TEXT(L_READY_SHARE_PRINTING_STEPS_THREE );
//    lblStep3->setText(strTextStep3);


//    lblStep4->setText(GENIE2_GET_TEXT(L_READY_SHARE_PRINTING_STEPS_FOUR ));

//    btnBack->setText(GENIE2_GET_TEXT(L_READY_SHARE_BACK ));

}

//void QGenieRSPrint::slot_linkActivated(const QString &url)
//{

//}

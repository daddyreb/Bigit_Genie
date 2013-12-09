#include "QGenieQRCodeManager.h"
#include "genie2_interface.h"
#include "QGenieRCWirelessObject.h"
#include "QGenieGaccObject.h"
#include <QUiLoader>
#include "language_RouterConfig.h"
#include "genie2_qrcode.h"
#include <QPrintDialog>
#include <QPrinterInfo>
#include <QPrinter>

extern quint64 sRCFlags;
QString autoWrap(const QString &s, int limit=34);

QGenieQRCodeManager::QGenieQRCodeManager()
    :mCurLabel(NULL)
    ,mbPrintText(true)
    ,but_print(NULL)
    ,mDialog(NULL)
{
    for(int i=0;i<QWT_NUM;i++)
    {
        mQRCodeW[i]=NULL;
    }
}

void QGenieQRCodeManager::clearAll()
{
    for(int i=0;i<QWT_NUM;i++)
    {
        GENIE2_SAFEDELETE(mQRCodeW[i]);
    }
}

void QGenieQRCodeManager::hideWidget(int type)
{
    Q_ASSERT((type >=0)&&(type < QWT_NUM));
    if(mQRCodeW[type])
    {
        mQRCodeW[type]->hide();
    }
}

void QGenieQRCodeManager::popDialog(QLabel *lab)
{
    QUiLoader loader;
    bool btemp;
    QFile f(GENIE2_RES("ui/qrcode.ui"));
    btemp=f.open(QFile::ReadOnly);
    Q_ASSERT(btemp);
    QWidget *w=loader.load(&f, QGenieIface2ShareObject::sSingleInstance->mMainFrame);
    Q_ASSERT(w);
    translateUi(w);
    QDialog *dlg=qobject_cast<QDialog *>(w);
    GENIE2_SET_FRAMELESSSTYLE(dlg);
    QLabel *lab_qrcode=w->findChild<QLabel *>("lab_qrcode");
    Q_ASSERT(lab_qrcode);
    mCurLabel=lab_qrcode;
    mDialog=dlg;
    but_print=w->findChild<QPushButton *>("but_print");
    Q_ASSERT(but_print);
//    QPixmap pixmap=lab->pixmap()->scaled(lab->pixmap()->size()*2);
    const QPixmap &pixmap=*(lab->pixmap());

//    lab_qrcode->resize(pixmap.size());

    lab_qrcode->setPixmap(pixmap);
//    QImage img(pixmap->size()*2,QImage::Format_RGB16);
//    QPainter p(&img);
//    p.drawTiledPixmap(img.rect(),*pixmap);
    lab_qrcode->setProperty("ssid",lab->property("ssid"));
    lab_qrcode->setProperty("password",lab->property("password"));
    QList<QPushButton *> bts=w->findChildren<QPushButton *>();
    int c=bts.count();
    for(int i=0;i<c;i++)
    {
        connect(bts[i],SIGNAL(clicked()),this,SLOT(slot_buttonClicked()));
    }
    QCheckBox *chk=w->findChild<QCheckBox*> ("chk_cleartext");
    Q_ASSERT(chk);

#ifndef PRINT_SSID
    chk->setVisible(false);
    mbPrintText=false;
#else
    chk->setChecked(mbPrintText);
    connect(chk,SIGNAL(clicked(bool)),this,SLOT(slot_setPrintText(bool)));
#endif



    sRCFlags |= RCF_MODAL_DIALOG_ON;
    dlg->exec();
    mCurLabel=NULL;
    but_print=NULL;
    mDialog=NULL;
    Q_ASSERT(dlg);

    w->deleteLater();





}

void QGenieQRCodeManager::translateUi(QWidget *w)
{
    //0.label   1.button    2.checkbox  3.radiobutton
    const struct _{const char *name;int lan;int type;}
    cPara[]={


        {"chk_cleartext",L_QRCODE_CLEARTEXT,2}
        ,{"but_print",L_QRCODE_PRINT,1}
        ,{"but_close",L_QRCODE_CLOSE,1}
        ,{"but_save",L_QRCODE_SAVE,1}
        ,{"lab_scan",L_QRCODE_SCAN,0}
    };
    for(int i=0;i<sizeof(cPara)/sizeof(cPara[0]);i++)
    {
        if(0==cPara[i].type)
        {
            GENIE2_TRANSLATE_CHILD_LABEL(w,cPara[i].name,cPara[i].lan);
        }
        if(1==cPara[i].type)
        {
            GENIE2_TRANSLATE_CHILD_BUTTON(w,cPara[i].name,cPara[i].lan);
        }
        if(2==cPara[i].type)
        {
            GENIE2_TRANSLATE_CHILD_CHECKBOX(w,cPara[i].name,cPara[i].lan);
        }
    }
}

bool QGenieQRCodeManager::eventFilter(QObject *o, QEvent *e)
{
    QEvent::Type type=e->type();
    QWidget *w=qobject_cast<QWidget *>(o);
    Q_ASSERT(w);

    if(type==QEvent::Enter)
    {
        w->setCursor(Qt::PointingHandCursor);
        w->setUserData(0,(QObjectUserData *)true);
        w->update();
    }
    else if(type==QEvent::Leave)
    {
        w->setCursor(Qt::ArrowCursor);
        w->setUserData(0,(QObjectUserData *)false);
        w->update();
    }
    else if(type ==QEvent::MouseButtonPress)
    {
        qDebug()<<"qrcode window clicked!!";
        QLabel *lab=qobject_cast<QLabel *>(o);
        Q_ASSERT(lab);
        popDialog(lab);
    }
    else if(type==QEvent::Paint)
    {
        QPainter p(w);
        p.setPen(qRgb(158,153,253));
        if((bool)(w->userData(0)))
        {
            p.drawRect(p.viewport().adjusted(0,0,-1,-1));
            p.drawRect(p.viewport().adjusted(1,1,-2,-2));
        }
    }
    return false;
};

void QGenieQRCodeManager::slot_buttonClicked()
{
    QObject *obj=sender();
    Q_ASSERT(obj);
    QString obj_name=obj->objectName();
    if(obj_name=="but_save")
    {
        Q_ASSERT(mCurLabel);
        static QString savedPath=QString("..");
        QString ssid=mCurLabel->property("ssid").toString();
        QString fileName = QFileDialog::getSaveFileName
                (mDialog
                 ,GENIE2_GET_TEXT(L_QRCODE_SAVE)
                 ,savedPath+"/"+(ssid)
                 ,tr("Png file (*.png)"));
        QRegExp regM("(.*)/(.*)");
        if(regM.exactMatch(fileName))
        {
            qDebug()<<regM.cap(0)<<regM.cap(1)<<regM.cap(2);
            savedPath=regM.cap(1);
        }
        else
        {
            savedPath=fileName;
        }
#ifdef Q_OS_MAC
        fileName+=".png";
#endif
        mCurLabel->pixmap()->save(fileName);

    }
    else if(obj_name=="but_print")
    {
		/////////////////////////////// kai.yan add///////////////////////////////////////////
		///get computer printer list 
        QList<QPrinterInfo> printerlist = QPrinterInfo::availablePrinters();
		//LPBYTE buffer;
		//DWORD needed = 0;
		//DWORD returned = 0;

        if (printerlist.count()<=0 /*EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 4, 0, 0, &needed, &returned)*/)
        {
            QStringList dialog_lans;
            dialog_lans<<GENIE2_GET_TEXT(L_QRCODE_PRINT_TITLE)<<GENIE2_GET_TEXT(L_QRCODE_PRINT_PROMPT);
            sRCFlags |= RCF_MODAL_DIALOG_ON;
            bool dialogOk=GENIE2_UI_COMMAND(UICMD_DIALOG_WARRING,dialog_lans).toBool();
            sRCFlags &=~RCF_MODAL_DIALOG_ON;
            if(!dialogOk)
            {
                return;
            }
        }
		///////////////////////////////// end /////////////////////////////////////////
        QPrinter printer;
        QPrintDialog printDialog(&printer,mDialog);
        if (printDialog.exec())
        {
            const QPixmap *pixmap=mCurLabel->pixmap();
            const QSizeF dcSize(397,559);
            QPixmap titlepixmap(GENIE2_RES("images/logo.png"));
            Q_ASSERT(titlepixmap.width()>10);
            QRectF rectF=printer.pageRect ( QPrinter::DevicePixel);


            rectF.moveTo(rectF.left()+(rectF.width()-dcSize.width())/2
                         ,rectF.top()+(rectF.height()-dcSize.height())/2
                         );
            rectF.setSize(dcSize);
//            qreal pixmapPara=rectF.width()*0.8/titlepixmap.width();
//            titlepixmap=titlepixmap.scaled(titlepixmap.size()*pixmapPara);

            int titleX=(int)(rectF.left()+(rectF.width()-titlepixmap.width())/2 );
//            int titleY=(int)(rectF.top()+(rectF.height()-titlepixmap.height())/2);
            int titleY=(int)(rectF.top()+80);


            QString ssid=mCurLabel->property("ssid").toString();
            QString pwd=mCurLabel->property("password").toString();

            QString text=QString("%1 \n\t%2\n%3 \n\t%4")
                    .arg(GENIE2_GET_TEXT(L_RTCF_SETTING_SSID)
                         ,autoWrap(ssid,25)
                         ,GENIE2_GET_TEXT(L_RTCF_INFO_PASSWOED)
                         ,autoWrap(pwd,25)
                         );


#ifdef DBG_PRINTER
            QImage bufferImg(printer.paperSize(QPrinter::DevicePixel).toSize(),QImage::Format_RGB16);

            QPainter bufferP(&bufferImg);
//            bufferP.setBrushOrigin(-rectF.topLeft());
            bufferP.fillRect(bufferImg.rect(),Qt::white);
            QPainter *realP=&bufferP;

#else
            QPainter p(&printer);
            QPainter *realP=&p;
#endif
            QFont font;
            font.setPointSize(16);
            font.setBold(true);
            realP->setFont(font);
//            int textX=(int)(rectF.left()+(rectF.width()-p.fontMetrics().width(text))/2 );
//            int textY=(int)(rectF.bottom()-30);




//            printer.getPageMargins (&left,&top,&right,&bottom  ,QPrinter::DevicePixel);
            const int bottomRectHeight=240;
            const int titleBotton=titleY+titlepixmap.height();
            QRect textRect(rectF.left(),rectF.bottom()-bottomRectHeight,rectF.width(),bottomRectHeight);
            const int textTop=QFontMetrics(font).boundingRect(textRect,Qt::TextWordWrap|Qt::AlignCenter,text).y();
                    /*rectF.bottom()-bottomRectHeight/2
                    - QFontMetrics(font).boundingRect().height()/2;*/

            int x=(int)(rectF.left()+(rectF.width()-pixmap->width())/2 );
            int y=(int)(titleBotton+textTop-pixmap->height())/2;
//            int y=(int)(rectF.top()+(rectF.height()-pixmap->height())/2 );

//            realP->drawLine(0,titleBotton,rectF.width(),titleBotton);
//            realP->drawLine(0,textTop,rectF.width(),textTop);

            const int scanY=y+(pixmap->height())+5;
            QString text_scan=GENIE2_GET_TEXT(L_QRCODE_SCAN)+"\n\n";
            realP->drawText(rectF.left(),scanY,rectF.width()
                            ,60,Qt::AlignTop|Qt::AlignHCenter|Qt::TextWordWrap,text_scan);



            realP->drawPixmap(titleX,titleY,titlepixmap);
            realP->drawPixmap(x,y,*pixmap);
//            realP->drawRect(rectF.adjusted(-1,-1,0,0));
            if(mbPrintText)
            {
                realP->drawText(textRect
                                ,Qt::TextWordWrap|Qt::AlignCenter,text);
            }
#ifdef DBG_PRINTER
            bufferImg.save("d:\\temp\\dbgprinter.png");
            QProcess::startDetached("mspaint" ,QStringList()<<"d:\\temp\\dbgprinter.png");
#endif
//            QPushButton *but_print=qobject_cast<QPushButton *>(obj);
            Q_ASSERT(but_print);
            but_print->setEnabled(false);
            QTimer::singleShot(5000,this,SLOT(slot_SetPrintButtonEnable()));
        }
	}
}

void QGenieQRCodeManager::slot_setPrintText(bool b)
{
    mbPrintText=b;
}

void QGenieQRCodeManager::slot_SetPrintButtonEnable()
{
    if(but_print)
    {
        but_print->setEnabled(true);
    }
}



void QGenieQRCodeManager::setData(const QString &ssid, const QString &pwd, int type)
{
    Q_ASSERT((type >=0)&&(type < QWT_NUM));
    if(!mQRCodeW[type])
    {
        mQRCodeW[type]=new QLabel();
//        mQRCodeW[type]->setFixedSize(100,100);
        mQRCodeW[type]->installEventFilter(this);
        mQRCodeW[type]->setCursor(Qt::PointingHandCursor);
        QWidget *parent=NULL;
        switch(type)
        {
        case QWT_WIRELESS:
            parent=QGenieRCWirelessObject::GetSingletonPtr()->mStackWC->widget(0);
            break;
        case QWT_GACC:
            parent=QGenieGaccObject::GetSingletonPtr()->stack_gacc->widget(0);
            break;
        default:
            Q_ASSERT(0);
        }
        mQRCodeW[type]->setParent(parent);

    }
    mQRCodeW[type]->show();
    mQRCodeW[type]->setProperty("ssid",ssid);
    mQRCodeW[type]->setProperty("password",pwd);
    QString data=QString("WIRELESS:%1;PASSWORD:%2").arg(ssid,pwd).toLocal8Bit();
    QGenieQRCode qrcode(data);

    int iLRet=qrcode.width();
    const unsigned char * s=qrcode.data();

//    bool* s = qrcode.calQrcode(data.data(),iLen,iLRet);
//    qDebug()<<iLen<<iLRet;
    const int cPointSize=3;
    QPixmap pixmap(iLRet*cPointSize,iLRet*cPointSize/*,QImage::Format_RGB16*/);
    QPainter p(&pixmap);
    for(int i=0;i<iLRet;i++)
        for(int j=0;j<iLRet;j++)
        {
            // qDebug()<<s[i][j];
            p.fillRect(i*cPointSize,j*cPointSize,cPointSize,cPointSize,(s[j*iLRet+i]&(1<<0))?qRgb(0,0,0):qRgb(255,255,255));
            //            image.setPixel(i,j,(s[i][j])?qRgb(0,0,0):qRgb(255,255,255));
        }
    //       image.save("d:\\qrcode.png");
//    qrcode.destroyMemory(s,iLRet);
//    delete [] s;
    mQRCodeW[type]->setFixedSize(pixmap.size()+QSize(4,4));
    mQRCodeW[type]->setAlignment(Qt::AlignCenter);
    mQRCodeW[type]->setPixmap(pixmap);
//    mQRCodeW[type]->lower();
//    mQRCodeW[type]->setIcon(QIcon(pixmap)) ;
//    mQRCodeW[type]->setIconSize(pixmap.size());
    QPoint br=mQRCodeW[type]->parentWidget()->geometry().bottomRight();
    br-=(mQRCodeW[type]->geometry().bottomRight()-mQRCodeW[type]->geometry().topLeft());
    const int cParas[QWT_NUM][2]={
      {0,0}
      ,{-11,-11}
    };
    br+=QPoint(cParas[type][0],cParas[type][1]);
    mQRCodeW[type]->move(br);

}

#include "QLaunchUpdateDialog.h"
#include "ui_QLaunchUpdateDialog.h"
#include "QLauncher.h"
#include <QDir>
#include <QDebug>
#include <QCryptographicHash>
#include "setting_def.h"
#include "QLaunchManager.h"
#include <QUrl>
#include <QSettings>
#include <QFileInfo>
#include "QLaunchFileDiscription.h"
#include "LanguageMainFrame.h"
#include "ISettings.h"
#include "defines.h"
#ifdef _WIN32
#include <afxres.h>
#include <tchar.h>
#endif
#include "QUpdateProcess.h"

#ifdef PLATFORM_WIN32
#define FINISH_FONT "  font-family:'Arial'; font-size:9pt; font-weight:400; font-style:normal;"
#else
#define FINISH_FONT "  font-family:'Arial'; font-size:11pt; font-weight:400; font-style:normal;"
#endif
#define FINISH_TEXT_MODE "<p style=\" "FINISH_FONT"\">%1 %2</p>"\
"<p style=\" "FINISH_FONT"\"><a href=\"http://www.netgear.com/genie\"><span style=\" text-decoration: underline; color:#0000ff;\">http://www.netgear.com/genie</span></a></p>"



QLaunchUpdateDialog::QLaunchUpdateDialog(QWidget *parent)
    :QDialog(parent)
    ,ui(new Ui::QLaunchUpdateDialog)
#ifndef SHEET_STYLE_FORMAC
    ,mDrag(false)
#endif
{

    mUpdateProcess=new QUpdateProcess(this);
    mButtonText[DCT_LEFT_BUTTON]=L_MAIN_UPDATE_CHECK;
    mButtonText[DCT_RIGHT_BUTTON]=L_MAIN_UPDATE_CANCEL;
    mButtonText[DCT_FINISHPAGE]=L_MAIN_UPDATE_PAGE_FINISH;
    ui->setupUi(this);

    this->setWindowFlags(DIALOG_FLAG);
    this->setAttribute(Qt::WA_TranslucentBackground);
    qDebug()<<windowModality();
    setWindowModality(Qt::ApplicationModal);
//    changeText();
}

QLaunchUpdateDialog::~QLaunchUpdateDialog()
{
  //  mReloadTimer.stop();
    delete ui;
    SAFE_DELETE(mUpdateProcess);
}

void QLaunchUpdateDialog::startup(UPDATE_RESON reson)
{
    mUpdateProcess->mUpdateReson=reson;
#ifdef DLG_AUTO_RUN
    on_but_left_clicked();
#endif
#ifndef SHEET_STYLE_FORMAC
	hide();
    exec();
#else

	show();
#endif
}

void QLaunchUpdateDialog::changeUiState(DIALOG_STATE state)
{
    qDebug()<<"changeUiState"<<state;
    switch(state)
    {
    case DS_INIT:
        ui->stackedWidget->setCurrentIndex(0);
        //ui->but_left->setText("Check");
        mButtonText[DCT_LEFT_BUTTON]=L_MAIN_UPDATE_CHECK;
        ui->but_left->setDisabled(false);
        ui->but_right->setDisabled(false);
        changeText();
        break;
    case DS_GETDISP_DB:
        ui->stackedWidget->setCurrentIndex(2);
        ui->but_left->setDisabled(true);
        ui->but_right->setDisabled(false);
        changeText();
        break;
    case DS_SHOW_RELEASENOTE:
        ui->stackedWidget->setCurrentIndex(4);
        ui->but_left->setDisabled(false);
        ui->but_right->setDisabled(false);
        mButtonText[DCT_LEFT_BUTTON]=L_MAIN_UPDATE_DOWNLOAD;
        changeText();
        break;
    case DS_GETFILE:
        ui->stackedWidget->setCurrentIndex(2);
        ui->but_left->setDisabled(true);
        ui->but_right->setDisabled(false);
        break;
    case DS_FINISH:           
        ui->but_right->setDisabled(false);
        ui->but_left->setDisabled(!(mUpdateProcess->mState ==PS_FINISH_OK));
        ui->stackedWidget->setCurrentIndex(3);
        mButtonText[DCT_LEFT_BUTTON]=L_MAIN_UPDATE_FINISH;
        if(mUpdateProcess->mState ==PS_FINISH_OK)
        {
           // ui->finish_state->setText("Update Finished");
            mButtonText[DCT_FINISHPAGE]=L_MAIN_UPDATE_PAGE_FINISH;
            ui->icon->setStyleSheet("background-image:url(:/label_update_right.png);");

        }
        else if(mUpdateProcess->mState ==PS_FINISH_ERROR_ALREADYNEWEST)
        {
          //  ui->finish_state->setText("Your Application is already newest");
            mButtonText[DCT_FINISHPAGE]=L_MAIN_UPDATE_ALREADY_NEWEST;
            ui->icon->setStyleSheet("background-image:url(:/label_update_right.png);");
        }
        else if(mUpdateProcess->mState >=PS_FINISH_ERROR_DIROP
                && mUpdateProcess->mState <= PS_FINISH_ERROR_USERCLICK
            )
        {
          //  ui->finish_state->setText("Update Fail!!");
            mButtonText[DCT_FINISHPAGE]=L_MAIN_UPDATE_PAGE_WRONG;
            ui->icon->setStyleSheet("background-image:url(:/label_update_wrong.png);");
        }
        changeText();
        break;
    }
}

void QLaunchUpdateDialog::on_but_left_clicked()
{
    if(mUpdateProcess->mState == PS_INIT)
    {
        mUpdateProcess->startup(UR_USER_CHOISE);
    //    ui->but_left->setDisabled(true);
    }
    else if(PS_GETDISP == mUpdateProcess->mState)
    {
        qDebug() << "Start Get AllFiles";
        mUpdateProcess->startGetAllFiles();
    }
    else if(PS_FINISH_OK == mUpdateProcess->mState)
    {

        hide();
  //      mReloadTimer.start(1000);
        QLaunchFileDiscription fdisp(QString(HTTP_LOCAL_DIR)+"/"+HTTP_DISP_FILE_NAME);

        QString tempfilename=fdisp.getPath();
        mUpdateProcess->runExeFile(QString(HTTP_LOCAL_DIR)+"/"+HTTP_UPDATE_FILE_NAME);
    }
    else if(mUpdateProcess->mState >=PS_FINISH_OK
       && mUpdateProcess->mState <= PS_FINISH_ERROR_USERCLICK)
    {
        qDebug()<< QString("delete dir ")+HTTP_LOCAL_DIR;
        mUpdateProcess->deleteDirectory(HTTP_LOCAL_DIR);
        hide();
     //   qApp->quit();

    }
}

void QLaunchUpdateDialog::on_but_right_clicked()
{
    QLaunchManager *manager=QLaunchManager::GetSingletonPtr();
    ASSERT(manager);

    if(mUpdateProcess->mUpdateReson==UR_WHEN_NOGENIE)
    {
        qDebug()<<"try quit app";
        manager->mQuitFlag=true;
        qApp->quit();
        return;
    }
    if(mUpdateProcess->mState==PS_INIT)
    {
        hide();
    }
    else
    {
        mUpdateProcess->userCancel();
        hide();
    }
}

//step1 getdiscription




QString QLaunchUpdateDialog::getText(int idx)
{
    static bool sFirstrun=true;
    static ILaunchInf *sLi=NULL;
    if(sFirstrun || !sLi)
    {
        QLauncher *launcher=QLauncher::GetSingletonPtr();
        ASSERT(launcher);
        sLi=launcher->getLaunchInf();
        sFirstrun=false;
    }
  //  ASSERT(sLi);
    if(!sLi)
    {
        return "";
    }
    return sLi->getMultiLanText(idx);
}



void QLaunchUpdateDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    if (e->type()==QEvent::LanguageChange)
    {

       changeText();
    }

}

void QLaunchUpdateDialog::changeText()
{
    ui->label->setText(getText(L_MAIN_UPDATE_TITLE));
    ui->label_3->setText(getText(L_MAIN_UPDATE_TITLE));
    ui->label_2->setText(getText(L_MAIN_UPDATE_TITLE));
    ui->label_4->setText(getText(L_MAIN_UPDATE_TITLE));

//    ui->label_5->setText(getText(L_MAIN_UPDATE_WELCOME));
    ui->but_left->setText(getText(mButtonText[DCT_LEFT_BUTTON]));
    ui->but_right->setText(getText(mButtonText[DCT_RIGHT_BUTTON]));




    if(mButtonText[DCT_FINISHPAGE]==L_MAIN_UPDATE_PAGE_WRONG)
    {
        QString text1,text2;
        text1=getText(mButtonText[DCT_FINISHPAGE]);
        text2=getText(L_MAIN_UPDATE_LINK);
        ui->finish_state->setText(QString(FINISH_TEXT_MODE).arg(text1,text2));
    }
    else
    {
        ui->finish_state->setText(getText(mButtonText[DCT_FINISHPAGE]));
    }

    ui->text_remain_title->setText(getText(L_MAIN_UPDATE_REMAIN));
}





void	QLaunchUpdateDialog::closeEvent ( QCloseEvent * event )
{
    qDebug()<<"closeEvent ( QCloseEvent * event )";
    mUpdateProcess->mHttp.close();


}

#ifndef SHEET_STYLE_FORMAC
void QLaunchUpdateDialog::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && mDrag && !isFullScreen()) {
        move(event->globalPos() - mDragPosition);
        //    update();
       event->accept();

    }
//    static int t=0;
//    qDebug()<<"mouse move"<<(t++)%100;
}

void QLaunchUpdateDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
      //  qDebug("mouse press");
        mDrag=true;
        mDragPosition = event->globalPos() - frameGeometry().topLeft();
     //   event->accept();
    }
}

void	QLaunchUpdateDialog::mouseReleaseEvent ( QMouseEvent * event )
{
    if (event->button() == Qt::LeftButton) {
     //   qDebug("mouse release");
        mDrag=false;
    //    event->accept();
    }

}
#endif




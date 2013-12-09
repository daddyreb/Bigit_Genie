#include <QtGui>

#include "downloadwizard.h"
#include "downloadfirmware.h"

#include "ILanguageManager.h"
#include "LanguageFirmwareUpdate.h"

#define PLUGIN_ID "{c02fb5db-6c62-42e7-bf4a-f704247ed7b8}"

IManagerHandler *DownloadWizard::sHandler = NULL;

DownloadWizard::DownloadWizard(QWidget *parent) :
        WizardPage(parent)
{
    lanmanager = DownloadWizard::sHandler->getLanguageManager();

    setTitle(tr("Downloading Firmware"));
    setExplain(tr("When the updated software has downloaded, you may select 'Next' to get that version's download Path."));

    BackButton->setDisabled(true);
    NextButton->setDisabled(true);

    m_Widget = isSetArea();
    //ViewInfo = new QLabel(m_Widget);
    //ViewInfo->setHidden(true);

    QVBoxLayout *mLayout = new QVBoxLayout(m_Widget);
    DownloadProgress = new QProgressBar(m_Widget);
    //DownloadProgress->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    DownloadProgress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    DownloadProgress->setHidden(false);

    //////View download time
    QHBoxLayout *mTimeLayout = new QHBoxLayout(m_Widget);
    ViewTime = new QLabel(m_Widget);
    //ViewTime->setText("Download leave time ");
    ViewTime->setHidden(true);
    TimeLabel = new QLabel(m_Widget);
    TimeLabel->setHidden(true);
    //ViewTime->setAlignment(Qt::AlignCenter);
    //mTimeLayout->addStretch();
    mTimeLayout->addWidget(ViewTime);
    mTimeLayout->addWidget(TimeLabel);
    mTimeLayout->addStretch();

    ViewInfo = new QLabel(m_Widget);
    ViewInfo->setHidden(true);
    ViewInfo->setWordWrap(true);

    mDownload = new DownloadFirmware(m_Widget, DownloadProgress);
    connect(mDownload, SIGNAL(DoneFinish(bool)), this, SLOT(GetFirmware(bool)));
    connect(mDownload, SIGNAL(LeaveDownloadTime(int)),
            this, SLOT(ViewLeaveTime(int)));

    mLayout->addStretch();
    mLayout->addWidget(DownloadProgress);
    mLayout->addLayout(mTimeLayout);
    mLayout->addWidget(ViewInfo);
    mLayout->addStretch();

}

DownloadWizard::~DownloadWizard()
{

}

void DownloadWizard::Download(QStringList &Info, QString**mInfo, QList<int>**err)
{
    qDebug() << "Enter Downloading Page.";
    //DownloadProgress->reset();
    ViewInfo->clear();
    ViewInfo->setHidden(true);
    ViewTime->setHidden(false);
    TimeLabel->clear();
    DownloadProgress->setHidden(false);
    NextButton->setDisabled(true);

    if (*mInfo==NULL)
        *mInfo = new QString;
    outInfo = *mInfo;
    outInfo->clear();
    if (*err==NULL)
        *err = new QList<int>;
    intErr = *err;
    intErr->clear();

    //tempErr = new QString;
    bool flag;
    flag = mDownload->DownloadFile(Info, &outInfo, &intErr);

//    if (!flag){
//        //outErr->append(*tempErr);
//        emit DownloadFinish(flag);
//    }

}

//void DownloadWizard::Download(QStringList &Info, QString**mInfo)
//{
//    qDebug() << "Enter Downloading Page.";
//    //DownloadProgress->reset();
//    ViewInfo->clear();
//    ViewInfo->setHidden(true);
//    DownloadProgress->setHidden(false);
//    NextButton->setDisabled(true);
//
//    if (*mInfo==NULL)
//        *mInfo = new QString;
//    outInfo = *mInfo;
//    outInfo->clear();
//
//    tempErr = new QString;
//    bool flag;
//    flag = mDownload->DownloadFile(Info, &outInfo);
//
////    if (!flag){
////        //outErr->append(*tempErr);
////        emit DownloadFinish(flag);
////    }
//
//}

void DownloadWizard::GetFirmware(bool err)
{
//    if (err){
//        //outErr->append("OK");
//    }else {
//        //outErr->append(*tempErr);
//    }
    if (err){
        NextButton->setDisabled(false);
    }

    ViewTime->setHidden(true);
    TimeLabel->clear();
    TimeLabel->setHidden(true);

    emit DownloadFinish(err);
}

//void DownloadWizard::SetInfo(const QString &Info)
//{
//    ViewInfo->setText(Info);
//    ViewInfo->setHidden(false);
//
//    DownloadProgress->setHidden(true);
//    DownloadProgress->reset();
//}

void DownloadWizard::SetInfo(const QList<int> &Info)
{
    //ViewInfo->setText(Info);
    vInfo = Info;
    QString tempView;
    for(int i=0; i<vInfo.size(); i++){
        tempView += lanmanager->getText(vInfo.value(i), PLUGIN_ID);
    }
    if (vInfo.size()!=0)
        DownloadWizard::ViewInfo->setText(tempView);
    ViewInfo->setHidden(false);

    DownloadProgress->setHidden(true);
    DownloadProgress->reset();
}

void DownloadWizard::SetNextButton(bool stat)
{
    NextButton->setDisabled(stat);
}

void DownloadWizard::SetBackButton(bool stat)
{
    BackButton->setDisabled(stat);
}

void DownloadWizard::CancelDownload()
{
    mDownload->CloseHttp();
}

void DownloadWizard::DestructorDownload()
{
    mDownload->DestructorHttp();
}

void DownloadWizard::ViewLeaveTime(int msec)
{
    //ViewTime->setHidden(false);

    int intSecond = msec/1000;
    int mHour = intSecond/3600;
    int mMin = (intSecond%3600)/60;
    int mSec = intSecond%60;
    int mMSec = msec%1000;
    QTime mTime = QTime(mHour, mMin, mSec, mMSec);

    QString strTime = mTime.toString("hh:mm:ss");
    TimeLabel->setText(strTime);
    TimeLabel->setHidden(false);
}

void DownloadWizard::changeEvent(QEvent *e)
{
    //ILanguageManager *lanmanager = DownloadWizard::sHandler->getLanguageManager();
    QWidget::changeEvent(e);

    switch (e->type()){
    case QEvent::LanguageChange:
        {
            QString viewInfo;
            for(int i=0; i<vInfo.size(); i++){
                viewInfo += lanmanager->getText(vInfo.value(i), PLUGIN_ID);
            }
            if (vInfo.size()!=0)
                DownloadWizard::ViewInfo->setText(viewInfo);

            DownloadWizard::m_Title->setText(lanmanager->getText(L_UPGRADE_DOWNLOAD_TITLE, PLUGIN_ID));
            DownloadWizard::m_Intro->setText(lanmanager->getText(L_UPGRADE_DOWNLOAD_EXPLAIN, PLUGIN_ID));
            DownloadWizard::ViewTime->setText(lanmanager->getText(L_UPGRADE_DOWNLOAD_LEAVETIME, PLUGIN_ID));
            WizardPage::BackButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_BACKBUT, PLUGIN_ID));
            WizardPage::NextButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_NEXTBUT, PLUGIN_ID));
            WizardPage::CancelButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_CANCELBUT, PLUGIN_ID));
            break;

        }
    default:
        break;
    }

}

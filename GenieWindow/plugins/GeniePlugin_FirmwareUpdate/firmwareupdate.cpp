#include <QtGui>

#include "firmwareupdate.h"

//#include "infofirmware.h"
#include "checkfirmware.h"
#include "IPluginInf.h"
#include "IUiManager.h"
#include "IInfoManager.h"
#include "IClickManager.h"
#include "ITrayManager.h"
#include "ILanguageManager.h"
#include "plugin_ui_def.h"

#include "genieplugin_firmwareupdate.h"

#include "LanguageFirmwareUpdate.h"


//IntroPage     ->  0
//CheckPage     ->  1
//DownloadPage  ->  2
//UpdatePage    ->  3    //Not use ---- delete
//CompletePage  ->  3    //The CompletePage is replace the UpdatePage.
//FinishPage    ->  4

#define PLUGIN_ID "{c02fb5db-6c62-42e7-bf4a-f704247ed7b8}"

IManagerHandler *FirmwareUpdate::sHandler = NULL;

FirmwareUpdate::FirmwareUpdate(QWidget *parent)
    : QWidget(parent)
{
    isIntroNextBtn = true;
    isFrameTurn = false;
    isDownloading = true;

    mUimanager = sHandler->getUiManager();
    Q_ASSERT(mUimanager);

    mInfoManager = sHandler->getInfoManager();
    Q_ASSERT(mInfoManager);

    //IUiManager *mUimanager = sHandler->getUiManager();
    //Q_ASSERT(mUimanager);
    //mUimanager->setNetStateFlag("c02fb5db-6c62-42e7-bf4a-f704247ed7b8", true);

    //UpdateUrl = "http://routerlogin.net/cgi-bin/upgrade.html";
    //UpdateUrl = "http://routerlogin.net/UPG_upgrade.htm";
    UpdateUrl = "http://routerlogin.net";
    isChecked = false;

    WholeLayout = new QVBoxLayout(parent);

    IntroPage = new IntroWizard(parent);
    CheckPage = new CheckWizard(parent);
    DownloadPage = new DownloadWizard(parent);
    //UpdatePage = new UpdateWizard(parent);
    CompletePage = new CompleteWizard(parent);
    FinishPage = new FinishWizard(parent);


    WizardLayout = new QStackedWidget();
    WizardLayout->addWidget(IntroPage);
    WizardLayout->addWidget(CheckPage);
    WizardLayout->addWidget(DownloadPage);
    //WizardLayout->addWidget(UpdatePage);
    WizardLayout->addWidget(CompletePage);
    WizardLayout->addWidget(FinishPage);


    //////Search NETGEAR's Firmware Version
//    NetgearProduct = new InfoFirmware();
    ProductInfo = new QMap<QString, QString>;
//    //rError = new QString;
    intErr = new QList<int>;
//    //NetgearProduct->GetInfo(&ProductInfo, &rError);
//    //connect(NetgearProduct, SIGNAL(GetSuccess()), this, SLOT(SetNetgear()));
//    //connect(NetgearProduct, SIGNAL(GetFailed()), this, SLOT(NotNetgear()));
//    //connect(NetgearProduct, SIGNAL(GetFinish(bool)), IntroPage, SLOT(closeView()));


    //////Check NETGEAR's Firmware Version
    ProductCheck = new CheckFirmware();
    CheckInfo = new QMap<QString, QString>;
    intCheckErr = new QList<int>;
    connect(ProductCheck, SIGNAL(CheckFinish(bool)), this, SLOT(CheckResult(bool)));
    connect(ProductCheck, SIGNAL(CheckFinish(bool)), IntroPage, SLOT(closeView()));
    connect(ProductCheck, SIGNAL(GetNewTimeOut(int)), this, SLOT(viewGetNewTimeout(int)));

    //////Download Firmware Infomation
    DownInfo = new QString;
    intDownloadErr = new QList<int>;


    WholeLayout->addWidget(WizardLayout);

    setLayout(WholeLayout);


    connect(DownloadPage, SIGNAL(DownloadFinish(bool)),
            this, SLOT(DownloadOver(bool)));

    connect(CheckPage, SIGNAL(ToBack()), this, SLOT(PageToBack()));
    connect(DownloadPage, SIGNAL(ToBack()), this, SLOT(PageToBack()));
    //connect(UpdatePage, SIGNAL(ToBack()), this, SLOT(PageToBack()));
    connect(CompletePage, SIGNAL(ToBack()), this, SLOT(PageToBack()));
    connect(FinishPage, SIGNAL(ToBack()), this, SLOT(PageToBack()));

    connect(IntroPage, SIGNAL(ToNext()), this, SLOT(PageToNext()));
    connect(IntroPage, SIGNAL(ToNext()), IntroPage, SLOT(openView()));
    connect(CheckPage, SIGNAL(ToNext()), this, SLOT(PageToNext()));
    connect(DownloadPage, SIGNAL(ToNext()), this, SLOT(PageToNext()));
    //connect(UpdatePage, SIGNAL(ToNext()), this, SLOT(PageToNext()));
    connect(CompletePage, SIGNAL(ToNext()), this, SLOT(PageToNext()));
    //connect(CompletePage, SIGNAL(ToNext()), this, SLOT(UpgradeComplete()));

    connect(IntroPage, SIGNAL(ToCancel()), this, SLOT(PageToHome()));
    connect(CheckPage, SIGNAL(ToCancel()), this, SLOT(PageToHome()));
    connect(DownloadPage, SIGNAL(ToCancel()), this, SLOT(PageToHome()));
    //connect(UpdatePage, SIGNAL(ToCancel()), this, SLOT(PageToHome()));
    connect(CompletePage, SIGNAL(ToCancel()), this, SLOT(PageToHome()));
    connect(FinishPage, SIGNAL(ToNext()), this, SLOT(FinishToHome()));

    TranslateUi();

    WizardLayout->setCurrentWidget(IntroPage);
    //WizardLayout->setCurrentIndex(4);

}

FirmwareUpdate::~FirmwareUpdate()
{
    //////2010-10-14 Jansion
//    NetgearProduct->CloseHttp();
//    ProductCheck->CloseHttp();
//    DownloadPage->CancelDownload();

    //NetgearProduct->DestructorHttp(); //2010-10-19
    ProductCheck->DestructorHttp();
    DownloadPage->DestructorDownload();
    //////

    //////Delete para
    delete ProductInfo;
    delete intErr;
    delete CheckInfo;
    delete intCheckErr;
    delete DownInfo;
    delete intDownloadErr;

    ////stop every page change Pic timer
    IntroPage->stopTimer();
    CheckPage->stopTimer();
    DownloadPage->stopTimer();
    CompletePage->stopTimer();
    FinishPage->stopTimer();

}

void FirmwareUpdate::PageToNext()
{
    int nextIndex = WizardLayout->currentIndex();
    nextIndex += 1;

    switch (nextIndex){
    case 1:
        {
            IntroPage->isCancelBtn(false);

            //////Get The NETGEAR's Firmware Version
            //ProductInfo->clear();
            //intErr->clear();
            //NetgearProduct->GetInfo(&ProductInfo, &intErr);
            GetInfoFromPlugin();


            //////Old Operation
            ////QString bbbb = "WNR2000_WW";
            //ProductCheck->CheckVersion(mRegionTag, &CheckInfo);

            //////User clicked upgrade,told server to const
            QString mOper = "R14";
            IClickManager *clickManager = FirmwareUpdate::sHandler->getClickManager();
            clickManager->click(mOper);
            //popWidget();

            CheckPage->setButtonEnabled(nextIndex);
            break;
        }
    case 2:
        {

            isDownloading = false;
            TurnToDownload();

//            mDownPara.clear();
//            QString mUrl = CheckInfo->value("url");
//            QString mMd5 = CheckInfo->value("md5");
//            QString mSize = CheckInfo->value("size");
////            QString mUrl = "http://updates1.netgear.com/wnr2000/ww/fileinfo.txt";
////            QString mMd5 = "7358cb7fd69da16f44eebdcbb0553be2";
////            QString mSize = "3129";
//
//            if (mUrl!=NULL && mMd5!=NULL && mSize!=NULL){
//                mDownPara.push_back(mUrl);
//                mDownPara.push_back(mSize);
//                mDownPara.push_back(mMd5);
//            }else {
//                mDownPara.push_back("http://updates1.netgear.com/wnr2000/ww/fileinfo.txt");
//                mDownPara.push_back("3129");
//                mDownPara.push_back("7358cb7fd69da16f44eebdcbb0553be2");
//            }
//
//            DownInfo = new QString;
//            DownInfo->clear();
//            qDebug() << "Downloading Firmware Version.";
//            DownloadPage->Download(mDownPara, &DownInfo);




//            //DownloadPage->setButtonEnabled(nextIndex);
//            DownloadPage->SetBackButton(false);
//            WizardLayout->setCurrentIndex(nextIndex);
            break;
        }
    case 3:
        {
            QString outString;
            outString = *DownInfo;
            //UpdatePage->SetInfo(outString);
            //UpdatePage->setButtonEnabled(nextIndex);
            CompletePage->SetInfo(outString);
            CompletePage->setButtonEnabled(nextIndex);

            WizardLayout->setCurrentIndex(nextIndex);
            break;
        }
    case 4:
        {
//            int intType;
//            intType = UpdatePage->isTypeCheck();
//            qDebug() << "Type Update: " << intType;
//
//            if (intType==0){
//
//                QUrl mUpdateUrl(UpdateUrl);
//                QDesktopServices::openUrl(mUpdateUrl);
//
//            }else if (intType==1){
//
//            }

//            //QString outString;
//            //outString = *DownInfo;
//            //FinishPage->SetInfo(outString);
//            QList<int> outInfo;
//            outInfo.push_back(1950);
//            //outInfo.push_back(*DownInfo);
//            FinishPage->SetInfo(outInfo);

            QUrl mUpdateUrl(UpdateUrl);
            QDesktopServices::openUrl(mUpdateUrl);

            QList<int> outInfo;
            outInfo.push_back(L_UPGRADE_SUCCESS_TITLE);
            outInfo.push_back(L_UPGRADE_COMPLETE_EXPLAIN);
            outInfo.push_back(L_UPGRADE_COMPLETE_PATH);
            //outInfo.push_back(L_UPGRADE_COMPLETE_EXPLAIN);
            QString tempPath;
            tempPath = *DownInfo;
            FinishPage->SetInfo(outInfo, tempPath);

            FinishPage->setButtonEnabled(nextIndex);
            WizardLayout->setCurrentIndex(nextIndex);





            break;

        }
    default:
        {
            WizardLayout->setCurrentIndex(0);
            break;
        }
    }

}

void FirmwareUpdate::PageToBack()
{
    int backIndex = WizardLayout->currentIndex();
    backIndex -= 1;

    switch (backIndex){
    case 3:
        {
            //UpdatePage->setButtonEnabled(backIndex);
            CompletePage->setButtonEnabled(backIndex);
            break;
        }
    case 2:
        {
            //DownloadPage->DownloadHttpClose();
            DownloadPage->setButtonEnabled(backIndex);
            break;
        }
    case 1:
        {
            qDebug() << "Need to close the Http.";
            DownloadPage->CancelDownload();
            CheckPage->setButtonEnabled(backIndex);
            break;
        }
    case 0:
        {
            qDebug() << "Get Product Info---Back Button";
            //IntroPage->ClearInfo();
            //NetgearProduct->GetInfo(&ProductInfo, &rError);

            IntroPage->setButtonEnabled(backIndex);
            break;
        }
    default:
        break;
    }

    WizardLayout->setCurrentIndex(backIndex);
}

void FirmwareUpdate::PageToHome()
{
    int intPageIndex = WizardLayout->currentIndex();
    QString strNum;
    GeniePlugin_FirmwareUpdate::writeGenieLog("Page number:" + strNum.setNum(intPageIndex));
    switch (intPageIndex){
    case 4:
        {
            //FinishPage->SetFinishButton();
            break;
        }
    case 3:
        {
            //CompletePage->SetFinishButton();
            break;
        }
    case 2:
        {
            DownloadPage->CancelDownload();
            isDownloading = true;

            //IntroPage->setButtonEnabled(0);
            //WizardLayout->setCurrentIndex(0);
            break;
        }
    case 0:
        {
            //NetgearProduct->CloseHttp(); //2010-10-19
            ProductCheck->CloseHttp();

            IntroPage->closeView();
            //IntroPage->isCancelBtn(true);
            //IntroPage->setButtonEnabled(0);
            //WizardLayout->setCurrentIndex(0);
            break;
        }
    default:
        {
            //IntroPage->setButtonEnabled(0);
            //WizardLayout->setCurrentIndex(0);
            break;
        }
    }

    FinishPage->SetFinishButton();
    CompletePage->SetFinishButton();

    ProductInfo->clear();
    intErr->clear();
    CheckInfo->clear();
    intCheckErr->clear();

    mProductInfo.clear();

    IntroPage->isCancelBtn(true);
    IntroPage->setButtonEnabled(0);
    WizardLayout->setCurrentIndex(0);

    //IUiManager *mUiPage = FirmwareUpdate::sHandler->getUiManager();
    //Q_ASSERT(mUiPage);
    //mUiPage->changeStackIdx(0);

}

QString FirmwareUpdate::getProductInfo(const QString &per)
{
//    ProductInfo->clear();
//    rError->clear();
//    NetgearProduct->GetInfo(&ProductInfo, &rError);

//    qDebug() << "Get Product Info: " << mProductInfo;
//    if (mProductInfo != NULL){
//        return mProductInfo;
//    }
//    return "Waiting";

    if (mProductInfo == "")
        return "?";
    else if (mProductInfo == "Failed")
        return "n";
    else
        return "y";

}

QString FirmwareUpdate::getUpgradeStat()
{
    //if (mRegionTag==NULL){
    if (mProductInfo == NULL){
        return "Waiting";
    }else {
        if (mProductInfo == "Failed"){
            return "Error";
        }else {

            if (CheckInfo->value("error")==NULL){
                if ( !isChecked ){
                    isChecked = true;
                    isUpgrade = false;
                    CheckInfo->clear();
                    intCheckErr->clear();
                    ProductCheck->CheckVersion(mRegionTag, &CheckInfo, &intCheckErr);
                }
                return "Waiting";

            }else {
                if (CheckInfo->value("error")=="NULL"){

                    if (!isUpgrade){
                        QString newVersion = CheckInfo->value("firmware");
                        QString oldVersion = ProductInfo->value("Firmware");
                        isStatus = CheckUpgrade(oldVersion, newVersion);

                        isUpgrade = true;
                    }

                    if ( isStatus ){
                        return "Warning";
                    }else {
                        return "Good";
                    }


                }else {
                    return "Good";
                }
            }

        }
    }
}

void FirmwareUpdate::SetNetgear()
{
//    QString mInfo = tr("Netgear Product: ") + ProductInfo->value("Model") + "\n";
//    mInfo = mInfo + tr("Firmware: ") + ProductInfo->value("Firmware") + "\n";
//
//    IntroPage->SetInfo(mInfo);
//
//    qDebug() << "Url: " << ProductInfo->value("RegionTag");
//    //mRegionTag = ProductInfo->value("RegionTag");
//    mRegionTag = ProductInfo->value("Model") + "_" + ProductInfo->value("Region");
//
//    QString sInfo = ProductInfo->value("Model") + ":" + ProductInfo->value("Firmware");
//    mProductInfo = sInfo;
//    //emit GetFinish(sInfo);
}

void FirmwareUpdate::NotNetgear()
{
//    QString mInfo = tr("Failed: ") + *rError;
//
//    IntroPage->SetInfo(mInfo);
//
//    QString sInfo = "Failed:" + *rError;
//    mProductInfo = sInfo;
//    //emit GetFinish(sInfo);

}

void FirmwareUpdate::FinishToHome()
{
    FinishPage->SetFinishButton();

    //IUiManager *mUiPage = FirmwareUpdate::sHandler->getUiManager();
    //Q_ASSERT(mUiPage);
    //mUiPage->changeStackIdx(0);
    QString mUiPara;
    mUiPara = "";
    //mUiPage->command(UI_CMD_SHOW_UPDATE_BUTTON, mUiPara);
    mUimanager->command(UI_CMD_HIDE_UPDATE_BUTTON, mUiPara);

    //////Update Netgear's Version
    QString mReturnInfo;
    mReturnInfo = mInfoManager->getInfo(INFO_CMD_NETGEAR_REFRESH);



    //isChecked = false;
    isUpgrade = false;

    IntroPage->ClearInfo();
    //NetgearProduct->GetInfo(&ProductInfo, &rError);

    //IntroPage->setButtonEnabled(0);
    QString strNetgear;
//    strNetgear = ProductInfo->value("Model");
//    if (strNetgear.isEmpty())
//        IntroPage->SetTitle(mCurrentModel, true);
//    else
//        IntroPage->SetTitle(strNetgear, true);
//    //IntroPage->SetTitle(mCurrentModel, true);

    strNetgear = mInfoManager->getInfo(INFO_CMD_ROUTE_TYPE_IMMEDIATE);
    IntroPage->SetTitle(strNetgear, true);


    mCurrentModel.clear();


    ProductInfo->clear();
    intErr->clear();
    CheckInfo->clear();
    intCheckErr->clear();

    mProductInfo.clear();

    //IntroPage->isCancelBtn(true);
    WizardLayout->setCurrentIndex(0);
}

void FirmwareUpdate::GetInfoFromPlugin()
{
    ProductInfo->clear();
    QString mCurrentSetting;
    mCurrentSetting = mInfoManager->getInfo(INFO_CMD_NETGEAR_CURRENTSET);
    qDebug() << "Currentsetting Information:" << mCurrentSetting;

    QStringList InfoArray = mCurrentSetting.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    QStringList TempArray;
    for (int i=0; i<InfoArray.size(); i++){
        TempArray.clear();
        TempArray = InfoArray[i].split("=");
        if (TempArray.size()>1){
            if (TempArray[0]=="Firmware"){
                //////Test
                //TempArray[1] = "A1.1.00.41_1.00.41NA";

                //////Modify The Version's Information
                //QRegExp vInfoMatch("(V\\d+(\\.\\d+){3}).*");
                QRegExp vInfoMatch("(V\\d+(\\.\\d+)*).*");
                int pos = vInfoMatch.indexIn(TempArray[1]);
                if (pos > -1){
                    ProductInfo->insert(TempArray[0], vInfoMatch.cap(1));
                    qDebug() << "Current Version Match: " << vInfoMatch.cap(1);
                }else{
                    ProductInfo->insert(TempArray[0], TempArray[1]);
                }
            }else {
                ProductInfo->insert(TempArray[0], TempArray[1]);
            }
        }
        //qDebug() << InfoArray.value(i) << "-->" << TempArray[1];
    }


    qDebug() << "Url: " << ProductInfo->value("RegionTag");
    //mRegionTag = ProductInfo->value("RegionTag");
    if (ProductInfo->value("Region").size()>2)
        mRegionTag = ProductInfo->value("Model") + "_ww";
    else
        mRegionTag = ProductInfo->value("Model") + "_" + ProductInfo->value("Region");
    mProductInfo = "Router Model " + ProductInfo->value("Model") + ":" + "Firmware Version " + ProductInfo->value("Firmware");

    qDebug() << "Current Netgear router Version: "
            << ProductInfo->value("Model")
            << "--" << ProductInfo->value("Firmware");

    CheckInfo->clear();
    intCheckErr->clear();
    ProductCheck->CheckVersion(mRegionTag, &CheckInfo, &intCheckErr);

}

void FirmwareUpdate::CheckResult(bool err)
{
    //IntroPage->ClearInfo();

    QString outString;
    if (err){

        qDebug() << "Check Version: " << CheckInfo->value("firmware");
        qDebug() << "Now Version: " << ProductInfo->value("Firmware");

        ////
        bool isUpdate;
        QString nowVersion, newVersion;
        nowVersion = ProductInfo->value("Firmware");
        newVersion = CheckInfo->value("firmware");
        //newVersion = "V1.2.3.11";
        isUpdate = CheckUpgrade(nowVersion, newVersion);
        if (isUpdate){
            QStringList tempInfo;
            tempInfo.push_back(ProductInfo->value("Model"));
            tempInfo.push_back(nowVersion);
            tempInfo.push_back(newVersion);
//            outString = tr("Router Model: ");
//            outString = outString + ProductInfo->value("Model") + "\n";
//            outString = outString + tr("Current Firmware Version: ");
//            outString = outString + nowVersion + "\n";
//            outString = outString + tr("Lastest Firmware Version: ");
//            outString = outString + newVersion + "\n";
//            CheckPage->SetInfo(outString);
            CheckPage->SetInfo(tempInfo);
            WizardLayout->setCurrentIndex(1);

        }else {
            //outString = tr("Current Firmware Version is abreast of the times.");
            //FinishPage->SetInfo(outString);
            QList<int> outInfo;
            outInfo.push_back(L_UPGRADE_FINISH_TITLE);
            outInfo.push_back(L_UPGRADE_FINISH_EXPLAIN);
            outInfo.push_back(L_UPGRADE_CHECK_ISLASTEST);
            FinishPage->SetInfo(outInfo, "NULL");
            FinishPage->setButtonEnabled(4);
            WizardLayout->setCurrentIndex(4);
        }
//        outString = tr("Router Model: ");
//        outString = outString + ProductInfo->value("Model") + "\n";
//        outString = outString + tr("Current Firmware Version: ");
//        outString = outString + ProductInfo->value("Firmware") + "\n";
//        outString = outString + tr("Lastest Firmware Version: ");
//        outString = outString + CheckInfo->value("firmware") + "\n";
//        CheckPage->SetInfo(outString);
//        WizardLayout->setCurrentIndex(1);


//        bool flagUpdate = false;
//
////        QString newVersion = "V1.2.3.8";
////        QString nowVersion = "V1.2.3.7";
//        QString newVersion = CheckInfo->value("firmware");
//        QString nowVersion = ProductInfo->value("Firmware");
//        //QString nowVersion = CheckInfo->value("firmware");
//
//        qDebug() << "Check Version: " << CheckInfo->value("firmware");
//        qDebug() << "Now Version: " << ProductInfo->value("Firmware");
//
//        QStringList newVersionNum = newVersion.split(QRegExp("\\."), QString::SkipEmptyParts);
//        QStringList nowVersionNum = nowVersion.split(QRegExp("\\."), QString::SkipEmptyParts);
//
//        int newNum;
//        int nowNum;
//        QString strNum;
//        bool oknew;
//        bool oknow;
//        for (int i=0; i<newVersionNum.size(); i++){
//
//            if (i==0){
//                QRegExp vNumMatch("^V(\\d+)$");
//                int newpos = vNumMatch.indexIn(newVersionNum[i]);
//                if (newpos > -1){
//                    strNum = vNumMatch.cap(1);
//                    newNum = strNum.toInt(&oknew, 10);
//                }
//                int nowpos = vNumMatch.indexIn(nowVersionNum[i]);
//                if (nowpos > -1){
//                    strNum = vNumMatch.cap(1);
//                    nowNum = strNum.toInt(&oknow, 10);
//                }
//
//                if (oknew && oknow){
//                    if (newNum>nowNum)
//                        flagUpdate = true;
//
//                    qDebug() << "Version Int: " << newNum << "-" << nowNum;
//                }
//            }else {
//                strNum = newVersionNum[i];
//                newNum = strNum.toInt(&oknew, 10);
//                strNum = nowVersionNum[i];
//                nowNum = strNum.toInt(&oknow, 10);
//
//                if (oknew && oknow){
//                    if (newNum>nowNum)
//                        flagUpdate = true;
//
//                    qDebug() << "Version Int: " << newNum << "-" << nowNum;
//                }
//            }
//
//        }
//
//        if (flagUpdate){
//            outString = tr("Current Firmware Version: ");
//            outString = outString + nowVersion + "\n";
//            outString = outString + tr("Lastest Firmware Version: ");
//            outString = outString + newVersion + "\n";
//            CheckPage->SetInfo(outString);
//            WizardLayout->setCurrentIndex(2);
//        }else {
//            outString = tr("Current Firmware Version is abreast of the times.");
//            FinishPage->SetInfo(outString);
//            FinishPage->setButtonEnabled(4);
//            WizardLayout->setCurrentIndex(4);
//        }


    }else {
        //outString = tr("Downloading is error.");

//        outString = tr("Router Model: ");
//        outString = outString + ProductInfo->value("Model") + "\n";
//        outString = outString + tr("Current Firmware Version: ");
//        outString = outString + ProductInfo->value("Firmware") + "\n";
//        outString = outString + tr("Lastest Firmware Version: ");
//        outString = outString + CheckInfo->value("error");
//        CheckPage->SetInfo(outString);
//        WizardLayout->setCurrentIndex(1);

        //outString = tr("Error: ");
        //outString = outString + CheckInfo->value("error");
        //FinishPage->SetInfo(outString);
        QList<int> outInfo;
        outInfo.push_back(L_UPGRADE_FINISH_TITLE);
        outInfo.push_back(L_UPGRADE_FINISH_EXPLAIN);
        //outInfo.push_back(L_UPGREAE_MAIN_ERROR);
        for(int i=0; i<intCheckErr->size(); i++){
            outInfo.push_back(intCheckErr->value(i));
        }
        FinishPage->SetInfo(outInfo, "NULL");
        FinishPage->setButtonEnabled(4);
        WizardLayout->setCurrentIndex(4);



//        FinishPage->SetInfo(outString);
//        FinishPage->setButtonEnabled(4);
//
//        WizardLayout->setCurrentIndex(4);
//        //Test-Jansion
//        //WizardLayout->setCurrentIndex(0);
    }

}

void FirmwareUpdate::DownloadOver(bool err)
{
    isFrameTurn = false;

    //QString outString;
    if (err){
//        popWidget();
//
//        //outString = tr("The Lastest Firmware's Version was Downloaded.");
//        //DownloadPage->SetInfo(outString);
//        QList<int> outInfo;
//        outInfo.push_back(L_UPGRADE_DOWNLOAD_FINISHED);
//        DownloadPage->SetInfo(outInfo);
//
//        //WizardLayout->setCurrentIndex(2);
//        //WizardLayout->setCurrentWidget(DownloadPage);

        popWidget();

        //outString = tr("The Lastest Firmware's Version was Downloaded.");
        //DownloadPage->SetInfo(outString);
        QList<int> outInfo;
        //outInfo.push_back(L_UPGRADE_DOWNLOAD_FINISHED);
        outInfo.push_back(L_UPGRADE_DOWNLOAD_POPEXPLAIN);
        DownloadPage->SetInfo(outInfo);

        QString strPath;
        strPath = *DownInfo;
        GeniePlugin_FirmwareUpdate::writeGenieLog("Firmware save Path:" + strPath);
        CompletePage->SetInfo(strPath);
        CompletePage->setButtonEnabled(3);
        WizardLayout->setCurrentIndex(3);
        //WizardLayout->setCurrentWidget(DownloadPage);

    }else {
        //outString = tr("The Firmware Version is worng.");
        //outString = *DownInfo;
        //FinishPage->SetInfo(outString);
        QList<int> outInfo;
        outInfo.push_back(L_UPGRADE_FINISH_TITLE);
        outInfo.push_back(L_UPGRADE_FINISH_EXPLAIN);
        for(int i=0; i<intDownloadErr->size(); i++){
            outInfo.push_back(intDownloadErr->value(i));
        }
        FinishPage->SetInfo(outInfo, "NULL");
        FinishPage->setButtonEnabled(4);
        //WizardLayout->setCurrentWidget(FinishPage);
        WizardLayout->setCurrentIndex(4);
    }

    isDownloading = true;

}

bool FirmwareUpdate::CheckUpgrade(QString mOld, QString mNew)
{
    qDebug() << "Old Version: " << mOld;
    qDebug() << "New Version: " << mNew;
    if ( mNew == "NULL" )
        return false;

    //////remove "V" form version
    //QRegExp vNumMatch("^V(\\d+.*\\d+)$");
    QRegExp vNumMatch("^\\D+(\\d+(\\.\\d+)*).*");
    int verpos = vNumMatch.indexIn(mOld);
    if (verpos > -1){
        mOld = vNumMatch.cap(1);
    }
    verpos = vNumMatch.indexIn(mNew);
    if (verpos > -1){
        mNew = vNumMatch.cap(1);
    }

    QStringList newVersionNum = mNew.split(QRegExp("\\."), QString::SkipEmptyParts);
    QStringList nowVersionNum = mOld.split(QRegExp("\\."), QString::SkipEmptyParts);


    int newNum;
    int nowNum;
    QString strNum;
    bool oknew;
    bool oknow;
    int complenght;
    int nowVerL = nowVersionNum.size();
    int newVerL = newVersionNum.size();
    //complenght = newVersionNum.size()<nowVersionNum.size() ? newVersionNum.size() : nowVersionNum.size();
    if ( newVerL<nowVerL ){
        complenght = newVerL;

        for (int k=nowVerL-newVerL; k>0; k--){
            if (!nowVersionNum.isEmpty())
                nowVersionNum.pop_front();
        }
    }else {
        complenght = nowVerL;

        for (int k=newVerL-nowVerL; k>0; k--){
            if (!newVersionNum.isEmpty())
                newVersionNum.pop_front();
        }
    }
    for (int i=0; i<complenght; i++){

        //////2010-10-25 jansion
//        if (i==0){
//            QRegExp vNumMatch("^V(\\d+)$");
//            int newpos = vNumMatch.indexIn(newVersionNum[i]);
//            if (newpos > -1){
//                strNum = vNumMatch.cap(1);
//                newNum = strNum.toInt(&oknew, 10);
//            }
//            int nowpos = vNumMatch.indexIn(nowVersionNum[i]);
//            if (nowpos > -1){
//                strNum = vNumMatch.cap(1);
//                nowNum = strNum.toInt(&oknow, 10);
//            }
//
//            if (oknew && oknow){
//                if (newNum>nowNum)
//                    return true;
//            }
//        }else {
//            strNum = newVersionNum[i];
//            newNum = strNum.toInt(&oknew, 10);
//            strNum = nowVersionNum[i];
//            nowNum = strNum.toInt(&oknow, 10);
//
//            if (oknew && oknow){
//                if (newNum>nowNum)
//                    return true;
//            }
//        }
        //////




        strNum = newVersionNum[i];
        newNum = strNum.toInt(&oknew, 10);
        strNum = nowVersionNum[i];
        nowNum = strNum.toInt(&oknow, 10);

        if (oknew && oknow){
            if (newNum>nowNum){
                qDebug() << "need to upgared firmware to:" << mNew;
                GeniePlugin_FirmwareUpdate::writeGenieLog("Need upgared firmware to:" + mNew);
                return true;
            }
        }

    }

    return false;

}

void FirmwareUpdate::TurnToDownload()
{

    if (!isFrameTurn){
        mDownPara.clear();
        QString mUrl = CheckInfo->value("url");
        QString mMd5 = CheckInfo->value("md5");
        QString mSize = CheckInfo->value("size");
        //QString mUrl = "http://updates1.netgear.com/wnr2000/ww/fileinfo.txt";
        //QString mMd5 = "7358cb7fd69da16f44eebdcbb0553be2";
        //QString mSize = "3129";

        if (mUrl!=NULL && mMd5!=NULL && mSize!=NULL){
            mDownPara.push_back(mUrl);
            mDownPara.push_back(mSize);
            mDownPara.push_back(mMd5);
        }else {
            mDownPara.push_back("http://updates1.netgear.com/wnr2000/ww/fileinfo.txt");
            mDownPara.push_back("3129");
            mDownPara.push_back("7358cb7fd69da16f44eebdcbb0553be2");
        }
    }

    //DownInfo = new QString;
    DownInfo->clear();
    intDownloadErr->clear();
    qDebug() << "Downloading Firmware Version.";
    //DownloadPage->Download(mDownPara, &DownInfo);
    DownloadPage->Download(mDownPara, &DownInfo, &intDownloadErr);

    //DownloadPage->setButtonEnabled(nextIndex);
    //DownloadPage->SetBackButton(false);
    WizardLayout->setCurrentIndex(2);






//
//    QString outString;
//
//    if (CheckInfo->value("error")=="NULL"){
//        bool flagUpdate = false;
//        //QString newVersion = "V1.2.3.8";
//        //QString nowVersion = "V1.2.3.7";
//        QString newVersion = CheckInfo->value("firmware");
//        QString nowVersion = ProductInfo->value("Firmware");
//
//        flagUpdate = CheckUpgrade(nowVersion, newVersion);
//
//        if (flagUpdate){
//
//            mDownPara.clear();
//            QString mUrl = CheckInfo->value("url");
//            QString mMd5 = CheckInfo->value("md5");
//            QString mSize = CheckInfo->value("size");
////            QString mUrl = "http://updates1.netgear.com/wnr2000/ww/fileinfo.txt";
////            QString mMd5 = "7358cb7fd69da16f44eebdcbb0553be2";
////            QString mSize = "3129";
//
//            if (mUrl!=NULL && mMd5!=NULL && mSize!=NULL){
//                mDownPara.push_back(mUrl);
//                mDownPara.push_back(mSize);
//                mDownPara.push_back(mMd5);
//            }else {
//                mDownPara.push_back("http://updates1.netgear.com/wnr2000/ww/fileinfo.txt");
//                mDownPara.push_back("3129");
//                mDownPara.push_back("7358cb7fd69da16f44eebdcbb0553be2");
//            }
//
//            //DownInfo = new QString;
//            DownInfo->clear();
//            qDebug() << "Downloading Firmware Version.";
//            DownloadPage->Download(mDownPara, &DownInfo);
//
//            //DownloadPage->setButtonEnabled(nextIndex);
//            DownloadPage->SetBackButton(false);
//            WizardLayout->setCurrentIndex(2);
//
//        }else {
//            outString = tr("Current Firmware Version is abreast of the times.");
//            FinishPage->SetInfo(outString);
//            FinishPage->setButtonEnabled(4);
//            WizardLayout->setCurrentIndex(4);
//
//        }
//
//    }else {
//
//        outString = CheckInfo->value("error");
//        FinishPage->SetInfo(outString);
//        FinishPage->setButtonEnabled(4);
//
//        WizardLayout->setCurrentIndex(4);
//        //Test-Jansion
//        //WizardLayout->setCurrentIndex(0);
//    }

}

void FirmwareUpdate::popWidget()
{
    QString strTitle;
    QString strExplan;
    //QSystemTrayIcon::MessageIcon iconTmp;

    ILanguageManager *lanmanager = FirmwareUpdate::sHandler->getLanguageManager();
    Q_ASSERT(lanmanager);
    strTitle = "";
    //strExplan = "The firmware download is complete.";
    strExplan = lanmanager->getText(L_UPGRADE_DOWNLOAD_POPEXPLAIN, PLUGIN_ID);

    ITrayManager *mTrayManager = FirmwareUpdate::sHandler->getTrayManager();
    Q_ASSERT(mTrayManager);
    mTrayManager->setMessage(strTitle, strExplan, QSystemTrayIcon::NoIcon);
}

void FirmwareUpdate::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);

    switch (e->type()){
    case QEvent::LanguageChange:
        TranslateUi();
        break;
    default:
        break;
    }

}

void FirmwareUpdate::TranslateUi()
{
//    ILanguageManager *mLanmanager = sHandler->getLanguageManager();

//    QString strOk = mLanmanager->getText(L_UPGRADE_MAIN_CLICK_HERE, PLUGIN_ID);
//    QString style=QString(BBT_MODE_CLICKHERE).arg(strOk);
//    mUimanager->setBigButtonUserStateUi(style, PLUGIN_ID);

    ILanguageManager *mLanmanager = sHandler->getLanguageManager();

    QString strOk = mLanmanager->getText(L_UPGRADE_MAIN_CLICK_HERE, PLUGIN_ID);
    QString style=QString(BBT_MODE_CLICKHERE).arg(strOk);
    mUimanager->setBigButtonUserStateText(style, PLUGIN_ID);

}

void FirmwareUpdate::changePagePic()
{
    IntroPage->ReloadFlash();
    CheckPage->ReloadFlash();
    DownloadPage->ReloadFlash();
    CompletePage->ReloadFlash();
    FinishPage->ReloadFlash();
}

void FirmwareUpdate::changePagePic(QString &mpic)
{
    QString ImagePath;
    ImagePath = mpic;
    qDebug() << "Change pictrue path: " << ImagePath;
    //ImagePath = ":/images/firmware.png";

    ////explain: QPixmap(":/images/firmware.png")
//    IntroPage->l_Image->setPixmap(QPixmap(ImagePath));
//    CheckPage->l_Image->setPixmap(QPixmap(ImagePath));
//    DownloadPage->l_Image->setPixmap(QPixmap(ImagePath));
//    CompletePage->l_Image->setPixmap(QPixmap(ImagePath));
//    FinishPage->l_Image->setPixmap(QPixmap(ImagePath));

}

void FirmwareUpdate::changePagePic(QString &pathA, QString &pathB)
{
    qDebug() << "Change pictrue path: " << pathA << pathB;
    IntroPage->setImageView(pathA, pathB);
    CheckPage->setImageView(pathA, pathB);
    DownloadPage->setImageView(pathA, pathB);
    CompletePage->setImageView(pathA, pathB);
    FinishPage->setImageView(pathA, pathB);
}

void FirmwareUpdate::turnFirstPage()
{
    //////hide firmware update button
    //IUiManager *mUiPage = FirmwareUpdate::sHandler->getUiManager();
    //Q_ASSERT(mUiPage);
    //QString mUiPara;
    //mUiPara = "";
    //mUimanager->command(UI_CMD_HIDE_UPDATE_BUTTON, mUiPara);

    //////close all Http request
    //NetgearProduct->CloseHttp(); //2010-10-19
    ProductCheck->CloseHttp();
    DownloadPage->CancelDownload();

    IntroPage->closeView();

    //isChecked = false;
    isUpgrade = false;
    ProductInfo->clear();
    mProductInfo.clear();


    //IntroPage->ClearInfo();
    IntroPage->setButtonEnabled(0);
    WizardLayout->setCurrentIndex(0);

}

void FirmwareUpdate::viewGetNewTimeout(int stat)
{
    //IntroPage->SetInfo("Get lastest firmware request is time out, try again.");
    if ( stat>0 )
        IntroPage->SetInfo(L_UPGRADE_INTRO_LASTESTTIMEOUT);
    else {
        QList<int> eInfoList;
        eInfoList.push_back(L_UPGRADE_FINISH_TITLE);
        eInfoList.push_back(L_UPGRADE_FINISH_EXPLAIN);
        eInfoList.push_back(L_UPGRADE_INTRO_LASTESTTIMEOUT_ERROR);
        FinishPage->SetInfo(eInfoList, "NULL");

        IntroPage->closeView();
        WizardLayout->setCurrentIndex(4);
    }
}

void FirmwareUpdate::setFirstPageTitle(QString &type)
{
    IntroPage->SetTitle(type, isIntroNextBtn);
    isIntroNextBtn = true;
    //if ( !(type==ProductInfo->value("Model")) )
    //    IntroPage->SetTitle(type);

    turnPagebyChangeModel();
}

void FirmwareUpdate::turnPagebyChangeModel()
{
    int intPageIndex = WizardLayout->currentIndex();
    switch (intPageIndex){
    case 2:
        {
            DownloadPage->CancelDownload();
            isDownloading = true;
            break;
        }
    case 0:
        {
            ProductCheck->CloseHttp();
            IntroPage->closeView();
            break;
        }
    default:
        {
            break;
        }
    }

    ProductInfo->clear();
    intErr->clear();
    CheckInfo->clear();
    intCheckErr->clear();

    mProductInfo.clear();

    //IntroPage->isCancelBtn(true);
    //IntroPage->setButtonEnabled(0);
    WizardLayout->setCurrentIndex(0);
}


void FirmwareUpdate::turnUpgradePage(QString model, QString twoVer, QString url, QString twoComp)
{

    if (isDownloading){
        qDebug() << "Turn to upgrade page: " << model
                << "-->" << twoVer
                << "-->" << url
                << "-->" << twoComp;

        QStringList mArrayVersion;
        mArrayVersion = twoVer.split(":");
        mCurrentModel = mArrayVersion[0];
    //    QString oVersion;
    //    QString nVersion;
    //    oVersion = mArrayVersion[0];
    //    nVersion = mArrayVersion[1];
    //    QStringList tempInfo;
    //    tempInfo.push_back(model);
    //    tempInfo.push_back(oVersion);
    //    tempInfo.push_back(nVersion);
    //    CheckPage->SetInfo(tempInfo);

        isFrameTurn = true;
        mDownPara.clear();
        mArrayVersion.clear();
        mArrayVersion = twoComp.split(":");
        QString fileSize;
        QString strMd5;
        if (mArrayVersion.size()>1){
            fileSize = mArrayVersion[0];
            strMd5 = mArrayVersion[1];
        }
        mDownPara.push_back(url);
        mDownPara.push_back(fileSize);
        mDownPara.push_back(strMd5);

        //////Turn to checked page
        //CheckPage->setButtonEnabled(1);
        //WizardLayout->setCurrentIndex(1);

        //////Turn to downloading page
        //DownloadPage->CancelDownload();

        isDownloading = false;
        TurnToDownload();
    }

}

void FirmwareUpdate::UpgradeComplete()
{
    QUrl mUpdateUrl(UpdateUrl);
    QDesktopServices::openUrl(mUpdateUrl);


    //IUiManager *mUiPage = FirmwareUpdate::sHandler->getUiManager();
    //Q_ASSERT(mUiPage);
    //mUiPage->changeStackIdx(0);
    QString mUiPara;
    mUiPara = "";
    //mUiPage->command(UI_CMD_SHOW_UPDATE_BUTTON, mUiPara);
    mUimanager->command(UI_CMD_HIDE_UPDATE_BUTTON, mUiPara);


    //isChecked = false;
    isUpgrade = false;

    IntroPage->ClearInfo();
    //NetgearProduct->GetInfo(&ProductInfo, &rError);

    //IntroPage->setButtonEnabled(0);

    //IntroPage->SetTitle(mCurrentModel, true);
    QString strNetgear;
//    strNetgear = ProductInfo->value("Model");
//    if (strNetgear.isEmpty())
//        IntroPage->SetTitle(mCurrentModel, true);
//    else
//        IntroPage->SetTitle(strNetgear, true);

    strNetgear = mInfoManager->getInfo(INFO_CMD_ROUTE_TYPE_IMMEDIATE);
    IntroPage->SetTitle(strNetgear, true);


    mCurrentModel.clear();

    ProductInfo->clear();
    intErr->clear();
    CheckInfo->clear();
    intCheckErr->clear();

    mProductInfo.clear();

    WizardLayout->setCurrentIndex(0);

}


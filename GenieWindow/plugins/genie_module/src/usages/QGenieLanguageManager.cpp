
#include "QGenieLanguageManager.h"
#define FRAME_LANGUAGE_FILE "LanguageMainFrame.txt"
//#ifdef _MACOS_
//#define LAN_DIR "./i18n"
//#else
#ifdef MIX_RESOURCE
#define LAN_DIR ":/i18n"
#else
#define LAN_DIR (GENIE_PATH+"../i18n")
#endif
//#endif
const char * cTypeStr[ELT_NUM]={
    "enu",
    "deu",
    "sve",
    "fra",
    "nld",
    "ita",
    "esp",
    "dan",
    "fin",
    "nor",
    "ell",
    "ptb",
    "rus",
    "plk",
    "hun",
    "csy",
    "slv",
    "sky",
    "rom",
    //  "bgr",
    //  "hrv",
    "chs",
    "kor",
    // "jpn",
    "ara"

};

const char * cQmStr[ELT_NUM]={
    "",
    ":/qt_de",
    ":/qt_sv",
    ":/qt_fr",
    "",//Nederlands
    "",//Italiano
    ":/qt_es",
    ":/qt_da",
    "",//suomi
    "",//Norwegian
    "",//Greece
    ":/qt_pt",
    ":/qt_ru",
    ":/qt_pl",
    ":/qt_hu",
    ":/qt_cs",
    ":/qt_sl",
    ":/qt_sk",
    "",//rom
    //  "bgr",
    //  "hrv",
    ":/qt_zh_CN",
    "",//korea
    // "jpn",
    ":/qt_ar"

};

#define DEFAULT_QM_FILE ""
/*
#if 1
const char * cLanguageDir[]={
"../i18n/enu",
"../i18n/deu",
"../i18n/sve",
"../i18n/fra",
"../i18n/nld",
"../i18n/ita",
"../i18n/esp",
"../i18n/dan",
"../i18n/fin",
"../i18n/nor",
"../i18n/ell",
"../i18n/ptb",
"../i18n/rus",
"../i18n/plk",
"../i18n/hun",
"../i18n/csy",
"../i18n/slv",
"../i18n/sky",
"../i18n/rom",
//"../i18n/bgr",
//"../i18n/hrv",
"../i18n/chs",
"../i18n/kor",
//"../i18n/jpn",
"../i18n/ara"

};
#else
const char * cLanguageDir[]={
    ":/english",
    ":/chinese",
    ":/germany",
    ":/spanish",
    ":/portuguese",
};

#endif
*/

QGenieLanguageManager::QGenieLanguageManager()
    :mCurrentType(ELT_ERROR)
    ,mInnerTextManager(new QGenieInerTextManager())
{
    mTranslator=new QTranslator();
    initDefaultMap();

}

QGenieLanguageManager::~QGenieLanguageManager()
{
    delete mTranslator;
    delete mInnerTextManager;
}

void QGenieLanguageManager::initDefaultMap()
{
    int syslan=getSysLanIdx();
    prepareMap(FRAME_LANGUAGE_FILE,mStrMap,syslan);
}


void QGenieLanguageManager::prepareMap(QString filename,QMap<int,QString> & result,int lantype)
{
    result.clear();
    //    QDir dir(qApp->applicationDirPath());
    //    if(!dir.cd(cLanguageDir[(int)mCurrentType]))
    //    {
    //        qDebug()<<"wrong dir name when change language!!";
    //        return;
    //    }
    if(lantype ==ELT_ERROR)
    {
        return;
    }
    ASSERT(lantype>=ELT_ENGLISH && lantype<ELT_NUM);


    QString fname= QString("%1/%2/%3").arg(LAN_DIR,cTypeStr[lantype],filename);

    QFile file(fname);
    qDebug()<<file.fileName();
    //  QFile file(":/LanguageFeedback.txt");
    if(file.open(QFile::ReadOnly))
    {
        QTextStream stream(&file);
        QRegExp rxBase("(#+)(.*)(#+)");

        QRegExp rxline("(.*)(\\s*)=(\\s*)\"(.*)\"");
        QString line, value;

        if(stream.atEnd())
        {
            qDebug() << "file have no data";
            return;
        }

        line=stream.readLine().trimmed();
        int key=0;
        if(rxBase.exactMatch(line))
        {
            bool btemp;
            key=line.replace("#","").trimmed().toInt(&btemp);
            if(!btemp)
            {
                qDebug()<<"line base is wrong 2";
                key=0;
            }

        }
        else
        {
            qDebug()<<"line base is wrong 1";
        }


        while (!stream.atEnd())
        {
            line = stream.readLine().trimmed();// line of text excluding '\n'
            int pos = rxline.indexIn(line);

            if (pos > -1)
            {
                //     QString key = rxline.cap(1);
                //                    value = rxline.cap(0);
                //                    value = rxline.cap(1);
                //                    value = rxline.cap(2);
                //                    value = rxline.cap(3);
                value = rxline.cap(4);
                value =value.replace("\\n","\n");
                result.insert(key, value);
                key++;
            }
            else
            {
                qDebug("\033[32mQRegExp Error!");
                continue;
            }
        }
        file.close();
    }
    else
    {
        GENIE_LOG(QString("ERROR can not find language file ")+file.fileName());
    }

    qDebug()<<"ERROR load language file end "<<file.fileName();
    dumpMap(result);
}

void QGenieLanguageManager::setLanguage(E_LANGUAGE_TYPE type,bool withmessage)
{


    //  prepareMap(type);
    mCurrentType=type;

    prepareMap(FRAME_LANGUAGE_FILE,mStrMap,type);

    if(withmessage)
    {
        //      qApp->removeTranslator(mTranslator);
        //     SAFE_DELETE(mTranslator);
        //    mTranslator=new QTranslator();
        QString qmname=(0<=mCurrentType&&mCurrentType<ELT_NUM)?cQmStr[mCurrentType]:"";

        if(!QFile::exists(qmname+".qm"))
        {
            qmname=DEFAULT_QM_FILE;
        }

        mTranslator->load(qmname);
        qApp->installTranslator(mTranslator);

        //setLayoutDirection(type);


        QTimer::singleShot(1,this,SLOT(slot_setLayoutDirWithCurType()));
    }

}

void QGenieLanguageManager::dumpMap(const QMap<int ,QString> & map)
{
#if 0
    QMap<int, QString>::const_iterator i = map.begin();
    while (i != map.end()) {
        qDebug()<<i.key()<<"="<<i.value();
        ++i;
    }
#endif
}

int QGenieLanguageManager::getSysLanIdx() const
{
    int curlan=ELT_ENGLISH;
    // QString lan=QLocale::system().name();
    // QString lan=QLocale::system().languageToString(QLocale::system().language()).toLower();
    //  int lenstr=lan.size()-2;
    // if(lenstr >0)
    //  {
    //    lan=lan.replace(2,lenstr,"");
    // }
    // qDebug()<<"Change Language when init "<< lan;

    QLocale::Language lan=QLocale::system().language();


    struct _lanmap{QLocale::Language s;int idx;};
    _lanmap lanmap[]={
        {QLocale::English,ELT_ENGLISH},
        {QLocale::German,ELT_GERMANY},
        {QLocale::Swedish,ELT_SWEDISH},
        {QLocale::French,ELT_FRENCH},
        {QLocale::Dutch,ELT_DUTCH},
        {QLocale::Italian,ELT_ITALIAN},
        {QLocale::Spanish,ELT_SPANISH},
        {QLocale::Danish,ELT_DANISH},
        {QLocale::Finnish,ELT_FINNISH},
        {QLocale::Norwegian,ELT_NORWEGIAN},
        {QLocale::Greek,ELT_GREEK},
        {QLocale::Portuguese,ELT_PORTUGUESE},
        {QLocale::Russian,ELT_RUSSIAN},
        {QLocale::Polish,ELT_POLISH},
        {QLocale::Hungarian,ELT_HUNGARIAN},
        {QLocale::Czech,ELT_CZECH},
        {QLocale::Slovenian,ELT_SLOVENIAN},
        {QLocale::Slovak,ELT_SLOVAK},
        {QLocale::Romanian,ELT_ROMANIAN},
        //         {QLocale::Bulgarian,ELT_BULGARIAN},
        //  {QLocale::Croatian,ELT_CROATIAN},
        {QLocale::Chinese,ELT_CHINESE},
        {QLocale::Korean,ELT_KOREAN},
        //   {QLocale::Japanese,ELT_JAPANESE},
        {QLocale::Arabic,ELT_ARABIC}
    };
    int mapsize=sizeof(lanmap)/sizeof(lanmap[0]);
    QMap<QLocale::Language,int> tempmap;
    for(int i=0;i<mapsize;i++)
    {
        tempmap.insert(lanmap[i].s,lanmap[i].idx);
    }

    curlan=tempmap.value(lan);
    if(!(curlan >=0 && curlan <ELT_NUM))
    {
        curlan=ELT_ENGLISH;
    }
    return curlan;
}

void QGenieLanguageManager::initLanguageSetting_step1()
{
    int curlan=getSysLanIdx();

    //  lanmanager->prepareMap(ELT_ENGLISH);
    setLayoutDirection(curlan);
    this->setLanguage((E_LANGUAGE_TYPE)curlan,false);
    //  lanmanager->setLanguage((E_LANGUAGE_TYPE)curlan);
    QGenieFrameTitle *title=QGenieFrameTitle::GetSingletonPtr();
    ASSERT(title);
    title->mUi.cob_sl_lan->setCurrentIndex(curlan);
}
void QGenieLanguageManager::initLanguageSetting_step2()
{
    //	qApp->removeTranslator(mTranslator);
    //	SAFE_DELETE(mTranslator);
    //	mTranslator=new QTranslator();
    QString qmname=(0<=mCurrentType&&mCurrentType<ELT_NUM)?cQmStr[mCurrentType]:"";
    if(!QFile::exists(qmname+".qm"))
    {
        qmname=DEFAULT_QM_FILE;
    }

    mTranslator->load(qmname);
    qApp->installTranslator(mTranslator);


}

void QGenieLanguageManager::setLayoutDirection(int type)
{
    GENIE_LOG(QString("setLayoutDirection (%1)").arg(mCurrentType));
    ASSERT(type>=ELT_ENGLISH && type<ELT_NUM);
    QGenieTrayManager *tray=QGenieTrayManager::GetSingletonPtr();
    Qt::LayoutDirection para=(type == ELT_ARABIC)?(Qt::RightToLeft):(Qt::LeftToRight);
    qApp->setLayoutDirection(para);
    tray->setLayoutDirection(para);
}
void QGenieLanguageManager::slot_setLayoutDirWithCurType()
{
    setLayoutDirection(mCurrentType);
}

void QGenieLanguageManager::slot_setLanguage(int idx)
{
    qDebug("QGenieLanguageManager::slot_setLanguage(%d)",idx);
    //  ASSERT(0<=idx && idx <ELT_NUM);
    QGenieFrameTitle *title=QGenieFrameTitle::GetSingletonPtr();
    ASSERT(title);





    static bool inchanging=false;  //use this to fix bug(the signal loop can not return)
    if(idx <0 )
    {
        inchanging=true;
    }
    else if(inchanging)
    {
        inchanging=false;
    }
    else
    {
        if(ELT_ENGLISH<=idx && idx <ELT_NUM)
        {
            setLanguage((E_LANGUAGE_TYPE)idx);
        }
    }
}



QString QGenieLanguageManager::getText(int idx,QUuid uuid)
{
    QString ret;//=QString("[%1]error").arg(QString::number(idx));
    static QUuid sFrameUuid(FRAME_UUID);
    if(uuid==sFrameUuid)
    {
        ret=mStrMap.value(idx,ret);
        return ret;
    }

    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);
    plug_inf *p=proxymanager->getPluginInf(uuid);
    if(p && p->pi)
    {
        E_LANGUAGE_TYPE type=mTypeCache.value(uuid,ELT_ERROR);
        if(type==ELT_ERROR)
        {
            int itemp=getSysLanIdx();
            if(itemp >=0 && itemp <ELT_NUM)
            {
                type=E_LANGUAGE_TYPE(itemp);
                QString filename=p->pi->getInfo(PLUGIN_INF_LANGUAGE_FILE);
                qDebug()<<"language file :"<<filename;
                prepareMap(filename,p->lanTexts,type);
                mTypeCache.insert(uuid,type);
            }
        }


        if(type!=mCurrentType && mCurrentType != ELT_ERROR)
        {
            QString filename=p->pi->getInfo(PLUGIN_INF_LANGUAGE_FILE);
            qDebug()<<"language file :"<<filename;
            prepareMap(filename,p->lanTexts,mCurrentType);
            mTypeCache.insert(uuid,mCurrentType);
        }


        ret=p->lanTexts.value(idx,ret);
    }
    if(!p)
    {
        bool btemp;
        QGenieInerTextManager *innertextmanager=QGenieInerTextManager::GetSingletonPtr();
        ASSERT(innertextmanager);
        ret =innertextmanager->getText(idx,uuid,&btemp);
    }
    //qDebug()<<ret;
    return ret;
}

void QGenieLanguageManager::unloadPi(IPluginInf *pi)
{
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);
    plug_inf *p=proxymanager->getPluginInf(pi);
    ASSERT(p);
    p->lanTexts.clear();
    mTypeCache.insert(p->uuid,ELT_ERROR);
}

QString QGenieLanguageManager::getTypeStr()
{

    if(mCurrentType>=ELT_ENGLISH && mCurrentType<ELT_NUM)
    {
        return cTypeStr[mCurrentType];
    }
    return "ww";
}

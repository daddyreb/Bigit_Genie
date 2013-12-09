#include "stable.h"
static const char * cTypeStr[ELT_NUM]={
    "enu",    "deu",    "sve",    "fra",    "nld",    "ita",    "esp",
    "dan",    "fin",    "nor",    "ell",    "ptb",    "rus",    "plk",
    "hun",    "csy",    "slv",    "sky",    "rom",    "chs",    "kor",
    "jpn",    "ara",    "tur"};
static const char *cNullStr="";

QGenieInterface2LanguageManager::QGenieInterface2LanguageManager()
    :mLan(-1)
{

}

QGenieInterface2LanguageManager::~QGenieInterface2LanguageManager()
{
    QMap<void *,_tTranslateDiscription *>::Iterator i=mLabelTranslateData.begin();
    while(i!=mLabelTranslateData.end())
    {
        delete i.value();
        i++;
    }
    mLabelTranslateData.clear();
}

const char *QGenieInterface2LanguageManager::currentLanShortStr()
{
    return (mLan>=0 && mLan<sizeof(cTypeStr)/sizeof(cTypeStr[0]))?cTypeStr[mLan]:cNullStr;
}

void QGenieInterface2LanguageManager::translateItem(_tTranslateDiscription *item)
{
    QString mode=(item->mode=="")?"%1":item->mode;
    switch(item->type)
    {
    case WT_LABEL:
        ((QLabel *)item->w)->setText(mode.arg(getText(item->uuid,item->lan_idx)));
        break;
    case WT_BUTTON:
        ((QPushButton *)item->w)->setText(mode.arg(getText(item->uuid,item->lan_idx)));
        break;
    case WT_CHECKBOX:
        ((QCheckBox *)item->w)->setText(mode.arg(getText(item->uuid,item->lan_idx)));
        break;
    case WT_RADIOBUT:
        ((QRadioButton *)item->w)->setText(mode.arg(getText(item->uuid,item->lan_idx)));
        break;

    }
}

void QGenieInterface2LanguageManager::translateAll()
{
    QTranslator translator;
    translator.load("");
    qApp->installTranslator(&translator);

    mCache.clear();
    QMap<void *,_tTranslateDiscription *>::Iterator i=mLabelTranslateData.begin();
    while(i!=mLabelTranslateData.end())
    {
        Q_ASSERT(i.value());
        translateItem(i.value());

        i++;
    }
}

void QGenieInterface2LanguageManager::loadMap(const QString &fname,QMap<int, QString> &map)
{
    map.clear();

    QFile file(fname);
    //  QFile file(":/LanguageFeedback.txt");
    if(file.open(QFile::ReadOnly))
    {
        QTextStream stream(&file);
        QRegExp rxBase("(#+)(.*)(#+)");

        QRegExp rxline("([!=]*)(\\s*)=(\\s*)\"(.*)\"");
        QString line, value;

        line=stream.readLine().trimmed();
        int key=-1;
        if(rxBase.exactMatch(line))
        {
            bool btemp;
            key=line.replace("#","").trimmed().toInt(&btemp);
            if(!btemp)
            {
                key=-1;
            }
        }
        key=(key<0)?0:key;

        do
        {
            if (rxline.indexIn(line) > -1)
            {
                value = rxline.cap(4);
                value =value.replace("\\n","\n");
                map.insert(key, value);
                key++;
            }
            line = stream.readLine().trimmed();// line of text excluding '\n'
        }while (!stream.atEnd()||!line.isEmpty());
        file.close();
    }
    else
    {
        GENIE2_LOG("error when QGenieInterface2LanguageManager::loadMap ");
    }
}

void QGenieInterface2LanguageManager::setLayoutDirection()
{
    if(mLan>=ELT_ENGLISH && mLan<ELT_NUM)
    {
//        QGenieTrayManager *tray=QGenieTrayManager::GetSingletonPtr();
        Qt::LayoutDirection para=(mLan == ELT_ARABIC)?(Qt::RightToLeft):(Qt::LeftToRight);
        qApp->setLayoutDirection(para);
//        tray->setLayoutDirection(para);
    }
}

QString QGenieInterface2LanguageManager::getText(int uuid, int lan_idx)
{
	if(uuid <0)
	{
		return "";
	}
    if(!mCache.contains(uuid) &&
       mLan>=0 && mLan <(sizeof(cTypeStr)/sizeof(cTypeStr[0])))
    {
        QString sLanFile=GENIE2_RES(QString("i18n/")+cTypeStr[mLan]+"/"+QUuid(uuidOfIndex(uuid)).toString()+".txt");
        qDebug()<<sLanFile;
        if(!QDir::home().exists(sLanFile))
        {
            return "";
        }
        mCache.insert(uuid,QMap<int,QString>());
        loadMap(sLanFile,mCache[uuid]);


    }
//    qDebug()<<mCache.value(uuid).value(lan_idx)<<mCache;
    return mCache.value(uuid).value(lan_idx);
}

QVariant QGenieInterface2LanguageManager::processCmd(int cmd, int uuid,const QVariant &para)
{


    Q_ASSERT(cmd == HCMD_GET_TEXT || cmd == HCMD_TRANSLATE_LABEL || cmd == HCMD_TRANSLATE_BUTTON
       ||cmd ==HCMD_TRANSLATE_CHECKBOX
       ||cmd ==HCMD_TRANSLATE_RADIOBUT);
    if(cmd==HCMD_GET_TEXT)
    {
        return getText(uuid,para.toInt());
    }

    if(cmd == HCMD_TRANSLATE_LABEL || cmd == HCMD_TRANSLATE_BUTTON
       ||cmd ==HCMD_TRANSLATE_CHECKBOX
       ||cmd ==HCMD_TRANSLATE_RADIOBUT)
    {
        QList<QVariant> list=para.toList();
        if(list.size()<2)
        {
            return QVariant();
        }
        int lan_idx=list[1].toInt();
        QString mode=(list.size()>=3)?list[2].toString():QString("%1");

        void *w=list[0].value<void *>();

        const int cPara[][2]={
            {HCMD_TRANSLATE_LABEL,WT_LABEL}
            ,{HCMD_TRANSLATE_BUTTON,WT_BUTTON}
            ,{HCMD_TRANSLATE_CHECKBOX,WT_CHECKBOX}
            ,{HCMD_TRANSLATE_RADIOBUT,WT_RADIOBUT}
        };
        int type=-1;
        for(int i=0;i<sizeof(cPara)/sizeof(cPara[0]);i++)
        {
            if(cmd==cPara[i][0])
            {
                type=cPara[i][1];
                break;
            }
        }
        Q_ASSERT(type>=0);
//        int type=(cmd==HCMD_TRANSLATE_LABEL)?WT_LABEL:
//                 WT_BUTTON;
        setTranslateData(w,type,uuid,lan_idx,mode);
    }
    return QVariant();
}

void QGenieInterface2LanguageManager::setTranslateData(void *w,int type ,int uuid, int lan_idx,const QString &mode)
{
    Q_ASSERT(w && !mLabelTranslateData.contains(w));
    _tTranslateDiscription *disp=new _tTranslateDiscription;
    disp->w=w;
    disp->type=type;
    disp->uuid=uuid;
    disp->lan_idx=lan_idx;
    disp->mode=mode;
    mLabelTranslateData.insert(w,disp);

    QWidget *wTmp=NULL;
    switch(type)
    {
    case WT_LABEL:
        wTmp=((QLabel *)disp->w);
        break;
    case WT_BUTTON:
        wTmp=((QPushButton *)disp->w);
        break;
    case WT_CHECKBOX:
        wTmp=((QCheckBox *)disp->w);
        break;
    case WT_RADIOBUT:
        wTmp=((QRadioButton *)disp->w);
        break;
    }
    if(wTmp)
    {
        connect(wTmp,SIGNAL(destroyed(QObject*)),this,SLOT(slot_WidgetDestroyd(QObject*)));
    }



    //do translate here
    translateItem(disp);
}

void QGenieInterface2LanguageManager::slot_WidgetDestroyd(QObject* sender)
{
    QWidget *w=qobject_cast<QWidget *>(sender);
    Q_ASSERT(w);
    Q_ASSERT(mLabelTranslateData.contains(w));
    _tTranslateDiscription *disp=mLabelTranslateData.value(w);
    Q_ASSERT(disp);
    mLabelTranslateData.remove(w);
    delete disp;
}

void QGenieInterface2LanguageManager::slot_setLanguage(int i)
{
    if(0<=i && i<ELT_NUM)
    {
        mLan=i;
    }
    translateAll();
    setLayoutDirection();
}

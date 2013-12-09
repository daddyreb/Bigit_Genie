#include "stable.h"

//#ifdef _MACOS_
//const char * cConfigFiles[]={
//        "./styles/default_plugin.qss;./styles/default.qss",
//        "other.qss"
//};
//#else
#ifdef MIX_RESOURCE
const char * cConfigFiles[]={
        ":/styles/default_plugin.qss;:/styles/default.qss",
        "./testui.qss"
};
#else
const char * cConfigFiles[]={
        "../styles/default_plugin.qss;../styles/default.qss",
	"other.qss"
};
#endif
//#endif

QGenieStyleManager::QGenieStyleManager()
{


}

QGenieStyleManager::~QGenieStyleManager()
{

}

void QGenieStyleManager::setStyle(E_STYLE_TYPE type)
{
    GENIE_LOG("set stylesheet start!");
    try{
    QStringList lis=QString(cConfigFiles[(int )type]).split(";");

    QStringList::const_iterator constIterator;
    QString stylesheet="";
    for (constIterator = lis.constBegin(); constIterator != lis.constEnd();
         ++constIterator)
    {

#ifdef MIX_RESOURCE
		QString s=(*constIterator);

#else
		QString s=GENIE_PATH+(*constIterator);
#endif
		if(QFile::exists(s))
		{
			QFile file(s);
			file.open(QFile::ReadOnly);
			stylesheet += QLatin1String(file.readAll());
		}


    }
    qApp->setStyleSheet(stylesheet);
    }catch(...) {
        ASSERT(0);
    }

    GENIE_LOG("set stylesheet end!!");


}

#include <QtGui/QApplication>
#include "QLaunchDBManager.h"
#include <iostream>
#include <QDebug>
#include <QDir>
#include <QFile>
#include "defines.h"


#define PARA_CREATE_DB "createdb"
#define PARA_CREATE_URL "createurl"
#define PARA_HELP "help"
#define PARA_DUMPDB "dumpdb"
#define PARA_TEST "test"
//#include "C:/Qt/2010.02.1/qt/src/sql/drivers/sqlite/qsql_sqlite.h"
//#include "C:/Qt/2010.02.1/qt/src/sql/drivers/sqlite/qsql_sqlite.cpp"

void writeUrlFile(QString dbname,QString url)
{
    QDir d=QDir::current();
    d.remove(dbname);
    const QString cDBPath="./package";
    QDir pluginsDir(qApp->applicationDirPath());
    if(!pluginsDir.exists(cDBPath))
    {
        pluginsDir.mkdir(cDBPath);
    }
    QFile file(cDBPath+"/"+dbname);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "can not open file "<<dbname;
        return;
    }
    QTextStream out(&file);
    out <<TAG_IN_DESPFILE_URL<<"\n";
    out <<url<<"\n";
}


int main(int argc, char *argv[])
{
//    QCoreApplication::addLibraryPath("./");
//    qDebug() << "QCoreApplication::libraryPaths()"<<QCoreApplication::libraryPaths();
//    qDebug() << "drivers"<<QSqlDatabase::drivers();
    qDebug() << "DBM started!!!!";
    std::cout << "cout test" <<std::endl;
    QString para = PARA_HELP;
    QApplication a(argc,argv);
    int ret=0;
    if(argc >= 2)
    {
        para=argv[1];
    }

    if(para == PARA_CREATE_DB)
    {
        QLaunchDBManager dbm;
        QString s="test.db";
        if(argc >=3)
        {
            s=argv[2];
        }
        dbm.clearDB(s);
        dbm.createDB(s);
        dbm.detectInfoFromLocal(s);
        dbm.dumpDB(s);
      //  ret =a.exec();
    }
    else if(para==PARA_CREATE_URL)
    {
        QString dbname="test.db";
        if(argc >=3)
        {
            dbname=argv[2];
        }
        QString url="www.siteview.com";
        if(argc>=4)
        {
            url=argv[3];
        }
        writeUrlFile(dbname,url);
    }
    else if(para == PARA_DUMPDB)
    {
        QLaunchDBManager dbm;
        QString s="test.db";
        if(argc >=3)
        {
            s=argv[2];
        }
        dbm.dumpDB(s);

    }
    else if(para==PARA_TEST)
    {
        QLaunchDBManager dbm;
        QString ldb="local.db";
        dbm.clearDB(ldb);
        dbm.createDB(ldb);
   //     dbm.updateLocalDB(ldb,"test.db");
        dbm.dumpDB(ldb);
    }
    else /*if(para ==PARA_HELP)*/
    {
        QString s="%1\tcreate database\n"
                  "%2\t\tdump database\n"
                  "%3\t\tget help";
       s= s.arg(PARA_CREATE_DB,PARA_DUMPDB,PARA_HELP);
        std::cout << s.toLatin1().data() <<std::endl;
    }
 //   a.exec();
   return ret;






}

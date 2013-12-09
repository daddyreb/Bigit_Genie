#include "puttyfuncs.h"
#include "topology.h"
#define WINSCP_INI "WinSCP.ini"
static quint32 sFlags=0;
#define PUTTY_EXE_CHECKED (1<<0)
#define PUTTY_EXE_EXIST   (1<<1)
#define WINSCP_EXE_CHECKED (1<<2)
#define WINSCP_EXE_EXIST      (1<<3)
#define WINSCP_INI_CHECKED (1<<4)
#define WINSCP_INI_EXIST    (1<<5)
#define WINSCP_INI_READED   (1<<6)

static QMap<QString,QString> sSessionMapHost;

bool _check(quint32 flag_checked,quint32 flag_exist,const char *file)
{
    Q_ASSERT(QThread::currentThread()==qApp->thread());
    if(!(sFlags & flag_checked))
    {
        sFlags |= flag_checked;
        sFlags |= (QDir::current().exists(file))?flag_exist:0;
    }
    return (sFlags& flag_exist);
}

bool puttyExeExists()
{
    return _check(PUTTY_EXE_CHECKED,PUTTY_EXE_EXIST,"putty.exe");
}

bool winscpExeExists()
{
    return _check(WINSCP_EXE_CHECKED,WINSCP_EXE_EXIST,"WinSCP.exe");
}

bool winscpIniExists()
{
    return _check(WINSCP_INI_CHECKED,WINSCP_INI_EXIST,WINSCP_INI);
}

void startPutty(int starttype,const QString &para)
{
    QStringList paralist;
    if(starttype==POT_IP)
    {

        paralist << para;
        QProcess::startDetached("putty.exe",paralist);
    }
    else if(starttype==POT_SESSION)
    {

        paralist<<"-load"<<para;
        QProcess::startDetached("putty.exe",paralist);
    }
}

void startWSCP(int starttype,const QString &para)
{
    QStringList paralist;
    if(starttype==POT_IP || starttype==POT_SESSION)
    {

        paralist << para;
        QProcess::startDetached("WinSCP.exe",paralist);
    }
}

QVariant getPuttyHint(const QString &sIp,const QString &sName)
{
    QString regstr="HKEY_CURRENT_USER\\Software\\SimonTatham\\PuTTY\\Sessions";
    QSettings reg(regstr, QSettings::NativeFormat);
    QStringList groups=reg.childGroups();
    QString session_name;
    QStringList retlist;
    foreach(QString group,groups)
    {
        QString hostname=reg.value(group+"/HostName").toString();
        if(hostname==sIp)
        {
            retlist<<group;
        }
        else if(hostname.compare(sName,Qt::CaseInsensitive)==0&& sName!="")
        {
            retlist<<group;
        }
    }
    return retlist;
}

void _parseWinSCPIni()
{
    if(sFlags &WINSCP_INI_READED)
    {
        return;
    }
    sFlags |= WINSCP_INI_READED;
    if(winscpIniExists())
    {
        QFile f(WINSCP_INI);
        if(f.open(QIODevice::ReadOnly))
        {
            QRegExp regTitle("\\s*\\[.*\\]\\s*");
            QRegExp regSessions("\\s*\\[Sessions\\\\.*\\]\\s*");
            QRegExp regConfig("\\s*(.*)=.*\\s*");
            QString tempsession;
            bool flagOpen=false;
            while (!f.atEnd()) {
                QByteArray temparray = f.readLine();
                QString line(temparray);
                line=line.trimmed();
                if(line=="")
                {
                    continue;
                }
                //                qDebug()<<line;
                if(regTitle.exactMatch(line))
                {
                    tempsession="";
                    flagOpen=(regSessions.exactMatch(line));
                    if(flagOpen)
                    {
                        tempsession=line.split("\\").value(1).replace("]","");
                    }
                }
                else if(flagOpen && regConfig.exactMatch(line) && tempsession!="")
                {
                    QStringList templist=line.split("=");
                    if(templist[0]=="HostName")
                    {
                        sSessionMapHost.insert(tempsession,templist[1]);
                    }
                }
            }
        }

    }
}

QVariant GetWinSCPHint(const QString &sIp,const QString &sName)
{
    QStringList retlist;
    _parseWinSCPIni();
    QMap<QString, QString>::Iterator i = sSessionMapHost.begin();
    while (i != sSessionMapHost.end() ) {
		qDebug()<<i.key()<<i.value();
        if(i.value()==sIp || (i.value().compare(sName,Qt::CaseInsensitive)==0 ))
        {
            retlist.append(i.key());
        }
        ++i;
    }
    return retlist;
}

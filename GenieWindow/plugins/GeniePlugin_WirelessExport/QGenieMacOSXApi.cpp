#include "QGenieMacOSXApi.h"
#include "QGenieCodec.h"

#include <assert.h>
#include <QDir>
#include <QFile>
#include <QtDebug>

#define VOLUMES_PATH    QString("/Volumes")

QGenieMacOSXApi::QGenieMacOSXApi()
{
}

QStringList QGenieMacOSXApi::get_volumes_list()
{
    QDir volumes_dir(VOLUMES_PATH);

    const QStringList &entry_list = volumes_dir.entryList(QDir::Dirs|QDir::NoSymLinks|QDir::NoDotAndDotDot|QDir::Readable|QDir::Writable|QDir::Executable);

    QStringList list_ret;
    foreach (QString entry,entry_list)
    {
        list_ret.append(VOLUMES_PATH+"/"+entry);
    }

    return list_ret;
}

bool QGenieMacOSXApi::export_to_file(const QString &ssid,const QString &pwd,
                    const QString &auth,const QString &cipher,const QString &filename)
{
    QString cfg_str;
    build_config_string(ssid,pwd,true,auth,cipher,cfg_str);
    return save_to_file(filename,cfg_str);
}

QString QGenieMacOSXApi::get_save_path(const QString &save_dir,const QString &filename)
{
    assert ( (!save_dir.isEmpty()) && (!filename.isEmpty()) );

    QString path = save_dir;

    if(path.right(1) != QString("/"))
    {
       path += "/";
    }

    path += filename;
    path = path.trimmed();

//    if(path.contains(QString(" ")))
//    {
//        path.prepend(QString("\""));
//        path.append(QString("\""));
//    }

    return path;
}

void QGenieMacOSXApi::build_config_string(const QString &ssid,const QString &pwd,bool bnetworkKey_or_passPhrase,
                                          const QString &auth,const QString &cipher,QString &cfg_str)
{
    cfg_str.clear();

    cfg_str =  ssid;
    cfg_str += "\n";
    cfg_str += pwd;
    cfg_str += "\n";
    cfg_str += (bnetworkKey_or_passPhrase?"true":"false");

    cfg_str += "\n";
    cfg_str += auth;
    cfg_str += "\n";
    cfg_str += cipher;

    qDebug() << "build_config_string result = " << cfg_str;

}

bool QGenieMacOSXApi::save_to_file(const QString &filename,const QString &cfg_str)
{
    QFile file(filename);

    qDebug () << "save_to_file path = " << filename;
    if(!file.open(QFile::WriteOnly|QFile::Truncate))
    {
        return false;
    }

    const char* pdata = base64_encode_bin((const char*)cfg_str.utf16(),(cfg_str.length()+1)*sizeof(ushort));
    int len = strlen(pdata)+1;
    int bytes_writen = 0;

    while(len > 0)
    {
        bytes_writen = file.write(pdata,len);

        if(-1 == bytes_writen)
        {
            break;
        }

        pdata += bytes_writen;
        len -= bytes_writen;
    }

    file.close();

    return (bytes_writen != -1);
}



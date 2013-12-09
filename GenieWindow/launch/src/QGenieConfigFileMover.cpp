#include "QGenieConfigFileMover.h"
#include <QtCore>
#include <QDesktopServices>
static bool removeDirectory(QString dirName)
{
    QDir dir(dirName);
    QString tmpdir ="";
    if(!dir.exists()){
        return false;
    }

    QFileInfoList fileInfoList = dir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList)
    {
        if(fileInfo.fileName()=="."|| fileInfo.fileName()=="..")
            continue;

        if(fileInfo.isDir()){
            tmpdir = dirName +("/")+ fileInfo.fileName();
            removeDirectory(tmpdir);
            dir.rmdir(fileInfo.fileName());/**< 移除子目录 */
        }
        else if(fileInfo.isFile()){
            QFile tmpFile(fileInfo.fileName());
            dir.remove(tmpFile.fileName());/**< 删除临时文件 */
        }
        else{
            ;
        }
    }

    dir.cdUp();            /**< 返回上级目录，因为只有返回上级目录，才可以删除这个目录 */
    if(dir.exists(dirName)){
        if(!dir.rmdir(dirName))
            return false;
    }
    return true;
}



QGenieConfigFileMover::QGenieConfigFileMover()
{
#ifdef Q_OS_WIN32
    QString old_dir_name =(QDir::home().absolutePath()+QString("/NETGEARGenie/"));
#else
    QString old_dir_name =(QDir::home().absolutePath()+QString("/.NETGEARGenie/"));
#endif
    //QString new_dir_name =(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/NETGEARGenie/");
    QString new_dir_name =QStandardPaths::writableLocation(QStandardPaths::DataLocation);

    bool old_exist=QDir::home().exists(old_dir_name);
    bool new_exist=QDir::home().exists(new_dir_name);
    if(!new_exist)
    {
        QDir::home().mkdir(new_dir_name);
        if(old_exist)
        {
            const char *cParas[]={"config.ini","function_statistics.dat"
                                  ,"routetype_statistics.dat","mapcustomdata.dat"};
            QDir d(old_dir_name);
            for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
            {
                if(d.exists(cParas[i]))
                {
                    QFile::copy(old_dir_name+"/"+cParas[i],new_dir_name+"/"+cParas[i]);
                }
            }
        }
    }
    if(old_exist)
    {
        removeDirectory(old_dir_name);
    }
}

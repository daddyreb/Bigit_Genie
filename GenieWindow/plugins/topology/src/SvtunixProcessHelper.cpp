#include "topology.h"

SVT_UnixProcessHelper::SVT_UnixProcessHelper(bool * exitmsg)
    :mExitMsg(exitmsg)
{
}

bool SVT_UnixProcessHelper::doCmd(const QString &cmd,QStringList &retlst)
{
    retlst.clear();
    QProcess process;
    process.setWorkingDirectory(UNIX_SCRIPTS_DIR);
   // qDebug()<<"process.start"<<cmd;
    process.start(cmd);
    bool done=false;
    if(mExitMsg)
    {
        for(int i=0;!(*mExitMsg) && !done && i<PROCESS_WAIT_TIME;i+=100)
        {
            done=process.waitForFinished(100);
        }
    }
    else
    {
        done=process.waitForFinished(PROCESS_WAIT_TIME);
    }

    if(done)
    {
        QByteArray _data= process.readAll();
        QString data=_data.constData();
        retlst=data.split(QRegExp("\n|\r\n"),QString::SkipEmptyParts);
    }
    else
    {
        qDebug()<<UNIX_SCRIPTS_DIR<<"process.kill();";
        process.kill();
    }
    return done;


}

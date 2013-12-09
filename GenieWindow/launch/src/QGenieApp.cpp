#include "QGenieApp.h"

#ifdef USE_CHEET_CODE
#include <QChar>
#include <QKeyEvent>
#include <QObject>
#include <QDebug>
#include "QLauncher.h"
#include "ILaunchInf.h"


const char* cCheetCode="siteviewtestzzj888";
const char* cReloadCode="siteviewreload";
const char* cShowupCode="siteviewshowzzj888";

QGenieApp::QGenieApp(int & argc, char ** argv)
    :QApplication(argc,argv)
    ,mCheetIdx(0)
    ,mReloadIdx(0)
    ,mShowPluginsIdx(0)
{
}

bool QGenieApp::notify ( QObject * receiver, QEvent * e )
{
    QLauncher *launcher=QLauncher::GetSingletonPtr();
    ASSERT(launcher);

  //  qDebug()<<"notify app "<< e->type();

    if(e->type() == QEvent::KeyPress)
    {
        QKeyEvent *kev=static_cast<QKeyEvent *>(e);
        if(kev &&!(kev->text().toLower().isEmpty()))
        {
            QChar c=kev->text().at(0);
            if(c == cCheetCode[mCheetIdx])
            {
                qDebug() << "fitted " << cCheetCode[mCheetIdx];
                mCheetIdx++;
                if(mCheetIdx ==(strlen(cCheetCode)))
                {
                    qDebug() << "cheet trigged ";
                    mCheetIdx=0;
                    if(launcher)
                    {
                        launcher->cmd(CMD_DBG_PAN);
                    }
                }
            }
            else
            {
                qDebug()<<"key not fit"<<c;
                mCheetIdx=0;
            }

            if(c == cReloadCode[mReloadIdx])
            {
                qDebug() << "fitted reload" << cReloadCode[mReloadIdx];
                mReloadIdx++;
                if(mReloadIdx ==(strlen(cReloadCode)))
                {
                    qDebug() << "cheet trigged ";
                    mReloadIdx=0;
                    if(launcher)
                    {
						launcher->debugRestart();

                    }
                }
            }
            else
            {
                qDebug()<<"key not fit"<<c;
                mReloadIdx=0;
            }



            if(c == cShowupCode[mShowPluginsIdx])
            {
                qDebug() << "fitted reload" << cReloadCode[mReloadIdx];
                mShowPluginsIdx++;
                if(mShowPluginsIdx ==(strlen(cShowupCode)))
                {
                    qDebug() << "cheet trigged ";
                    mShowPluginsIdx=0;
                    if(launcher)
                    {
                        launcher->cmd(CMD_DBG_SHOW_PLUGINS);
                    }
                }
            }
            else
            {
                qDebug()<<"key not fit"<<c;
                mShowPluginsIdx=0;
            }


        }

    }
   return QApplication::notify(receiver,e);

}
#endif

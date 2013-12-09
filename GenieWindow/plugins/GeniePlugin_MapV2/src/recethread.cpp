//add by caiyujun
#include "recethread.h"
#include "udt.h"
#include "api.h"
#include <QtDebug>
#include <QDir>

recethread::recethread(QString strFileName, int handle, int iFolder, qint64 iSize, QObject *parent) :
    QThread(parent)
{
    m_strFileName = strFileName;
    m_handle = handle;
    m_iFolder = iFolder;
    m_size = iSize;
    m_closeFlag = false;
}

void recethread::onSendProgress(qint64 iSize, double iProgress)
{
    emit sendProgress(iSize,iProgress);
}

void recethread::run()
{  
    if (m_iFolder == 0)
    {
        int iContent = 3;
        char content[4];
        if (UDT::ERROR == UDT::recv(m_handle, content, iContent, 0))
        {
            qDebug() << "recv: " << UDT::getlasterror().getErrorMessage();
            emit recvFail(UDT::getlasterror().getErrorMessage());
            UDT::close(m_handle);
            return ;
        }
        else
        {
            qDebug()<< "receive file or directory request success";
            content[iContent] = '\0';

            char *strContent = new char[iContent+1];
            memset(strContent,0,iContent+1);
            memcpy(strContent, content, iContent);

            if (0 == strcmp(strContent, "FCS"))
            {
                qDebug() << "receive FCS";
            }
            else
            {
                UDT::close(m_handle);
                return;
            }
        }

        if (UDT::ERROR == UDT::recv(m_handle, (char*)&size, sizeof(uint64_t), 0))
        {
            qDebug() << "handle: " << m_handle;
            qDebug() << "send: " << UDT::getlasterror().getErrorMessage() << endl;
            emit recvFail(UDT::getlasterror().getErrorMessage());
            UDT::close(m_handle);
            return;
        }

        if (size < 0)
        {
            QString strMessage = "Receive File Size Error";
            emit recvFail(strMessage);
            UDT::close(m_handle);
            return;
        }

        qDebug() << "receive file size : " << size;


//        std::fstream ofs((const char *)m_strFileName.toLocal8Bit(), std::ios::out | std::ios::binary | std::ios::trunc);
//        int64_t recvsize;
//        int64_t offset = 0;

//        CUDT *udt = CUDT::getUdt(m_handle);
//        udt->setFolderSize(size);
//        connect(udt,SIGNAL(sendProgress(qint64,double)),this,SLOT(onSendProgress(qint64,double)));

//        if (UDT::ERROR == (recvsize = udt->recvfile(ofs, offset, size)))
//        {
//            QString str = UDT::getlasterror().getErrorMessage();
//            //cout << "recvfile: " << UDT::getlasterror().getErrorMessage() << endl;
//            //return -1;
//        }

        QByteArray btFileName = m_strFileName.toLocal8Bit();
        std::fstream ofs(btFileName.constData(), std::ios::out | std::ios::binary | std::ios::trunc);
        if (!ofs)
        {
            QString strMessage = "Open File Error";
            emit recvFail(strMessage);
            UDT::close(m_handle);
        }
        int64_t offset = 0;
        int64_t left = size;

        qint64 iLastPercent = 0;
        bool bFirstTime = true;
        while(left > 0 && !m_closeFlag)
        {
            UDT::TRACEINFO trace;
            UDT::perfmon(m_handle, &trace);
            int recv = 0;

            if (left > 51200)
                recv = UDT::recvfile(m_handle, ofs, offset, 51200);
            else
                recv = UDT::recvfile(m_handle, ofs, offset, left);

            if (UDT::ERROR == recv)
            {
                qDebug()<< "recv size: " << UDT::getlasterror().getErrorMessage() << endl;
                emit recvFail(UDT::getlasterror().getErrorMessage());
                UDT::close(m_handle);
                ofs.close();
                return;
            }
            left -= recv;
            offset +=recv;

            qint64 iPercent = (offset*100/size);
            if(iPercent ==1) {
                iPercent = 1;
            }
            if(iPercent > 100)
            {
                iPercent = iPercent;
            }
            if (iPercent != iLastPercent)
            {
                iLastPercent = iPercent;
                qDebug() << "race :" << trace.mbpsRecvRate;

                char strContent[8];
                memset(strContent, 0, 8);
                strcpy(strContent,"FSC1");
                if (UDT::ERROR == UDT::send(m_handle, (char*)strContent, 4, 0))
                {
                    qDebug() << "send: " << UDT::getlasterror().getErrorMessage() << endl;
                    emit recvFail(UDT::getlasterror().getErrorMessage());
                    UDT::close(m_handle);
                    ofs.close();
                    return;
                }
                emit sendProgress(iPercent, trace.mbpsRecvRate / 8);
            }

            if (0 == iPercent && bFirstTime)
            {
                bFirstTime = false;
                emit sendProgress(iPercent, 0);
            }
        }
        if(m_closeFlag)
        {
            emit recvFail("Cancel Receive");
        }else
        {
           emit sendSuccess();
        }

      //  UDT::close(m_handle);///kai.yan del

        ofs.close();
        UDT::close(m_handle);
    }
    else if (1 == m_iFolder)
    {
        //CUDT *udt = CUDT::getUdt(m_handle);
        qDebug() << "set folder size : " << m_size;
        //udt->setFolderSize(m_size);
        //connect(udt,SIGNAL(sendProgress(qint64,double)),this,SLOT(onSendProgress(qint64,double)));
        qint64 iReceiveSize = 0;
        qint64 iLastPercent = 0;

        while(true)
        {            
            // receive request protocal
            bool bFirstTime = true;
            int iReqPro = 3;
            char reqPro[4];
            if (UDT::ERROR == UDT::recv(m_handle, reqPro, iReqPro, 0))
            {
               qDebug() << "recvive request protocal error: " << UDT::getlasterror().getErrorMessage();
               emit recvFail(UDT::getlasterror().getErrorMessage());
               UDT::close(m_handle);
               return ;
            }
            else
            {
                if (m_strFileName.contains('\\'))
                {
                    m_strFileName.replace('\\','/');
                }
                if (m_strFileName.at(m_strFileName.size()-1) != '/')
                {
                    m_strFileName += "/";
                }

                reqPro[iReqPro] = '\0';
                qDebug()<< "receive request protocal success : " << reqPro;

                char *ptReqPro = new char[iReqPro+1];
                memset(ptReqPro, 0, iReqPro+1);
                memcpy(ptReqPro,reqPro,iReqPro);
                if (0 == strcmp("DCR", ptReqPro))
                {
                    qDebug() << "receive DCR request";

                    char file[1024];
                    int len;
                    // receive dir name size
                    if (UDT::ERROR == UDT::recv(m_handle, (char*)&len, sizeof(int), 0))
                    {
                        //cout << "recv: " << UDT::getlasterror().getErrorMessage() << endl;
                        emit recvFail(UDT::getlasterror().getErrorMessage());
                        UDT::close(m_handle);
                        return;
                    }
                    else
                     {
                         qDebug() << "receive dir name size success : ";
                     }

                     // receive dir name
                     if (UDT::ERROR == UDT::recv(m_handle, file, len, 0))
                     {
                         qDebug() << "recv: " << UDT::getlasterror().getErrorMessage();
                         emit recvFail(UDT::getlasterror().getErrorMessage());
                         UDT::close(m_handle);
                         return ;
                     }
                     qDebug()<< "receive folder name success";
                     file[len] = '\0';

                     char *strDir = new char[len+1];
                     memset(strDir,0,len+1);
                     memcpy(strDir, file, len);

                     QString strToCreateDirName;

                     QByteArray btFileName = m_strFileName.toLocal8Bit();
                     strToCreateDirName += btFileName.constData();
                     //strToCreateDirName += QString::fromLocal8Bit(strDir);
                     strToCreateDirName += QString::fromUtf8(strDir);
                     qDebug() << "to create dir : " << strToCreateDirName;

                     QDir *dir = new QDir;
                     bool exist = dir->exists(strToCreateDirName);
                     if(!exist)
                     {
                             exist = dir->mkpath(strToCreateDirName);
                             if(!exist)
                             {
                                     UDT::close(m_handle);
                                     return ;
                             }
                     }
                     else
                     {
                         qDebug() << "create dir success : " << strToCreateDirName;
                     }

                     //break;
                 }
                 else if(0 == strcmp("DFS", ptReqPro))
                 {
                     qDebug() << "receive DFS request";

                     qDebug() << "start";
                     char file[1024];
                     int len;
                     // receive file name size
                     if (UDT::ERROR == UDT::recv(m_handle, (char*)&len, sizeof(int), 0))
                     {
                         qDebug() << "recv file name size error : " << UDT::getlasterror().getErrorMessage();
                         emit recvFail(UDT::getlasterror().getErrorMessage());
                         UDT::close(m_handle);
                         return;
                     }
                     else
                     {
                         qDebug() << "receive file name size success : ";
                     }

                     // receive file name
                     if (UDT::ERROR == UDT::recv(m_handle, file, len, 0))
                     {
                         qDebug() << "recv file name error : " << UDT::getlasterror().getErrorMessage();
                         emit recvFail(UDT::getlasterror().getErrorMessage());
                         UDT::close(m_handle);
                         return ;
                     }

                     qDebug()<< "receive file name success";
                     file[len] = '\0';

                     char *strFile = new char[len+1];
                     memset(strFile,0,len+1);
                     memcpy(strFile, file, len);
                     uint64_t iFileSize;

                     if (UDT::ERROR == UDT::recv(m_handle, (char*)&iFileSize, sizeof(uint64_t), 0))
                     {
                         qDebug() << "handle: " << m_handle;
                         qDebug() << "send: " << UDT::getlasterror().getErrorMessage();
                         emit recvFail(UDT::getlasterror().getErrorMessage());
                         UDT::close(m_handle);
                         return;
                     }

                     if (iFileSize < 0)
                     {
                         UDT::close(m_handle);
                         return;
                     }

                     qDebug() << "receive file size : " << iFileSize;

                     if (m_strFileName.contains('\\'))
                     {
                         m_strFileName.replace('\\','/');
                     }
                     if (m_strFileName.at(m_strFileName.size()-1) != '/')
                     {
                         m_strFileName += "/";
                     }
                     QString strFileName;
                     strFileName += m_strFileName;
                     //strFileName += QString::fromLocal8Bit(strFile);
                     strFileName += QString::fromUtf8(strFile);
                     qDebug() << "folder file name : " << strFileName;
                     qDebug() << "file name : " << strFile;
                     QString tempFileName = strFileName;

                     QString strLocalFileName = QString::fromUtf8(strFile);
                     QString strTemp = getActualFileName(strLocalFileName);
                     int iIndex = tempFileName.lastIndexOf(strTemp);
                     QString dirString;
                     dirString = tempFileName.left(iIndex);//如果目录不存在创建目录
                     qDebug() << "create dir : " << dirString;
                     QDir *dir = new QDir;
                     bool exist = dir->exists(dirString);
                     if(!exist)
                     {
                             exist = dir->mkpath(dirString);
                             if(!exist)
                             {
                                    UDT::close(m_handle);
                                     return ;
                             }
                     }
                     else
                     {
                         qDebug() << "create dir success : " << dirString;
                     }

//                     std::fstream ofs((const char *)strFileName.toLocal8Bit(), std::ios::out | std::ios::binary | std::ios::trunc);
//                     int64_t recvsize;
//                     int64_t offset = 0;

//                     if (UDT::ERROR == (recvsize = udt->recvfile(ofs, offset, iFileSize)))
//                     {
//                         QString str = UDT::getlasterror().getErrorMessage();
//                         //cout << "recvfile: " << UDT::getlasterror().getErrorMessage() << endl;
//                         //return -1;
//                         break;
//                     }
//                     else
//                     {
//                         qDebug() << "success receive folder file : ";
//                     }
                     //std::fstream ofs((const char *)m_strFileName.toLocal8Bit(), std::ios::out | std::ios::binary | std::ios::trunc);
                     QByteArray btFileName = strFileName.toLocal8Bit();
                     std::fstream ofs(btFileName.constData(), std::ios::out | std::ios::binary | std::ios::trunc);
                     if (!ofs)
                     {
                         QString strMessage = "Open File Error";
                         emit recvFail(strMessage);
                         UDT::close(m_handle);
                         return ;
                     }

                     int64_t offset = 0;
                     int64_t left = iFileSize;

                     while(left > 0 && !m_closeFlag)
                     {
                         UDT::TRACEINFO trace;
                         UDT::perfmon(m_handle, &trace);

                         int recv = 0;

                         if (left > 51200)
                             recv = UDT::recvfile(m_handle, ofs, offset, 51200);
                         else
                             recv = UDT::recvfile(m_handle, ofs, offset, left);

                         if (UDT::ERROR == recv)
                         {
                             qDebug()<< "recv size: " << UDT::getlasterror().getErrorMessage();
                             emit recvFail(UDT::getlasterror().getErrorMessage());
                             UDT::close(m_handle);
                             ofs.close();
                             return;
                         }
                         left -= recv;
                         offset +=recv;
                         iReceiveSize += recv;
                         qint64 iPercent = (iReceiveSize*100/m_size);
                         if(iPercent ==1) {
                             iPercent = 1;
                         }
                         if(iPercent > 100)
                         {
                             iPercent = iPercent;
                         }
                         if (iPercent != iLastPercent)
                         {
                             iLastPercent = iPercent;
                             qDebug() << "race :" << trace.mbpsRecvRate;

                             char strContent[8];
                             memset(strContent, 0, 8);
                             strcpy(strContent,"FSC1");
                             if (UDT::ERROR == UDT::send(m_handle, (char*)strContent, 4, 0))
                             {
                                 qDebug() << "send: " << UDT::getlasterror().getErrorMessage() << endl;
                                 emit recvFail(UDT::getlasterror().getErrorMessage());
                                 UDT::close(m_handle);
                                 ofs.close();
                                 return;
                             }
                             emit sendProgress(iPercent, trace.mbpsRecvRate / 8);
                         }

                         if (0 == iPercent && bFirstTime)
                         {
                             bFirstTime = false;
                             emit sendProgress(iPercent, 0);
                         }
                     }
//                     iReceiveSize += recvsize;
//                     qDebug() << "iReceiveSize : " << recvsize;
                     if(m_closeFlag)
                     {
                         emit recvFail("Cancel Receive");
                         UDT::close(m_handle);
                         return;

                     }
                     ofs.close();

                 }
                 else if(0 == strcmp("DSF", ptReqPro))
                 {
                     qDebug() << "receive DSF request";
                     UDT::close(m_handle);
                     emit sendSuccess();
                     break;
                 }            
            }
        }
    }
    else if (2 == m_iFolder)
    {
        //CUDT *udt = CUDT::getUdt(m_handle);
        qDebug() << "set folder size : " << m_size;
        qint64 iLastPercent = 0;
        qint64 iReceiveSize = 0;
        //udt->setFolderSize(m_size);
        //connect(udt,SIGNAL(sendProgress(qint64,double)),this,SLOT(onSendProgress(qint64,double)));

        if (m_strFileName.contains('\\'))
        {
            m_strFileName.replace('\\','/');
        }
        if (m_strFileName.at(m_strFileName.size()-1) != '/')
        {
            m_strFileName += "/";
        }
        while(true)
        {
            // receive request protocal
            bool bFirstTime = true;
            int iReqPro = 3;
            char reqPro[4];
            if (UDT::ERROR == UDT::recv(m_handle, reqPro, iReqPro, 0))
            {
               qDebug() << "recvive request protocal error: " << UDT::getlasterror().getErrorMessage();
               emit recvFail(UDT::getlasterror().getErrorMessage());
               UDT::close(m_handle);
               return ;
            }
            else
            {

                reqPro[iReqPro] = '\0';
                qDebug()<< "receive request protocal success : " << reqPro;

                char *ptReqPro = new char[iReqPro+1];
                memset(ptReqPro, 0, iReqPro+1);
                memcpy(ptReqPro,reqPro,iReqPro);

                if(0 == strcmp("MCS", ptReqPro))
                {
                    qDebug() << "receive MCS request";
                    char file[1024];
                    int len;
                    // receive file name size
                    if (UDT::ERROR == UDT::recv(m_handle, (char*)&len, sizeof(int), 0))
                    {
                        qDebug() << "recv file name size error : " << UDT::getlasterror().getErrorMessage();
                        emit recvFail(UDT::getlasterror().getErrorMessage());
                        UDT::close(m_handle);
                        return ;
                    }
                    else
                    {
                        qDebug() << "receive file name size success : ";
                    }

                    // receive file name
                    if (UDT::ERROR == UDT::recv(m_handle, file, len, 0))
                    {
                        qDebug() << "recv file name error : " << UDT::getlasterror().getErrorMessage();
                        emit recvFail(UDT::getlasterror().getErrorMessage());
                        UDT::close(m_handle);
                        return ;
                    }

                    qDebug()<< "receive file name success";
                    file[len] = '\0';

                    char *strFile = new char[len+1];
                    memset(strFile,0,len+1);
                    memcpy(strFile, file, len);
                    //emit recvFileChange(QString::fromLocal8Bit(strFile));
                    emit recvFileChange(QString::fromUtf8(strFile));
                    uint64_t iFileSize;

                    if (UDT::ERROR == UDT::recv(m_handle, (char*)&iFileSize, sizeof(uint64_t), 0))
                    {
                        qDebug() << "send: " << UDT::getlasterror().getErrorMessage();
                        emit recvFail(UDT::getlasterror().getErrorMessage());
                        UDT::close(m_handle);
                        return;
                    }

                    if (iFileSize < 0)
                    {
                        UDT::close(m_handle);
                        return;
                    }

                    qDebug() << "receive file size : " << iFileSize;

                    QString strFileName;
                    strFileName += m_strFileName;
                    //strFileName += QString::fromLocal8Bit(strFile);
                    strFileName += QString::fromUtf8(strFile);

                    //std::string strTest = strFileName.toStdString();
                    //qDebug() << "to write file name :" << strFileName;

                    //std::fstream ofs((const char *)strFileName.toLocal8Bit(), std::ios::out | std::ios::binary | std::ios::trunc);
//                    std::fstream ofs((const char *)strFileName.toLocal8Bit(), std::ios::out | std::ios::binary | std::ios::trunc);
//                    int64_t recvsize;
//                    int64_t offset = 0;

//                    if (UDT::ERROR == (recvsize = udt->recvfile(ofs, offset, iFileSize)))
//                    {
//                        QString str = UDT::getlasterror().getErrorMessage();
//                        //cout << "recvfile: " << UDT::getlasterror().getErrorMessage() << endl;
//                        //return -1;
//                        break;
//                    }
//                    else
//                    {
//                        qDebug() << "success receive folder file : ";
//                    }
                    QByteArray btFileName = strFileName.toLocal8Bit();
                    std::fstream ofs(btFileName.constData(), std::ios::out | std::ios::binary | std::ios::trunc);
                    if (!ofs)
                    {
                        QString strMessage = "Open File Error";
                        emit recvFail(strMessage);
                        UDT::close(m_handle);
                        return;
                    }

                    int64_t offset = 0;
                    int64_t left = iFileSize ;

                    while(left > 0 && !m_closeFlag)
                    {
                        int recv = 0;
                        UDT::TRACEINFO trace;
                        UDT::perfmon(m_handle, &trace);

                        if (left > 51200)
                            recv = UDT::recvfile(m_handle, ofs, offset, 51200);
                        else
                            recv = UDT::recvfile(m_handle, ofs, offset, left);

                        if (UDT::ERROR == recv)
                        {
                            qDebug()<< "recv size: " << UDT::getlasterror().getErrorMessage() << endl;                        
                            emit recvFail(UDT::getlasterror().getErrorMessage());
                            UDT::close(m_handle);
                            ofs.close();
                            return;
                        }
                        left -= recv;
                        offset +=recv;

                        iReceiveSize += recv;
                        qint64 iPercent = (iReceiveSize*100/m_size);
                        if(iPercent ==1) {
                            iPercent = 1;
                        }
                        if(iPercent > 100)
                        {
                            iPercent = iPercent;
                        }
                        if (iPercent != iLastPercent)
                        {
                            iLastPercent = iPercent;
                            char strContent[8];
                            memset(strContent, 0, 8);
                            strcpy(strContent,"FSC1");
                            if (UDT::ERROR == UDT::send(m_handle, (char*)strContent, 4, 0))
                            {
                                qDebug() << "send: " << UDT::getlasterror().getErrorMessage() << endl;
                                emit recvFail(UDT::getlasterror().getErrorMessage());
                                UDT::close(m_handle);
                                ofs.close();
                                return;
                            }
                            emit sendProgress(iPercent, trace.mbpsRecvRate / 8);
                        }

                        if (0 == iPercent && bFirstTime)
                        {
                            bFirstTime = false;
                            emit sendProgress(iPercent, 0);
                        }
                    }
//                    iReceiveSize += recvsize;
//                    qDebug() << "iReceiveSize : " << recvsize;
                    if(m_closeFlag)
                    {
                        UDT::close(m_handle);
                        emit recvFail("Cancel Receive");
                        return;

                    }
                    ofs.close();
                }
                else if(0 == strcmp("MSF", ptReqPro))
                {
                    qDebug() << "receive MSF request";
                    UDT::close(m_handle);
                    emit sendSuccess();
                    break;
                }
             }
        }
    }
}

QString recethread::getActualFileName(QString strFileName)
{
    QString strActualFileName;
    if (strFileName.contains('/'))
    {
        QStringList strList = strFileName.split("/");
        int iSize = strList.size();
        int iLen = strFileName.length();
        if ('/' == strFileName.at(iLen-1))
        {
            strActualFileName = strList.at(iSize-2);
        }
        else
        {
            strActualFileName = strList.at(iSize-1);
        }

    }
    else if(strFileName.contains('\\'))
    {
        QStringList strList = strFileName.split("\\");
        int iSize = strList.size();
        int iLen = strFileName.length();
        if ('\\' == strFileName.at(iLen-1))
        {
            strActualFileName = strList.at(iSize-2);
        }
        else
        {
            strActualFileName = strList.at(iSize-1);
        }
    }

    return strActualFileName;
}
void recethread::slot_cancelReceive()
{
    qDebug()<<"-------receivefile::slot_cancelReceive()-------";
    m_closeFlag = true;
}

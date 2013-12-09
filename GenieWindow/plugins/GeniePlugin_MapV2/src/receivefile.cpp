//add by caiyujun
#include "receivefile.h"
#include <fstream>
#include <string>
#include <QMessageBox>
#include "newfilenotifywidget.h"
#include <QFileDialog>
#include <QtDebug>


receivefile::receivefile(QObject *parent) :
    QThread(parent)
{
    UDT::startup();
}

bool receivefile::initSocket(QString strIp)
{
    addrinfo hints;
    addrinfo* res;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    std::string service("7777");

    if (0 != getaddrinfo(NULL, service.c_str(), &hints, &res))
    {
        qDebug() << "illegal port number or port is busy.\n" << endl;
        //return 0;
        return false;
    }

    if (UDT::ERROR == (serv = UDT::socket(res->ai_family, res->ai_socktype, res->ai_protocol)))
    {
        qDebug() << "start socket error" << UDT::getlasterror().getErrorMessage();
    }


    // Windows UDP issue
    // For better performance, modify HKLM\System\CurrentControlSet\Services\Afd\Parameters\FastSendDatagramThreshold
#ifdef WIN32
    int mss = 1052;
    UDT::setsockopt(serv, 0, UDT_MSS, &mss, sizeof(int));
#endif

    if (UDT::ERROR == UDT::bind(serv, res->ai_addr, res->ai_addrlen))
    {
        qDebug() << "bind: " << UDT::getlasterror().getErrorMessage();
        qDebug() << "bind failed";
        return false;
    }
    else
    {
        qDebug() << "bind success";
    }

    freeaddrinfo(res);

    //cout << "server is ready at port: " << service << endl;

    if (UDT::ERROR == UDT::listen(serv, 20))
    {
        qDebug() << "listen failed"  << UDT::getlasterror().getErrorMessage();
    }
    else
    {
        qDebug() << "listen success";
    }

    return true;
}



void receivefile::startThread()
{
    start(HighestPriority);
}

void receivefile::run()
{


    sockaddr_storage clientaddr;
    int addrlen = sizeof(clientaddr);

    char clienthost[NI_MAXHOST];
    char clientservice[NI_MAXSERV];
    getnameinfo((sockaddr *)&clientaddr, addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);


    //UDTSOCKET fhandle;

    while (true)
    {
        if (UDT::INVALID_SOCK == (fhandle = UDT::accept(serv, (sockaddr*)&clientaddr, &addrlen)))
        {
            qDebug() << "accept: " << UDT::getlasterror().getErrorMessage() << endl;
            qDebug() << UDT::getlasterror().getErrorMessage();
            UDT::close(fhandle);
            continue ;
        }
        else
        {
             qDebug() << "accept success";
        }

        QString strIp = inet_ntoa(((sockaddr_in*)&clientaddr)->sin_addr);
        //QString strIp = (sockaddr*)clientaddr->

        // receive file or directory
//        int iResult;
//        if (UDT::ERROR == UDT::recv(fhandle, (char*)&iResult, sizeof(int), 0))
//        {
//            qDebug() << UDT::getlasterror().getErrorMessage() << endl;
//            continue;
//        }

        int iRequest = 3;
        int iResult;
        char request[4];
        char *strRequest = new char[iRequest+1];
        if (UDT::ERROR == UDT::recv(fhandle, request, iRequest, 0))
        {
           qDebug() << "recv: " << UDT::getlasterror().getErrorMessage() << endl;
           UDT::close(fhandle);
           return ;
        }
        else
        {
             qDebug()<< "receive file or directory request success";
             request[iRequest] = '\0';


            memset(strRequest,0,iRequest+1);
            memcpy(strRequest, request, iRequest);

            if (0 == strcmp(strRequest, "FSR"))
            {
                iResult = 0;
                qDebug() << "receive file send request";
            }
            else if(0 == strcmp(strRequest, "DSR"))
            {
                iResult = 1;
                qDebug() << "receive folder send request :";
            }
            else if(0 == strcmp(strRequest, "MSR"))
            {
                iResult = 2;
                qDebug() << "receive multi file send request";
            }
        }


        if (0 == strcmp(strRequest, "FSR"))
        {
            char file[1024];
            int len;
            // receive file size
            if (UDT::ERROR == UDT::recv(fhandle, (char*)&len, sizeof(int), 0))
            {
               //cout << "recv: " << UDT::getlasterror().getErrorMessage() << endl;
               UDT::close(fhandle);
               continue;
            }
            else
            {
                qDebug() << "receive file name size success : ";
            }

            // receive file name
            if (UDT::ERROR == UDT::recv(fhandle, file, len, 0))
            {
               qDebug() << "recv: " << UDT::getlasterror().getErrorMessage() << endl;
               UDT::close(fhandle);
               return ;
            }
            else
            {
                qDebug()<< "receive file name success";
                file[len] = '\0';

                char *strFile = new char[len+1];
                memset(strFile,0,len+1);
                memcpy(strFile, file, len);

                emit showReceiveFile(strIp, QString::fromUtf8(strFile), fhandle, iResult, 0, 1);
            }

        }
        else if (0 == strcmp(strRequest, "DSR"))
        {
            uint64_t size;
            if (UDT::ERROR == UDT::recv(fhandle, (char*)&size, sizeof(uint64_t), 0))
            {
                qDebug() << "handle: " << fhandle;
                qDebug() << "send: " << UDT::getlasterror().getErrorMessage() << endl;
                UDT::close(fhandle);
                //qDebug() << "receive file size success";
                return;
            }

            if (size < 0)
            {
                return;
            }

            qDebug() << "receive all size : " << size;


            char file[1024];
            int len;
            // receive file size
            if (UDT::ERROR == UDT::recv(fhandle, (char*)&len, sizeof(int), 0))
            {
               //cout << "recv: " << UDT::getlasterror().getErrorMessage() << endl;
                UDT::close(fhandle);
               continue;
            }
            else
            {
                qDebug() << "receive file name size success : ";
            }

            // receive file name
            if (UDT::ERROR == UDT::recv(fhandle, file, len, 0))
            {
               qDebug() << "recv: " << UDT::getlasterror().getErrorMessage() << endl;
               UDT::close(fhandle);
               return ;
            }
            else
            {
                qDebug()<< "receive file name success";
                file[len] = '\0';

                char *strFile = new char[len+1];
                memset(strFile,0,len+1);
                memcpy(strFile, file, len);

                //emit showReceiveFile(strIp, QString::fromLocal8Bit(strFile), fhandle, iResult, size, 1);
                emit showReceiveFile(strIp, QString::fromUtf8(strFile), fhandle, iResult, size, 1);
            }
        }
        else if (0 == strcmp(strRequest, "MSR"))
        {
            // recv multi files total size
            uint64_t size;
            if (UDT::ERROR == UDT::recv(fhandle, (char*)&size, sizeof(uint64_t), 0))
            {
                qDebug() << "receive multi files total size error : " << UDT::getlasterror().getErrorMessage() << endl;
                UDT::close(fhandle);
                continue;
            }

            // recv multi files count
            int iCount;
            if (UDT::ERROR == UDT::recv(fhandle, (char*)&iCount, sizeof(int), 0))
            {
               qDebug() << "recv: " << UDT::getlasterror().getErrorMessage() << endl;
               UDT::close(fhandle);
               continue;
            }

            char file[1024];
            int len;
            // receive file size
            if (UDT::ERROR == UDT::recv(fhandle, (char*)&len, sizeof(int), 0))
            {
                qDebug() << "recv file name length error : " << UDT::getlasterror().getErrorMessage() << endl;
                UDT::close(fhandle);
                continue;
            }

            // receive file name
            if (UDT::ERROR == UDT::recv(fhandle, file, len, 0))
            {
                qDebug() << "recv: " << UDT::getlasterror().getErrorMessage() << endl;
                UDT::close(fhandle);
                continue ;
            }
            else
            {
                qDebug()<< "receive file name success";
                file[len] = '\0';

                char *strFile = new char[len+1];
                memset(strFile,0,len+1);
                memcpy(strFile, file, len);

                //emit showReceiveFile(strIp, QString::fromLocal8Bit(strFile), fhandle, iResult, size, iCount);
                emit showReceiveFile(strIp, QString::fromUtf8(strFile), fhandle, iResult, size, iCount);
            }
        }

    }
}

void receivefile::onSaveFile(QString strFileName,int iFolder, int handle)
{

    const char *ptResponse= NULL;
    if (1 == iFolder)
    {
        ptResponse = "DSP1";
    }
    else if (0 == iFolder)
    {
        ptResponse = "FSP1";
    }
    else if (2 == iFolder)
    {
        ptResponse = "MSP1";
    }

    int iResponse = strlen(ptResponse);
    qDebug() << "save handle:" << handle;
    if (UDT::ERROR == UDT::send(handle, ptResponse, iResponse, 0))
    {
        qDebug() << "send: " << UDT::getlasterror().getErrorMessage() << endl;
        UDT::close(handle);
        return ;
    }


//    int iIsSave = 1;
//    // send save info to sender
//    if (UDT::ERROR == UDT::send(fhandle, (char*)&iIsSave, sizeof(int), 0))
//    {
//       qDebug() << "send: " << UDT::getlasterror().getErrorMessage() << endl;
//       return ;
//    }

//    qDebug() << "on save file handle:" << fhandle;

//    std::fstream ofs(strFileName.toStdString().c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
//    int64_t recvsize;
//    int64_t offset = 0;

//    if (UDT::ERROR == (recvsize = UDT::recvfile(fhandle, ofs, offset, size)))
//    {
//        QString str = UDT::getlasterror().getErrorMessage();
//        //cout << "recvfile: " << UDT::getlasterror().getErrorMessage() << endl;
//        //return -1;
//    }

    //UDT::close(fhandle);

    //ofs.close();
}

void receivefile::onRejectSaveFile(int iFolder, int handle)
{
    const char *ptResponse= NULL;
    if (1 == iFolder)
    {
        ptResponse = "DSP0";
    }
    else if (0 == iFolder)
    {
        ptResponse = "FSP0";
    }
    else if (2 == iFolder)
    {
        ptResponse = "MSP0";
    }

    int iResponse = strlen(ptResponse);
    if (UDT::ERROR == UDT::send(handle, ptResponse, iResponse, 0))
    {
        qDebug() << "send: " << UDT::getlasterror().getErrorMessage() << endl;
        UDT::close(handle);
        return ;
    }

    

//    int iIsSave = 0;
//    // send reject info to sender
//    if (UDT::ERROR == UDT::send(fhandle, (char*)&iIsSave, sizeof(int), 0))
//    {
//       //cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
//       return ;
//    }

    UDT::close(handle);
}




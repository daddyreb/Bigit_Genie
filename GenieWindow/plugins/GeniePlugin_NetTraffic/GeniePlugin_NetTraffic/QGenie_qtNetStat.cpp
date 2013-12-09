#include "QGenie_qtNetStat.h"
#include "ExTable.h"
#include "plugin_ui_def.h"

#include "QGenie_qtNetTraffic.h"
#include "ILanguageManager.h"
#include "LanguageNetTraffic.h"
#include <QEvent>

QGenie_qtNetStat::QGenie_qtNetStat()
{

    this->setGeometry(350,200,610,500);
    qTree=new QTreeWidget();
    btnRefresh=new QPushButton("Refresh",0);
    btnClose=new QPushButton("Close",0);
    QGridLayout* mainLy=new QGridLayout();
    headItem=new QTreeWidgetItem();
    headItem->setText(0,"");
    headItem->setText(1,"");
    headItem->setText(2,"");
    headItem->setText(3,"");
    headItem->setText(4,"");


    qTree->setHeaderItem(headItem);
    qTree->setAllColumnsShowFocus(true);
    qTree->setFirstItemColumnSpanned(headItem,true);
    qTree->setColumnWidth(0,80);
    qTree->setColumnWidth(1,150);
    qTree->setColumnWidth(2,150);
    qTree->setColumnWidth(3,80);
    qTree->setColumnWidth(4,120);
    qTree->setRootIsDecorated(false);
    qTree->setAlternatingRowColors(true);
    mainLy->addWidget(qTree,0,0,1,4);
    mainLy->addWidget(btnRefresh,1,2,1,1);
    mainLy->addWidget(btnClose,1,3,1,1);
    this->setLayout(mainLy);
    this->setWindowIcon(QIcon(":/image/aa.ico"));
    connect(btnClose,SIGNAL(clicked ( bool )),this,SLOT(closeWidget (bool)));
    connect(btnRefresh,SIGNAL(clicked ( bool  )),this,SLOT(refreshWidget (bool)));

    listAllConnections();


    retranslateUi();
    SET_FIX_SIZE(NORMAL_BUTTON,btnRefresh);
    SET_FIX_SIZE(NORMAL_BUTTON,btnClose);
    btnRefresh->setFocusPolicy(Qt::NoFocus);
    btnClose->setFocusPolicy(Qt::NoFocus);
}


void QGenie_qtNetStat::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        //ui->mainmenubut->setText(lanmanager->getText(L_TESTUI_MAINBUT));
        retranslateUi();
        break;
    default:
        break;
    }
}

void QGenie_qtNetStat::retranslateUi()
{
   // ILanguageManager *lanmanager=qtNet->sHandler->getLanguageManager();
    //ui->mainmenubut->setText(lanmanager->getText(L_TESTUI_MAINBUT));
    /*
        L_NETTRAFFIC_PROCTO,
        L_NETTRAFFIC_LOCAL_ADDRESS,
        L_NETTRAFFIC_REMOTE_ADDRESS,
        L_NETTRAFFIC_STATE,
        L_NETTRAFFIC_PROGRAM,
        L_NETTRAFFIC_REFRESH,
        L_NETTRAFFIC_CLOSE,
        L_NETTRAFFIC_OK,
        L_NETTRAFFIC_CANCEL,


    this->setWindowTitle(lanmanager->getText(L_NETTRAFFIC_TITLE_NETSTAT,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")).trimmed());
    headItem->setText(0,lanmanager->getText(L_NETTRAFFIC_PROCTO,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")).trimmed());
    headItem->setText(1,lanmanager->getText(L_NETTRAFFIC_LOCAL_ADDRESS,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")).trimmed());
    headItem->setText(2,lanmanager->getText(L_NETTRAFFIC_REMOTE_ADDRESS,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")).trimmed());
    headItem->setText(3,lanmanager->getText(L_NETTRAFFIC_STATE,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")).trimmed());
    headItem->setText(4,lanmanager->getText(L_NETTRAFFIC_PROGRAM,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")).trimmed());

    btnRefresh->setText(lanmanager->getText(L_NETTRAFFIC_REFRESH,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
    btnClose->setText(lanmanager->getText(L_NETTRAFFIC_CLOSE,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
   // label->setText(lanmanager->getText(L_FEEDBACK_LABLE_FROM));
*/
}

void QGenie_qtNetStat::listAllConnections()
{
    qTree->clear();
    m_dNumEntries=0;
    QString cProtocol="TCP";

    //int x;
    //unsigned short sLocalPort=0;
    //unsigned short sRemotePort=0;



    QString cJudgeState;

    if(cProtocol=="TCP" )
    {
    //TCP
        DWORD tcpTableSize = sizeof(MIB_TCPEXTABLE) * 128;
        PMIB_TCPEXTABLE pTcpExTable = (MIB_TCPEXTABLE *)malloc(tcpTableSize);
        pTcpExTable->dwNumEntries = 0;
        BOOL nRtnCode;
        nRtnCode=getTcpExTable( pTcpExTable);
        if(nRtnCode == FALSE){
                return ;
        }
        char	szLocalAddr[128];
        char	szRemoteAddr[128];
        //WCHAR	szProcessName[128];
        in_addr inadLocal, inadRemote;
        WCHAR    strState[128];
        DWORD   dwRemotePort = 0;

        // 打印TCP扩展连接表信息
        for(UINT i = 0; i < pTcpExTable->dwNumEntries; ++i)
        {
                // 状态
            switch (pTcpExTable->table[i].dwState)
            {
            case MIB_TCP_STATE_CLOSED:
                    wcscpy(strState, L"CLOSED");
                    break;
            case MIB_TCP_STATE_TIME_WAIT:
                    wcscpy(strState, L"TIME_WAIT");
                    break;
            case MIB_TCP_STATE_LAST_ACK:
                    wcscpy(strState, L"LAST_ACK");
                    break;
            case MIB_TCP_STATE_CLOSING:
                    wcscpy(strState, L"CLOSING");
                    break;
            case MIB_TCP_STATE_CLOSE_WAIT:
                    wcscpy(strState, L"CLOSE_WAIT");
                    break;
            case MIB_TCP_STATE_FIN_WAIT1:
                    wcscpy(strState, L"FIN_WAIT1");
                    break;
            case MIB_TCP_STATE_ESTAB:
                    wcscpy(strState, L"ESTABLISHED");
                    break;
            case MIB_TCP_STATE_SYN_RCVD:
                    wcscpy(strState, L"SYN_RCVD");
                    break;
            case MIB_TCP_STATE_SYN_SENT:
                    wcscpy(strState, L"SYN_SENT");
                    break;
            case MIB_TCP_STATE_LISTEN:
                    wcscpy(strState, L"LISTEN");
                    break;
            case MIB_TCP_STATE_DELETE_TCB:
                    wcscpy(strState,L"DELETE");
                    break;
            default:
                    //printf("Error: unknown state!\n");
                    break;
            }
            // 本地IP地址
            inadLocal.s_addr = pTcpExTable->table[i].dwLocalAddr;

            // 远程端口
            if(wcscmp(strState, L"LISTEN") != 0)
            {
                    //wcscmp()
                    dwRemotePort = pTcpExTable->table[i].dwRemotePort;
            }
            else
                    dwRemotePort = 0;

            // 远程IP地址
            inadRemote.s_addr = pTcpExTable->table[i].dwRemoteAddr;


            sprintf(szLocalAddr, "%s:%u", inet_ntoa(inadLocal),
                    ntohs((unsigned short)(0x0000FFFF & pTcpExTable->table[i].dwLocalPort)));
            sprintf(szRemoteAddr, "%s:%u", inet_ntoa(inadRemote),
                    ntohs((unsigned short)(0x0000FFFF & dwRemotePort)));

            QTreeWidgetItem* qitem=new QTreeWidgetItem();
            qitem->setText(0,"tcp");
            qitem->setText(1,QString(szLocalAddr));
            qitem->setText(2,QString(szRemoteAddr));
            QString str;

            str=QString::fromWCharArray(strState);
            qitem->setText(3,str);
            WCHAR strName[128];
            ProcessPidToName( pTcpExTable->table[i].dwProcessId,strName);
            str=QString::fromWCharArray(strName);
            //str.fromStdWString(strName);
            qitem->setText(4,str);
            //ProcessPidToName(hProcessSnap, pTcpExTable->table[i].dwProcessId, szProcessName)
            qTree->addTopLevelItem(qitem);
        }//if(cProtocol.CompareNoCase("TCP") == 0)
         free(pTcpExTable);
    }
    cProtocol="UDP";
    if (cProtocol=="UDP")
    {
        DWORD udpTableSize = sizeof(MIB_UDPEXTABLE) * 128;
        PMIB_UDPEXTABLE pUdpExTable = (MIB_UDPEXTABLE *)malloc(udpTableSize);
        pUdpExTable->dwNumEntries = 0;
        BOOL bRtnCode;
        bRtnCode=getUdpExTable( pUdpExTable);
        if(bRtnCode == FALSE){
                return ;
        }
        char	szLocalAddr[128];
        in_addr inadLocal;
        for(UINT i = 0; i < pUdpExTable->dwNumEntries; ++i)
        {
        // 本地IP地址
        inadLocal.s_addr = pUdpExTable->table[i].dwLocalAddr;

        sprintf(szLocalAddr,  "%s:%u", inet_ntoa(inadLocal),
                        ntohs((unsigned short)(0x0000FFFF & pUdpExTable->table[i].dwLocalPort)));

        QTreeWidgetItem* qitem=new QTreeWidgetItem();
        qitem->setText(0,"udp");
        qitem->setText(1,QString(szLocalAddr));
        QString str;
        WCHAR strName[128];
        ProcessPidToName( pUdpExTable->table[i].dwProcessId,strName);
        str=QString::fromWCharArray(strName);
        qitem->setText(4,str);
        qTree->addTopLevelItem(qitem);
        }
        free(pUdpExTable);
    }
}

void QGenie_qtNetStat::GetAddressName(unsigned long raddr,char *rbuf, unsigned long rbuffsize,unsigned char risSrcAddress)
{

        //struct hostent *h;

        if((risSrcAddress) || (raddr == 0))
        {
                char chHostName[30];

                memset(chHostName, 0, 30);

                //if(gethostname(chHostName, sizeof(chHostName) - 1) == 0)
                     //  strncpy(rbuf,chHostName,rbuffsize);

        }
        //else if((h = gethostbyaddr((const char*)&raddr, 4, AF_INET)))
              //  strncpy(rbuf,h->h_name,rbuffsize);
}

void QGenie_qtNetStat::closeWidget(bool checked )
{
    Qt::WindowFlags flags = 0;
    flags=Qt::CustomizeWindowHint;
    flags |= Qt::WindowStaysOnTopHint;
    flags |=Qt::Tool;
    if (qtNet->iMode==1)
    {
        flags |=Qt::FramelessWindowHint;
    }
    qtNet->setWindowFlags(flags);
    /*
    qtNet->txtInput->setAlignment(Qt::AlignHCenter);
    qtNet->txtOutput->setAlignment(Qt::AlignHCenter);
    qtNet->txtInput->setAlignment(Qt::AlignBottom);
    qtNet->txtOutput->setAlignment(Qt::AlignBottom);
    */
    qtNet->show();
    qtNet->bStatPoped=false;
    this->close();
}

void QGenie_qtNetStat::refreshWidget(bool checked)
{
    listAllConnections();
}

void QGenie_qtNetStat::closeEvent ( QCloseEvent * event )
{
    qtNet->bStatPoped=false;
}


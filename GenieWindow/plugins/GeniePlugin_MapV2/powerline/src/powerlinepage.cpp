#include "powerlinepage.h"
#include "ui_powerlinepage.h"

#include <QtDebug>

PowerLinePage::PowerLinePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PowerLinePage)
  ,m_withPendingRefreshReq(false)
  ,m_isStarted(false)
  ,m_totalPage(0)
  ,m_curPageIndex(0)
{
    ui->setupUi(this);

    connect(ui->m_powerLineView, SIGNAL(refreshOver()),
            this, SLOT(onRefreshOver()));
    connect(ui->m_powerLineView, SIGNAL(pageChange(int)), SLOT(onPageChange(int)));

    connect(ui->m_powerLineView, SIGNAL(queryPLCDevicePasswordSignal(QString,QString*)),
            this , SIGNAL(queryPLCDevicePassword(QString,QString*)));
    connect(ui->m_powerLineView, SIGNAL(storePLCDevicePasswordSignal(QString,QString)),
            this, SIGNAL(storePLCDevicePassword(QString,QString)));
}

PowerLinePage::~PowerLinePage()
{
    delete ui;
}

bool PowerLinePage::refresh()
{
    if(!m_withPendingRefreshReq){
        m_withPendingRefreshReq = ui->m_powerLineView->refresh();
    }

    return m_withPendingRefreshReq;
}

void PowerLinePage::onRefreshOver()
{
    m_withPendingRefreshReq = false;

    emit refreshOver();
}

void PowerLinePage::onPageChange(int count)
{
    m_totalPage = count;
    m_curPageIndex = 1;

    emit pageChange(count);
}


bool PowerLinePage::dataAvailable()
{
    return ui->m_powerLineView->haveData();
}

void PowerLinePage::verifyStart()
{
    if(!m_isStarted){
        m_isStarted = refresh();
    }
}

void PowerLinePage::notifyRaise() const
{
    qDebug () << "PowerLinePage::notifyRaise()";
    ui->m_powerLineView->notifyRaise();
}

void PowerLinePage::notifyLower() const
{
    qDebug () << "PowerLinePage::notifyLower()";
    ui->m_powerLineView->notifyLower();
}

void PowerLinePage::notifyInternetState(bool connected)
{
    ui->m_powerLineView->notifyInternetState(connected);
}

void PowerLinePage::hideHubView()
{
    ui->m_powerLineView->hideHubView();
}

void PowerLinePage::prevPage()
{
    if(m_curPageIndex > 1){
        --m_curPageIndex;
        ui->m_powerLineView->pageBtnClick(m_curPageIndex);
    }
}

void PowerLinePage::nextPage()
{
    if(m_curPageIndex < m_totalPage){
        ++m_curPageIndex;
        ui->m_powerLineView->pageBtnClick(m_curPageIndex);
    }
}

void PowerLinePage::firstPage()
{
    if( m_totalPage >= 1){
        m_curPageIndex = 1;
        ui->m_powerLineView->pageBtnClick(m_curPageIndex);
    }
}

void PowerLinePage::lastPage()
{
    if( m_totalPage >= 1){
        m_curPageIndex = m_totalPage;
        ui->m_powerLineView->pageBtnClick(m_curPageIndex);
    }
}

bool PowerLinePage::isCurFisrtPage()
{
    return (m_curPageIndex == 1);
}

bool PowerLinePage::isCurLastPage()
{
    return (m_curPageIndex == m_totalPage);
}

int PowerLinePage::pageCount()
{
    return m_totalPage;
}

int PowerLinePage::curPage()
{
    return m_curPageIndex;
}

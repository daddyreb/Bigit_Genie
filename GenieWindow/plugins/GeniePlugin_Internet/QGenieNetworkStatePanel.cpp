#include "QGenieNetworkStatePanel.h"
#include "Genieplugin_Internet.h"
#include "genie2_interface.h"

#include <QPen>
#include <QColor>


#define  ANIMATE_TIME_INTERVAL_MS   100
#define  ANIMATE_ROUTER_LIGHT_MS    500
#define  ANIMATE_REPAIR_INTERVAL_MS 200

#define INFO_GENIE_B1 ":/images/node/genie_b1.png"
#define INFO_GENIE_B2 ":/images/node/genie_b2.png"
#define INFO_INTERNET_B1 ":/images/node/internet_b1.png"
#define INFO_INTERNET_B2 ":/images/node/internet_b2.png"

QGenieNetworkStatePanel::QGenieNetworkStatePanel(QWidget *parent)
    :QFrame(parent),
    m_paint_strategy(0)
{
    //load pictures
    m_pcpixmap.load(QString(INFO_GENIE_B1),"png");
    m_pcflashpixmap.load(QString(INFO_GENIE_B2),"png");
    update_routerpixmap();
    m_internetpixmap.load(QString(INFO_INTERNET_B1),"png");
    m_internetflashpixmap.load(QString(INFO_INTERNET_B2),"png");

    m_linkstate_unknown_pmp.load(QString(":/image/LinkStateUnknown.png"),"png");
    m_linkstate_failed_pmp.load(QString(":/image/LinkStateFailed.png"),"png");
    m_linkstate_ok_pmp.load(QString(":/image/LinkStateOk.png"),"png");

    m_cur_pcpixmap          = &m_pcpixmap;
    m_cur_routerpixmap      = &m_routerpixmap;
    m_cur_internetpixmap    = &m_internetpixmap;

    //init window size
    m_best_paint_width=m_pcpixmap.width()+m_routerpixmap.width()+m_internetpixmap.width()
                       +120*2+2*2;
    m_best_paint_height=qMax(m_pcpixmap.height(),
                             qMax(m_routerpixmap.height(),m_internetpixmap.height()))/*+2*50*/;
    this->resize(m_best_paint_width,m_best_paint_height);

    //initialize link state
    pcrouter_linkstate          = UNKNOWN;
    routerinternet_linkstate    = UNKNOWN;

    pcrouter_flashstate         = UNKNOWN;
    routerinternet_flashstate   = UNKNOWN;

    bconnecting_network              = false;
    bflash_pcrouter_link             = false;
    bflash_routerinternet_link       = false;
    bpcflash                         = false;
    brouterflash                     = false;
    brouterlight                     = false;
    brouterlight_timeout             = false;
    binternetflash                   = false;

    animate_timer_ticks              = 0;

    //new a timer
    animate_timer=new QTimer(this);
    connect(animate_timer,SIGNAL(timeout()),SLOT(animate_timeout()));
    animate_timer->setInterval(ANIMATE_TIME_INTERVAL_MS);
    begin_routerlight();
}

QGenieNetworkStatePanel::~QGenieNetworkStatePanel()
{
    animate_timer->stop();
    delete m_paint_strategy;
}

void QGenieNetworkStatePanel::draw_link(QPainter *painter,bool pc_router)
{
    LinkState state;
    if(pc_router)
    {
        if(bflash_pcrouter_link)
        {
            state=pcrouter_flashstate;
        }
        else
        {
            state=pcrouter_linkstate;
        }
    }
    else
    {
        if(bflash_routerinternet_link)
        {
            state=routerinternet_flashstate;
        }
        else
        {
            state=routerinternet_linkstate;
        }
    }
    QPen pen;
    pen.setWidth(3);
    switch(state)
    {
    case UNKNOWN:
        pen.setStyle(Qt::DashDotLine);
        pen.setColor(QColor(201,201,201));
        break;
    case OK:
        if((pc_router&&bflash_pcrouter_link)||((!pc_router)&&bflash_routerinternet_link))
        {
            pen.setStyle(Qt::DotLine);
            pen.setColor(QColor(83,166,73));
        }
        else
        {
            pen.setStyle(Qt::SolidLine);
            pen.setColor(QColor(83,166,73));
        }
        break;
    case FAILED:
        pen.setStyle(Qt::DashDotDotLine);
        pen.setColor(QColor(206,1,0));
        break;
    default:
        pen.setStyle(Qt::DashDotLine);
        pen.setColor(QColor(201,201,201));
    }
    painter->setPen(pen);
    if(pc_router)
    {
        painter->drawLine(pc_end,/*router_end*/router_end_1);
        if(!bflash_pcrouter_link)
        {
            if(UNKNOWN==state)
            {
                painter->drawImage(pcrouterlinkstate_lefttop,m_linkstate_unknown_pmp);
            }
            else if(FAILED==state)
            {
                painter->drawImage(pcrouterlinkstate_lefttop,m_linkstate_failed_pmp);
            }
            else if(OK==state)
            {
                painter->drawImage(pcrouterlinkstate_lefttop,m_linkstate_ok_pmp);
            }
        }
    }
    else
    {
        painter->drawLine(/*router_end*/router_end_2,internet_end);
        if(!bflash_routerinternet_link)
        {
            if(UNKNOWN==state)
            {
                painter->drawImage(routerinternetlinkstate_lefttop,m_linkstate_unknown_pmp);
            }
            else if(FAILED==state)
            {
                painter->drawImage(routerinternetlinkstate_lefttop,m_linkstate_failed_pmp);
            }
            else if(OK==state)
            {
                painter->drawImage(routerinternetlinkstate_lefttop,m_linkstate_ok_pmp);
            }
        }
    }
}

void QGenieNetworkStatePanel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    // painter.setWindow(0,0,m_best_paint_width,m_best_paint_height);
    painter.save();
    draw_link(&painter,true);
    draw_link(&painter,false);
    painter.restore();

    if(bpcflash)
    {
        painter.drawImage(pc_lefttop,*m_cur_pcpixmap);
    }
    else
    {
        painter.drawImage(pc_lefttop,m_pcpixmap);
    }

    if(brouterflash /*|| brouterlight*/)
    {
        painter.drawImage(router_lefttop,*m_cur_routerpixmap);
    }
    else if(brouterlight && brouterlight_timeout)
    {

        if(m_paint_strategy)
        {
            QRect old_viewport = painter.viewport();
            painter.setViewport(router_lefttop.x(),router_lefttop.y(),
                                size().width(),
                                size().height());
            m_paint_strategy->paint_B1_B3(&painter,false);
            //m_paint_strategy->paint(&painter,false,true);

            painter.setViewport(old_viewport);
        }
        
        brouterlight_timeout = false;
        
    }
    else
    {
        painter.drawImage(router_lefttop,m_routerpixmap);
    }

    if(binternetflash)
    {
        painter.drawImage(internet_lefttop,*m_cur_internetpixmap);
    }
    else
    {
        painter.drawImage(internet_lefttop,m_internetpixmap);
    }

    if(bconnecting_network)
    {
        painter.setOpacity(0.75);
        painter.drawImage(pc_animate_pos,m_pcpixmap);
        painter.setOpacity(1.0);
    }
}

void QGenieNetworkStatePanel::resizeEvent (QResizeEvent * event )
{
    //initialize pc router internet center position;
    pc_lefttop      =QPoint(2,(size().height()-m_pcpixmap.height())/2);
    router_lefttop  =QPoint((size().width()-m_routerpixmap.width())/2,
                            (size().height()-m_routerpixmap.height())/2);
    internet_lefttop=QPoint(size().width()-m_internetpixmap.width()-2,
                            (size().height()-m_internetpixmap.height())/2);

    //pc_animate_pos  =pc_lefttop;
    //initialize link ends
    //    pc_end       =QPoint(2+m_pcpixmap.width()/2,size().height()/2+1);
    //    router_end  =QPoint(size().width()/2+1,size().height()/2+1);
    //    internet_end=QPoint(size().width()-m_pcpixmap.width()/2-2,size().height()/2+1);
    pc_end        =QPoint(pc_lefttop.x()+m_pcpixmap.width()+2,size().height()/2+1);
    router_end_1  =QPoint(router_lefttop.x()-2,size().height()/2+1);
    router_end_2  =QPoint(router_lefttop.x()+m_routerpixmap.width()+2,size().height()/2+1);
    internet_end  =QPoint(internet_lefttop.x()-2,size().height()/2+1);

    //initialize link state icon
    pcrouterlinkstate_lefttop=QPoint((pc_end.x()+router_end_1.x()/*router_end.x()*/)/2-m_linkstate_unknown_pmp.width()/2,
                                     (size().height()-m_linkstate_unknown_pmp.height())/2);

    routerinternetlinkstate_lefttop=QPoint((router_end_2.x()/*router_end.x()*/+internet_end.x())/2-m_linkstate_unknown_pmp.width()/2,
                                           (size().height()-m_linkstate_unknown_pmp.height())/2);
}
QSize QGenieNetworkStatePanel::sizeHint ()  const
{
    return QSize(m_best_paint_width,m_best_paint_height);
}

void QGenieNetworkStatePanel::begin_pcflash()
{
    bpcflash=true;
    if(!animate_timer->isActive())
        animate_timer->start();

}

void QGenieNetworkStatePanel::end_pcflash()
{
    bpcflash=false;
}

void QGenieNetworkStatePanel::begin_routerflash()
{
    brouterflash=true;
    if(!animate_timer->isActive())
        animate_timer->start();

}

void QGenieNetworkStatePanel::end_routerflash()
{
    brouterflash=false;
}

void QGenieNetworkStatePanel::begin_routerlight()
{
    brouterlight = true;
    if(!animate_timer->isActive())
    {
        animate_timer->start();
    }
}

void QGenieNetworkStatePanel::end_routerlight()
{
    brouterlight = false;
}

void QGenieNetworkStatePanel::begin_internetflash()
{
    binternetflash=true;
    if(!animate_timer->isActive())
        animate_timer->start();

}

void QGenieNetworkStatePanel::end_internetflash()
{
    binternetflash=false;
}


void QGenieNetworkStatePanel::begin_pcrouterlinkflash()
{
    bflash_pcrouter_link=true;
    if(!animate_timer->isActive())
        animate_timer->start();
}

void QGenieNetworkStatePanel::end_pcrouterlinkflash()
{
    bflash_pcrouter_link=false;
}

void QGenieNetworkStatePanel::begin_routerinternetlinkflash()
{
    bflash_routerinternet_link=true;
    if(!animate_timer->isActive())
        animate_timer->start();
}

void QGenieNetworkStatePanel::end_routerinternetlinkflash()
{
    bflash_routerinternet_link=false;
}

void QGenieNetworkStatePanel::begin_connect()
{
    bconnecting_network=true;
    if(!animate_timer->isActive())
        animate_timer->start();
}

void QGenieNetworkStatePanel::end_connect()
{
    bconnecting_network=false;
}

void QGenieNetworkStatePanel::pcrouter_link_ok()
{
    pcrouter_linkstate=OK;
    update();
}
void QGenieNetworkStatePanel::pcrouter_link_failed()
{
    pcrouter_linkstate=FAILED;
    update();
}
void QGenieNetworkStatePanel::pcrouter_link_unknown()
{
    pcrouter_linkstate=UNKNOWN;
    update();
}

void QGenieNetworkStatePanel::routerinternet_link_ok()
{
    routerinternet_linkstate=OK;
    update();
}
void QGenieNetworkStatePanel::routerinternet_link_failed()
{
    routerinternet_linkstate=FAILED;
    update();
}
void QGenieNetworkStatePanel::routerinternet_link_unknown()
{
    routerinternet_linkstate=UNKNOWN;
    update();
}

void QGenieNetworkStatePanel::stop_all_animate()
{
    bconnecting_network         = false;
    bpcflash                    = false;
    brouterflash                = false;
    binternetflash              = false;
    bflash_pcrouter_link        = false;
    bflash_routerinternet_link  = false;
}

void QGenieNetworkStatePanel::animate_timeout()
{
    bool need_update = false;

    ++animate_timer_ticks;

    if(0==(animate_timer_ticks%(ANIMATE_ROUTER_LIGHT_MS/ANIMATE_TIME_INTERVAL_MS)))
    {
        if(!brouterflash && brouterlight)
        {
            brouterlight_timeout = true;

            if(m_cur_routerpixmap==&m_routerpixmap)
            {
                m_cur_routerpixmap = &m_routerlightpixmap;
            }
            else
            {
                m_cur_routerpixmap = &m_routerpixmap;
            }
        }

        need_update = true;
    }

    if(0==(animate_timer_ticks%(ANIMATE_REPAIR_INTERVAL_MS/ANIMATE_TIME_INTERVAL_MS)))
    {
        if(bconnecting_network)
        {
            pc_animate_pos+=QPoint(20,0);
            if(pc_animate_pos.x()>router_lefttop.x())
            {
                pc_animate_pos=pc_lefttop;
            }
        }

        if(bpcflash)
        {
            if(m_cur_pcpixmap==&m_pcpixmap)
            {
                m_cur_pcpixmap=&m_pcflashpixmap;
            }
            else
            {
                m_cur_pcpixmap=&m_pcpixmap;
            }
        }

        if(brouterflash)
        {
            if(m_cur_routerpixmap==&m_routerpixmap)
            {
                m_cur_routerpixmap = &m_routerflashpixmap;
            }
            else
            {
                m_cur_routerpixmap = &m_routerpixmap;
            }
        }

        if(binternetflash)
        {
            if(m_cur_internetpixmap==&m_internetpixmap)
            {
                m_cur_internetpixmap = &m_internetflashpixmap;
            }
            else
            {
                m_cur_internetpixmap = &m_internetpixmap;
            }
        }

        if(bflash_pcrouter_link)
        {
            pcrouter_flashstate=LinkState((int(pcrouter_flashstate)+1)%(UNKNOWN+1));
        }

        if(bflash_routerinternet_link)
        {
            routerinternet_flashstate=LinkState((int(routerinternet_flashstate)+1)%(UNKNOWN+1));
        }

        if(bconnecting_network||bpcflash||brouterflash ||
           binternetflash || bflash_pcrouter_link||
           bflash_routerinternet_link)
        {
            need_update = true;
        }
    }

    if(!(bconnecting_network||bpcflash||brouterflash ||
         binternetflash || bflash_pcrouter_link||
         bflash_routerinternet_link || brouterlight))
    {
        animate_timer->stop();
    }
    else if(need_update)
    {
        QResizeEvent sizeEvent(size(),size());
        this->resizeEvent(&sizeEvent);
        update();
    }

}

void QGenieNetworkStatePanel::update_routerpixmap()
{
//    m_routerpixmap.load(GeniePlugin_Internet::infomanager_getinfo(INFO_CMD_ROUTE_NAME_B3,1),"png");
//    m_routerflashpixmap.load(GeniePlugin_Internet::infomanager_getinfo(INFO_CMD_ROUTE_NAME_B2),"png");
//    GeniePlugin_Internet::output_log(QString("INFO_CMD_ROUTE_NAME_B3_2=")+GeniePlugin_Internet::infomanager_getinfo(INFO_CMD_ROUTE_NAME_B3,2));
//    m_routerlightpixmap.load(GeniePlugin_Internet::infomanager_getinfo(INFO_CMD_ROUTE_NAME_B3,2),"png");

    m_routerpixmap.load(GENIE2_HANDLER_CMD(HCMD_IMAGE_NAME,2).toString(),"png");
    m_routerflashpixmap.load(GENIE2_HANDLER_CMD(HCMD_IMAGE_NAME,1).toString(),"png");
    m_routerlightpixmap.load(GENIE2_HANDLER_CMD(HCMD_IMAGE_NAME,3).toString(),"png");

    if(m_paint_strategy)
    {
        delete m_paint_strategy;
        m_paint_strategy = 0;
    }
    m_paint_strategy = new QGenieTopoNodePaintStrategy(AT_BIG3,NULL,
        GENIE2_HANDLER_CMD(HCMD_ASK_SUBSCRIBED_MESSAGE,SCCMD_ROUTER_MODEL).toString());

    QResizeEvent sizeEvent(size(),size());
    this->resizeEvent(&sizeEvent);
    this->update();
}

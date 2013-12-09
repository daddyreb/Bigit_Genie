#ifndef QGENIENETWORKSTATEPANEL_H
#define QGENIENETWORKSTATEPANEL_H
#include <QPainter>
#include <QFrame>
#include <QImage>
#include <QPixmap>
#include <QPoint>
#include <QTimer>
#include <QResizeEvent>

#include "QGenieTopoNodePaintStrategy.h"

class QGenieNetworkStatePanel : public QFrame
{
    Q_OBJECT
public:
    enum LinkState{OK,FAILED,UNKNOWN};
public:
    QGenieNetworkStatePanel(QWidget *parent=0);
    virtual ~QGenieNetworkStatePanel();
protected:
    QImage        m_pcpixmap;
    QImage        m_pcflashpixmap;
    QImage         m_routerpixmap;
    QImage         m_routerflashpixmap;
    QImage         m_routerlightpixmap;
    QImage         m_internetpixmap;
    QImage         m_internetflashpixmap;

    QImage         m_linkstate_unknown_pmp;
    QImage         m_linkstate_failed_pmp;
    QImage         m_linkstate_ok_pmp;

    QImage        *m_cur_pcpixmap;
    QImage         *m_cur_routerpixmap;
    QImage         *m_cur_internetpixmap;
protected:
    int             m_best_paint_width;
    int             m_best_paint_height;
    QPoint          pc_lefttop;
    QPoint          router_lefttop;
    QPoint          internet_lefttop;

    QPoint          pcrouterlinkstate_lefttop;
    QPoint          routerinternetlinkstate_lefttop;

    QPoint          pc_end;
    //QPoint          router_end;
    QPoint          router_end_1;
    QPoint          router_end_2;
    QPoint          internet_end;

    QPoint          pc_animate_pos;
protected:
    LinkState       pcrouter_linkstate;
    LinkState       routerinternet_linkstate;

    LinkState       pcrouter_flashstate;
    LinkState       routerinternet_flashstate;

    bool            bconnecting_network;
    bool            bflash_pcrouter_link;
    bool            bflash_routerinternet_link;
    bool            bpcflash;
    bool            brouterflash;
    bool            brouterlight;
    bool            brouterlight_timeout;
    bool            binternetflash;

    unsigned int    animate_timer_ticks;
protected:
    QTimer          *animate_timer;
    QGenieTopoNodePaintStrategy *m_paint_strategy;
private:
    void draw_link(QPainter *painter,bool pc_router);
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent (QResizeEvent * event );
public:
    virtual QSize  sizeHint ()  const;
public slots:
    void begin_pcflash();
    void end_pcflash();
    void begin_routerflash();
    void end_routerflash();
    void begin_routerlight();
    void end_routerlight();
    void begin_internetflash();
    void end_internetflash();
    void begin_pcrouterlinkflash();
    void end_pcrouterlinkflash();
    void begin_routerinternetlinkflash();
    void end_routerinternetlinkflash();
    void begin_connect();
    void end_connect();
    void pcrouter_link_ok();
    void pcrouter_link_failed();
    void pcrouter_link_unknown();

    void routerinternet_link_ok();
    void routerinternet_link_failed();
    void routerinternet_link_unknown();

    void animate_timeout();
    void stop_all_animate();
public:
    void update_routerpixmap();
};

#endif // QGENIENETWORKSTATEPANEL_H

#ifndef QGENIEFRAMEMAINPROXY_H
#define QGENIEFRAMEMAINPROXY_H
#ifdef USE_MAIN_PROXY
#include <QWidget>

class QGenieFrameMainProxy : public QWidget,public Singleton<QGenieFrameMainProxy>
{
public:
    QGenieFrameMainProxy(QWidget *parent=NULL);
protected:
    virtual void	closeEvent ( QCloseEvent * event );
};
#endif
#endif // QGENIEFRAMEMAINPROXY_H

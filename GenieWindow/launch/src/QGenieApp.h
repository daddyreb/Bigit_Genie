#ifndef QGENIEAPP_H
#define QGENIEAPP_H
#include <QApplication>

class QGenieApp:public QApplication
{

public:
    QGenieApp(int & argc, char ** argv);
    virtual bool notify ( QObject * receiver, QEvent * e );
protected:
    int mCheetIdx;
    int mReloadIdx;
    int mShowPluginsIdx;
};

#endif // QGENIEAPP_H

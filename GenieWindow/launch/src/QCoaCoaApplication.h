#ifndef MACCOCOAAPPLICATION_H
#define MACCOCOAAPPLICATION_H

//#include "Application.h"
#include <QApplication>
#include <QtCore>
#include <QtGui>
class QCoaCoaApplication : public QApplication
{
    Q_OBJECT
public:
    QCoaCoaApplication( int& argc, char* argv[] );
    ~QCoaCoaApplication();
    // This method to be public due to lack of friend classes in Objective-C and
    // the lack inheritance of Objective-C classes from C++ ones.
    void dockIconClickEvent();


private:
    static QList< QShortcut* > shortcuts( QWidget* parent );
    static QList< QShortcut* > activeShortcuts( const QKeySequence& seq, bool autorep, QWidget* parent = 0);

//    QMenu m_dockMenu;


    class Private;
    Private* m_private;
protected slots:
    void slot_InstallHandler();
};

#endif


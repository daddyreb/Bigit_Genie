#ifndef QGenieRSPrint_H
#define QGenieRSPrint_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QGridLayout>
#include <QStackedLayout>
#include <QLineEdit>
#include <QGroupBox>
#include <QRadioButton>
#include "Singleton.h"

class QGenieRSPrint : public QObject,public Singleton<QGenieRSPrint>
{
    Q_OBJECT
public:
    explicit QGenieRSPrint();
    QWidget *getUi();
//    void changeEvent(QEvent *e);
    void retranslateUi();
    QWidget *mW;

public slots:
    void slot_ButBack( );
//    void slot_linkActivated(const QString &url);

};

#endif

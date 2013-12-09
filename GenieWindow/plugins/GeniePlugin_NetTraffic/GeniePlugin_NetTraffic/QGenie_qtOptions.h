#ifndef QTOPTIONS_H
#define QTOPTIONS_H

#include "Globals.h"
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtCore/QTimer>
#include <QtGui/QFrame>
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include "stdio.h"
#include "windows.h"
#include <QtCore>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QtGui/QRadioButton>

class QGenie_qtNetTraffic;

class QGenie_qtOptions : public QWidget
{
Q_OBJECT
public:
    QGenie_qtOptions(QWidget *parent = 0);


    QFrame *frame;
    QLabel *lblGeneral;
    QGroupBox *gboxUnits;
    QGroupBox *gboxShow;
    QLabel *lblUnits;
    QComboBox *comboBox;
    QLabel *label_unit;
    QComboBox *comboBox_show_unit;
    QGroupBox *gboxAdapters;
    QLabel *lblAdapter;
    QTreeWidget *treeAdapters;
    QPushButton *btnOk;
    QPushButton *btnCancel;

    QGenie_qtNetTraffic* qtNet;


    QRadioButton* rdbkb;
    QRadioButton* rdbKB;
    QRadioButton* rdbShowUnit;
    QRadioButton* rdbNotShow;


    void retranslateUi(QWidget *Widget);
    void setupUi(QWidget *Widget);
    void closeEvent ( QCloseEvent * event );
    void changeEvent(QEvent *e);

signals:

public slots:
    void okClicked(bool checked = false );
    void cancelClicked(bool checked = false);

};

#endif // QTOPTIONS_H

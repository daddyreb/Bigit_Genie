#ifndef PROFILE_H
#define PROFILE_H

#include <QtGui>
#include <QThread>
#include <QtWidgets>
#include "../../DragonNetTool/interface/dragonnettool.h"

class GetInfoThread : public QThread
{
    Q_OBJECT
public:
    GetInfoThread(QObject *parent=0);
    ~GetInfoThread();

protected:
     void run();

private:
    DragonNetTool dragon;
    QMap<QString, QMap<QString, QStringList> >  *InfoMap;

public:
    void begin_GetInfo(QMap<QString, QMap<QString, QStringList> > *Info);

signals:
    void GetInfoFinfish();
};

class Profile : public QWidget
{
    Q_OBJECT
public:
    Profile(QWidget* parent);
    ~Profile();

private:
    void retranslateUi();

private:
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QSpacerItem *horizontalSpacer;
    QTreeWidget *treeWidget;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QTreeWidgetItem *TreeRootItem;
    QTreeWidgetItem *p_nodeDetail;
    QTreeWidgetItem *c_nodeDetail;
    QTreeWidgetItem *cc_nodeDetail;

    QTextEdit* txt;
    QString line;
    int aa,bb,cc,dd;

    QUuid uuid;

    DragonNetTool* dragon;
    QMap<QString, QStringList> *ComputerInfo;
    QMap<QString, QMap<QString, QStringList> > *ComputerInfoMap;

	QMap<QString, QMap<QString, QStringList> > ComputerInfoMapNew;

  //  GetInfoThread thread;

private slots:
    void GetSysInfo();
    void Refulsh();
    void TreeWidgetCpoy(QTreeWidgetItem*,int);
    void save();
    void ViewSysInfo();
    void strcopy();
    void treeWidgetExpaned(QTreeWidgetItem*);
    void treeWidgetCollapsed(QTreeWidgetItem*);
//    void treeWidgetChange(QModelIndex);

protected:
    void changeEvent(QEvent* event);
    void saveas();
};

#endif // PROFILE_H

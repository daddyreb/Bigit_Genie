#ifndef QGENIEFRAMEDBGPLUGININFO_H
#define QGENIEFRAMEDBGPLUGININFO_H
#ifndef IN_STABLE_H
#include "QGenieFrameDbgPluginInfo.h"
#include "ui_QGenieFrameDbgPluginInfo.h"
#include <QWidget>
#include "Singleton.h"
#endif

namespace Ui {
    class QGenieFrameDbgPluginInfo;
}

class QGenieFrameDbgPluginInfo : public QWidget,public Singleton<QGenieFrameDbgPluginInfo>
{
    Q_OBJECT

public:
    explicit QGenieFrameDbgPluginInfo(QWidget *parent = 0);
    ~QGenieFrameDbgPluginInfo();

public:
    Ui::QGenieFrameDbgPluginInfo *ui;
private slots:
    void on_tableWidget_currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);
};

#endif // QGENIEFRAMEDBGPLUGININFO_H

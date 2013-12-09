#ifndef POWERLINEDEVICERLIST_H
#define POWERLINEDEVICERLIST_H

#include <QDialog>

namespace Ui {
    class PowerlineDevicerList;
}

class PowerlineDevicerList : public QDialog
{
    Q_OBJECT

public:
    explicit PowerlineDevicerList(QWidget *parent = 0);
    ~PowerlineDevicerList();
private:
	void setHeaderStyle();
	void slotConnect();

	void initializeValue();
private slots:
	void slot_BtnClose();
	void slot_BtnRefresh();
	void slot_BtnSave();
private:
    Ui::PowerlineDevicerList *ui;
};

#endif // POWERLINEDEVICERLIST_H

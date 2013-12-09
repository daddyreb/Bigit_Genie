#ifndef POWERLINEQOS_H
#define POWERLINEQOS_H

#include <QDialog>

namespace Ui {
    class PowerlineQos;
}

class PowerlineQos : public QDialog
{
    Q_OBJECT

public:
    explicit PowerlineQos(QWidget *parent = 0);
    ~PowerlineQos();
private:
	void setHeaderStyle();
	void slotConnect();

	void initializeValue();
private slots:
	void slot_BtnClose();
	void slot_BtnApply();
private:
    Ui::PowerlineQos *ui;
};

#endif // POWERLINEQOS_H

#include "PowerlineQos.h"
#include "ui_PowerlineQos.h"

PowerlineQos::PowerlineQos(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PowerlineQos)
{
    ui->setupUi(this);

	setHeaderStyle();
	slotConnect();
}

PowerlineQos::~PowerlineQos()
{
    delete ui;
}
//====================================================
//  Initialization function
//====================================================

void PowerlineQos::setHeaderStyle()
{
	const QString tmpHeaderStyle = (
		"QHeaderView::section {"
		"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgb(  240, 240,251), stop:1 rgb( 116, 113, 190));"
		"color:rgb(255,255,255);"
		"border-color:rgb( 255, 255, 255);"
		"border-style:solid;"
		"border-left-width:1px;height:25px;"
		"border-right-width:1px;"
		"}"
/*		"QHeaderView::section:checked{background-color: red;}"
		"QHeaderView::down-arrow {image: url(down_arrow.png);}"
		"QHeaderView::up-arrow {image: url(up_arrow.png);}"*/);
	//const QString tmpStyle = (
	//	"QTableWidget{"
	//	"border-style:solid;border-width:1px;"
	//	"border-color:rgb(203, 197, 235);"
	//	"}");
	ui->table_addr->horizontalHeader()->setStyleSheet(tmpHeaderStyle);

	ui->table_port->horizontalHeader()->setStyleSheet(tmpHeaderStyle);

		
}
void PowerlineQos::slotConnect()
{
	connect(ui->but_close, SIGNAL(clicked()), this, SLOT(slot_BtnClose()));
	connect(ui->but_apply, SIGNAL(clicked()), this, SLOT(slot_BtnApply()));
}


void PowerlineQos::initializeValue()
{
	//所有变量的初始化
}
//====================================================
//  Slot function
//====================================================
void PowerlineQos::slot_BtnClose()
{
	this->hide();
}

void PowerlineQos::slot_BtnApply()
{

}
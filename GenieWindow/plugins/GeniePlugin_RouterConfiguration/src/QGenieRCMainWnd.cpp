#include "src/QGenieRCMainWnd.h"
#include "ui_QGenieRCMainWnd.h"

QGenieRCMainWnd::QGenieRCMainWnd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QGenieRCMainWnd)
{
    ui->setupUi(this);
}

QGenieRCMainWnd::~QGenieRCMainWnd()
{
    delete ui;
}

#include "stable.h"

QGenieButtonSub::QGenieButtonSub(QWidget *parent) :
    QPushButton(parent),
    ui(new Ui::QGenieButtonSub)
{
    ui->setupUi(this);
}

QGenieButtonSub::~QGenieButtonSub()
{
    delete ui;
}

#include "onlineuser.h"
#include "ui_onlineuser.h"

OnlineUser::OnlineUser(unsigned long ID, QWidget *parent) :
        QWidget(parent),
        ui(new Ui::OnlineUser)
{
	m_ID = ID;
    ui->setupUi(this);
    setWindowFlags(Qt::SubWindow | Qt::FramelessWindowHint);
	ui->userNameLabel->setAlignment(Qt::AlignCenter);
	ui->computerNameLabel->setAlignment(Qt::AlignCenter);
	ui->ipLabel->setAlignment(Qt::AlignCenter);
}

OnlineUser::~OnlineUser()
{
    delete ui;
}

void OnlineUser::SetText(QString userName, QString computerName, QString IP)
{
    m_userName = userName;
    m_computerName = computerName;
    m_IP = IP;

    refurbishShow();
}

void OnlineUser::refurbishShow()
{
    ui->userNameLabel->setText(m_userName);
    ui->computerNameLabel->setText(m_computerName);
    ui->ipLabel->setText(m_IP);
}

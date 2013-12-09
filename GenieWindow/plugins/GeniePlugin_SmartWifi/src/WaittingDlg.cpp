#include "WaittingDlg.h"
#include <QHBoxLayout>

WaittingDlg::WaittingDlg()
{
	setFixedSize(300, 180);
	m_frame = new QFrame();
	m_frame->setObjectName("iframe");

	m_note = new QLabel(this);
	m_note->setText("123123");
	QHBoxLayout *noteLayout = new QHBoxLayout();
	noteLayout->setContentsMargins(15, 10, 5, 10);
	noteLayout->addWidget(m_note);

	m_ok = new QPushButton("OK");
	QHBoxLayout *btnLayout = new QHBoxLayout();
	btnLayout->setContentsMargins(0, 5, 5, 5);
	btnLayout->addStretch();
	btnLayout->addWidget(m_ok);

	QVBoxLayout *lay = new QVBoxLayout();
	lay->addLayout(noteLayout);
	lay->addLayout(btnLayout);

	QVBoxLayout *centerlay = new QVBoxLayout();
	centerlay->addWidget(m_frame);

	m_frame->setLayout(lay);
	setLayout(centerlay);

	m_ok->setEnabled(false);
	connect(m_ok, SIGNAL(clicked()), this, SLOT(accept()));
}

void WaittingDlg::SetText(const QString& text)
{
	m_note->setText(text);
}

void WaittingDlg::Finish(const QString& text)
{
	m_note->setText(text);
	m_ok->setEnabled(true);
}

void WaittingDlg::StartWaitting(const QString& text)
{
	m_note->setText(text);
	m_ok->setEnabled(false);
	exec();
}
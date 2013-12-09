#include "QGenieModalDialog.h"
#include "ui_QGenieModalDialog.h"
#include "genie2_interface.h"
#include "language_Ui.h"

QGenieModalDialog::QGenieModalDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QGenieModalDialog)
{
    ui->setupUi(this);

	ui->lab_explain->setWordWrap(true);

    ui->lab_explain->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    ui->but_ok->setText(GENIE2_GET_TEXT(L_UI_OK));
    ui->but_cancel->setText(GENIE2_GET_TEXT(L_UI_CANCEL));
}

QGenieModalDialog::~QGenieModalDialog()
{
    delete ui;
}
////////////////////////////////////////////////////////////
//        NAME: setTitle()
// DESCRIPTION: set dialog title text.
//   ARGUMENTS: const QString &title.
//     RETURNS: None.
//      AUTHOR: Chensi
//        DATA: 2012-2-17
////////////////////////////////////////////////////////////
void QGenieModalDialog::setTitle(const QString &title)
{
    ui->lab_title->setText(title);
}
////////////////////////////////////////////////////////////
//        NAME: setExplain()
// DESCRIPTION: set dialog content text.
//   ARGUMENTS: const QString &explain.
//     RETURNS: None.
//      AUTHOR: Chensi
//        DATA: 2012-2-17
////////////////////////////////////////////////////////////
void QGenieModalDialog::setExplain(const QString &explain)
{
    ui->lab_explain->setText(explain);
}

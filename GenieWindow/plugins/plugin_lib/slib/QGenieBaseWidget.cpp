#include "QGenieBaseWidget.h"
#include "IManagerHandler.h"
#include "ILanguageManager.h"
#include "uuid.h"
#include "IUiManager.h"

extern IManagerHandler *sHandler;

QGenieBaseWidget::QGenieBaseWidget(QWidget *parent) :
    QWidget(parent)
{
}

QString QGenieBaseWidget::getI18nLan(int idx)
{
    if(!sHandler)
    {
        return "no handler";
    }
    return sHandler->getLanguageManager()->getText(idx,UUID_OF_THIS);

}

void QGenieBaseWidget::setBigButtonStyle(const QString & s)
{
    if(sHandler)
    {
        sHandler->getUiManager()->setBigButtonUserStateText(s,UUID_OF_THIS);
    }
}

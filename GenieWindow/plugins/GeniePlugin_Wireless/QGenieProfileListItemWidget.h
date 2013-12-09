#ifndef QGENIEPROFILELISTITEMWIDGET_H
#define QGENIEPROFILELISTITEMWIDGET_H

#include <QFrame>
#include <QPixmap>

namespace Ui {
    class QGenieProfileListItemWidget;
}

class QGenieProfileListItemWidget : public QFrame
{
    Q_OBJECT

public:
    explicit QGenieProfileListItemWidget(QWidget *parent = 0);
    ~QGenieProfileListItemWidget();

private:
    Ui::QGenieProfileListItemWidget *ui;
    bool                            m_connType;
public:
    void setProfileConnType(bool binfrastructure);
    void setProfileName(const QString &name);
    void setProfileSecurity(const QString &security);

    bool profileConnType();

    void exchangeUi(QGenieProfileListItemWidget *itemWidget);
};

#endif // QGENIEPROFILELISTITEMWIDGET_H

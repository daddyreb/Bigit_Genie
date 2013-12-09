#ifndef POWERLINEPAGE_H
#define POWERLINEPAGE_H

#include <QWidget>

namespace Ui {
class PowerLinePage;
}

class PowerLinePage : public QWidget
{
    Q_OBJECT
    
public:
    explicit PowerLinePage(QWidget *parent = 0);
    ~PowerLinePage();
    
public:
    bool refresh();
    bool dataAvailable();
    void verifyStart();

    void notifyRaise() const;
    void notifyLower() const;
    void notifyInternetState(bool connected);

    void hideHubView();
    void prevPage();
    void nextPage();
    void firstPage();
    void lastPage();
    bool isCurFisrtPage();
    bool isCurLastPage();
    int pageCount();
    int curPage();
signals:
    void refreshOver();
    void pageChange(int);
    void queryPLCDevicePassword(const QString &devMac, QString *pwd);
    void storePLCDevicePassword(const QString &devMac, const QString &pwd);

private slots:
    void onRefreshOver();
    void onPageChange(int count);

private:
    Ui::PowerLinePage *ui;
    int m_totalPage;
    int m_curPageIndex;

private:
    bool m_withPendingRefreshReq;
    bool m_isStarted;
};

#endif // POWERLINEPAGE_H

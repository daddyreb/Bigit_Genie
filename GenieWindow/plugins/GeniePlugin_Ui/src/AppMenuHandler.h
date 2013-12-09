#ifndef APPMENUHANDLER_H
#define APPMENUHANDLER_H

#include <QObject>
class QAction;
class QMenuBar;
//class QGeniePrefrencesPanel;
class QGeniePrivilegeProcess;
class QProcess;

class AppMenuHandler : public QObject
{
    Q_OBJECT
private:
    enum
    {
        APPMENU_ABOUT,
        APPMENU_CHECKFORUPDATE,
        APPMENU_PREFERENCES,
        APPMENU_QUIT
    };
private:
    explicit AppMenuHandler(QObject *parent = 0);
    AppMenuHandler(const AppMenuHandler &);
    AppMenuHandler & operator = (const AppMenuHandler&);

    ~AppMenuHandler();
private:
    QMenuBar *m_appMenuBar;
    QAction  *m_actAbout;
    QAction  *m_actUpdate;
private:
//    static QGeniePrefrencesPanel *m_prefrencesPanel;
    static AppMenuHandler *g_handler;
    QProcess *mProcess;
signals:

public slots:
    void triggered(QAction *act);
    void retranslateUi();
//    void slot_SettingButClicked();

public:
    static void installAppMenu();
    static void uninstallAppMenu();
//    static void s_retranslateUi();
private:
    void about();
    void checkForUpdate();
    void prefrences();
    void quit();
};

#endif // APPMENUHANDLER_H

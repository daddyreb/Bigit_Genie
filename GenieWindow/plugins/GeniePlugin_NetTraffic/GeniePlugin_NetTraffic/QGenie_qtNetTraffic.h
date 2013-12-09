#ifndef QTNETTRAFFIC_H
#define QTNETTRAFFIC_H

#include "Globals.h"
#include <QtGui/QWidget>
#include <QtGui/QLineEdit>
#include <QtGui/QGridLayout>
#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QMutex>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QtGui/QTreeWidget>
#include <QtGui/QColor>
#include "windows.h"
#include <QtCore>
#include <QString>
#include <QStringList>
#include <QMap>
#include "limits.h"
#include <QPushButton>
#include <QFrame>
#include <QComboBox>
#include <QSystemTrayIcon>

#include "QGenie_MFNetTraffic.h"
//#include "QGenie_qtNetTraffic.h"
#include "IManagerHandler.h"
#include "ILogManager.h"

class QGenie_qtOptions;
class QGenie_qtNetStat;
class QGenie_qtGraphColor;
class QGenieAboutThread;

class QGenie_qtNetTraffic : public QWidget
{
    Q_OBJECT

public:
    QGenie_qtNetTraffic(QWidget *parent = 0);

    ~QGenie_qtNetTraffic();

    QGenie_qtOptions* qOpt;
    QGenie_qtNetStat* qNetStat;
    QGenie_qtGraphColor* qColor;

    bool bLog;
    void Genie_Log(QString s);
    QLabel* txtInput;
    QLabel* txtOutput;
    QLabel* lblInput;
    QLabel* lblOutput;

    QLabel* lblAlert;
    QLabel* txtAlert;


    QLabel* lbl_internet_input;
    QLabel* lbl_internet_output;

    //QLabel* lblDraw;
    QGridLayout* grdLayOut;

    QFrame* qfrPtn;
    QPushButton* ptnMin;
    QPushButton* ptnClose;
    QPushButton* ptnNormal;

    QSystemTrayIcon *trayIcon;
   // QMenu *trayIconMenu;


    QColor qclrReceive;
    QColor qclrSent;
    QColor qclrTotal;
    QColor qclrGraphBg;
    QColor qclrWindowBg;
    double dbOpt;

    QString strMenuNormal;
    QString strMenuFont;
    QString strMenuNetchoose;
    QString strMenuGraphColor;
    QString strMenuNetStat;

    QString strMenuTroy;


    QString strTxtInput;
    QString strTxtOutput;

    QAction* actTrayNormal;
    QAction* actTrayFont;
    QAction* actTrayNetchoose;
    QAction* actTrayGraphColor;
    QAction* actTrayNetStat;

    QMenu* qmenuTroy;

    int iDrawCount;


    int iunit;
    int iShowUnit;
    int infChoose[3];

    bool bOptPoped;
    bool bColorPoped;
    bool bStatPoped;

    bool bNetOk;

    void startGetDataThread();

    void initialTxt();
    void mousePressEvent ( QMouseEvent * event );
    void mouseDoubleClickEvent ( QMouseEvent * event ) ;
    void  mouseMoveEvent ( QMouseEvent * event ) ;
    void  mouseReleaseEvent ( QMouseEvent * event );
    void initTreeWithInterfaces(QTreeWidget* qtree);
    void resizeEvent ( QResizeEvent * event );
    void  leaveEvent ( QEvent * event );
    void enterEvent ( QEvent * event  );
    void showEvent ( QShowEvent * event );
    QRect qrectRestored;
    void setComboBoxCurIndex(QComboBox* qcmb,QColor qclr);
    void setComboBoxCurIndex(QComboBox* qcmb,double qopt);
    void createTrayIcon();

    void InitTreeCheckState(QTreeWidget* treeAdapter);
    void InitTreeCheckState(QComboBox* cmbUnit);
    void InitcmbCheckState();
    void retranslateUi();

    void changeEvent(QEvent *e);


public:
        void SetAdaptiveScaling(BOOL adaptive);
        void SetInterfaceNumber(int interfacenumber);
        enum traffictype
        {
                Traffic_Total		= 0,
                Traffic_Incoming	= 1,
                Traffic_Outgoing	= 2
        };

        void SelectTrafficType(int trafficType);
        //void SetInterfaceNumberNotificationFunction(INTERFACECHANCEDPROC callfunct);
        //void SetUpdateSpeed(UINT netspeed, UINT gridspeed);
        //void ReInit(int newInterface);
        //void ReInit(RECT newSize);
        //virtual ~MFTrafficButton();

        // Generated message map functions
protected:
        //{{AFX_MSG(MFTrafficButton)
        //afx_msg void OnTimer(UINT nIDEvent);
        //afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
        //}}AFX_MSG

        //DECLARE_MESSAGE_MAP()
public:
        void InterfaceHasChanged();

        //INTERFACECHANCEDPROC interfaceCallBack;


        QGenie_MFNetTraffic m_cTrafficClass[3];
        QGenie_MFNetTraffic m_cTrafficClass1[3];
        QGenie_MFNetTraffic m_cTrafficClass2[3];

        //CFont	smallFont;
        //CBrush	colorbrush;

        //COLORREF red, green, back, cyan, darkblue, darkgray, white, black, lightgreen, darkgreen;
        //CBitmap	colorbrushbmp;
        //CPen	GridPen;
        //CSize	TGSize;

        //CBrush	greenbrush;
        //CBrush	redbrush;
        //CPoint	orgBrushOrigin;

        //RECT	TrafficDrawRectangle;
        //RECT	TrafficDrawUpdateRectangle;

        QString CurrentTraffic;
        QString MaximalTraffic;
        QString AllTraffic;

        DWORD	TrafficEntries;

        BOOL	initalized;
        BOOL	isOnline;
        BOOL	brushInitalized;

        //CRgn	ShapeWNDRegion;
        //CRgn	ShapeDCRegion;

        double	MaxTrafficAmount;
        QString	ToggleStatusText;

        int SelectedInterface;

        BOOL useAdaptiveScale;

        TRAFFICENTRY* TrafficStats;
        TRAFFICENTRY* TrafficStatsIncoming;
        TRAFFICENTRY* TrafficStatsOutgoing;

        int gridxstartpos;
        int gridystartpos;
        int plotgranularity;

        QTimer* Timerdraw;
        int ICountTimer;

public:
        int gridxresolution;		// 网格大小
        int gridyresolution;
        int gridscrollxspeed;		// 网格滚动速度
        int gridscrollyspeed;
        int netupdatespeed;			// 网络更新速度
        int gridupdatespeed;
        QPainter* painter;
        QPixmap* pixmap;
        QMutex mutex;
        int leftkeypressed;
        int xMouse;
        int yMouse;
        int iMode;
        // QPainter* painter1;
        bool bFontBold;
        QGenieAboutThread* thd;



public:
    static IManagerHandler *sHandler;
    void paintEvent ( QPaintEvent * event )  ;

protected slots:
    void TimerDrawtimeout () ;
    void popMenuClicked(QAction* action);

    void minClicked(bool checked=false);
    void closeClicked(bool checked=false);
    void normalClicked(bool checked=false);
    void messageClicked();

    void drawGraphic();
public slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason,bool toShow);




};

#endif // QTNETTRAFFIC_H

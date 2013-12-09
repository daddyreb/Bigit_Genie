#ifndef DEVICEDETAILVIEW_H
#define DEVICEDETAILVIEW_H

#include <QFrame>
#include <QList>
#include <QPair>

#include "device.h"

class QPushButton;
class QLineEdit;
class QComboBox;
class QLabel;
class QHBoxLayout;
class QGridLayout;
class QPixmap;
#ifdef RETRANSLATE_PROPERTYS
#define OBJECT_PROPERTY_FORTRANSLATE "{trans{kdb5dfjh}}"
#endif
class DeviceDetailView : public QFrame
{
    Q_OBJECT
public:
    explicit DeviceDetailView(QWidget *parent = 0);
    ~DeviceDetailView();
protected:
    QLabel          *m_whatisitLabel;
    QHBoxLayout     *m_whatisitLayout;
    QPushButton     *m_modifyApplyBtn;
    QPushButton     *m_closeBtn;
#ifdef BLOCK_DEVICE
    QPushButton     *m_blockDeviceBtn;
#endif

    QLabel          *m_nameLabel;
    QLabel          *m_nameContentLabel;
    QLineEdit       *m_nameContentEdit;
    QHBoxLayout     *m_nameSwitchLayout;

    QLabel          *m_typeLabel;
    QLabel          *m_typeContentLabel;


    QComboBox       *m_typeContentCB;

    QHBoxLayout     *m_typeSwitchLayout;
    QLabel          *m_ConnectionLabel;
    QLabel          *m_ConnectionContenteLabel;	
    QComboBox       *m_ConnectionContentCB;	
    QHBoxLayout     *m_ConnectionSwitchLayout;
    QHBoxLayout     *m_btnsLayout;
//    QHBoxLayout     *m_ConnectionSwitchLayout;

    QList<QPair<QLabel *,QLabel *> > m_labelContentPairList;
    QGridLayout     *m_proLayout;
    QWidget         *m_proPanel;
protected:
    Device          m_device;
    QList<int>      m_proList;
    bool            m_isModifying;
    bool            m_canModify;
    bool            m_selectedDeviceTypeCanModify;
    int             m_curSelectedDevType;
#ifdef RETRANSLATE_PROPERTYS
    QList<QLabel *> m_transLabels;
#endif
public:
    void viewDeviceDetails(const Device &dev);

protected:
    virtual void closeEvent(QCloseEvent *);
    virtual void changeEvent(QEvent *);

private:
    //Property label and cotent QLabels list manger
    QPair<QLabel*,QLabel*> getLabelPair(int idx);
    void verifyLabelPairVisiblity(int from);
    void releasePairList();
    //----------------end--------------------------
private:
    void applyUiElementsStyle();
    void retranslateUi();
    void retranslateDeviceTypesList();
    void retranslateConnectionTypeList();
    QString translateText(int idx, const QString &defaultText = QString());

private:
#ifdef BLOCK_DEVICE
    void verifyBlockDeviceUi();
    void retranslateBlockDeviceUi();
#endif
    void prepareShowUi();
    void prepareModifyUiElements();
    void preparePersistUiElements();
    void initDeviceTypesCB();
    void initDeviceConnectionTypeCB();

    void layoutUi(int devType ,bool needUpdateNameLineEdit = false);
    void prepareLayouts();
    void reclaimLayouts();
    void prepareUiElementsVisiblity(const QList<int> &proList);
    void updateTitleAndLogo(int devType);

    bool verifyModify();
signals:
    void viewDetailsOver(const Device &dev ,bool modified);

    void requestDisplayNewTitle(const QString &title);
    void requestDisplayNewLogo(const QPixmap &logo);

    void translateText(int idx ,QString *text);
    void requestRouterImagePath(const QString &model,int imageRole,QString *imagePath);
private slots:
    void btnClicked();
    void currentIndexChanged(int);
public slots:
    void onNewData(const QList<Device *> &data);
    void excuteViewDetailsOver(bool modified = false);

};

#endif // DEVICEDETAILVIEW_H

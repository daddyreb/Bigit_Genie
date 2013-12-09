#include "devicedetailview.h"
#include "mapdevicetype.h"
#include "mapuitilities.h"

#include "language_Map.h"
#include "detailviewcontentwidget.h"

#ifdef BLOCK_DEVICE
#include "blockdevicemanager.h"
#include "topology.h"
#endif

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

#include <QLabel>
#include <QPushButton>
#include <QCloseEvent>
#include <QLineEdit>
#include <QComboBox>
#include <QFont>
#include <QMap>
#include <QList>
#include <QtAlgorithms>
#include <QtDebug>
#include <QScrollArea>


#define CUSTOMNAME_MAXIMLENGTH 53
#define DEVICE_EMPTYITEM "N/A"
const int cLanParas[]={L_MAP_WIRED,L_MAP_WIRELESS,L_MAP_AUTO};

bool devicetypeLessThan(const QString str1,const QString str2)
{
    if(!str1.isEmpty() && str1[0].isLower() && !str2.isEmpty() && str2[0].isUpper())
    {
        return true;
    }
    else if((!str1.isEmpty() && str1[0].isUpper() && !str2.isEmpty() && str2[0].isLower()))
    {
        return false;
    }

    return (str1 < str2);
}

DeviceDetailView::DeviceDetailView(QWidget *parent) :
        QFrame(parent),
        m_whatisitLabel(0),
        m_whatisitLayout(0),
        m_btnsLayout(0),
        m_proLayout(0),
        m_isModifying(false),
        m_canModify(false),
        m_selectedDeviceTypeCanModify(m_canModify),
        m_curSelectedDevType(MDT_DEVICETYPE_COUNT)
{
    preparePersistUiElements();

    prepareModifyUiElements();

    applyUiElementsStyle();
	//this->setStyleSheet("background-color:yellow");
}

DeviceDetailView::~DeviceDetailView()
{
    reclaimLayouts();
}

void DeviceDetailView::prepareModifyUiElements()
{
    m_nameSwitchLayout = 0;
    m_typeSwitchLayout = 0;
    m_ConnectionSwitchLayout =0;


    m_nameLabel         = new QLabel(this);
    m_nameContentLabel  = new QLabel(this);
    m_nameContentEdit   = new QLineEdit(this);

    m_nameLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_nameContentLabel->setWordWrap(true);
    m_nameContentLabel->setTextFormat(Qt::PlainText);
    m_nameContentEdit->setVisible(false);
    m_nameContentEdit->setMaxLength(CUSTOMNAME_MAXIMLENGTH);


    m_typeLabel         = new QLabel(this);
    m_typeContentLabel  = new QLabel(this);
    m_ConnectionLabel   = new QLabel(this);
    m_ConnectionContenteLabel= new QLabel(this);

    m_typeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_typeContentLabel->setWordWrap(true);
    m_ConnectionContenteLabel->setWordWrap(true);
    m_ConnectionLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
//    m_ConnectionLabel->setText("bbbbb");
//    m_ConnectionContenteLabel->setText("aaaaa");
    initDeviceTypesCB();
    initDeviceConnectionTypeCB();
}

void DeviceDetailView::initDeviceTypesCB()
{
    m_typeContentCB = new QComboBox(this);
    m_typeContentCB->setVisible(false);


//    QFont font;
//    font.setPointSize(10);
//    m_typeContentCB->setFont(font);


    QMap<QString,int> textDeviceTypeMap;

    for(int k = 1 ; k < MDT_DEVICETYPE_COUNT ; ++k)
    {
        textDeviceTypeMap[getDeviceTypeText(k)] = k;
    }

    QList<QString> keysList = textDeviceTypeMap.keys();
    qSort(keysList.begin(),keysList.end(),devicetypeLessThan);

    for(int idx = 0 ; idx < keysList.size() ; ++idx)
    {
        m_typeContentCB->addItem(keysList[idx],textDeviceTypeMap[keysList[idx]]);
    }

    connect(m_typeContentCB,SIGNAL(currentIndexChanged(int)),
            this,SLOT(currentIndexChanged(int)));
}

void DeviceDetailView::initDeviceConnectionTypeCB()
{
    m_ConnectionContentCB=new QComboBox(this);
    m_ConnectionContentCB->setVisible(false);
    const char *cParas[]={"auto","wired","wireless"};
    const int cParas2[]={MCT_AUTO,MCT_WIRED,MCT_WIRELESS};
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        m_ConnectionContentCB->addItem(cParas[i],cParas2[i]);
    }
}

void DeviceDetailView::preparePersistUiElements()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);

    m_proPanel = new DetailViewContentWidget(this);
    m_proPanel->setFocusPolicy(Qt::NoFocus);

    m_modifyApplyBtn = new QPushButton(this);
    m_modifyApplyBtn->setAutoDefault(true);
#ifdef BLOCK_DEVICE
    m_blockDeviceBtn = new QPushButton(this);
    connect(m_blockDeviceBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    m_blockDeviceBtn->setAutoDefault(true);
#endif
    m_closeBtn = new QPushButton(this);
    m_closeBtn->setAutoDefault(true);




    connect(m_modifyApplyBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(m_closeBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));

    m_btnsLayout = new QHBoxLayout();
    m_btnsLayout->setSpacing(20);
    m_btnsLayout->addStretch(1);
    m_btnsLayout->addWidget(m_modifyApplyBtn);
#ifdef BLOCK_DEVICE
    m_btnsLayout->addWidget(m_blockDeviceBtn);
#endif
    m_btnsLayout->addWidget(m_closeBtn);
    m_btnsLayout->addStretch(1);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setFocusPolicy(Qt::NoFocus);
    scrollArea->setWidget(m_proPanel);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShadow(QFrame::Plain);
    scrollArea->setFrameShape(QFrame::NoFrame);

    layout->addWidget(scrollArea);
    layout->addLayout(m_btnsLayout);
}


void DeviceDetailView::prepareLayouts()
{

    reclaimLayouts();

    m_proLayout = new QGridLayout(m_proPanel);
    m_proLayout->setContentsMargins(0,0,0,0);
    m_proLayout->setVerticalSpacing(4);

    m_nameSwitchLayout = new QHBoxLayout();
    m_nameSwitchLayout->setSpacing(0);
    m_nameSwitchLayout->setContentsMargins(0,0,0,0);
    m_nameSwitchLayout->addWidget(m_nameContentLabel);
    m_nameSwitchLayout->addWidget(m_nameContentEdit);

    m_typeSwitchLayout = new QHBoxLayout();
    m_typeSwitchLayout->setSpacing(0);
    m_typeSwitchLayout->setContentsMargins(0,0,0,0);
    m_typeSwitchLayout->addWidget(m_typeContentLabel);
    m_typeSwitchLayout->addWidget(m_typeContentCB);


    m_ConnectionSwitchLayout= new QHBoxLayout();
    m_ConnectionSwitchLayout->setSpacing(0);
    m_ConnectionSwitchLayout->setContentsMargins(0,0,0,0);
    m_ConnectionSwitchLayout->addWidget(m_ConnectionContenteLabel);
    m_ConnectionSwitchLayout->addWidget(m_ConnectionContentCB);

    m_whatisitLayout = new QHBoxLayout();
    m_whatisitLayout->setSpacing(2);
    m_whatisitLayout->setContentsMargins(0,0,0,0);
}

void DeviceDetailView::reclaimLayouts()
{

    delete m_nameSwitchLayout;
    m_nameSwitchLayout = 0;

    delete m_typeSwitchLayout;
    m_typeSwitchLayout = 0;

    delete m_whatisitLayout;
    m_whatisitLayout = 0;

    delete m_ConnectionSwitchLayout;
    m_ConnectionSwitchLayout=0;

    delete m_proLayout;
    m_proLayout = 0;

}

void DeviceDetailView::prepareUiElementsVisiblity(const QList<int> &proList)
{
    m_nameLabel->setVisible(m_canModify && proList.contains(DP_NAME));
    m_nameContentLabel->setVisible((!m_isModifying) && m_canModify && proList.contains(DP_NAME));
    m_nameContentEdit->setVisible(m_isModifying && proList.contains(DP_NAME));

    m_typeLabel->setVisible(m_canModify && proList.contains(DP_TYPE));
    m_typeContentLabel->setVisible((!m_isModifying) && m_canModify && proList.contains(DP_TYPE));
    m_typeContentCB->setVisible(m_isModifying && proList.contains(DP_TYPE));

    m_ConnectionLabel->setVisible(m_canModify && proList.contains(DP_ISWIRED));
     m_ConnectionContenteLabel->setVisible(m_canModify &&!m_isModifying
                                           &&proList.contains(DP_ISWIRED));
    m_ConnectionContentCB->setVisible(m_canModify &&m_isModifying&& proList.contains(DP_ISWIRED));


    m_modifyApplyBtn->setVisible(m_canModify && m_selectedDeviceTypeCanModify);

}


void DeviceDetailView::layoutUi(int devType ,bool needUpdateNameLineEdit)
{
#ifdef RETRANSLATE_PROPERTYS
    m_transLabels.clear();
#endif
    m_curSelectedDevType = devType;
    m_selectedDeviceTypeCanModify = !(m_device[DP_ISDEFAULTGATEWAY].toBool() ||
                                      m_device[DP_ISLOCALHOST].toBool());

    m_proList = m_device.getDetailPropertyList(devType);

    prepareLayouts();
    prepareUiElementsVisiblity(m_proList);
    if(m_whatisitLabel)
    {
        m_whatisitLabel->setVisible(false);
    }


    QPair<QLabel*,QLabel*> labelPair;
    int k = 0;
    for(int idx = 0 ; idx < m_proList.size() ; ++idx)
    {
        if(/*m_canModify*/m_selectedDeviceTypeCanModify &&
                          (m_proList[idx] == DP_TYPE || m_proList[idx] == DP_NAME ||m_proList[idx]==DP_ISWIRED))
        {
            if(m_proList[idx] == DP_NAME)
            {
                m_proLayout->addWidget(m_nameLabel,idx,0,1,1);

                QString name = m_device[DP_CUSTOMNAME].toString();
                if(name.isEmpty())
                {
                    name = m_device[DP_NAME].toString();
                }

                if(needUpdateNameLineEdit){
                    m_nameContentEdit->setText(name);
                }

                if(name.isEmpty())
                {
                    name = m_device[DP_IP].toString();
                }

                m_nameContentLabel->setText(name.isEmpty() ? DEVICE_EMPTYITEM : name);

                m_proLayout->addLayout(m_nameSwitchLayout,idx,1,1,1);
            }
            else if(m_proList[idx]==DP_TYPE)
            {
                m_proLayout->addWidget(m_typeLabel,idx,0,1,1);

                m_typeContentLabel->setText(m_typeContentCB->currentText());

                m_proLayout->addLayout(m_typeSwitchLayout,idx,1,1,1);

           }
            else
            {
                Q_ASSERT(m_proList[idx]==DP_ISWIRED);
                m_proLayout->addWidget(m_ConnectionLabel,idx,0,1,1);
                int wiredInShowing        = m_device.parseWiredInShowing();
                wiredInShowing=wiredInShowing%(sizeof(cLanParas)/sizeof(cLanParas[0]));
                m_ConnectionContenteLabel->setText(translateText(cLanParas[wiredInShowing]));

                m_proLayout->addLayout(m_ConnectionSwitchLayout,idx,1,1,1);

            }

        }
        else
        {
            labelPair = getLabelPair(k++);
            m_proLayout->addWidget(labelPair.first,idx,0,1,1);
            QString strItem;
#ifdef RETRANSLATE_PROPERTYS
            if(m_proList[idx]==DP_STATUS)
            {
                labelPair.second->setProperty(OBJECT_PROPERTY_FORTRANSLATE,
                                              m_device[DP_STATUS].toBool()?L_MAP_ONLINE:L_MAP_OFFLINE);
                m_transLabels.append(labelPair.second);
            }
            else
#endif
            {
                strItem = m_device.getDetailPropertyContent(m_proList[idx]);
            }



            if(((m_proList[idx] == DP_IP) && (m_device.deviceIpShouldLink(devType))) || (m_proList[idx] == DP_UPNP_MANUFACTURERURL))
            {
                if(!strItem.isEmpty())
                {
                    if(strItem.contains("http://",Qt::CaseInsensitive))
                    {
                        strItem = QString("<a href=\"") + strItem
                                  + "\">" + strItem + "</a>";
                    }
                    else
                    {
                        strItem = QString("<a href=\"http://") + strItem
                                  + "\">" + strItem + "</a>";
                    }

                    labelPair.second->setTextInteractionFlags(Qt::TextBrowserInteraction);
                    labelPair.second->setOpenExternalLinks(true);
                }
            }

            if(strItem.isEmpty())
            {
                strItem = DEVICE_EMPTYITEM;
            }

            labelPair.second->setText(strItem);

            if(m_proList[idx] == DP_ATTACHDEVICE_LINKRATE)
            {
                if(0 == m_whatisitLabel)
                {
                    m_whatisitLabel = new QLabel(this);
                    m_whatisitLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
                    m_whatisitLabel->setOpenExternalLinks(true);
                }
                m_whatisitLabel->setVisible(true);
                m_whatisitLayout->addWidget(labelPair.second);
                m_whatisitLayout->addWidget(m_whatisitLabel);

                m_proLayout->addLayout(m_whatisitLayout,idx,1,1,1);
            }
            else
            {
                m_proLayout->addWidget(labelPair.second,idx,1,1,1);
            }
        }
    }//for

    verifyLabelPairVisiblity(k);

    updateTitleAndLogo(devType);

    retranslateUi();
    //updateGeometry();
}


void DeviceDetailView::updateTitleAndLogo(int devType)
{
    QString imagePath;

    if(devType == MDT_ROUTER)
    {
        QString model=m_device[DP_CURRENTSETTING_MODEL].toString();
        if(model.isEmpty())
        {
            model=m_device[DP_UPNP_MODEL].toString();
        }
//       model = "wnr2000v4"; //yankai debug and test
        emit requestRouterImagePath(model,DTIR_DETAILVIEWICON,&imagePath);
    }
    if(imagePath.isEmpty())
    {
        imagePath = (m_device[DP_ISLOCALHOST].toBool() ?
                     getLocalHostImagePath(DTIR_DETAILVIEWICON) :
                     getDeviceTypeImagePath(devType,DTIR_DETAILVIEWICON));
    }
qDebug()<<"---imagePath2:"<<imagePath;
    emit requestDisplayNewLogo(imagePath);

    QString title = m_device[DP_CUSTOMNAME].toString();
    if(title.isEmpty())
    {
        title = m_device[DP_NAME].toString();
        if(title.isEmpty())
        {
            title = m_device[DP_IP].toString();
        }
    }

    emit requestDisplayNewTitle(title);
}

void DeviceDetailView::btnClicked()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());

    if((btn == m_modifyApplyBtn) && m_canModify)
    {
        if(m_isModifying)
        {
            bool changed = verifyModify();

            excuteViewDetailsOver(changed);
        }
        else
        {
            m_isModifying = true;

            m_nameContentLabel->setVisible(false);
            m_nameContentEdit->setVisible(m_isModifying && m_device.getDetailPropertyList(m_device.parseType()).contains(DP_NAME));

            m_typeContentLabel->setVisible(false);
            m_typeContentCB->setVisible(m_isModifying && m_device.getDetailPropertyList(m_device.parseType()).contains(DP_TYPE));

            m_ConnectionContenteLabel->setVisible(false);
            m_ConnectionContentCB->setVisible(m_isModifying && m_device.getDetailPropertyList(m_device.parseType()).contains(DP_ISWIRED));

            retranslateUi();
        }
    }
#ifdef BLOCK_DEVICE
    else if(btn == m_blockDeviceBtn)
    {
        BlockDeviceManager *bdm = BlockDeviceManager::GetSingletonPtr();
        bool bok = false;
        int blocked = m_device[DP_ATTACHDEVICE_BLOCKED].toInt(&bok);
        if(bdm && bok && (blocked == 1))
        {
            bdm->enableDevice(m_device[DP_PHYSICALADDR].toString());
        }
        else if(bdm && bok && (blocked == 0))
        {
            bdm->disableDevice(m_device[DP_PHYSICALADDR].toString());
        }

        excuteViewDetailsOver();
    }
#endif
    else
    {
        excuteViewDetailsOver();
    }
}


void DeviceDetailView::currentIndexChanged(int idx)
{
    if(idx != -1)
    {
        int devType = m_typeContentCB->itemData(idx).toInt();
        layoutUi(devType);
    }
}

bool DeviceDetailView::verifyModify()
{
    bool changed = false;

    QString oldName = m_device[DP_CUSTOMNAME].toString();
    if(oldName.isEmpty())
    {
        oldName = m_device[DP_NAME].toString();
    }

    if(oldName != m_nameContentEdit->text())
    {
        m_device[DP_CUSTOMNAME] = m_nameContentEdit->text();
        m_nameContentLabel->setText(m_nameContentEdit->text());

        changed = true;
    }

    int oldType = m_device.parseType();

    if(oldType != m_typeContentCB->itemData(m_typeContentCB->currentIndex()).toInt())
    {
        m_device[DP_CUSTOMTYPE] = m_typeContentCB->itemData(m_typeContentCB->currentIndex()).toInt();
        m_typeContentLabel->setText(m_device.getDetailPropertyContent(DP_CUSTOMTYPE));

        changed = true;
    }

    int oldConnectionType=m_device.parseWiredInCB();
    if(oldConnectionType!=m_ConnectionContentCB->itemData(
            m_ConnectionContentCB->currentIndex()).toInt())
    {
        m_device[DP_CUSTOMWIRED]=m_ConnectionContentCB->itemData(
                m_ConnectionContentCB->currentIndex()).toInt();
        changed=true;

    }


    return changed;
}

void DeviceDetailView::excuteViewDetailsOver(bool modified)
{
    if(modified)
    {
        if(!m_device[DP_CUSTOMNAME].toString().isEmpty())
        {
            qDebug () << "New m_deviceice name is " << m_device[DP_CUSTOMNAME].toString();

        }

        if(m_device[DP_CUSTOMTYPE].toInt() != MDT_DEVICETYPE_COUNT)
        {
            qDebug () << "New m_deviceice type is " << getDeviceTypeText(m_device[DP_CUSTOMTYPE].toInt());

        }
    }
    emit viewDetailsOver(m_device,modified);
}

void DeviceDetailView::closeEvent(QCloseEvent *event)
{
    excuteViewDetailsOver();
    event->accept();
}


void DeviceDetailView::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        retranslateDeviceTypesList();
        retranslateConnectionTypeList();
        retranslateUi();
#ifdef BLOCK_DEVICE
        retranslateBlockDeviceUi();
#endif
    }

    QFrame::changeEvent(event);
}

#ifdef BLOCK_DEVICE
void DeviceDetailView::verifyBlockDeviceUi()
{
    QList<int> items = m_device.getDeviceMenuItems();
    BlockDeviceManager *bdm = BlockDeviceManager::GetSingletonPtr();
    bool brunning = (bdm && (bdm->state()==BLOCKDEVICE_FSM::FS_RUNNING
                             || bdm->state()==BLOCKDEVICE_FSM::FS_RUNNING_ADMIN) &&
                     (items.contains(DMI_BLOCK_ENABLE) || items.contains(DMI_BLOCK_DISABLE)));
//    if(brunning)
//    {
//        m_btnsLayout->setSpacing(8);
//    }
//    else
//    {
//        m_btnsLayout->setSpacing(16);
//    }
    m_blockDeviceBtn->setVisible(brunning);

    retranslateBlockDeviceUi();
}

void DeviceDetailView::retranslateBlockDeviceUi()
{
    QList<int> items = m_device.getDeviceMenuItems();
    BlockDeviceManager *bdm = BlockDeviceManager::GetSingletonPtr();

    if(bdm && (bdm->state()==BLOCKDEVICE_FSM::FS_RUNNING
               || bdm->state()==BLOCKDEVICE_FSM::FS_RUNNING_ADMIN) &&
       (items.contains(DMI_BLOCK_ENABLE) || items.contains(DMI_BLOCK_DISABLE)))
    {
        if(items.contains(DMI_BLOCK_ENABLE))
        {
            m_blockDeviceBtn->setText(translateText(L_MAP_BLOCKDEVICE_NOBLOCK,"Enable..."));
        }
        else if(items.contains(DMI_BLOCK_DISABLE))
        {
            m_blockDeviceBtn->setText(translateText(L_MAP_BLOCKDEVICE_BLOCK,"Disable..."));
        }
    }
}

#endif

void DeviceDetailView::prepareShowUi()
{

    int deviceType = m_device.parseType();
    int connectionType=m_device.parseWiredInCB();

    m_selectedDeviceTypeCanModify = m_canModify = !(m_device[DP_ISDEFAULTGATEWAY].toBool() ||
                                                    m_device[DP_ISLOCALHOST].toBool());

    if(m_canModify &&
       m_device.getDetailPropertyList(deviceType).contains(DP_TYPE) &&
       (m_typeContentCB->findData(deviceType) != m_typeContentCB->currentIndex()))
    {
        //layoutUi may also be called in currentIndexChanged as a side affect,so disable it
        disconnect(m_typeContentCB,SIGNAL(currentIndexChanged(int)),
                   this,SLOT(currentIndexChanged(int)));

        m_typeContentCB->setCurrentIndex(m_typeContentCB->findData(deviceType));



        m_ConnectionContentCB->setCurrentIndex(
                m_ConnectionContentCB->findData(connectionType)
                );

        connect(m_typeContentCB,SIGNAL(currentIndexChanged(int)),
                this,SLOT(currentIndexChanged(int)));
    }

    layoutUi(deviceType,true);


    m_isModifying = false;
    retranslateUi();

    //added by taoyidong do disable focus navigation of these qlabel
    QList<QLabel *> lab_lst=this->findChildren<QLabel *>();
    int c=lab_lst.count();
    for(int i=0;i<c;i++)
    {
        lab_lst[i]->setFocusPolicy(Qt::NoFocus);
    }
}

void DeviceDetailView::viewDeviceDetails(const Device &dev)
{
    m_device = dev;

    retranslateDeviceTypesList();
    retranslateConnectionTypeList();
    retranslateUi();
#ifdef BLOCK_DEVICE
    verifyBlockDeviceUi();
#endif
    prepareShowUi();
}


void DeviceDetailView::onNewData(const QList<Device *> &data)
{
    foreach(Device *dev , data)
    {
        if((*dev)[DP_PHYSICALADDR].toString() == m_device[DP_PHYSICALADDR].toString())
        {
            m_device = *dev;
#ifdef BLOCK_DEVICE
            verifyBlockDeviceUi();
#endif
            prepareShowUi();
            break;
        }
    }
}

QPair<QLabel*,QLabel*> DeviceDetailView::getLabelPair(int idx)
{
    Q_ASSERT(idx >= 0 && idx <= m_labelContentPairList.size());

    if(idx < m_labelContentPairList.size())
    {
        m_labelContentPairList[idx].first->setVisible(true);
        m_labelContentPairList[idx].second->setVisible(true);

        m_labelContentPairList[idx].second->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
        m_labelContentPairList[idx].second->setOpenExternalLinks(false);

        return m_labelContentPairList[idx];
    }

    QPair<QLabel*,QLabel*> pair;
    pair.first  = new QLabel(this);
    pair.second = new QLabel(this);

    pair.first->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    pair.second->setFocusPolicy(Qt::NoFocus);
    pair.second->setWordWrap(true);


    m_labelContentPairList.append(pair);

    return pair;
}

void DeviceDetailView::verifyLabelPairVisiblity(int from)
{
    Q_ASSERT (from >= 0);

    for(int k = from ; k < m_labelContentPairList.size() ; ++k)
    {
        m_labelContentPairList[k].first->setVisible(false);
        m_labelContentPairList[k].second->setVisible(false);
    }
}

void DeviceDetailView::releasePairList()
{
    typedef QPair<QLabel*,QLabel*> LabelPairType;

    foreach(LabelPairType pair,m_labelContentPairList)
    {
        delete pair.first;
        delete pair.second;
    }

    m_labelContentPairList.clear();
}

void DeviceDetailView::applyUiElementsStyle()
{
//    SET_STYLE_PROPERTY(FLEX_BUTTON,m_modifyApplyBtn);
//    SET_STYLE_PROPERTY(FLEX_BUTTON,m_closeBtn);

#ifdef BLOCK_DEVICE
//    SET_STYLE_PROPERTY(FLEX_BUTTON,m_blockDeviceBtn);
#endif
}

void DeviceDetailView::retranslateUi()
{
    m_modifyApplyBtn->setText(m_isModifying ? translateText(L_MAP_APPLY,QString("Apply")) : translateText(L_MAP_MODIFY,QString("Modify")));
    m_closeBtn->setText(translateText(L_MAP_CLOSE,QString("Close")));

    if(m_canModify)
    {
        if(m_proList.contains(DP_NAME))
        {
            m_nameLabel->setText(translateText(L_MAP_DEVICEPROPERTY_NAME,m_device.getDetailPropertyLabel(DP_NAME)));
        }

        if(m_proList.contains(DP_TYPE))
        {
            m_typeLabel->setText(translateText(L_MAP_DEVICEPROPERTY_TYPE,m_device.getDetailPropertyLabel(DP_TYPE)));
        }
        if(m_proList.contains(DP_ISWIRED))
        {
            m_ConnectionLabel->setText(translateText(L_MAP_DEVICEPROPERTY_CONNECTIONTYPE));
        }
    }

    for(int idx = 0 , k = 0; idx < m_proList.size() ; ++idx)
    {
        if(m_canModify &&
           (m_proList[idx] == DP_NAME
            || m_proList[idx] == DP_TYPE
            ||m_proList[idx] == DP_ISWIRED)
           )
        {
            continue;
        }

        int langIdx = L_MAP_DEVICEPROPERTY_NAME + (m_proList[idx] - DP_NAME);
        m_labelContentPairList[k++].first->setText(translateText(langIdx,m_device.getDetailPropertyLabel(m_proList[idx])));;
    }

    if(m_whatisitLabel)
    {
        QString whatisit = translateText(L_MAP_WHATISIT,QString("What is it?"));

        QString text("<a href=\"http://support.netgear.com/search/link%20rate\">");
        text = (text + whatisit + "</a>");
        m_whatisitLabel->setText(text);
    }

    if(m_curSelectedDevType != MDT_DEVICETYPE_COUNT)
    {
        updateTitleAndLogo(m_curSelectedDevType);
    }
#ifdef RETRANSLATE_PROPERTYS
    foreach(QLabel *label,m_transLabels)
    {
        int lan=label->property(OBJECT_PROPERTY_FORTRANSLATE).toInt();
        label->setText(translateText(lan,"lan error"__FILE__));
    }

#endif
}

void DeviceDetailView::retranslateDeviceTypesList()
{
    for(int idx = 0 ; idx < m_typeContentCB->count() ; ++idx)
    {
        int devType = m_typeContentCB->itemData(idx).toInt();
        m_typeContentCB->setItemText(idx,translateText(L_MAP_DEVICETYPE_INTERNET + devType,getDeviceTypeText(devType)));
    }

    m_typeContentLabel->setText(m_typeContentCB->currentText());
}

void DeviceDetailView::retranslateConnectionTypeList()
{

    for(int idx=0;idx <m_ConnectionContentCB->count();idx++)
    {
        int connectionType=m_ConnectionContentCB->itemData(idx).toInt();


        connectionType=connectionType%(sizeof(cLanParas)/sizeof(cLanParas[0]));
        m_ConnectionContentCB->setItemText(idx,translateText(cLanParas[connectionType]));
    }
    int wiredInShowing        = m_device.parseWiredInShowing();
    wiredInShowing=wiredInShowing%(sizeof(cLanParas)/sizeof(cLanParas[0]));
    m_ConnectionContenteLabel->setText(translateText(cLanParas[wiredInShowing]));
}

QString DeviceDetailView::translateText(int idx, const QString &defaultText)
{
    QString text;
    emit translateText(idx,&text);
    return text.isEmpty() ? defaultText : text;
}

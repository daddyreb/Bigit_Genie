#include "faq.h"
#include "mainwindow.h"

FAQ::FAQ(QWidget* parent)
    :QWidget(parent)
{
    uuid ="{5ddd2596-5211-4d83-980b-71f3516d0b5c}";

    verticalLayout = new QVBoxLayout(this);

    widget = new QWidget(this);

    verticalLayout_2 = new QVBoxLayout(widget);
    verticalLayout_2->setSpacing(0);
    verticalLayout_2->setContentsMargins(0, 0, 0, 0);

    scrollArea = new QScrollArea(widget);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setFrameShape(QFrame::NoFrame);
	scrollArea->setFrameShadow(QFrame::Plain);
    scrollArea->setWidgetResizable(true);

    scrollAreaWidgetContents = new QWidget();
    scrollAreaWidgetContents->setGeometry(QRect(0, 0, 719, 570));
#ifdef Q_OS_MAC
   scrollAreaWidgetContents->setStyleSheet(".QScrollArea{background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:1, y2:0, stop:0 rgba(214, 214, 214, 253), stop:1 rgba(253, 253, 253, 253));}");
#else	
    scrollAreaWidgetContents->setStyleSheet("background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:1, y2:0, stop:0 rgba(214, 214, 214, 253), stop:1 rgba(253, 253, 253, 253));");
#endif
    verticalLayout_3 = new QVBoxLayout(scrollAreaWidgetContents);
    verticalLayout_3->setSpacing(0);

    frame_8 = new QFrame(scrollAreaWidgetContents);
    frame_8->setFrameShape(QFrame::NoFrame);
    frame_8->setFrameShadow(QFrame::Plain);
    frame_8->setAttribute(Qt::WA_TranslucentBackground,true);

    gridLayout = new QGridLayout(frame_8);
    gridLayout->setContentsMargins(0, 0, 0, 6);
    gridLayout->setHorizontalSpacing(0);

    widget_2 = new QWidget(frame_8);
    widget_2->setAttribute(Qt::WA_TranslucentBackground,true);

    gridLayout_3 = new QGridLayout(widget_2);
    gridLayout_3->setSpacing(0);
    gridLayout_3->setContentsMargins(0, 6, 0, 0);

//    label = new QLabel(widget_2);
//    label->setMinimumSize(QSize(24, 0));
//    label->setMaximumSize(QSize(24, 16777215));
//    QPixmap px(":/Images/xt1.png");
//    label->setMaximumSize(px.size());
//    label->setMinimumSize(px.size());
//    label->setPixmap(px);
//    label->setAttribute(Qt::WA_TranslucentBackground,true);
//
//    gridLayout_3->addWidget(label, 0, 0, 1, 1);

    label_2 = new QLabel(widget_2);
    label_2->setText(QString(tr("If you have a frequently asked question of your NETGEAR product then you will find the answer here.")));
    label_2->setWordWrap(true);
    label_2->setAttribute(Qt::WA_TranslucentBackground,true);
    label_2->setProperty("stylehintplugin",77);

//    gridLayout_3->addWidget(label_2, 0, 1, 1, 1);
    gridLayout_3->addWidget(label_2, 0, 0, 1, 1);

//    label_3 = new QLabel(widget_2);
//    label_3->setMaximumSize(QSize(10, 16777215));
//    label_3->setAttribute(Qt::WA_TranslucentBackground,true);

//    gridLayout_3->addWidget(label_3, 0, 2, 1, 1);
//    gridLayout_3->addWidget(label_3, 0, 1, 1, 1);

    gridLayout->addWidget(widget_2, 0, 0, 1, 1);

    QString filepath =":/Images/xt5.png";
    twidget = new TWidget(frame,filepath);

    gridLayout->addWidget(twidget, 1, 0, 1, 1);

    verticalLayout_3->addWidget(frame_8);

    str =tr("How to add wireless client devices to my wireless network");
    str_1 =tr("1.If you have run NETGEAR Router Smart Wizard on a PC, check Router_Setup.html file on that PC desktop, if it does not exist, go to http://routerlogin.net, select 'Wireless Settings' page, get the wireless network name (SSID) and security inthisation.\n"
              "\n"
              "2.On the computer or device you want to connect to the wireless network, if your wireless device has a Wi-Fi Protected Setup (WPS) push-button (a physical button or a soft button like Windows Push-button Configuration (PBC) in the adapter software), go to step 3. If your wireless device does not support WPS, or you choose not to use it, go to step 4.\n"
              "\n"
              "3.Simply click the WPS button on your NETGEAR wireless router, then, within 2 minutes, click the button on the client computer or device. The computer or device will connect to your wireless router.\n"
              "\n"
              "4.On device without WPS support, open 'Wireless Network Connection', view the wireless networks, then select your wireless network name and enter the correct security information.\n"
              "\n"
              "\n"
              "Repeat these steps to add an additional computer or device to your wireless network.");
    frame = new Flex(str,str_1,"NQ1",scrollAreaWidgetContents);
    verticalLayout_3->addWidget(frame);

    str1 =tr("First-resort when no connection to router or Internet");
    str1_1 =tr("Be sure to try this power cycling sequence as a first resort to slove network connection issue:\n"
               "\n"
               "1.Turn off and unplug the power of the modem\n"
               "2.Turn of the router\n"
               "3.Plug in the power of the modem and turn it on.Wait for 2 mimutes\n"
               "4.Turn on the router and wait for 1 minute");
    frame_3 = new Flex(str1,str1_1,"NQ2",scrollAreaWidgetContents);
    verticalLayout_3->addWidget(frame_3);

    str2 =tr("I can connect to the router, but not to the Internet");
    str2_1 =tr("If you can connect to the router but not to the Internet, check the ISP parameters match in the Basic Settings page on http://routerlogin.net. The parameters depend on your broadband service. Typically for DSL, you enter the DSL username (like xxx@att.net) and the DSL password.\n"
               "If unsure which settings to use, or of their values, contact your ISP.\n"
               "If you have Internet through cable service, make sure your cable modem is powered off and then on during router set-up. If router continues to not get Internet connection from the modem, it could be because the modem is remembering the previous device's MAC address. You can enter that device's MAC in Basic Settings page on http://routerlogin.net");
    frame_2 = new Flex(str2,str2_1,"NQ3",scrollAreaWidgetContents);
    verticalLayout_3->addWidget(frame_2);

    str3 =tr("Forgotten or Lost Router Password");
    str3_1 =tr("The router username is always admin\n"
               "The default password is password\n"
               "Try the default username and password before resetting the router to the factory defaults.\n"
               "When a router resets, it goes back to the state when it was new - all inthisation that was entered by hand is lost! Examples are critical things you need to get online, such as ISP username, ISP password, IP addresses, security keys, and ports and services that were opened. It all needs to be re-entered.\n"
               "So, have your ISP information before resetting the router.");
    frame_4 = new Flex(str3,str3_1,"NQ4",scrollAreaWidgetContents);
    verticalLayout_3->addWidget(frame_4);

    //////2011-03-21 begin
    str4 =tr("My router stopped working and the Power LED blinks slowly, what should I do?");
    str4_1 =tr("If the router¡¯s power LED blinks slowly, it means the router¡¯s firmware is corrupted. You can use the following procedure to recover the router. If this procedure does not work and your router is still under warranty, please contact NETGEAR Technical Support.<br><br>Step 1: Using another Internet connection (such as at work or in a Wi-Fi hotspot), download the latest firmware for your router from http://www.netgear.com."
               "You should be able to ping http://routerlogin.net after setting it to automatic IP address.\n"
               "Sometimes a PC reboot is required to bring it back to work.");
    frame_5 = new Flex(str4,str4_1,"NQ5",scrollAreaWidgetContents);
    verticalLayout_3->addWidget(frame_5);
    //////2011-03-21 end

    str5 =tr("Troubleshooting Wireless Networks");
    str5_1 =tr("Each wireless device must have the same settings:\n"
               "SSID: This is case sensitive. ('Netgear' is different from 'NETGEAR'). The default SSID for NETGEAR wireless device is 'NETGEAR'.\n"
               "Channel: The wireless channels are 1 to 11 in North America (1 to 13 in other countries)\n"
               "Mode: The wireless mode should be set to infrastructure mode for client to connect to a wireless router.\n"
               "Encryption: You have these options for encryption:\n"
               "No security\n"
               "Use WEP\n"
               "Use WPA-PSK\n"
               "Use WPA\n"
               "Wireless radio should be ON (the router's wireless LED should be lit.)\n"
               "If Access Control List (ACL) is enabled on your router, make sure the MAC address of the wireless adapter is included in that list.");
    frame_6 = new Flex(str5,str5_1,"NQ6",scrollAreaWidgetContents);
    verticalLayout_3->addWidget(frame_6);

    str6 =tr("One PC is not connected to the Internet, but other PCs are");
    str6_1 =tr("Make sure this PC is set to Obtain an IP address automatically and Obtain DNS server address automatically for its IP address (You can right-click the network connection and select Properties then TCP/IP Properties to check or change the settings.)\n"
               "You should be able to ping http://routerlogin.net after setting it to automatic IP address.\n"
               "Sometimes a PC reboot is required to bring it back to work.");
    frame_7 = new Flex(str6,str6_1,"NQ7",scrollAreaWidgetContents);
    verticalLayout_3->addWidget(frame_7);


    str7 =tr("Configuring Wireless Security WPA-PSK");
    str7_1 =tr("Hackers can easily trap information transmitted over wireless network, it has to be encrypted to avoid unauthorized users to intercept and decode data.\n"
               "In order to secure the wireless network, enable WPA-PSK (WEP is an old wireless security standard, it's not recommended).\n"
               "You can either enable WPA-PSK when you run CD-based Smart Wizard to install the router, or log into http://routerlogin.net and go to Wireless Settings page to set it up.\n"
               "After you enable wireless security features on the router, you must configure your wireless clients (wireless computers) with matching settings. Your wireless clients will not be able to connect unless you configure matching settings.");
    frame_9 = new Flex(str7,str7_1,"NQ8",scrollAreaWidgetContents);
    verticalLayout_3->addWidget(frame_9);



    scrollArea->setWidget(scrollAreaWidgetContents);

    verticalLayout_2->addWidget(scrollArea);

    verticalLayout->addWidget(widget);

    retranslateUi();
}
FAQ::~FAQ()
{

}
void FAQ::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    switch(event->type())
    {
        case QEvent::LanguageChange:
            retranslateUi();
            break;
        default:
                break;
    }
}
void FAQ::retranslateUi()
{
    label_2->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_FAQ_TITLE));//lanmanager->getText(L_NETWORKPROBLEM_FAQ_TITLE,uuid));
    str = GENIE2_GET_TEXT(L_NETWORKPROBLEM_FAQ_LABEL);//lanmanager->getText(L_NETWORKPROBLEM_FAQ_LABEL,uuid);
    str_1 = GENIE2_GET_TEXT(L_NETWORKPROBLEM_FAQ_TEXT);//lanmanager->getText(L_NETWORKPROBLEM_FAQ_TEXT,uuid);
    frame ->labelchange(str,str_1);

    str1 =GENIE2_GET_TEXT(L_NETWORKPROBLEM_FAQ_LABEL_1);//lanmanager->getText(L_NETWORKPROBLEM_FAQ_LABEL_1,uuid);
    str1_1 = GENIE2_GET_TEXT(L_NETWORKPROBLEM_FAQ_TEXT_1);//lanmanager->getText(L_NETWORKPROBLEM_FAQ_TEXT_1,uuid);
    frame_2->labelchange(str1,str1_1);

    str2 =GENIE2_GET_TEXT(L_NETWORKPROBLEM_FAQ_LABEL_2);//lanmanager->getText(L_NETWORKPROBLEM_FAQ_LABEL_2,uuid);
    str2_1 = GENIE2_GET_TEXT(L_NETWORKPROBLEM_FAQ_TEXT_2);//lanmanager->getText(L_NETWORKPROBLEM_FAQ_TEXT_2,uuid);
    frame_3->labelchange(str2,str2_1);

    str3 =GENIE2_GET_TEXT(L_NETWORKPROBLEM_FAQ_LABEL_3);//lanmanager->getText(L_NETWORKPROBLEM_FAQ_LABEL_3,uuid);
    str3_1 = GENIE2_GET_TEXT(L_NETWORKPROBLEM_FAQ_TEXT_3);//lanmanager->getText(L_NETWORKPROBLEM_FAQ_TEXT_3,uuid);
    frame_4->labelchange(str3,str3_1);

    //////2011-03-21 begin
    str4 =GENIE2_GET_TEXT(L_NETWORKPROBLEM_FAQ_LABEL_7);//lanmanager->getText(L_NETWORKPROBLEM_FAQ_LABEL_7,uuid);
    str4_1 = GENIE2_GET_TEXT(L_NETWORKPROBLEM_FAQ_TEXT_7);//lanmanager->getText(L_NETWORKPROBLEM_FAQ_TEXT_7,uuid);
    frame_5->labelchange(str4,str4_1);
    //////2011-03-21 end

    str5 =GENIE2_GET_TEXT(L_NETWORKPROBLEM_FAQ_LABEL_5);//lanmanager->getText(L_NETWORKPROBLEM_FAQ_LABEL_5,uuid);
    str5_1 = GENIE2_GET_TEXT(L_NETWORKPROBLEM_FAQ_TEXT_5);//lanmanager->getText(L_NETWORKPROBLEM_FAQ_TEXT_5,uuid);
    frame_6->labelchange(str5,str5_1);

    str6 =GENIE2_GET_TEXT(L_NETWORKPROBLEM_FAQ_LABEL_6);//lanmanager->getText(L_NETWORKPROBLEM_FAQ_LABEL_6,uuid);
    str6_1 = GENIE2_GET_TEXT(L_NETWORKPROBLEM_FAQ_TEXT_6);//lanmanager->getText(L_NETWORKPROBLEM_FAQ_TEXT_6,uuid);
    frame_7->labelchange(str6,str6_1);

    str7 =GENIE2_GET_TEXT(L_NETWORKPROBLEM_FAQ_LABEL_4);//lanmanager->getText(L_NETWORKPROBLEM_FAQ_LABEL_4,uuid);
    str7_1 = GENIE2_GET_TEXT(L_NETWORKPROBLEM_FAQ_TEXT_4);//lanmanager->getText(L_NETWORKPROBLEM_FAQ_TEXT_4,uuid);
    frame_9->labelchange(str7,str7_1);
}

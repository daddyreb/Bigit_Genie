#include "stable.h"
//#ifdef Q_OS_WIN32
//#define NETGEAR_DIR (QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/NETGEARGenie/")

//#else
//#define NETGEAR_DIR (QDir::home().absolutePath()+QString("/.NETGEARGenie/"))
//#endif
QGenieHandlerInf::QGenieHandlerInf()
{
    if(!QDir::home().exists(NETGEAR_DIR))
    {
        QDir::home().mkdir(NETGEAR_DIR);
    } 
}

QVariant QGenieHandlerInf::handler_command(int cmd,int uuid_index,const QVariant &paras)
{
    QGenieInterface2LanguageManager *lanm2=QGenieInterface2LanguageManager::GetSingletonPtr();
    QGenieInterface2Helper *helper=QGenieInterface2Helper::GetSingletonPtr();;
    QGenieSettings *settings=QGenieAllSingletonClasses::GetSingletonPtr()->mSettings;
    QGenieInterface2SubscribedMessageManager *subsm=QGenieInterface2SubscribedMessageManager::GetSingletonPtr();
    QGenieInterface2LogManager *logm=QGenieInterface2LogManager::GetSingletonPtr();
    Q_ASSERT(helper);
    Q_ASSERT(lanm2);
    Q_ASSERT(settings);
    Q_ASSERT(subsm);
    Q_ASSERT(logm);
    QString stemp;
    bool btemp;
    switch(cmd)
    {
    case HCMD_UUID_IDX:
//        Q_ASSERT(uuid_index==-1);
        return indexOfUuid(QUuid(paras.toString()));
    case HCMD_CHECK_DIR:
        stemp=NETGEAR_DIR+paras.toString();
        if(!QDir::home().exists(stemp))
        {
            btemp=QDir::home().mkpath(stemp);
            Q_ASSERT(btemp);
        }
        return stemp.replace("\\","/");
    case HCMD_HOMEDIR_NAME:
        return NETGEAR_DIR;
//    case HCMD_MAINFRAME_VERSION:
//        return VERSION_STRING;
    case HCMD_INIT_DBGMODLE:
        helper->initDBGModel(paras);
        break;
    case HCMD_REGISTER_UI:
        helper->registerUi(paras);
        break;
    case HCMD_REGISTER_DBG_UI:
        helper->registerDBGUi(paras);
        break;
    case HCMD_LANGUAGE_CHANGE:
        lanm2->slot_setLanguage(paras.toInt());
////        HANDLER_CMD(HCMD_SET_SETTINGS,LANGUAGE_SETTING,);
        GENIE2_SET_SETTING(LANGUAGE_SETTING,paras.toInt());
//        helper->tellAllPlutinMessage(I2CMD_LANGUAGE_CHANGE);
        break;
    case HCMD_GET_TEXT:
    case HCMD_TRANSLATE_LABEL:
    case HCMD_TRANSLATE_BUTTON:
    case HCMD_TRANSLATE_CHECKBOX:
    case HCMD_TRANSLATE_RADIOBUT:
        return lanm2->processCmd(cmd,uuid_index,paras);
    case HCMD_TELL_OTHERPLUGIN:
        return helper->tellPluginMessage(paras);
    case HCMD_GET_SETTINGS:
        return settings->value(paras.toString());
    case HCMD_SET_SETTINGS:
        settings->setingCmd(paras);
        break;
    case HCMD_SUBSCRIB_MESSAGE:
        subsm->subscrib(paras.toInt(&btemp),uuid_index);
        Q_ASSERT(btemp);
        break;
    case HCMD_NOTIFY_MESSAGE:
        subsm->notify(paras);
        break;
    case HCMD_ASK_SUBSCRIBED_MESSAGE:
        return subsm->getCache(paras);
            case HCMD_LOG:
        logm->writeLog(paras.toString(),uuid_index);
        break;
    case HCMD_IMAGE_NAME:
        return helper->getImageName(paras);
    case HCMD_LOADPLUGIN:
        return helper->loadPlugin(paras.toString());
    case HCMD_EXIT:
        QGenieInterface2LogManager::GetSingletonPtr()->closeFile();
        helper->tellAllPlutinMessage(I2CMD_ABOUTTO_QUIT,paras);
		break;
    default:
        qDebug()<<"unknown handler commander";
    }
    qDebug()<<"QGenieHandlerInf::handler_command"<<cmd;
    return QVariant();
}

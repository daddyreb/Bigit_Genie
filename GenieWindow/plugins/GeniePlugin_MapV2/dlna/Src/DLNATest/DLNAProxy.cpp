#include "DLNAProxy.h"
#include <winsock2.h>

#define TRACK_TIMEOUT_COUNT 3
extern int externtest;
using namespace SiteView;

DLNAProxy * DLNAProxy::m_pThis = NULL;

DLNAProxy::DLNAProxy()
{
	m_core = new SiteView::DLNACore(this);
    m_bPhotosImpor = false;
	m_dmsTitle = "";

    const char *dmrProtocolInfo =
    "http-get:*:image/png:*,"
    "http-get:*:image/jpeg:*,"
    "http-get:*:image/bmp:*,"
    "http-get:*:image/gif:*,"
    "http-get:*:audio/mpeg:*,"
    "http-get:*:audio/3gpp:*,"
    "http-get:*:audio/mp4:*,"
    "http-get:*:audio/x-ms-wma:*,"
    "http-get:*:audio/wav:*,"
    "http-get:*:video/mp4:*,"
    "http-get:*:video/mpeg:*,"
    "http-get:*:video/x-ms-wmv:*,"
    "http-get:*:video/x-ms-asf:*,"
    "http-get:*:video/3gpp:*,"
    "http-get:*:video/avi:*,"
    "http-get:*:video/quicktime:*";

	setProperty("PlatformName", "Win8");
	setProperty("OSVersion", "1.1");
	setProperty("DMRProtocolInfo", dmrProtocolInfo);
	

	char ac[80];
	int i = gethostname(ac, sizeof(ac));
	strcat(ac, "[Windows]");

	setProperty("FriendlyName", ac);
	setProperty("CUSTOM", ac);

	//setProperty("CUSTOM", "zhujianwen");

	m_core->start();
}

DLNAProxy::~DLNAProxy()
{
    if (m_currentServer)
    {
        m_currentServer->release();
        m_currentServer = NULL;
    }
    if (m_currentRender)
    {
        m_currentRender->release();
        m_currentRender = NULL;
    }
    if (m_trackOp) 
    {
		m_trackOp->release();
		m_trackOp = NULL;
	}

    delete m_core;
}

DLNAProxy* DLNAProxy::GetInstance()
{
	if (NULL == m_pThis)
	{
		m_pThis = new DLNAProxy();
	}
	return m_pThis;
}


//////////////////////////////////////////////////////////////////////////
// Callback
 void DLNAProxy::onDiscoveryListChaned(const NPT_String& ip, const NPT_String& devName, const int type)
 {
	 emit dlnaChange(ip.GetChars(),devName.GetChars(),type);
 }

void DLNAProxy::onMediaRendererListChanged()
{
}


void DLNAProxy::onMediaServerListChanged()
{
	DeviceDescList ls = m_core->snapshotMediaServerList();
}


void DLNAProxy::onMediaServerStateVariablesChanged(SiteView::DeviceDesc *deviceDesc, SiteView::ServiceDesc *serviceDesc, const NPT_List<NPT_String>& nameList, const NPT_List<NPT_String>& valueList)
{
}

void DLNAProxy::onMediaRendererStateVariablesChanged(SiteView::DeviceDesc *deviceDesc, SiteView::ServiceDesc *serviceDesc, const NPT_List<NPT_String>& nameList, const NPT_List<NPT_String>& valueList)
{
}


void DLNAProxy::dmrOpen(const NPT_String& url, const NPT_String& mimeType, const NPT_String& metaData)
{
}
void DLNAProxy::dmrPlay()
{
}
void DLNAProxy::dmrPause()
{
}
void DLNAProxy::dmrStop()
{
}
void DLNAProxy::dmrSeekTo(NPT_Int64 timeInMillis)
{
}
void DLNAProxy::dmrSetMute(bool mute)
{
}
void DLNAProxy::dmrSetVolume(int volume)
{
}

// DLNA browse callback 
void DLNAProxy::onDLNACoreOpFinished(SiteView::DLNACoreOp *op)
{
	printf("browse fineshed\n");
	if (m_dmsTitle != "")
	{
		m_core->browseMediaServerEx(m_dmsCurrentUUID, m_dmsTitle, 100, this, &m_BroOp);
		m_dmsTitle = "";
		m_BroOp->checkFinishedIfNotSetCallback(this);
	}
}

void DLNAProxy::onDLNAProgressiveBrowseOpResult(SiteView::DLNAProgressiveBrowseOp *op, NPT_UInt32 startingIndex, NPT_UInt32 numberReturned, NPT_UInt32 totalMatches, const SiteView::DLNAObjectList& ls)
{
	if (m_dmsTitle == "")
	{
		for (NPT_Ordinal i = 0; i < ls.count(); i++)
		{
			if (ls.itemAt(i)->title() == "Pictures")
			{
				const char * str = ls.itemAt(i)->title().GetChars();
				m_dmsTitle = "Pictures";
				break;
			}
			else if (ls.itemAt(i)->title() == "All Pictures")
			{
				m_dmsTitle = "Pictures";
				break;
			}
		}
	}
	/*printf("browse fileNumber:%d\n", numberReturned);
	if (wait(m_BroOp))
        {
            if (m_BroOp->succeeded()) 
            {
                printf("borwse op  successed\n");
            }
            else
            {
                printf("borwse op un successed\n");
            }
        }
		else
        {
            printf("wait wrong-->click cancle btn\n");
        }
		m_BroOp->release();*/
}


SiteView::DLNACore* DLNAProxy::getCore()  const
{
    return m_core;
}

SiteView::DeviceDescList DLNAProxy::getMediaRenderList() const
{
    return m_core->snapshotMediaRendererList();
}

SiteView::DeviceDescList DLNAProxy::getMediaServerList() const
{
    return m_core->snapshotMediaServerList();
}


SiteView::UUID DLNAProxy::getCurrentServerUUID() const
{
    return m_dmsCurrentUUID;
}

SiteView::UUID DLNAProxy::getCurrentRenderUUID() const
{
    return m_dmrCurrentUUID;
}

SiteView::DeviceDesc* DLNAProxy::getCurrentServer() const
{
    return m_currentServer;
}

SiteView::DeviceDesc* DLNAProxy::getCurrentRender() const
{
    return m_currentRender;
}

void DLNAProxy::traverseRenderList()
{
    const SiteView::DeviceDescList& dmrList = getMediaRenderList();
    SiteView::UUID defaultRenderUUID = SiteView::UUID::fromString(m_defaultRenderUUID);
    SiteView::DeviceDesc * defaultRender = dmrList.find(defaultRenderUUID);
    //setActiveRender(*defaultRender);
}

const SiteView::UUID& DLNAProxy::getServerUuidOfSelf()
{
    return m_core->getMediaServerUuid();
}
const SiteView::UUID& DLNAProxy::getRenderUuidOfSelf()
{
    return m_core->getMediaRendererUuid();
}

bool DLNAProxy::currentRenderIsSelf()
{
    if(m_defaultRenderUUID == getRenderUuidOfSelf().toString())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void DLNAProxy::getMediaObjType(const SiteView::DLNAObject* media, DLNAMediaObjType & eType)
{
    if (media)
    {
        const NPT_String contentType = media->upnpClass().Left(21);
		
        if (contentType == "object.item.imageItem" ) 
        {
            eType = DLNAMediaObjTypeImage;
        } 
        else if (contentType == "object.item.videoItem") 
        {
            eType = DLNAMediaObjTypeVideo;
        } 
        else if (contentType == "object.item.audioItem")
        {
            eType = DLNAMediaObjTypeAudio;
        }
    }
}

/////////////////////
void DLNAProxy::restartUPNP()
{
        m_core->start();
}

void DLNAProxy::refreshMediaSource()
{
    m_core->clearMediaServerContent();
    m_core->mediaStore()->importIOSPhotos("");
}

void DLNAProxy::enableFunction(DLNACore::Function fun, bool enable)
{
	m_core->enableFunction(fun, enable);
}

void DLNAProxy::setProperty(const NPT_String& name, const NPT_String& value)
{
    m_core->setProperty(name,value);
}

void DLNAProxy::importFileSystemToMediaServer(const NPT_List<NPT_String>& dirs, const NPT_List<NPT_String>& names, SiteView::ShareType eType)
{
	m_core->clearMediaServerContent();
	m_core->importFileSystemToMediaServer(dirs, names, false, eType);
}

SiteView::ServiceDesc* DLNAProxy::findServiceByType(const NPT_String& serviceType) const
{
    return m_currentRender->findServiceByType(serviceType);
}

SiteView::ServiceDesc* DLNAProxy::findServiceById(const NPT_String& serviceType) const
{
    return m_currentRender->findServiceById(serviceType);
}

NPT_Result DLNAProxy::queryStateVariables(const NPT_String& serviceId, const NPT_List<NPT_String>& nameList, NPT_List<NPT_String>& valueList)
{
    return m_core->queryStateVariables(m_dmrCurrentUUID, serviceId, nameList, valueList);
}

///////////////////UPNP
void DLNAProxy::browse(const int nUUID, const NPT_String& serverOrDirTitle, const NPT_String& containerId)
{
	//DLNAProgressiveBrowseOp * op;
	DeviceDescList devlist = m_core->snapshotMediaServerList();
	UUID uid = devlist.itemAt(nUUID)->uuid();
	m_dmsCurrentUUID = uid;
	m_core->browseMediaServerEx(uid, containerId, 100, this, &m_BroOp);
	m_BroOp->checkFinishedIfNotSetCallback(this);
	printf("browse....\n");

	//SiteView::DLNABrowseOp *op;
	//if (NPT_SUCCEEDED(m_core->browseMediaServer(m_dmsCurrentUUID, containerId, false, &op))) 
/*	if (NPT_SUCCEEDED(m_core->browseMediaServerEx(uid, containerId, 100, this, &op)))
    {
        if (wait(op))
        {
            if (op->succeeded()) 
            {
                printf("borwse op  successed\n");
            }
            else
            {
                printf("borwse op un successed\n");
            }
        }
		else
        {
            printf("wait wrong-->click cancle btn\n");
        }
		op->release();
	}	*/
}

void DLNAProxy::refreshDevices(DLNACore::FlushMode mode)
{
//	m_core->flushDeviceList(flushMode);
	m_core->searchDevices(15);
}


void DLNAProxy::openMediaObj(const SiteView::DLNAItem* mediaItem)
{
	DeviceDescList devlist = m_core->snapshotMediaServerList();
	UUID renderUuid = devlist.itemAt(0)->uuid();

	DLNAObjectList objList;
	DLNAObject * obj = objList.itemAt(0);
	DLNAItem * item = obj->asItem();

   // SiteView::UUID renderUuid = getCurrentRenderUUID();
	if (!renderUuid.isNull()) 
    {
        DLNAMediaObjType type;
		getMediaObjType(mediaItem, type);
		
        NPT_String iconUrl;
        if (((SiteView::DLNAObject*)mediaItem)->findThumbnailURL(200, 200, NULL, iconUrl))
        {
            //setThumbImg(iconUrl, type);
        }
        
		SiteView::DLNACoreOp *op;
		if (NPT_SUCCEEDED(m_core->playMedia(renderUuid, mediaItem, &op))) 
        {
			op->release();
		}
        else
        {
            //NSLog(@"create Op error");
        }
	}
    else
    {
        //[m_delegate shouldSelectRenderForMedia:mediaItem];
    }
}

void DLNAProxy::openMediaObj(const SiteView::DLNAItem* mediaItem, unsigned int timeout)
{
    SiteView::UUID renderUuid = getCurrentRenderUUID();
	if (!renderUuid.isNull()) 
    {
        DLNAMediaObjType type;
		getMediaObjType(mediaItem, type);
        NPT_String iconUrl;
        if (((SiteView::DLNAObject*)mediaItem)->findThumbnailURL(200, 200, NULL, iconUrl))
        {
        }
        
		SiteView::DLNACoreOp *op;
		if (NPT_SUCCEEDED(m_core->playMedia(renderUuid, mediaItem, &op))) 
        {
			op->release();
		}
        else
        {
            
        }
	}
    else
    {
        //[m_delegate shouldSelectRenderForMedia:mediaItem];
    }
}

void DLNAProxy::setActiveServer(SiteView::DeviceDesc& device)
{
}

void DLNAProxy::setActiveRender(SiteView::DeviceDesc& device)
{
}



void DLNAProxy::queryTrackInfo()
{
}

int DLNAProxy::onQueryMediaPositionFinished()
{
    int track = 0;
	if (m_trackOp) 
    {
		if (m_trackOp->succeeded()) 
        {
			track = m_trackOp->trackTime();
		}

        printf("-------------%d\n",track);
		m_trackOp->release();
		m_trackOp = NULL;
	}
    m_trackCount = 0;//if track successed  reset  count
    return track;
}

void DLNAProxy::stop()
{
    if (m_dmrCurrentUUID.isNull())
    {
        //[delegate() notAviableRender];
        return;
    }
    
	SiteView::DLNACoreOp *op;
    if (NPT_SUCCEEDED(m_core->stopMedia(m_dmrCurrentUUID, &op)))
    {
        op->release();
    }
}

void DLNAProxy::pause()
{
    if (m_dmrCurrentUUID.isNull())
    {
        //[delegate() notAviableRender];
        return;
    }
    
	SiteView::DLNACoreOp *op;
    if (NPT_SUCCEEDED(m_core->pauseMedia(m_dmrCurrentUUID, &op)))
    {
        op->release();
    }
}

void DLNAProxy::play()
{
    if (m_dmrCurrentUUID.isNull())
    {
        //[delegate() notAviableRender];
        return;
    }
    
	SiteView::DLNACoreOp *op;
    if (NPT_SUCCEEDED(m_core->playMedia(m_dmrCurrentUUID, NULL, &op)))
    {
        op->release();
    }
}


void DLNAProxy::setVolume(int volume)
{
    if (m_dmrCurrentUUID.isNull()) 
    {
        //[delegate() notAviableRender];
		return;
	}
	
	SiteView::DLNACoreOp *op;
    if (NPT_SUCCEEDED(m_core->changeMediaVolume(m_dmrCurrentUUID, volume, &op)))
    {
        op->release();
    }
}

void DLNAProxy::setProgress(int progress)
{
    if (m_dmrCurrentUUID.isNull())
    {
        //[delegate() notAviableRender];
        return;
    }
    
	SiteView::DLNACoreOp *op;
    if (NPT_SUCCEEDED(m_core->seekMedia(m_dmrCurrentUUID, progress, &op)))
    {
        op->release();
    }
}


void DLNAProxy::setMute(bool mute)
{
    if (m_dmrCurrentUUID.isNull())
    {
        //[delegate() notAviableRender];
        return;
    }

    SiteView::DLNACoreOp *op;
    if (NPT_SUCCEEDED(m_core->muteMedia(m_dmrCurrentUUID, mute, &op)))
    {
        op->release();
    }
}

void DLNAProxy::renderReportPlayBackState(DLNAMediaPlayBackState state)
{
    if (m_core)
    {
		switch (state) 
        {
            case DLNAMediaPlayBackStatePlaying:
                m_core->dmrReportState(SiteView::DLNACore::DMRState_Playing);
                break;
            case DLNAMediaPlayBackStatePause:
                m_core->dmrReportState(SiteView::DLNACore::DMRState_Paused);
                break;
            case DLNAMediaPlayBackStateStop:
                m_core->dmrReportState(SiteView::DLNACore::DMRState_Stopped);
                break;
            case DLNAMediaPlayBackStateErr:
                m_core->dmrReportErrorStatus(true);
                break;
		}
	}
}

void DLNAProxy::renderReportProgress(long long playbackSeconds, long long durationSeconds)
{
    if (m_core) 
    {
		m_core->dmrReportProgress(playbackSeconds*1000, durationSeconds*1000);
	}
}
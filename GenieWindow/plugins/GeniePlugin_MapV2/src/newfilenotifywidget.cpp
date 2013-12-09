//add by caiyujun

#include <QDateTime>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QFile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QPropertyAnimation>
#include <QDesktopWidget>
#include <QTimerEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QtDebug>
#include <QFont>
#include <ShlObj.h>
#include "recethread.h"
#include "UdtFile.h"
#include <shellapi.h>
#include <CommDlg.h>

#include "genie2_interface.h"
#include "newfilenotifywidget.h"
#include "language_Map.h"


#define NDNW_BACKGROUDIMAGE "map/others/NewDeviceNotifyWidgetBk.png"
#define NEWDEVICENOTIFYWIDGET_STYLEFILE "ui/newdevicenotifywidget.qss"
#define DELAYINTERVAL_MS 1000
#define DELAY_COUNTER_MAX 10

extern int MakePath(wchar_t *dest, const wchar_t *dir, const wchar_t *file);
extern int CALLBACK BrowseDirDlgProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM path);

NewFileNotifyWidget::NewFileNotifyWidget(int type, QString strDeviceName, int fhandle, int iFolder, uint64_t size, QWidget *parent) :
        QWidget(parent),
        m_canMove(false),
        m_notifyAnimation(0),
        m_timeoutCounter(DELAY_COUNTER_MAX),
        m_timerId(0)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    setAttribute(Qt::WA_TranslucentBackground);

	m_iType = type;
	m_iFolder = iFolder;
	m_currentPercent = 0;
    m_bSendOver = false;
    m_okBtn = new QPushButton(this);
    m_cancelBtn = new QPushButton(this);	

	connect(m_okBtn,SIGNAL(clicked()),this,SLOT(okBtnClicked()));
	connect(m_cancelBtn,SIGNAL(clicked()),this,SLOT(cancelBtnClicked()));
	
	if (0 == m_iType)
	{
		m_rejectBtn = new QPushButton(this);
		m_saveBtn = new QPushButton(this);		
		m_openBtn = new QPushButton(this);
		m_openFolderBtn = new QPushButton(this);

		connect(m_saveBtn,SIGNAL(clicked()),this,SLOT(saveBtnClicked()));
		connect(m_rejectBtn,SIGNAL(clicked()),this,SLOT(rejectBtnClicked()));
		connect(m_openBtn,SIGNAL(clicked()),this,SLOT(openBtnClicked()));
		connect(m_openFolderBtn,SIGNAL(clicked()),this,SLOT(openFolderBtnClicked()));

		m_okBtn->setVisible(false);
		m_cancelBtn->setVisible(false);
		m_openBtn->setVisible(false);
		m_openFolderBtn->setVisible(false);
	}
	else
	{
		m_okBtn->setEnabled(false);
	}

    m_progressBar = new QProgressBar(this);

    m_titleLabel = new QLabel(this);
	m_titleLabel->adjustSize();
	m_titleLabel->setWordWrap(true);
    //m_titleContentLabel = new QLabel(this);
    m_bodyEdit = new QTextEdit(this);
    m_bodyEdit->setObjectName("NewDeviceNotifyWidgetEdit");
    m_bodyEdit->setEnabled(true);
    m_bodyEdit->setReadOnly(true);

    m_titleLabel->setObjectName("NewDeviceNotifyWidgetTitle");
    m_progressBar->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    //QFont font;
    //font.setPointSize(7);
    //font.setWeight(2);
    //m_titleLabel->setFont(font);
    //m_titleContentLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    m_bodyEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QPixmap pixDetecter(GENIE2_RES(NDNW_BACKGROUDIMAGE) );
    setFixedSize(pixDetecter.size());

    //QHBoxLayout *btnLayout = new QHBoxLayout();
#ifdef Q_OS_MAC
    btnLayout->setContentsMargins(0,2,0,0);
#endif
//    btnLayout->addStretch(1);
    //btnLayout->addWidget(m_closeBtn);

    QHBoxLayout *okbtnLayout = new QHBoxLayout();
    okbtnLayout->setContentsMargins(0,0,0,0);
    okbtnLayout->addStretch(1);
	if (0 == m_iType)
	{
		okbtnLayout->addWidget(m_openBtn);
		okbtnLayout->addWidget(m_openFolderBtn);
		okbtnLayout->addWidget(m_saveBtn);
		okbtnLayout->addWidget(m_rejectBtn);
		okbtnLayout->addWidget(m_cancelBtn);
		okbtnLayout->addWidget(m_okBtn);

		m_openBtn->setVisible(false);
		m_openFolderBtn->setVisible(false);
		m_okBtn->setVisible(false);
	}
	else
	{
		okbtnLayout->addWidget(m_okBtn);
		okbtnLayout->addWidget(m_cancelBtn);
	}

    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(10,5,10,0);
    //titleLayout->addWidget(m_progressBar);
    titleLayout->addWidget(m_titleLabel);
    //titleLayout->addWidget(m_titleContentLabel);

    QVBoxLayout *aboveLayout = new QVBoxLayout();
#ifdef Q_OS_MAC
    aboveLayout->setContentsMargins(0,0,0,0);
#endif
    aboveLayout->setSpacing(9);
    //aboveLayout->addLayout(btnLayout);
    aboveLayout->addLayout(titleLayout);

    QVBoxLayout *belowLayout = new QVBoxLayout();
    belowLayout->setContentsMargins(10,10,10,6);
    belowLayout->addWidget(m_bodyEdit);
    belowLayout->addWidget(m_progressBar);
    belowLayout->addLayout(okbtnLayout);

    QVBoxLayout *layOut = new QVBoxLayout(this);
#ifdef Q_OS_MAC
    layOut->setContentsMargins(9,2,4,2);
#else
    layOut->setContentsMargins(9,12,9,2);
#endif
    layOut->addLayout(aboveLayout);
    layOut->addLayout(belowLayout);

    m_strDeviceName = strDeviceName;
    applyStyle();

    m_fhandle = fhandle;
    m_size = size;
	
}

void NewFileNotifyWidget::closeWidget(int status)
{
	if (0 == status)
	{
		if (0 == m_timerId)
		{
			m_timerId = startTimer(DELAYINTERVAL_MS);
		}
	}
}

void NewFileNotifyWidget::finishReuslt(int nReturnCode)
{
	m_bSendOver = true;
	if (0 == m_timerId)
	{
		if (102 != nReturnCode)
		{
			m_timerId = startTimer(DELAYINTERVAL_MS);
		}
	}
	switch(nReturnCode)
	{	
	case 115:
		{
			if (m_iType == 1)
			{
				m_okBtn->setEnabled(true);
				m_cancelBtn->setVisible(false);
			}
			if (0 == m_iFolder || 2 == m_iFolder)
			{
				m_titleLabel->setText(translateText(L_TURBO_EMPTY_FILE));
			}
			else
			{
				m_titleLabel->setText(translateText(L_TURBO_EMPTY_FOLDER));
			}
			break;
		}
	case 114:
		{
			if (m_iType == 1)
			{
				m_okBtn->setEnabled(true);
				m_cancelBtn->setVisible(false);
			}
			else
			{
				m_okBtn->setVisible(true);
				m_openBtn->setVisible(false);
				m_openFolderBtn->setVisible(false);
				m_cancelBtn->setVisible(false);
			}
			m_titleLabel->setText(translateText(L_TURBO_REMOTE_BUSY));
			break;
		}
	case 113:
		{
			if (m_iType == 1)
			{
				m_okBtn->setEnabled(true);
				m_cancelBtn->setVisible(false);
			}
			else
			{
				m_okBtn->setVisible(true);
				m_openBtn->setVisible(false);
				m_openFolderBtn->setVisible(false);
				m_cancelBtn->setVisible(false);
			}
			m_titleLabel->setText(translateText(L_TURBO_OWN_BUSY));
			break;
		}
    case 111:
		{
			if (m_iType == 1)
			{
				m_okBtn->setEnabled(true);
				m_cancelBtn->setVisible(false);
			}
			else
			{
				m_okBtn->setVisible(true);
				m_openBtn->setVisible(false);
				m_openFolderBtn->setVisible(false);
				m_cancelBtn->setVisible(false);
			}
			m_titleLabel->setText(translateText(L_TURBO_OTHER_GENIE_NEED_UPGRADE));
			break;
		}
	case 112:
		{
			if (m_iType == 1)
			{
				m_okBtn->setEnabled(true);
				m_cancelBtn->setVisible(false);
			}
			else
			{
				m_okBtn->setVisible(true);
				m_openBtn->setVisible(false);
				m_openFolderBtn->setVisible(false);
				m_cancelBtn->setVisible(false);
			}
			m_titleLabel->setText(translateText(L_TURBO_OTHER_GENIE_NEED_UPGRADE));
			break;
		}
	case 110:
		{
			if (0 == m_iFolder)
			{
				m_openFolderBtn->setEnabled(true);		
			}
			m_openBtn->setEnabled(true);
			m_cancelBtn->setVisible(false);
			m_okBtn->setVisible(true);
			QString strTemp;
			for(int i=0; i<m_strFileList.count(); i++)
			{
				if (i != (m_strFileList.count()-1))
				{
					strTemp += m_strFileList.at(i);
					strTemp += ", ";	
				}
				else
				{
					strTemp += m_strFileList.at(i);
				}
			}
			if ("" != strTemp)
			{
				m_bodyEdit->setText(strTemp);
			}
			m_titleLabel->setText(translateText(L_TURBO_RECEIVE_SUCCESS));
			break;
		}
	case 109:
		{
			m_okBtn->setEnabled(true);
			m_cancelBtn->setVisible(false);
			m_titleLabel->setText(translateText(L_TURBO_SEND_SUCCESS));
			break;
		}
	case 108:
		{
			if (m_iType == 1)
			{
				m_okBtn->setEnabled(true);
				m_cancelBtn->setVisible(false);
			}
			else
			{
				m_okBtn->setVisible(true);
				m_openBtn->setVisible(false);
				m_openFolderBtn->setVisible(false);
				m_cancelBtn->setVisible(false);
			}
			m_titleLabel->setText(translateText(L_TURBO_NETWORK_ERROR));
			break;
		}
	case 107:
		{
			m_okBtn->setEnabled(true);
			m_cancelBtn->setVisible(false);
			m_titleLabel->setText(translateText(L_TURBO_CANCELED));
			break;
		}
	case 105:
		{
			m_cancelBtn->setVisible(false);
			m_okBtn->setVisible(true);
			m_titleLabel->setText(translateText(L_TURBO_REMOTE_STOP_SEND));
			break;
		}
	case 104:
		{
			m_okBtn->setVisible(true);
			m_cancelBtn->setVisible(false);
			m_titleLabel->setText(translateText(L_TURBO_FILE_SENDING_CANCELED));
			break;
		}
	case 103:
		{
			m_okBtn->setEnabled(true);
			m_cancelBtn->setVisible(false);
			m_titleLabel->setText(translateText(L_TURBO_NOT_ACCEPTED));
			break;
		}
	case 101:
		{
			m_saveBtn->setVisible(false);
			m_rejectBtn->setVisible(false);
			m_okBtn->setVisible(true);
			m_openBtn->setVisible(false);
			m_openFolderBtn->setVisible(false);
			m_cancelBtn->setVisible(false);
			m_titleLabel->setText(translateText(L_TURBO_FILE_RECEVING_CANCELED));
			break;
		}
	default:
		break;
	}
}

void NewFileNotifyWidget::transferPercent(double percent,double speed,QString filename)
{
	setAttribute(Qt::WA_Mapped);

	if (!m_bSendOver)
	{
		if (m_iFolder == 2)
		{
			QString strActualName;
			if (filename.contains("/") || (filename.contains("\\")))
			{
				strActualName = getActualName(filename);
				m_bodyEdit->setText(strActualName);
				if (strActualName != m_strCurrentFile)
				{
					m_strFileList.push_back(strActualName);
					m_strCurrentFile = strActualName;
				}
			}
			else
			{
				m_bodyEdit->setText(filename);
				if (filename != m_strCurrentFile)
				{
					m_strFileList.push_back(filename);
					m_strCurrentFile = filename;
				}
			}	
		}

		int iPercent = int(percent*100);
		if (m_currentPercent != iPercent)
		{
			m_currentPercent = iPercent;
			m_progressBar->setValue(int(percent*100));

			if (0.009 < speed)
			{
				QString strSpeed = QString::number(speed);
				QString strSpilt = strSpeed.left(4);
				strSpilt += "MB/sec";
				m_titleLabel->setText(strSpilt);
			}
		}	
	}
}

void NewFileNotifyWidget::setSock(int sock)
{
	m_fhandle = sock;
}

QString NewFileNotifyWidget::getActualName(QString strFile)
{
	QString strActualFileName;
	strFile.replace("\\","/");
	QStringList strList = strFile.split("/");
	int iSize = strList.size();
	int iLen = strFile.length();
	if ('/' == strFile.at(iLen-1))
	{
		strActualFileName = strList.at(iSize-2);
	}
	else
	{
		strActualFileName = strList.at(iSize-1);
	}

	return strActualFileName;
}

void NewFileNotifyWidget::setLastFolder(QString strLastFolder)
{
	m_strLastFolder = strLastFolder;
}

NewFileNotifyWidget::~NewFileNotifyWidget()
{
}

void NewFileNotifyWidget::timerEvent(QTimerEvent *event)
{
	setAttribute(Qt::WA_Mapped);
    if(event->timerId() != m_timerId)
    {
        return;
    }

    if(--m_timeoutCounter <= 0)
    {
        killTimer(m_timerId);
        m_timerId = 0;
        close();
    }
    else
    {
        retranslateUi();
    }
}

void NewFileNotifyWidget::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        retranslateUi();
    }

    QWidget::changeEvent(event);
}

void NewFileNotifyWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.drawPixmap(0,0,QPixmap(GENIE2_RES(NDNW_BACKGROUDIMAGE)));
}

void NewFileNotifyWidget::retranslateUi()
{
	if (1 == m_iType)
	{
		QString tmpText = translateText(L_TURBO_BTN_OK);
		if(m_timeoutCounter > 0)
		{
			tmpText += "(";
			tmpText += QString::number(m_timeoutCounter);
			tmpText += ")";
		}
		m_okBtn->setText(tmpText);
		m_cancelBtn->setText(translateText(L_TURBO_BTN_CANCEL));
	}
	else
	{
		QString tmpText1 = translateText(L_TURBO_BTN_OK);
		if(m_timeoutCounter > 0)
		{
			tmpText1 += "(";
			tmpText1 += QString::number(m_timeoutCounter);
			tmpText1 += ")";
		}
		m_okBtn->setText(tmpText1);
	}
}

void NewFileNotifyWidget::applyStyle()
{
    QFile cssFile(GENIE2_RES(NEWDEVICENOTIFYWIDGET_STYLEFILE));
    if (cssFile.open(QIODevice::ReadOnly)) {
        setStyleSheet(QString::fromUtf8(cssFile.readAll().data()));
    }
}

QString NewFileNotifyWidget::translateText(int idx, const QString &defaultText)
{
    QString text;
    emit translateText(idx,&text);
    return text.isEmpty() ? defaultText : text;
}

void NewFileNotifyWidget::mousePressEvent(QMouseEvent *event)
{
    m_pos = event->globalPos();
    m_canMove = true;
}

void NewFileNotifyWidget::mouseReleaseEvent(QMouseEvent *)
{
    m_canMove = false;
}

void NewFileNotifyWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_canMove && (event->buttons() & Qt::LeftButton))
    {
        move(pos().x() - (m_pos.x() - event->globalPos().x()),
             pos().y() - (m_pos.y() - event->globalPos().y()));

        m_pos = event->globalPos();
    }
}

void NewFileNotifyWidget::openBtnClicked()
{
	if (0 == m_iFolder)
	{
		m_strFile.replace("/","\\");

		QProcess::startDetached("explorer "+m_strFile);
	}
	else if(1 == m_iFolder)
	{
		QString strTemp;
		m_strFolder.replace("/","\\");
		if ("\\" == m_strFolder.at(m_strFolder.count()-1))
		{
			strTemp = m_strFolder + m_strFileName;
		}
		else
		{
			strTemp = m_strFolder + "\\" + m_strFileName;
		}	
		QProcess::startDetached("explorer "+strTemp);
	}
	else if(2 == m_iFolder)
	{
		m_strFolder.replace("/","\\");

		QProcess::startDetached("explorer "+ m_strFolder);
	}
}

void NewFileNotifyWidget::openFolderBtnClicked()
{
	m_strFile.replace("/","\\");
	QString strOpen  = "explorer /select, " + m_strFile;

	QProcess::startDetached(strOpen);
}

void NewFileNotifyWidget::rejectBtnClicked()
{
	emit rejectSaveFile(m_iFolder, m_fhandle);
	emit saveResult(m_fhandle,"REJECT");
	close();
}

void NewFileNotifyWidget::closeBtnClicked()
{
    if(m_timerId)
    {
        killTimer(m_timerId);
        m_timerId = 0;
    }
    close();
}

void NewFileNotifyWidget::okBtnClicked()
{
	if(m_timerId)
	{
		killTimer(m_timerId);
		m_timerId = 0;
	}
	close();
}

void NewFileNotifyWidget::saveBtnClicked()
{
	m_saveBtn->setEnabled(false);
	m_rejectBtn->setEnabled(false);

	bool bContainsPoint;
	QStringList strList;
	if (m_strFileName.contains("."))
	{
		strList = m_strFileName.split(".");
		bContainsPoint = true;
	}
	else
	{
		bContainsPoint = false;
	}

	QString strSuffix;
	QString strFilter;
	if (bContainsPoint)
	{
		int iLen = strList.length();
		strSuffix = strList.at(iLen-1);
		strFilter += strSuffix;
		strFilter += " files(*.";
		strFilter += strSuffix;
		strFilter += ")";
	}
	else
	{
		strFilter += "All Files(*.*)";
	}
	QString fileName;
	std::vector<std::wstring> vecNames;
	if (0 == m_iFolder)
	{
		qDebug() << "save file";
		QString strTemp;
#ifdef Q_OS_MAC
		if ("" != m_strLastFolder)
		{
			strTemp = m_strLastFolder + "/" + m_strFileName;
		}
		else
		{
			strTemp = m_strFileName;
		}
#else
		if ("" != m_strLastFolder)
		{
			strTemp = m_strLastFolder + "\\" + m_strFileName;
		}
		else
		{
			strTemp = m_strFileName;
		}
#endif

		OPENFILENAME ofn;
		TCHAR szOpenFileNames[80*MAX_PATH] = L"";
		
		ZeroMemory( &ofn, sizeof(ofn) );
		ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
		ofn.lStructSize = sizeof(ofn);
		std::wstring wsFileName = m_strFileName.toStdWString();
		const wchar_t *wstrFileName = wsFileName.c_str();
		int wstrFileNameLen = wcslen(wstrFileName);
		wmemcpy(szOpenFileNames,wstrFileName,wstrFileNameLen);
		ofn.hwndOwner = GetParent(HWND(winId()));
		ofn.lpstrFile =szOpenFileNames;
		ofn.nMaxFile = sizeof(szOpenFileNames);
		ofn.lpstrFilter = L"All Files(*.*)\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;

		if ("" != m_strLastFolder)
		{
			TCHAR szDir[MAX_PATH] = L"";
			std::wstring wstrDir = m_strLastFolder.toStdWString();
			wmemcpy(szDir,wstrDir.c_str(),wcslen(wstrDir.c_str()));
			ofn.lpstrInitialDir = szDir;
		}
		else
		{
			ofn.lpstrInitialDir = NULL;
		}

		TCHAR defaultDir[1024] = L"";
		int nLen = 0;
		if(GetSaveFileName(&ofn))
		{  
			wcsncpy_s(defaultDir, ofn.lpstrFile, ofn.nFileOffset);
			int nDirLength = wcslen(defaultDir);
			nLen = lstrlen(szOpenFileNames);

			m_strLastFolder = WIN32_WC2CU(defaultDir);
			emit folderChange(m_strLastFolder);
			if (szOpenFileNames[nDirLength])
			{
				vecNames.push_back(szOpenFileNames);
				m_strFile = WC2CU(szOpenFileNames);
			}
		}
		else
		{
			m_saveBtn->setEnabled(true);
			m_rejectBtn->setEnabled(true);
		}
	}
	else if (1 == m_iFolder || 2 == m_iFolder)
	{
		qDebug() << "save directory";

		IMalloc			*iMalloc = NULL;
		if (!SUCCEEDED(SHGetMalloc(&iMalloc)))
			return ;

		std::wstring strLastFolder = m_strLastFolder.toStdWString();

		TCHAR szBuffer[MAX_PATH] = L"";
		BROWSEINFO bi;   
		ZeroMemory(&bi,sizeof(BROWSEINFO));   
		bi.hwndOwner = GetParent(HWND(winId()));   
		bi.pidlRoot = 0;
		bi.pszDisplayName = szBuffer;
		bi.ulFlags = BIF_RETURNONLYFSDIRS;	
		bi.lParam = (LPARAM)(strLastFolder.c_str());
		bi.lpfn = BrowseDirDlgProc;
		bi.iImage = 0;

		LPITEMIDLIST	pidlBrowse;
		BOOL			ret = FALSE;
		if ((pidlBrowse = SHBrowseForFolder(&bi)) != NULL)
		{
			ret = SHGetPathFromIDList(pidlBrowse, szBuffer);
			iMalloc->Free(pidlBrowse);
			if (!ret)
			{
				iMalloc->Release();

				m_saveBtn->setEnabled(true);
				m_rejectBtn->setEnabled(true);

				return;
			}
		}
		else
		{
			iMalloc->Release();

			m_saveBtn->setEnabled(true);
			m_rejectBtn->setEnabled(true);
			return;
		}

		iMalloc->Release();

		m_strLastFolder = WC2CU(szBuffer);

		vecNames.push_back(szBuffer);
		m_strFolder = m_strLastFolder;
	}

	if (!vecNames.empty())
	{
		qDebug() << "folder change";
		emit folderChange(m_strLastFolder);

		m_bodyEdit->setText(m_strFileName);

		m_openBtn->setEnabled(false);
		m_openBtn->setText(translateText(L_TURBO_BTN_OPEN));
		m_openBtn->setVisible(true);	
		m_saveBtn->setVisible(false);
		m_rejectBtn->setVisible(false);
		m_okBtn->setVisible(false);
		QString tmpText = translateText(L_TURBO_BTN_OK);
		if(m_timeoutCounter > 0)
		{
			tmpText += "(";
			tmpText += QString::number(m_timeoutCounter);
			tmpText += ")";
		}
		m_okBtn->setText(tmpText);
		m_cancelBtn->setVisible(true);
		m_cancelBtn->setText(translateText(L_TURBO_BTN_CANCEL));

		if (0 == m_iFolder)
		{
			m_openFolderBtn->setText(translateText(L_TURBO_BTN_OPEN_FOLDER));
			m_openFolderBtn->setEnabled(false);
			m_openFolderBtn->setVisible(true);
		}
		m_okBtn->setVisible(false);

		emit saveResult(m_fhandle,WC2CU(vecNames.at(0).c_str()));       
	}
}


void NewFileNotifyWidget::notifyNewDevice(const QString &title,int defaultCount)
{
	if (0 == m_iType)
	{
		m_bodyEdit->setText(m_strFileName);

		m_saveBtn->setText(translateText(L_TURBO_BTN_SVAE));
		m_rejectBtn->setText(translateText(L_TURBO_BTN_REJECT));

		QString tmpText = translateText(L_TURBO_BTN_OK);
		if(m_timeoutCounter > 0)
		{
			tmpText += "(";
			tmpText += QString::number(m_timeoutCounter);
			tmpText += ")";
		}
		m_okBtn->setText(tmpText);

		QStringList strList = title.split("\\");
		qDebug() << "list size :" << strList.count();
		m_strFileName = strList.at(0);
		QString strText;
		if (m_iFolder == 0 || m_iFolder == 2)
		{
			if (defaultCount>1)
			{
				m_bodyEdit->setText(translateText(L_TURBO_RECEIVE_FILES).arg(m_strFileName).arg(QString::number(defaultCount-1)).arg(m_strDeviceName));
			}
			else
			{
				QString strTest = translateText(L_TURBO_RECEIVE_FILE).arg(m_strFileName).arg(m_strDeviceName);
				m_bodyEdit->setText(translateText(L_TURBO_RECEIVE_FILE).arg(m_strFileName).arg(m_strDeviceName));
			}

			m_titleLabel->setText(translateText(L_TURBO_TRANSFER));
		}
		else
		{
			m_bodyEdit->setText(translateText(L_TURBO_RECEIVE_FOLDER).arg(m_strFileName).arg(m_strDeviceName));
			m_titleLabel->setText(translateText(L_TURBO_TRANSFER));
		}

		setVisible(true);
		QRect rect = QApplication::desktop()->screenGeometry(-1);
		int x = rect.width() - width() - 10;

		if(!m_notifyAnimation)
		{
			m_notifyAnimation = new QPropertyAnimation(this, "geometry",this);
		}

		m_notifyAnimation->setDuration(1000);
		m_notifyAnimation->setStartValue(QRect(x, rect.height(), width(), height()));
		m_notifyAnimation->setEndValue(QRect(x, rect.height() - height() - 40, width(), height()));
		m_notifyAnimation->start();
	}
	else if(1 == m_iType)
	{
		QString tmpText = translateText(L_TURBO_BTN_OK);
		if(m_timeoutCounter > 0)
		{
			tmpText += "(";
			tmpText += QString::number(m_timeoutCounter);
			tmpText += ")";
		}
		m_okBtn->setText(tmpText);
		m_cancelBtn->setText(translateText(L_TURBO_BTN_CANCEL));

		m_strFileName = title;
		QString strTemp = getActualName(title);

		if (0 == m_iFolder)
		{
			m_bodyEdit->setText(translateText(L_TURBO_SEND_FILE).arg(strTemp));
		}
		else if (1 == m_iFolder)
		{
			m_bodyEdit->setText(translateText(L_TURBO_SEND_FOLDER).arg(strTemp));
		}
		else if (2 == m_iFolder)
		{
			m_bodyEdit->setText(translateText(L_TURBO_SEND_FILES).arg(strTemp).arg(QString::number(defaultCount-1)));
		}

		m_titleLabel->setText(translateText(L_TURBO_TRANSFER));
		setVisible(true);
		QRect rect = QApplication::desktop()->screenGeometry(-1);
		int x = rect.width() - width() - 10;

		if(!m_notifyAnimation)
		{
			m_notifyAnimation = new QPropertyAnimation(this, "geometry",this);
		}

		m_notifyAnimation->setDuration(1000);
		m_notifyAnimation->setStartValue(QRect(x, rect.height(), width(), height()));
		m_notifyAnimation->setEndValue(QRect(x, rect.height() - height() - 40, width(), height()));
		m_notifyAnimation->start();

		if(m_timerId != 0)
		{
			killTimer(m_timerId);
			m_timerId = 0;
			m_timeoutCounter = (defaultCount <= 0 ? DELAY_COUNTER_MAX : defaultCount);
		}
	}
}

void NewFileNotifyWidget::cancelBtnClicked()
{
	m_bSendOver = true;
	m_cancelBtn->setVisible(false);
	if (0 == m_iType)
	{
		m_openBtn->setVisible(false);
		m_openFolderBtn->setVisible(false);
		m_okBtn->setVisible(true);
		m_titleLabel->setText(translateText(L_TURBO_FILE_RECEVING_CANCELED));	
	}
	else
	{
		m_okBtn->setEnabled(true);
		m_titleLabel->setText(translateText(L_TURBO_FILE_SENDING_CANCELED));
	}
	if (0 == m_timerId)
	{
		m_timerId = startTimer(DELAYINTERVAL_MS);
	}
	emit stopTransfer(m_fhandle);
}


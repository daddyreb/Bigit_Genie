//GetWhiteListThread
#include <QThread>
#include <QStringList>
#include "smartWifiProtocol.h"
#include "SmartWifiConnector.h"

class GetWhiteListThread : public QThread
{
	Q_OBJECT
public:
	GetWhiteListThread(QUrl server_url){ m_server_url = server_url; };
signals:
	void FinishUpdateWhiteList(bool success, QStringList list_private, QStringList list_public, QString error);
private:
	void run()
	{
		SmartWifiConnector connector;

		QString error;
		QStringList list1, list2;

		bool success = false;

		do 
		{
			if (! connector.GetWhiteUrl(m_server_url, WHITE_URL_PRIVATE, list1, error))
				break;

			if (! connector.GetWhiteUrl(m_server_url, WHITE_URL_PUBLIC, list2, error))
				break;


			success = true;
		} while (false);
		emit FinishUpdateWhiteList(success, list1, list2, error);
	};

	QUrl m_server_url;
};

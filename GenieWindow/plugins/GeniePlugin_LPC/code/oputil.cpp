#include "common.h"
#include "logger.h"
#include "oputil.h"
#include <QtEndian>
#include <QByteArray>
#include <QHostAddress>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStack>
#include <QStringList>
#include <QVector>
#include <QXmlStreamReader>

#include <QSettings>

const char *API_KEY =
	//"3443313B70772D2DD73B45D39F376199"
	/* Offical for Mac */"3D8C85A77ADA886B967984DF1F8B3711"
	;

void getRidOfSslErrors(QNetworkReply *reply);

SimpleOp::SimpleOp(QObject *parent)
: AsyncOp(parent)
{
}

SimpleOp::~SimpleOp()
{
}

void SimpleOp::internalStart()
{
	connect(this, SIGNAL(internalStartSignal()), SLOT(internalStartSlot()), Qt::QueuedConnection);
	emit internalStartSignal();
}

void SimpleOp::startImpl()
{
}

void SimpleOp::internalStartSlot()
{
	startImpl();
}

static QByteArray urlencode(const QByteArray& data)
{
	return data.toPercentEncoding();
}

static QByteArray encodePostData(const QMap<QByteArray, QByteArray>& params)
{
	QByteArray data;

	QMap<QByteArray, QByteArray>::const_iterator it = params.begin();
	QMap<QByteArray, QByteArray>::const_iterator ie = params.end();
	for (; it != ie; ++it) {
		data.append(urlencode(it.key()));
		data.append('=');
		data.append(urlencode(it.value()));
		data.append('&');
	}

	if (!data.isEmpty()) {
		data.resize(data.length() - 1);
	}

	return data;
}

QNetworkReply *webApiPost(QNetworkAccessManager *nam, const QByteArray& function, const QStringList& paramNames, const QStringList& paramValues)
{
	static const QString WEBAPI_URL = QString::fromUtf8("https://api.opendns.com/v1/");

	if (paramNames.length() != paramValues.length()) {
		DBGLOG(LOG_ERROR, 1, QString::fromUtf8("Invalid arguments! paramNames.length() != paramValues.length(), VERY BAD!!"));
		return NULL;
	}

	QMap<QByteArray, QByteArray> params;
	params.insert("api_key", API_KEY);
	params.insert("method", function);
	for (int i = 0; i < paramNames.length(); i++) {
		params.insert(paramNames.at(i).toUtf8(), paramValues.at(i).toUtf8());
	}

	QByteArray postData = encodePostData(params);

	QNetworkRequest req(WEBAPI_URL);
	req.setHeader(QNetworkRequest::ContentTypeHeader, QByteArray("application/x-www-form-urlencoded"));
	req.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());

	DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("webApiPost data: %1").arg(QString::fromUtf8(postData)));
        qDebug()<<"****webApiPost():***"<<postData;
	return nam->post(req, postData);
}

#if 1

QByteArray xmlElWithVal(const char *el, const char *val, int indent, bool escapeVal)
{
	QString retval;
	if (indent == -1) {
		return retval.sprintf("<%s>%s</%s>", el, val, el).toUtf8();
	}

	QString str = QString(" ").repeated(indent);
	QString str2 = str;
	str2 += "  ";
	QString tt =  QString("%1<%2>\n%3%4\n%5</%6>").arg(str).arg(el).arg(str2).arg(val).arg(str).arg(el);
        qDebug()<<"****xmlElWithVal()****:"<<tt;
        return tt.toUtf8();
}

QNetworkReply *routerApiPost(QNetworkAccessManager *nam, const QString& action, const QByteArray& ns, const QByteArray& cmd, const QStringList& paramNames, const QStringList& paramValues)
{
	static const QByteArray SOAP_TEMPLATE = "<?xml version=\"1.0\" encoding=\"utf-8\" ?> \r\n<env:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\r\n        xmlns:env=\"http://schemas.xmlsoap.org/soap/envelope/\">\r\n  <env:Header>\r\n<SessionID>\r\n  E6A88AE69687E58D9A88\r\n</SessionID>\r\n</env:Header>\r\n<env:Body>\r\n%s\r\n  </env:Body> \r\n</env:Envelope>";

	if (paramNames.length() != paramValues.length()) {
		DBGLOG(LOG_ERROR, 1, QString::fromUtf8("Invalid arguments! paramNames.length() != paramValues.length(), VERY BAD!!"));
		return NULL;
	}
	QString xmlText;

	if (paramNames.isEmpty()) {
		xmlText.sprintf(SOAP_TEMPLATE, xmlElWithVal(cmd, "", -1, false).data());
	} else {
		QString s;
		for (int i = 0; i < paramNames.length(); i++) {
			if (i != 0) {
				s += "\n      ";
			}
			s += xmlElWithVal(paramNames.at(i).toUtf8(), paramValues.at(i).toUtf8(), -1, true);
		}
		xmlText.sprintf(SOAP_TEMPLATE, xmlElWithVal(cmd, s.toUtf8(), 4, false).data());
	}

	QByteArray xml = xmlText.toUtf8();

	DBGLOG(LOG_DEBUG, 20, QString::fromUtf8("soapPost xml: %1").arg(QString::fromUtf8(xml.data())));

	QNetworkRequest req(action);
	req.setHeader(QNetworkRequest::ContentTypeHeader, QString::fromUtf8("text/xml; charset=utf-8"));
	req.setHeader(QNetworkRequest::ContentLengthHeader, xml.length());
	req.setRawHeader("Accept", "text/xml");
	req.setRawHeader("SOAPAction", QString::fromUtf8("\"%1#%2\"").arg(QString::fromUtf8(ns)).arg(QString::fromUtf8(cmd)).toUtf8());
        qDebug()<<"**action:"<<action<<" ns:"<<ns<<" cmd:"<<cmd;
        qDebug()<<"****routerApiPost():****"<<xml;
	return nam->post(req, xml);
};

#else

QNetworkReply *routerApiPost(QNetworkAccessManager *nam, const QString& action, const QByteArray& ns, const QByteArray& cmd, const QStringList& paramNames, const QStringList& paramValues)
{
	static const QByteArray SOAP_TEMPLATE_P1 = "\xef\xbb\xbf<?xml version=\"1.0\" encoding=\"utf-8\" ?> \r\n<env:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\r\n        xmlns:env=\"http://schemas.xmlsoap.org/soap/envelope/\">\r\n  <env:Header>\r\n<SessionID>E6A88AE69687E58D9A88</SessionID></env:Header>\r\n<env:Body>\r\n    ";
	static const QByteArray SOAP_TEMPLATE_P2 = "\r\n  </env:Body> \r\n</env:Envelope>";

	if (paramNames.length() != paramValues.length()) {
		DBGLOG(LOG_ERROR, 1, QString::fromUtf8("Invalid arguments! paramNames.length() != paramValues.length(), VERY BAD!!"));
		return NULL;
	}

	bool needNS = true;
	if (cmd == "Authenticate" && ns == "urn:NETGEAR-ROUTER:service:ParentalControl:1") {
		needNS = false;
	}

	QByteArray body;
	if (needNS) {
		body.append("<M1:");
		body.append(cmd);
		body.append(" xmlns:M1=\"");
		body.append(ns);
		body.append("\">\r\n");
	} else {
		body.append("<");
		body.append(cmd);
		body.append(">");
	}

	for (int i = 0; i < paramNames.length(); i++) {
		QByteArray nameUtf8 = paramNames.at(i).toUtf8();
		body.append("<");
		body.append(nameUtf8);
		body.append(">");

		body.append(paramValues.at(i).toUtf8());

		body.append("</");
		body.append(nameUtf8);
		body.append(">\r\n");
	}

	if (needNS) {
		body.append("</M1:");
		body.append(cmd);
		body.append(">");
	} else {
		body.append("</");
		body.append(cmd);
		body.append(">");
	}

	QByteArray xml = SOAP_TEMPLATE_P1 + body + SOAP_TEMPLATE_P2;
	DBGLOG(LOG_DEBUG, 20, QString::fromUtf8("soapPost xml: %1").arg(QString::fromUtf8(xml.data())));

	QNetworkRequest req(action);
	req.setHeader(QNetworkRequest::ContentTypeHeader, QString::fromUtf8("text/xml; charset=utf-8"));
	req.setHeader(QNetworkRequest::ContentLengthHeader, xml.length());
	req.setRawHeader("Accept", "text/xml");
	req.setRawHeader("SOAPAction", QString::fromUtf8("\"%1#%2\"").arg(QString::fromUtf8(ns)).arg(QString::fromUtf8(cmd)).toUtf8());

	return nam->post(req, xml);
};

#endif

class SimpleJsonParser
{
public:
	SimpleJsonParser();
	~SimpleJsonParser();

	bool parse(const QByteArray& json);
	const QVariantMap& result() const;

private:
	void resetState();
	static bool isNumberLike(char c);

private:
	enum Token {
		Token_Begin,
		Token_End,
		Token_String,
		Token_Number,
		Token_Comma,
		Token_Colon,
		Token_LBracket,
		Token_RBracket,
		Token_NULL,
		Token_EOF,
		Token_ERROR,
	};

	Token nextToken();

	QVariantMap m_result;
	QByteArray m_source;
	const char *m_ptr;
	const char *m_end;
	QByteArray m_str;
	double m_num;
};

SimpleJsonParser::SimpleJsonParser()
{
	resetState();
}

SimpleJsonParser::~SimpleJsonParser()
{
}

bool SimpleJsonParser::isNumberLike(char c)
{
	if (c == '+' || c == '-' || (c >= '0' && c <= '9') || c == '.') {
		return true;
	}
	return false;
}

void SimpleJsonParser::resetState()
{
	m_ptr = NULL;
	m_end = NULL;
}

SimpleJsonParser::Token SimpleJsonParser::nextToken()
{
	if (!m_ptr) {
		return Token_ERROR;
	}

	if (m_ptr == m_end) {
		return Token_EOF;
	}

	static const char *ws = " \t\n\r";

	// skip whitespace
	while (m_ptr != m_end && strchr(ws, *m_ptr)) {
		++m_ptr;
	}

	if (m_ptr == m_end) {
		return Token_EOF;
	}

	char c = *m_ptr++;

	switch (c) {
	case '{':
		return Token_Begin;
	case '}':
		return Token_End;
	case ':':
		return Token_Colon;
	case ',':
		return Token_Comma;
	case '[':
		return Token_LBracket;
	case ']':
		return Token_RBracket;
	}

	// string ?
	if (c == '"') {
		m_str.clear();
		const char *ss = m_ptr;
		while (m_ptr != m_end && *m_ptr != '"') {
			++m_ptr;
		}
		if (m_ptr == m_end) {
			return Token_ERROR;
		}

		m_str = QByteArray(ss, m_ptr++ - ss);
		return Token_String;
	}

	// number ?
	if (isNumberLike(c)) {
		const char *ss = m_ptr - 1;
		while (m_ptr != m_end && isNumberLike(*m_ptr)) {
			++m_ptr;
		}
		bool ok;
		m_num = QByteArray(ss, m_ptr - ss).toDouble(&ok);
		if (!ok) {
			return Token_ERROR;
		}
		return Token_Number;
	}

	// null ?
	if (c == 'n' && m_ptr + 2 < m_end && m_ptr[0] == 'u' && m_ptr[1] == 'l' && m_ptr[2] == 'l') {
		m_ptr += 3;
		return Token_NULL;
	}

	return Token_ERROR;
}

bool SimpleJsonParser::parse(const QByteArray& json)
{
	resetState();

	m_source = json;
	m_ptr = m_source.data();
	m_end = m_ptr + m_source.length();

	QStack<QString> stack;
	QVariantMap result;

	QVector<Token> expectedTokens;
	expectedTokens.push_back(Token_Begin);
	Token prevToken = Token_ERROR;

	bool inList = false;
	QStringList ls0;

	QString lastStr;
	QString prefix;
	QString lastKey;

	for (;;) {
		Token token = nextToken();
		if (expectedTokens.indexOf(token) < 0) {
			DBGLOG(LOG_ERROR, 2, QString::fromUtf8("unexpected token %1").arg(token));
			return false;
		}
		switch (token) {
		case Token_ERROR:
			return false;
		case Token_EOF:
			if (!stack.empty()) {
				DBGLOG(LOG_ERROR, 2, QString::fromUtf8("unblanced { }, bad json!"));
				return false;
			}
			m_result = result;
			return true;
		case Token_Begin:
			DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("token BEGIN"));
			if (stack.empty()) {
				stack.push_back(QString::fromUtf8(""));
			} else {
				stack.push_back(prefix);
				prefix.append(lastKey);
                prefix.append(QChar::fromLatin1('.'));
			}
			expectedTokens.clear();
			expectedTokens.push_back(Token_End);
			expectedTokens.push_back(Token_String);
			break;
		case Token_End:
			DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("token END"));
			prefix = stack.pop();
			if (!stack.empty()) {
				expectedTokens.clear();
				expectedTokens.push_back(Token_Comma);
				expectedTokens.push_back(Token_End);
			} else {
				expectedTokens.push_back(Token_EOF);
			}
			break;
		case Token_Comma:
			DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("token COMMA"));
			expectedTokens.clear();
			expectedTokens.push_back(Token_End);
			expectedTokens.push_back(Token_String);
			break;
		case Token_Colon:
			DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("token COLON"));
			expectedTokens.clear();
			expectedTokens.push_back(Token_Begin);
			expectedTokens.push_back(Token_String);
			expectedTokens.push_back(Token_Number);
			expectedTokens.push_back(Token_LBracket);
			expectedTokens.push_back(Token_NULL);
			break;
		case Token_String:
			DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("token STRING [%1]").arg(QString::fromUtf8(m_str)));

			if (inList) {
				ls0.push_back(QString::fromUtf8(m_str));

				expectedTokens.clear();
				expectedTokens.push_back(Token_Comma);
				expectedTokens.push_back(Token_RBracket);
			} else {
				if (prevToken == Token_Colon) {
					DBGLOG(LOG_DEBUG, 12, QString::fromUtf8("submit '%3%1'='%2'").arg(lastKey).arg(QString::fromUtf8(m_str)).arg(prefix));
					result.insert(prefix + lastKey, QString::fromUtf8(m_str));
				} else {
					lastKey = QString::fromUtf8(m_str);
				}

				// TODO:
				expectedTokens.clear();
				expectedTokens.push_back(Token_End);
				expectedTokens.push_back(Token_Comma);
				expectedTokens.push_back(Token_Colon);
			}

			break;
		case Token_Number:
			DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("token NUMBER [%1]").arg(m_num));
			DBGLOG(LOG_DEBUG, 12, QString::fromUtf8("submit '%3%1'=%2").arg(lastKey).arg(m_num).arg(prefix));
			result.insert(prefix + lastKey, m_num);
			expectedTokens.clear();
			expectedTokens.push_back(Token_End);
			expectedTokens.push_back(Token_Comma);
			break;
		case Token_LBracket:
			DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("token LBracket"));
			expectedTokens.clear();
			expectedTokens.push_back(Token_String);
			expectedTokens.push_back(Token_RBracket);
			inList = true;
			ls0.clear();
			break;
		case Token_RBracket:
			DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("token RBracket"));
			result.insert(prefix + lastKey, ls0);
			expectedTokens.clear();
			expectedTokens.push_back(Token_End);
			expectedTokens.push_back(Token_Comma);
			inList = false;
			break;
		case Token_NULL:
			DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("token RBracket"));
			result.insert(prefix + lastKey, QVariant());
			expectedTokens.clear();
			expectedTokens.push_back(Token_End);
			expectedTokens.push_back(Token_Comma);
			break;
		}

		prevToken = token;
	}

	return false;
}

const QVariantMap& SimpleJsonParser::result() const
{
	return m_result;
}

bool parseJsonLite(const QByteArray& json, QVariantMap& result)
{
	SimpleJsonParser parser;
	if (!parser.parse(json)) {
		return false;
	}

	result = parser.result();
	return true;
}
/*
int test_dummy()
{
// {"status":"success","response":{"available":"no"}}
// {"status":"failure","error":3001,"error_message":"Username exists -- easyrock"}

	QVariantMap result;
	parseJsonLite("{\"status\":\"success\",\"response\":{\"available\":\"no\" ,\"ak\":{\"kb\":5},\"aa\":3.0},\"bkt\":\"40\"}", result);
	parseJsonLite("{\"status\":\"failure\",\"error\":3001,\"error_message\":\"Username exists -- easyrock\"}", result);

	return 0;
}
*/
bool isPrivateAddress(quint32 ipv4)
{
	if ((ipv4 & 0xFF000000) == 0x0A000000) {
		return true;
	}

	if ((ipv4 & 0xFF000000) == 0xAC000000) {
		return true;
	}

	if ((ipv4 & 0xFFFF0000) == 0xC0A80000) {
		return true;
	}

	return false;
}

#ifdef LPC_HAVE_GATEWAY_MAC

#if defined(Q_OS_WIN32)

#include <windows.h>
#include <iphlpapi.h>

static bool findMac(const IP_ADAPTER_INFO *adapterInfo, quint32 ipv4Host, QByteArray& mac)
{
	Q_UNUSED(adapterInfo)

	DWORD ip = qToBigEndian(ipv4Host);

	ULONG bufferLen = sizeof(MIB_IPNETROW) * 1000;
	std::vector<BYTE> buffer(bufferLen);
	MIB_IPNETTABLE *ipTable = reinterpret_cast<MIB_IPNETTABLE*>(&buffer[0]);
	if (NO_ERROR != GetIpNetTable(ipTable, &bufferLen, FALSE)) {
		return false;
	}

	for (DWORD i = 0; i < ipTable->dwNumEntries; i++) {
		const MIB_IPNETROW& row = ipTable->table[i];
		if (row.dwAddr == ip && row.dwPhysAddrLen == 6) {
			mac = QByteArray(reinterpret_cast<const char*>(&row.bPhysAddr[0]), 6);
			return true;
		}
	}

	return false;
}

bool getGatewayMacAddress(quint32 ipv4Host, QByteArray& mac)
{
	ULONG bufferLen = sizeof(IP_ADAPTER_INFO) * 32;
	std::vector<BYTE> buffer(bufferLen);
	IP_ADAPTER_INFO *ar = reinterpret_cast<IP_ADAPTER_INFO*>(&buffer[0]);
	if (ERROR_SUCCESS != GetAdaptersInfo(ar, &bufferLen)) {
		return false;
	}

	IP_ADAPTER_INFO *adapterInfo = ar;
	while (adapterInfo) {
		IP_ADDR_STRING *gatewayAddress = &adapterInfo->GatewayList;
		while (gatewayAddress) {
			QHostAddress gatewayAddr(QString::fromUtf8(gatewayAddress->IpAddress.String));
			if (gatewayAddr.toIPv4Address() == ipv4Host) {
				// don't findMac, use our mac instead!!!
				//return findMac(adapterInfo, ipv4Host, mac);
				for (int i = 0; i < 6; i++) {
					mac[i] = adapterInfo->Address[i];
				}
				return true;
			}
			gatewayAddress = gatewayAddress->Next;
		}
		adapterInfo = adapterInfo->Next;
	}

	return false;
}

#elif defined(Q_OS_DARWIN)

#include <sys/param.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/sysctl.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <arpa/inet.h>

#include <err.h>
#include <errno.h>
#include <netdb.h>
#include <nlist.h>
#include <paths.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if 0

bool getGatewayMacAddress(quint32 ipv4Host, QByteArray& mac)
{
	std::vector<char> data(512 * 1024);

	int mib[6];
	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;
	mib[3] = AF_INET;
	mib[4] = NET_RT_FLAGS;
	mib[5] = RTF_LLINFO;

	size_t cb = data.size();
	if (sysctl(mib, 6, &data[0], &cb, NULL, 0)  == 0) {
		const char *p = &data[0];
		const char *pe = p + cb;
		const rt_msghdr *rtm;
		for (; p != pe; p += rtm->rtm_msglen) {
	        rtm = reinterpret_cast<const rt_msghdr*>(p);

			const sockaddr_inarp *sin = reinterpret_cast<const sockaddr_inarp*>(rtm + 1);
			const sockaddr_dl *sdl = reinterpret_cast<const sockaddr_dl*>(sin + 1);

			if (sdl->sdl_alen) {
				if (ntohl(sin->sin_addr.s_addr) == ipv4Host) {
					u_char *cp = (u_char*)LLADDR(sdl);
					for (int i = 0; i < 6; i++) {
						mac.push_back(cp[i]);
					}
					return true;
				}
			}
		}
	}
	return false;
}

#else

#define ROUNDUP(a) ((a) > 0 ? (1 + (((a) - 1) | (sizeof(uint32_t) - 1))) : sizeof(uint32_t))

void get_rtaddrs(int addrs, const sockaddr *sa, const sockaddr **rti_info)
{
	for (int i = 0; i < RTAX_MAX; i++) {
		if (addrs & (1 << i)) {
			rti_info[i] = sa;
			sa = reinterpret_cast<const sockaddr*>(reinterpret_cast<const char*>(sa) + ROUNDUP(sa->sa_len));
		} else {
			rti_info[i] = NULL;
		}
	}
}

bool getGatewayMacAddress(quint32 ipv4Host, QByteArray& mac)
{
	std::vector<char> data(512 * 1024);

	u_short if_index = 0xFFFF;
	
	int mib[6];
	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;
	mib[3] = AF_INET;
	mib[4] = NET_RT_DUMP2;;
	mib[5] = 0;

	size_t cb = data.size();
	if (sysctl(mib, 6, &data[0], &cb, NULL, 0) == 0) {
		const char *p = &data[0];
		const char *pe = p + cb;
		const rt_msghdr2 *rtm;
		for (; p != pe; p += rtm->rtm_msglen) {
	        rtm = reinterpret_cast<const rt_msghdr2*>(p);

			const sockaddr *sa = reinterpret_cast<const sockaddr*>(rtm + 1);
			const sockaddr *rti_info[RTAX_MAX];

			get_rtaddrs(rtm->rtm_addrs, sa, rti_info);

			if ((rtm->rtm_addrs & RTA_DST) && (rtm->rtm_addrs & RTA_NETMASK) && (rtm->rtm_addrs & RTA_GATEWAY)) {
				const sockaddr_in *in_dst = reinterpret_cast<const sockaddr_in*>(rti_info[RTAX_DST]);
				const sockaddr_in *in_gw = reinterpret_cast<const sockaddr_in*>(rti_info[RTAX_GATEWAY]);
				const sockaddr_in *in_mask = reinterpret_cast<const sockaddr_in*>(rti_info[RTAX_NETMASK]);
				char nnb[IFNAMSIZ];
				if (ntohl(in_mask->sin_addr.s_addr) == 0 || rti_info[RTAX_NETMASK]->sa_len == 0) {
					if (ntohl(in_gw->sin_addr.s_addr) == ipv4Host) {
						printf("%d %d %d %x %x %d [%s]\n", sa->sa_family, AF_INET, AF_INET6, ntohl(in_dst->sin_addr.s_addr), ntohl(in_gw->sin_addr.s_addr), rtm->rtm_index, if_indextoname(rtm->rtm_index, nnb));
						if_index = rtm->rtm_index;
						break;
					}
				}
			}
			
		}
	}
	
	if (if_index == 0xFFFF) {
		return false;
	}

	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;
	mib[3] = 0;
	mib[4] = NET_RT_IFLIST2;;
	mib[5] = 0;
	
	cb = data.size();
	if (sysctl(mib, 6, &data[0], &cb, NULL, 0) == 0) {
		const char *p = &data[0];
		const char *pe = p + cb;
		const if_msghdr *ifm;
		for (; p != pe; p += ifm->ifm_msglen) {
	        ifm = reinterpret_cast<const if_msghdr*>(p);
			if (ifm->ifm_type == RTM_IFINFO2) {
				const if_msghdr2 *if2m = reinterpret_cast<const if_msghdr2*>(ifm);
				const sockaddr_dl *sdl = reinterpret_cast<const sockaddr_dl*>(if2m + 1);
				if (sdl->sdl_index == if_index) {
					char nnb[IFNAMSIZ];
					strncpy(nnb, sdl->sdl_data, sdl->sdl_nlen);
					nnb[sdl->sdl_nlen] = 0;
					const u_char *cp = reinterpret_cast<const u_char*>(LLADDR(sdl));
					for (int i = 0; i < 6; i++) {
						mac[i] = cp[i];
					}
					printf("55 [%d] [%s] %d %x %x %x %x %x %x\n", sdl->sdl_index, nnb, IFNAMSIZ, cp[0], cp[1], cp[2], cp[3], cp[4], cp[5]);
					return true;
				}
			}
		}
	}

	return false;
}

#endif

#else

bool getGatewayMacAddress(quint32 ipv4Host, QByteArray& mac)
{
	// TODO:
	DBGLOG(LOG_ERROR, 0, QString::fromUtf8("Unimplemented getGatewayMacAddress on this platform!!!"));
	return false;
}

#endif

#endif // LPC_HAVE_GATEWAY_MAC

QList<QByteArray> nice_split(const QByteArray& data)
{
	QList<QByteArray> result;
	const char *p = data.data();
	const char *pe = p + data.length();
	const char *pl = p;
	for (; p != pe; ++p) {
		if (*p == '\r' || *p == '\n') {
			if (p != pl) {
				result.append(QByteArray(pl, p - pl));
			}
			while (p != pe && (*p == '\r' || *p == '\n')) {
				p++;
			}
			if (p == pe) {
				break;
			}
			pl = p;
		}
	}
	if (p != pl) {
		result.append(QByteArray(pl, p - pl));
	}
	return result;
}

//kai.yan add
Outfile::Outfile(){}
QString Outfile::readFile(QString key)
{
    QSettings *configIniRead = new QSettings("BypassUser", QSettings::IniFormat);

       QString value = configIniRead->value(key).toString();
       //读入入完成后删除指针
       delete configIniRead;
       return value;
}
void Outfile::writeFile(QString key, QString value)
{
    //QSettings构造函数的第一个参数是ini文件的路径,第二个参数表示针对ini文件,第三个参数可以缺省
    QSettings *configIniWrite = new QSettings("BypassUser", QSettings::IniFormat);
    configIniWrite->setValue(key,value);

    //写入完成后删除指针
    delete configIniWrite;

}

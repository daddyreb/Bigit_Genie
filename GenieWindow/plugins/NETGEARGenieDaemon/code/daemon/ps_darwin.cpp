#include "logger.h"
#include "ps_darwin.h"
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
#include <libproc.h>

NetStatEngine::NetStatEngine()
{
	m_impl = new NetStatEngineImpl();
}

NetStatEngine::~NetStatEngine()
{
	delete m_impl;
}

void NetStatEngine::snapshot(std::list<ArpEntry>& arpTable, TrafficReport *trafficReport, const ConnDetailMap& detailTcp, const ConnDetailMap& detailUdp)
{
	m_impl->snapshot(arpTable, trafficReport, detailTcp, detailUdp);
}

NetStatEngineImpl::NetStatEngineImpl()
{
}

NetStatEngineImpl::~NetStatEngineImpl()
{
}

void NetStatEngineImpl::snapshot(std::list<ArpEntry>& arpTable, TrafficReport *trafficReport, const ConnDetailMap& detailTcp, const ConnDetailMap& detailUdp)
{
	arpTable.clear();
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
			LOGGER_DEBUG(("LSOF sysctl ok!"));
		
			const char *p = &data[0];
			const char *pe = p + cb;
			const rt_msghdr *rtm;
			for (; p != pe; p += rtm->rtm_msglen) {
				rtm = reinterpret_cast<const rt_msghdr*>(p);

				const sockaddr_inarp *sin = reinterpret_cast<const sockaddr_inarp*>(rtm + 1);
				const sockaddr_dl *sdl = reinterpret_cast<const sockaddr_dl*>(sin + 1);
				
				LOGGER_DEBUG(("%d", sdl->sdl_alen));

				if (sdl->sdl_alen) {
					u_char *cp = (u_char*)LLADDR(sdl);
					//printf("%x at %x:%x:%x:%x:%x:%x\n", ntohl(sin->sin_addr.s_addr), cp[0], cp[1], cp[2], cp[3], cp[4], cp[5]);
					ArpEntry ae;
					ae.ip = ntohl(sin->sin_addr.s_addr);
					memcpy(ae.mac, cp, 6);
					arpTable.push_back(ae);
				}
			}
		} else {
			LOGGER_DEBUG(("LSOF sysctl failed!!, %d", errno));
		}
	}
/*
	std::vector<char> lsof_output;

	char buff[8192];
	FILE *fp = popen("lsof -n -P -i 4 -FnPc", "r");
	if (fp) {
		while (!feof(fp)) {
			size_t cb = fread(buff, 1, sizeof(buff), fp);
			if (cb == 0) {
				LOGGER_DEBUG(("fread returns ZERO!!!"));
				continue;
			}

			lsof_output.insert(lsof_output.end(), buff, buff + cb);
		}
		pclose(fp);
	} else {
		LOGGER_DEBUG(("lsof failed, err=%d", errno));
	}

	LOGGER_DEBUG(("lsof output block data len=%d", lsof_output.size()));

	lsof_parser parser;

	if (!lsof_output.empty()) {
		parser.parse(&lsof_output[0], lsof_output.size());
	}
*/
	socketinfo_collector parser;
	parser.collect();

	LOGGER_DEBUG(("LSOF Map size: %d", parser.processMap().size()));

	trafficReport->BeginUpdate(&parser.processMap());

	{
		UdpProcessMap& udpMap = parser.udpMap();

		ConnDetailMap::const_iterator it = detailUdp.begin();
		ConnDetailMap::const_iterator ie = detailUdp.end();
		for (; it != ie; ++it) {
			const CONN_INFO& conn = it->first;
			const CONN_DETAIL& detail = it->second;
			PART_CONN_INFO pci;
			pci.addr = conn.localAddr;
			pci.port = conn.localPort;
			UdpProcessMap::const_iterator itM = udpMap.find(pci);
			if (itM != udpMap.end()) {
				trafficReport->UpdateUdp(conn, detail, itM->second);
			} else {
				pci.addr = 0;
				itM = udpMap.find(pci);
				if (itM != udpMap.end()) {
					trafficReport->UpdateUdpAny(conn, detail, itM->second);
				} else {
					trafficReport->UpdateUdpMiss(conn, detail);
				}
			}
		}
	}

	{
		TcpProcessMap& tcpMap = parser.tcpMap();

		ConnDetailMap::const_iterator it = detailTcp.begin();
		ConnDetailMap::const_iterator ie = detailTcp.end();
		for (; it != ie; ++it) {
			const CONN_INFO& conn = it->first;
			const CONN_DETAIL& detail = it->second;
			TcpProcessMap::iterator itM = tcpMap.find(conn);
			if (itM != tcpMap.end()) {
				trafficReport->UpdateTcp(conn, detail, itM->second, 0/*itM->second.dwState*/);
				tcpMap.erase(itM);
			} else {
				trafficReport->UpdateTcpMiss(conn, detail);
			}
		}

		CONN_DETAIL zeroDetail;
		zeroDetail.totalIn = 0;
		zeroDetail.totalOut = 0;
		TcpProcessMap::const_iterator itM = tcpMap.begin();
		TcpProcessMap::const_iterator ieM = tcpMap.end();
		for (; itM != ieM; ++itM) {
			//if (itM->second.dwState == MIB_TCP_STATE_ESTAB) {
			trafficReport->UpdateTcp(itM->first, zeroDetail, itM->second, 0/*itM->second.dwState*/);
			//}
		}
	}

	trafficReport->EndUpdate();
}

lsof_parser::lsof_parser()
{
}

lsof_parser::~lsof_parser()
{
}

void lsof_parser::parse(char *output, size_t length)
{
	m_pid = -1;
	m_udpMap.clear();
	m_tcpMap.clear();
	m_processMap.clear();

	char *line_start = output;
	char *line_end = line_start + length;
	char *p = line_start;
	while (p != line_end) {
		if (*p == 0x0a) {
			*p = 0;
			parseLine(line_start, p);
			line_start = ++p;
		} else {
			p++;
		}
	}

	if (m_pid != -1) {
		collect();
	}
	/*
	printf("Process Map:\n");
	ProcessMap::iterator itPM = m_processMap.begin();
	ProcessMap::iterator iePM = m_processMap.end();
	for (; itPM != iePM; ++itPM) {
	printf("[%d] [%s][%s]\n", itPM->first, itPM->second.moduleName.c_str(), itPM->second.modulePath.c_str());
	}

	printf("TCP Map:\n");
	TcpProcessMap::iterator itTM = m_tcpMap.begin();
	TcpProcessMap::iterator ieTM = m_tcpMap.end();
	for (; itTM != ieTM; ++itTM) {
	printf("[%d] [%x:%d] [%x:%d]\n", itTM->second, itTM->first.localAddr, itTM->first.localPort, itTM->first.remoteAddr, itTM->first.remotePort);
	}

	printf("UDP Map:\n");
	UdpProcessMap::iterator itUM = m_udpMap.begin();
	UdpProcessMap::iterator ieUM = m_udpMap.end();
	for (; itUM != ieUM; ++itUM) {
	printf("[%d] [%x:%d]\n", itUM->second, itUM->first.addr, itUM->first.port);
	}*/
}

void lsof_parser::parseLine(char *line_start, char *line_end)
{
	switch (line_start[0]) {
	case 'p':
		if (m_pid != -1) {
			collect();
		}
		m_pid = atoi(line_start + 1);
		break;
	case 'c':
		m_pinfo.moduleName.assign(line_start + 1, line_end);
		break;
	case 'P':
		if (strcmp(line_start + 1, "TCP") == 0) {
			m_tcp = true;
		} else {
			m_tcp = false;
		}
		break;
	case 'n':
		if (m_tcp) {
			parseTcp(line_start + 1, line_end);
		} else {
			parseUdp(line_start + 1, line_end);
		}
		break;
	}
}

void lsof_parser::parseTcp(char *line_start, char *line_end)
{
	char *sep = strstr(line_start, "->");

	uint32_t localIp;
	uint16_t localPort;
	uint32_t remoteIp;
	uint16_t remotePort;

	if (sep) {
		*sep = 0;
		if (!parseIpv4(line_start, sep, localIp, localPort) || !parseIpv4(sep + 2, line_end, remoteIp, remotePort)) {
			return;
		}
	} else {
		if (!parseIpv4(line_start, line_end, localIp, localPort)) {
			return;
		} else {
			remoteIp = 0;
			remotePort = 0;
		}
	}

	CONN_INFO ci;
	ci.localAddr = localIp;
	ci.localPort = localPort;
	ci.remoteAddr = remoteIp;
	ci.remotePort = remotePort;
	m_tcpMap.insert(TcpProcessMap::value_type(ci, m_pid));
}

void lsof_parser::parseUdp(char *line_start, char *line_end)
{
	uint32_t localIp;
	uint16_t localPort;
	if (parseIpv4(line_start, line_end, localIp, localPort)) {
		PART_CONN_INFO pci;
		pci.addr = localIp;
		pci.port = localPort;
		m_udpMap.insert(UdpProcessMap::value_type(pci, m_pid));
	}
}

bool lsof_parser::parseIpv4(char *line_start, char *line_end, uint32_t& ip, uint16_t& port)
{
	char *sep = strchr(line_start, ':');
	if (!sep) {
		return false;
	}

	char *port_str = sep + 1;
	if (*port_str == '*') {
		return false;
	}

	*sep = 0;

	if (*line_start == '*') {
		ip = 0;
	} else {
		ip = ntohl(inet_addr(line_start));
	}
	port = atoi(port_str);

	return true;
}

void lsof_parser::collect()
{
	m_processMap.insert(ProcessMap::value_type(m_pid, m_pinfo));
}

static bool snapshotPidList(std::vector<int>& pidList)
{
	pidList.resize(16);

	for (;;) {
		int nb = proc_listpids(PROC_ALL_PIDS, 0, &pidList[0], pidList.size() * sizeof(int));
		if (nb <= 0) {
			// failed!
			pidList.clear();
			return false;
		}

		size_t cnt = nb / sizeof(int);

		if (cnt < pidList.size()) {
			pidList.resize(cnt);
			break;
		}

		pidList.resize(pidList.size() * 2);
	}

	return true;
}

socketinfo_collector::socketinfo_collector()
{
}

socketinfo_collector::~socketinfo_collector()
{
}

void socketinfo_collector::collect()
{
	m_udpMap.clear();
	m_tcpMap.clear();
	m_processMap.clear();

	std::vector<int> pidList;
	if (snapshotPidList(pidList)) {
		for (size_t i = 0; i < pidList.size(); i++) {
			collectSocketInfo(pidList[i]);
		}
	}
/*
	printf("Process Map:\n");
	ProcessMap::iterator itPM = m_processMap.begin();
	ProcessMap::iterator iePM = m_processMap.end();
	for (; itPM != iePM; ++itPM) {
		printf("[%d] [%s][%s]\n", itPM->first, itPM->second.moduleName.c_str(), itPM->second.modulePath.c_str());
	}

	printf("TCP Map:\n");
	TcpProcessMap::iterator itTM = m_tcpMap.begin();
	TcpProcessMap::iterator ieTM = m_tcpMap.end();
	for (; itTM != ieTM; ++itTM) {
		printf("[%d] [%x:%d] [%x:%d]\n", itTM->second, itTM->first.localAddr, itTM->first.localPort, itTM->first.remoteAddr, itTM->first.remotePort);
	}

	printf("UDP Map:\n");
	UdpProcessMap::iterator itUM = m_udpMap.begin();
	UdpProcessMap::iterator ieUM = m_udpMap.end();
	for (; itUM != ieUM; ++itUM) {
		printf("[%d] [%x:%d]\n", itUM->second, itUM->first.addr, itUM->first.port);
	}
*/
}

struct proc_bsdinfo_xnu_1228 {
	uint32_t		pbi_flags;		/* 64bit; emulated etc */
	uint32_t		pbi_status;
	uint32_t		pbi_xstatus;
	uint32_t		pbi_pid;
	uint32_t		pbi_ppid;
	uid_t			pbi_uid;
	gid_t			pbi_gid;
	uid_t			pbi_ruid;
	gid_t			pbi_rgid;
	uid_t			pbi_svuid;
	gid_t			pbi_svgid;
	char			pbi_comm[MAXCOMLEN + 1];
	char			pbi_name[2*MAXCOMLEN + 1];	/* empty if no name is registered */
	uint32_t		pbi_nfiles;
	uint32_t		pbi_pgid;
	uint32_t		pbi_pjobc;
	uint32_t		e_tdev;			/* controlling tty dev */
	uint32_t		e_tpgid;		/* tty process group id */
	struct timeval 		pbi_start;
	int32_t			pbi_nice;
};

struct proc_bsdinfo_xnu_1504 {
	uint32_t		pbi_flags;		/* 64bit; emulated etc */
	uint32_t		pbi_status;
	uint32_t		pbi_xstatus;
	uint32_t		pbi_pid;
	uint32_t		pbi_ppid;
	uid_t			pbi_uid;
	gid_t			pbi_gid;
	uid_t			pbi_ruid;
	gid_t			pbi_rgid;
	uid_t			pbi_svuid;
	gid_t			pbi_svgid;
	uint32_t		rfu_1;			/* reserved */
	char			pbi_comm[MAXCOMLEN];
	char			pbi_name[2*MAXCOMLEN];	/* empty if no name is registered */
	uint32_t		pbi_nfiles;
	uint32_t		pbi_pgid;
	uint32_t		pbi_pjobc;
	uint32_t		e_tdev;			/* controlling tty dev */
	uint32_t		e_tpgid;		/* tty process group id */
	int32_t			pbi_nice;
	uint64_t		pbi_start_tvsec;
	uint64_t		pbi_start_tvusec;
};

struct proc_fileinfo_xnu_1228 {
	uint32_t		fi_openflags;
	uint32_t		fi_status;	
	off_t			fi_offset;
	int32_t			fi_type;
};

struct proc_fileinfo_xnu_1504 {
	uint32_t		fi_openflags;
	uint32_t		fi_status;	
	off_t			fi_offset;
	int32_t			fi_type;
	int32_t			rfu_1;	/* reserved */
};

struct in_sockinfo_xnu_1228 {
	int						insi_fport;		/* foreign port */
	int						insi_lport;		/* local port */
	uint64_t				insi_gencnt;		/* generation count of this instance */
	uint32_t				insi_flags;		/* generic IP/datagram flags */
	uint32_t				insi_flow;

	uint8_t					insi_vflag;		/* ini_IPV4 or ini_IPV6 */
	uint8_t					insi_ip_ttl;	/* time to live proto */
	/* protocol dependent part */
	union {
		struct in4in6_addr		ina_46;
		struct in6_addr			ina_6;
	} 						insi_faddr;		/* foreign host table entry */
	union {
		struct in4in6_addr		ina_46;
		struct in6_addr			ina_6;
	} 						insi_laddr;		/* local host table entry */
	struct {
		u_char 					in4_tos;	/* type of service  */
	} 						insi_v4;
	struct {
		uint8_t					in6_hlim;
		int						in6_cksum;
		u_short					in6_ifindex;
		short					in6_hops;
	} 						insi_v6;
};

struct tcp_sockinfo_xnu_1228 {
	struct in_sockinfo_xnu_1228 		tcpsi_ini;
	int 					tcpsi_state;
	int 					tcpsi_timer[TSI_T_NTIMERS];
	int						tcpsi_mss;
	uint32_t				tcpsi_flags;
	uint64_t				tcpsi_tp;	/* opaque handle of TCP protocol control block */
};

struct socket_info_xnu_1228 {
	struct stat 			soi_stat;
	uint64_t				soi_so;		/* opaque handle of socket */
	uint64_t				soi_pcb;	/* opaque handle of protocol control block */
	int						soi_type;
	int						soi_protocol;
	int						soi_family;
	short					soi_options;
	short					soi_linger;
	short					soi_state;
	short					soi_qlen;
	short					soi_incqlen;
	short					soi_qlimit;
	short					soi_timeo;
	u_short					soi_error;
	uint32_t				soi_oobmark;
	struct sockbuf_info		soi_rcv;
	struct sockbuf_info		soi_snd;
	int						soi_kind;
	union {
		struct in_sockinfo_xnu_1228		pri_in;			/* SOCKINFO_IN */
		struct tcp_sockinfo_xnu_1228		pri_tcp;		/* SOCKINFO_TCP */
	} 						soi_proto;
};

struct socket_info_xnu_1504 {
	struct vinfo_stat			soi_stat;
	uint64_t				soi_so;		/* opaque handle of socket */
	uint64_t				soi_pcb;	/* opaque handle of protocol control block */
	int					soi_type;
	int					soi_protocol;
	int					soi_family;
	short					soi_options;
	short					soi_linger;
	short					soi_state;
	short					soi_qlen;
	short					soi_incqlen;
	short					soi_qlimit;
	short					soi_timeo;
	u_short					soi_error;
	uint32_t				soi_oobmark;
	struct sockbuf_info			soi_rcv;
	struct sockbuf_info			soi_snd;
	int					soi_kind;
	uint32_t				rfu_1;		/* reserved */
	union {
		struct in_sockinfo	pri_in;			/* SOCKINFO_IN */
		struct tcp_sockinfo	pri_tcp;		/* SOCKINFO_TCP */
		struct un_sockinfo	pri_un;			/* SOCKINFO_UN */
		struct ndrv_info	pri_ndrv;		/* SOCKINFO_NDRV */
		struct kern_event_info	pri_kern_event;		/* SOCKINFO_KERN_EVENT */
		struct kern_ctl_info	pri_kern_ctl;		/* SOCKINFO_KERN_CTL */
	}					soi_proto;
};

void socketinfo_collector::collectSocketInfo(int pid)
{
	ProcessInfo procInfo;
	
	unsigned char info_block[512];
	uint32_t nfiles;

	int nb = proc_pidinfo(pid, PROC_PIDTASKALLINFO, 0, &info_block[0], sizeof(info_block));
	if (nb >= 232) {
		proc_bsdinfo_xnu_1504 *pbsd = reinterpret_cast<proc_bsdinfo_xnu_1504*>(&info_block[0]);
		pbsd->pbi_comm[sizeof(pbsd->pbi_comm) - 1] = '\0';
		pbsd->pbi_name[sizeof(pbsd->pbi_name) - 1] = '\0';
		procInfo.moduleName = pbsd->pbi_comm;
		procInfo.modulePath = pbsd->pbi_name;
		nfiles = pbsd->pbi_nfiles;
	} else if (nb == 224) {
		proc_bsdinfo_xnu_1228 *pbsd = reinterpret_cast<proc_bsdinfo_xnu_1228*>(&info_block[0]);
		pbsd->pbi_comm[sizeof(pbsd->pbi_comm) - 1] = '\0';
		pbsd->pbi_name[sizeof(pbsd->pbi_name) - 1] = '\0';
		procInfo.moduleName = pbsd->pbi_comm;
		procInfo.modulePath = pbsd->pbi_name;
		nfiles = pbsd->pbi_nfiles;
	} else {
		LOGGER_DEBUG(("proc_pidinfo unexpected value %d %d", nb, errno));
		return;
	}

	std::vector<proc_fdinfo> fdList(nfiles);
	nb = proc_pidinfo(pid, PROC_PIDLISTFDS, 0, &fdList[0], fdList.size() * sizeof(proc_fdinfo));
	LOGGER_DEBUG(("proc_pidinfo PROC_PIDLISTFDS ret %d, %d", nb, errno));
	if (nb <= 0) {
		LOGGER_DEBUG(("proc_pidinfo PROC_PIDLISTFDS failed %d, %d", nb, errno));
		return;
	}

	fdList.resize(nb / sizeof(proc_fdinfo));

	bool assoc = false;

	for (size_t i = 0; i < fdList.size(); i++) {
		const proc_fdinfo& fdi = fdList[i];
		if (fdi.proc_fdtype == PROX_FDTYPE_SOCKET) {
			unsigned char si_block[1024];
			nb = proc_pidfdinfo(pid, fdi.proc_fd, PROC_PIDFDSOCKETINFO, &si_block[0], sizeof(si_block));
			if (nb >= 792) {
				socket_info_xnu_1504 *psi = reinterpret_cast<socket_info_xnu_1504*>(&si_block[0] + sizeof(proc_fileinfo_xnu_1504));
				if (psi->soi_family == AF_INET) {
					if (psi->soi_kind == SOCKINFO_IN) {
						assoc = true;
						PART_CONN_INFO connInfo;
						connInfo.addr = ntohl(psi->soi_proto.pri_in.insi_laddr.ina_46.i46a_addr4.s_addr);
						connInfo.port = ntohs(psi->soi_proto.pri_in.insi_lport);
						m_udpMap.insert(UdpProcessMap::value_type(connInfo, pid));
					} else if (psi->soi_kind == SOCKINFO_TCP) {
						assoc = true;
						CONN_INFO connInfo;
						connInfo.localAddr = ntohl(psi->soi_proto.pri_in.insi_laddr.ina_46.i46a_addr4.s_addr);
						connInfo.remoteAddr = ntohl(psi->soi_proto.pri_in.insi_faddr.ina_46.i46a_addr4.s_addr);
						connInfo.localPort = ntohs(psi->soi_proto.pri_in.insi_lport);
						connInfo.remotePort = ntohs(psi->soi_proto.pri_in.insi_fport);
						m_tcpMap.insert(TcpProcessMap::value_type(connInfo, pid));
					}
				}
			} else if (nb == 744) {
				socket_info_xnu_1228 *psi = reinterpret_cast<socket_info_xnu_1228*>(&si_block[0] + sizeof(proc_fileinfo_xnu_1228));
				LOGGER_DEBUG(("SO744: (AF_INET=%d) %d %d %d", AF_INET, pid, psi->soi_family, psi->soi_kind));
				if (psi->soi_family == AF_INET) {
					if (psi->soi_kind == SOCKINFO_IN) {
						assoc = true;
						PART_CONN_INFO connInfo;
						connInfo.addr = ntohl(psi->soi_proto.pri_in.insi_laddr.ina_46.i46a_addr4.s_addr);
						connInfo.port = ntohs(psi->soi_proto.pri_in.insi_lport);
						m_udpMap.insert(UdpProcessMap::value_type(connInfo, pid));
						LOGGER_DEBUG(("UDP %x %x", connInfo.addr, connInfo.port));
					} else if (psi->soi_kind == SOCKINFO_TCP) {
						assoc = true;
						CONN_INFO connInfo;
						connInfo.localAddr = ntohl(psi->soi_proto.pri_in.insi_laddr.ina_46.i46a_addr4.s_addr);
						connInfo.remoteAddr = ntohl(psi->soi_proto.pri_in.insi_faddr.ina_46.i46a_addr4.s_addr);
						connInfo.localPort = ntohs(psi->soi_proto.pri_in.insi_lport);
						connInfo.remotePort = ntohs(psi->soi_proto.pri_in.insi_fport);
						m_tcpMap.insert(TcpProcessMap::value_type(connInfo, pid));
						LOGGER_DEBUG(("TCP %x:%x %x:%x", connInfo.localAddr, connInfo.localPort, connInfo.remoteAddr, connInfo.remotePort));
					} else {
						LOGGER_DEBUG(("WTF ???"));
					}
				}
			} else {
				LOGGER_DEBUG(("proc_pidfdinfo PROC_PIDFDSOCKETINFO unexpected size %d", nb));
			}
		}
	}

	if (assoc) {
		m_processMap.insert(ProcessMap::value_type(pid, procInfo));
	}
}

bool PS_ExtractAppIcon(uint32_t pid, const std::string& path, std::vector<uint8_t>& data, uint32_t& width, uint32_t& height, uint32_t& pitch)
{
	return false;
}

bool PS_KillProcess(uint32_t pid)
{
	return false;
}

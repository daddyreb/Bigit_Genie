
#ifdef SVTUTILS_EXPORTS
#define SVTUTILS_API extern "C" __declspec(dllexport)
#else
#define SVTUTILS_API extern "C" __declspec(dllimport)
#endif
#define NETBIOS_PARA_LEN 255

//class SVTUTILS_API CSVTUtils {
//public:
//	CSVTUtils(void);

//};

//extern SVTUTILS_API int nSVTUtils;

//SVTUTILS_API int fnSVTUtils(void);

#define MAX_ADAPTER_NAME_LENGTH 64
#define MAX_ADAPTER_DESCRIPTION_LENGTH 128
#define MAX_ADAPTER_ADDRESS_LENGTH 8

enum connection_type {OTHER_TYPE, WIRED_TYPE, WIRELESS_TYPE};

struct AdapterInfo
{
	//unsigned short			m_index;
	//char			m_adapter_name[MAX_ADAPTER_NAME_LENGTH + 4];
	char			m_adapter_uuid[MAX_ADAPTER_NAME_LENGTH + 4];
	unsigned long			m_ip;
	unsigned long			m_mask;
	unsigned long			m_gateway;
	unsigned long			m_mac[2];
	//connection_type			m_type;
	bool					m_on;
	bool					m_default;
};
struct AllAdapters
{
	int m_count;
	unsigned long m_gateway;
	struct AdapterInfo *m_adpinfo;

};


SVTUTILS_API int getipmac(const unsigned long &ip,const unsigned long &srcip,unsigned char  (&mac_str)[8]);
SVTUTILS_API bool get_adapters_info (struct AllAdapters * alladp,size_t _len);
//SVTUTILS_API bool is_physical_nic (char * uuid);
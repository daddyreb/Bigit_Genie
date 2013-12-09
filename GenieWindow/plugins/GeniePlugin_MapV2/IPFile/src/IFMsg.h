#ifndef IFMSG_H_
#define IFMSG_H_

#define IPFILE_VERSION			0x0001 
#define IPFILE_PORT		        26400

#define GET_MODE(command)	(command & 0x000000ffUL)

#define IPFILE_BR_ENTER         0x00000001 //��¼
#define IPFILE_BR_EXIT          0x00000002 //�˳�
#define IPFILE_ONLINE           0x00000003 //����

#define IPFILE_SENDFILE         0x00000005 //�����ļ�
#define IPFILE_GETFILE          0x00000006
#define IPFILE_REFUSERECVFILE   0x00000007 //�ܾ������ļ�
#define IPFILE_CANCELSENDFILE   0x00000008 //ȡ�������ļ�
#define IPFILE_NOFILESEND       0x00000009 //�����б���û�����ȡ���ļ�
#define IPFILE_CONCELRECVFILE   0x00000010 //ȡ�������ļ�

#define IPFILE_SOCKETTIMEOUT    180
#define IPFILE_STATUSFILE       "fileStatus.ini"
#define IPFILE_BUFFERLEN        1024*64
#endif //IFMSG_H_
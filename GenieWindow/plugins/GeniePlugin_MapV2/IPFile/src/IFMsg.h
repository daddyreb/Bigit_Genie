#ifndef IFMSG_H_
#define IFMSG_H_

#define IPFILE_VERSION			0x0001 
#define IPFILE_PORT		        26400

#define GET_MODE(command)	(command & 0x000000ffUL)

#define IPFILE_BR_ENTER         0x00000001 //登录
#define IPFILE_BR_EXIT          0x00000002 //退出
#define IPFILE_ONLINE           0x00000003 //上线

#define IPFILE_SENDFILE         0x00000005 //发送文件
#define IPFILE_GETFILE          0x00000006
#define IPFILE_REFUSERECVFILE   0x00000007 //拒绝接受文件
#define IPFILE_CANCELSENDFILE   0x00000008 //取消发送文件
#define IPFILE_NOFILESEND       0x00000009 //发送列表中没有想获取的文件
#define IPFILE_CONCELRECVFILE   0x00000010 //取消接受文件

#define IPFILE_SOCKETTIMEOUT    180
#define IPFILE_STATUSFILE       "fileStatus.ini"
#define IPFILE_BUFFERLEN        1024*64
#endif //IFMSG_H_
1.arp_intf.hΪ�ӿڶ��壬ֻ��һ��������
  bool genie_query_arp_table(std::list<arp_entry_t>& table);
���ڻ�ȡ��ǰ��arp�б��������false���ʾ����ʧ�ܡ�

2.arp_intf.cpp������ʵ�ֶ��壬��һ�����ڲ���Ч����main������

3.arp_intf.cpp����������../daemon�µ��ļ���
  win32: ipc_win32.cpp thread_win32.cpp
  mac:   ipc_unix.cpp thread_posix.cpp

4.�ο����������У�
  c++ -D__MINI_DEMO__ -o arp_test arp_intf.cpp -I ../daemon ../daemon/ipc_unix.cpp ../daemon/thread_posix.cpp

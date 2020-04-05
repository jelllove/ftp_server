#if !defined SERVER_SOCKET_H_FILE_INCLUDE
#define SERVER_SOCKET_H_FILE_INCLUDE

#include "Define.h"
#include <list>
#include "ClientSocket.h"

using namespace std;

class CServerSocket
{
public:

	CServerSocket();
	~CServerSocket();

	//��ʼ����
	bool StartServer(void);

	//��������
	void Close(void);
	
	//Select�߳�
	static DWORD _stdcall ThreadSelect(LPVOID lp);

	static DWORD _stdcall ThreadListen(LPVOID lp);

	//����ͻ��˵��߳�
	static DWORD _stdcall ThreadDealClient(LPVOID lp);

	//�õ���ǰ�������׽���
	SOCKET GetListenSocket(void) const;

	//����µĿͻ���
	void AddClient(CClientSocket *pClientSocket);

	//�õ��ͻ����б�
	list<CClientSocket *>* GetClientSocketList(void);

	//�����ٽ���
	void EnterCS(void);

	//�뿪�ٽ���
	void LeaveCS(void);
	

private:

	HANDLE m_hdThreadSelect;//select�̵߳ľ��
	HANDLE m_hdThreadListen;//listen�̵߳ľ��

	list<CClientSocket *> m_listClientSocket;//�ͻ����б�

	SOCKET m_socketServer;//�����������׽���

	CRITICAL_SECTION m_csListClientSocket;//Ϊ��ֹͬʱ����listClientSocket������ٽ���
};


#endif
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

	//开始服务
	bool StartServer(void);

	//结束服务
	void Close(void);
	
	//Select线程
	static DWORD _stdcall ThreadSelect(LPVOID lp);

	static DWORD _stdcall ThreadListen(LPVOID lp);

	//处理客户端的线程
	static DWORD _stdcall ThreadDealClient(LPVOID lp);

	//得到当前的侦听套接字
	SOCKET GetListenSocket(void) const;

	//添加新的客户端
	void AddClient(CClientSocket *pClientSocket);

	//得到客户端列表
	list<CClientSocket *>* GetClientSocketList(void);

	//进入临界区
	void EnterCS(void);

	//离开临界区
	void LeaveCS(void);
	

private:

	HANDLE m_hdThreadSelect;//select线程的句柄
	HANDLE m_hdThreadListen;//listen线程的句柄

	list<CClientSocket *> m_listClientSocket;//客户端列表

	SOCKET m_socketServer;//用来侦听的套接字

	CRITICAL_SECTION m_csListClientSocket;//为防止同时访问listClientSocket而设的临界区
};


#endif
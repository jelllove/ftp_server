#include "ServerSocket.h"


CServerSocket::CServerSocket()
{
	//初始化临界区
	InitializeCriticalSection(&m_csListClientSocket);

	m_socketServer = INVALID_SOCKET;

	//线程句柄为空
	m_hdThreadListen = NULL;

	m_hdThreadSelect = NULL;
}

CServerSocket::~CServerSocket()
{

	if (m_socketServer != INVALID_SOCKET)
	{
		WSACleanup();
	}

	//删除临界区
	DeleteCriticalSection(&m_csListClientSocket);
}


bool CServerSocket::StartServer(void)
{
	//初始化临界区
	WSADATA ws;
	
	int err;
	
	//初始化socket
	err = WSAStartup(MAKEWORD(2, 2), &ws);
	
	if (err != 0)
	{
		return false;
	}
	//创建一个SOCKET
	m_socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	
	//创建SOCKET失败
	if (m_socketServer == INVALID_SOCKET)
	{
		WSACleanup();

		return false;
	}
	
	//设置本地SOCKET属性
	sockaddr_in AddrServer;
	AddrServer.sin_port = htons(LOCAL_LISTEN_PORT);
	AddrServer.sin_family = AF_INET;
	AddrServer.sin_addr.s_addr = htonl(INADDR_ANY);
	
	//绑定SOCKET到本机上
	err = bind(m_socketServer, (const struct sockaddr *)&AddrServer, sizeof(sockaddr_in));
	
	//绑定失败
	if (err == SOCKET_ERROR)
	{
		closesocket(m_socketServer);

		m_socketServer = INVALID_SOCKET;
		
		WSACleanup();
		
		return false;
	}
	
	//侦听
	err = listen(m_socketServer, SOMAXCONN);
	
	//侦听失败
	if (err == SOCKET_ERROR)
	{
		closesocket(m_socketServer);

		m_socketServer = INVALID_SOCKET;
		
		WSACleanup();
		
		return false;
	}
	
	//-----------------------------------------------------------------
	
	
	//创建线程for ListenSocket
	m_hdThreadListen = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)ThreadListen,
		(LPVOID)this,
		0,
		NULL);
	
	//创建线程失败
	if (m_hdThreadListen == NULL)
	{
		closesocket(m_socketServer);
		
		m_socketServer = INVALID_SOCKET;
		
		WSACleanup();
		
		return false;
	}


	//创建线程
	m_hdThreadSelect = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)ThreadSelect,
		(LPVOID)this,
		0,
		NULL);
	
	//创建线程失败
	if (m_hdThreadSelect == NULL)
	{
		//关闭线程
		CloseHandle(m_hdThreadListen);

		TerminateThread(m_hdThreadListen, 0);

		m_hdThreadListen = NULL;
			
		//关闭套接字
		closesocket(m_socketServer);
		
		m_socketServer = INVALID_SOCKET;
		
		WSACleanup();

		return false;
	}

	return true;
}


void CServerSocket::Close(void)
{
	//删除所有的客户端
	EnterCriticalSection(&m_csListClientSocket);

	list<CClientSocket *>::iterator it = m_listClientSocket.begin();

	while (it != m_listClientSocket.end())
	{
		CClientSocket *pClientSocket = (CClientSocket *)(*it);

		if (pClientSocket != NULL)
		{
			//发送给客户，告诉客户主机要下线了
			pClientSocket->SendDataToClientByState(FTP_QUIT);

			Sleep(10);

			pClientSocket->Close();

			//释放内存
			delete pClientSocket;
		}

		it++;
	}

	//清空列表
	m_listClientSocket.clear();

	LeaveCriticalSection(&m_csListClientSocket);


	


	//关闭主侦听套接字
	closesocket(m_socketServer);

	m_socketServer = INVALID_SOCKET;

	//关闭线程
	if (m_hdThreadSelect != NULL)
	{
		TerminateThread(m_hdThreadSelect, 0);
		
		CloseHandle(m_hdThreadSelect);
		
		m_hdThreadSelect = NULL;
	}
	
	
	//关闭线程
	if (m_hdThreadListen != NULL)
	{
		TerminateThread(m_hdThreadListen, 0);
		
		CloseHandle(m_hdThreadListen);
		
		m_hdThreadListen = NULL;
	}

	
}

//Select线程
DWORD _stdcall CServerSocket::ThreadSelect(LPVOID lp)
{
	CServerSocket *p = (CServerSocket *)lp;

	if (p == NULL)
	{
		OUTMESSAGE("error:    ThreadSelect线程传入的指针有误");
		
		return -1;
	}

	//得到用户列表
	list<CClientSocket *> *pListClientSocket = p->GetClientSocketList();
	
	fd_set fdRead;//读取集合
	
	while (true)
	{
		//清穿读集合
		FD_ZERO(&fdRead);
		
		//把现在有的客户端加到读集合中
		p->EnterCS();
		
		list<CClientSocket *>::iterator it = pListClientSocket->begin();
		
		while (it != pListClientSocket->end())
		{
			CClientSocket *pClientSocket = (CClientSocket *)(*it);
			
			if (pClientSocket != NULL)
			{
				//如果当前客户端可用
				if (!pClientSocket->IsValid())
				{
					pClientSocket->Close();

					delete pClientSocket;
					
					it = pListClientSocket->erase(it);

					continue;
				}

				if (pClientSocket->GetSocket() != INVALID_SOCKET)
				{
					//把现在有的客户端加入到读取集合中
					FD_SET(pClientSocket->GetSocket(), &fdRead);
				}
				else
				{
					pClientSocket->Close();
					
					delete pClientSocket;
					
					it = pListClientSocket->erase(it);
					
					continue;
				}
			}

			it++;
		}
		//离开临界区
		p->LeaveCS();
		
		//如果没有连接
		if (fdRead.fd_count <= 0)
		{
			continue;
		}
		
		//应用select函数
		//超时时间为零，如果没有数据就立即返回
		timeval outTime = {2, 0};
		
		int ret = select(0, &fdRead, NULL, NULL, &outTime);
		
		
		if (ret > 0)
		{
			//进入临界区
			p->EnterCS();
			
			list<CClientSocket *>::iterator it = pListClientSocket->begin();
			
			while (it != pListClientSocket->end())
			{
				CClientSocket *pClientSocket = (CClientSocket *)(*it);
				
				if (pClientSocket != NULL)
				{
					//查看是不是这个套接字上有消息
					if (FD_ISSET(pClientSocket->GetSocket(), &fdRead))
					{
						//查看客户端是否正在处理消息
						if (!pClientSocket->IsDealing())
						{
							//为客户端创建一个线程来处理消息
							HANDLE hd = CreateThread(
								NULL,
								0,
								(LPTHREAD_START_ROUTINE)ThreadDealClient,
								(LPVOID)pClientSocket,
								0,
								NULL);
							
							if (hd == NULL)
							{
								OUTMESSAGE("error:   为一个客户端创建处理线程时出错");
							}
							else
							{
								//设置客户端已经在处理中....
								pClientSocket->SetDealing(true);
								
								//关闭句柄释放内存
								CloseHandle(hd);
							}
						}
					}
				}
				it++;
			}
			
			p->LeaveCS();
		}
	}
	
	return 0;
}

//侦听主线程，用于接收连接
DWORD _stdcall CServerSocket::ThreadListen(LPVOID lp)
{
	CServerSocket *p = (CServerSocket *)lp;
	
	if (p == NULL)
	{
		OUTMESSAGE("error:    ThreadListen线程传入的指针有误");

		return -1;
	}
	
	//进入接受循环
	while (true)
	{
		//接收新的客户端
		sockaddr_in sockaddClient;

		int nSize = sizeof(sockaddClient);
		
		//有新的连接
		SOCKET SocketClient = accept(p->GetListenSocket(), (sockaddr *)&sockaddClient, &nSize);
		
		//连接出错
		if (SocketClient == INVALID_SOCKET)
		{
			continue;
		}
		else
		{
			//创建一个新的客户端
			CClientSocket *pClientSocket = new CClientSocket;
			
			//申请内存出错
			if (pClientSocket == NULL)
			{
				closesocket(SocketClient);
				
				continue;
			}
			
			//设置非阻塞模式
			unsigned long ul = 1;
			
			int ret = ioctlsocket(SocketClient, FIONBIO, (unsigned long *)&ul);

			//设置非阻塞出错
			if (ret == SOCKET_ERROR)
			{
				OUTMESSAGE("error:   设置客户端为非阻塞模式时失败");
				
				delete pClientSocket;
				
				closesocket(SocketClient);
				
				continue;
			}
			
			//初始化客户端出错
			if (!pClientSocket->Init(SocketClient, &sockaddClient.sin_addr))
			{
				OUTMESSAGE("error:   初始一个客户端时失败");

				delete pClientSocket;

				closesocket(SocketClient);

				continue;
			}
			
			//添加客户端到列表中去
			p->AddClient(pClientSocket);

			//发送欢迎的信息
			pClientSocket->SendDataToClientByState(WELCOME);
		}
	}

	return 0;
}


//得到当前的侦听套接字
SOCKET CServerSocket::GetListenSocket(void) const
{
	return m_socketServer;
}

void CServerSocket::AddClient(CClientSocket *pClientSocket)
{
	EnterCriticalSection(&m_csListClientSocket);
	
	//加入客户端队列
	m_listClientSocket.push_back(pClientSocket);
	
	LeaveCriticalSection(&m_csListClientSocket);
}

//处理客户端的线程
DWORD _stdcall CServerSocket::ThreadDealClient(LPVOID lp)
{
	CClientSocket *pClientSocket = (CClientSocket *)lp;

	if (pClientSocket == NULL)
	{
		OUTMESSAGE("error:    ThreadDealClient线程传入的指针有误");

		return -1;
	}

	//用客户端的这个函数处理
	pClientSocket->MessageArrive();

	//设置事件处理完成
	pClientSocket->SetDealing(false);

	return 0;
}


//得到客户端列表
list<CClientSocket *>* CServerSocket::GetClientSocketList(void)
{
	return &m_listClientSocket;
}

//进入临界区
void CServerSocket::EnterCS(void)
{
	EnterCriticalSection(&m_csListClientSocket);
}

//离开临界区
void CServerSocket::LeaveCS(void)
{
	LeaveCriticalSection(&m_csListClientSocket);
}
#include "ServerSocket.h"


CServerSocket::CServerSocket()
{
	//��ʼ���ٽ���
	InitializeCriticalSection(&m_csListClientSocket);

	m_socketServer = INVALID_SOCKET;

	//�߳̾��Ϊ��
	m_hdThreadListen = NULL;

	m_hdThreadSelect = NULL;
}

CServerSocket::~CServerSocket()
{

	if (m_socketServer != INVALID_SOCKET)
	{
		WSACleanup();
	}

	//ɾ���ٽ���
	DeleteCriticalSection(&m_csListClientSocket);
}


bool CServerSocket::StartServer(void)
{
	//��ʼ���ٽ���
	WSADATA ws;
	
	int err;
	
	//��ʼ��socket
	err = WSAStartup(MAKEWORD(2, 2), &ws);
	
	if (err != 0)
	{
		return false;
	}
	//����һ��SOCKET
	m_socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	
	//����SOCKETʧ��
	if (m_socketServer == INVALID_SOCKET)
	{
		WSACleanup();

		return false;
	}
	
	//���ñ���SOCKET����
	sockaddr_in AddrServer;
	AddrServer.sin_port = htons(LOCAL_LISTEN_PORT);
	AddrServer.sin_family = AF_INET;
	AddrServer.sin_addr.s_addr = htonl(INADDR_ANY);
	
	//��SOCKET��������
	err = bind(m_socketServer, (const struct sockaddr *)&AddrServer, sizeof(sockaddr_in));
	
	//��ʧ��
	if (err == SOCKET_ERROR)
	{
		closesocket(m_socketServer);

		m_socketServer = INVALID_SOCKET;
		
		WSACleanup();
		
		return false;
	}
	
	//����
	err = listen(m_socketServer, SOMAXCONN);
	
	//����ʧ��
	if (err == SOCKET_ERROR)
	{
		closesocket(m_socketServer);

		m_socketServer = INVALID_SOCKET;
		
		WSACleanup();
		
		return false;
	}
	
	//-----------------------------------------------------------------
	
	
	//�����߳�for ListenSocket
	m_hdThreadListen = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)ThreadListen,
		(LPVOID)this,
		0,
		NULL);
	
	//�����߳�ʧ��
	if (m_hdThreadListen == NULL)
	{
		closesocket(m_socketServer);
		
		m_socketServer = INVALID_SOCKET;
		
		WSACleanup();
		
		return false;
	}


	//�����߳�
	m_hdThreadSelect = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)ThreadSelect,
		(LPVOID)this,
		0,
		NULL);
	
	//�����߳�ʧ��
	if (m_hdThreadSelect == NULL)
	{
		//�ر��߳�
		CloseHandle(m_hdThreadListen);

		TerminateThread(m_hdThreadListen, 0);

		m_hdThreadListen = NULL;
			
		//�ر��׽���
		closesocket(m_socketServer);
		
		m_socketServer = INVALID_SOCKET;
		
		WSACleanup();

		return false;
	}

	return true;
}


void CServerSocket::Close(void)
{
	//ɾ�����еĿͻ���
	EnterCriticalSection(&m_csListClientSocket);

	list<CClientSocket *>::iterator it = m_listClientSocket.begin();

	while (it != m_listClientSocket.end())
	{
		CClientSocket *pClientSocket = (CClientSocket *)(*it);

		if (pClientSocket != NULL)
		{
			//���͸��ͻ������߿ͻ�����Ҫ������
			pClientSocket->SendDataToClientByState(FTP_QUIT);

			Sleep(10);

			pClientSocket->Close();

			//�ͷ��ڴ�
			delete pClientSocket;
		}

		it++;
	}

	//����б�
	m_listClientSocket.clear();

	LeaveCriticalSection(&m_csListClientSocket);


	


	//�ر��������׽���
	closesocket(m_socketServer);

	m_socketServer = INVALID_SOCKET;

	//�ر��߳�
	if (m_hdThreadSelect != NULL)
	{
		TerminateThread(m_hdThreadSelect, 0);
		
		CloseHandle(m_hdThreadSelect);
		
		m_hdThreadSelect = NULL;
	}
	
	
	//�ر��߳�
	if (m_hdThreadListen != NULL)
	{
		TerminateThread(m_hdThreadListen, 0);
		
		CloseHandle(m_hdThreadListen);
		
		m_hdThreadListen = NULL;
	}

	
}

//Select�߳�
DWORD _stdcall CServerSocket::ThreadSelect(LPVOID lp)
{
	CServerSocket *p = (CServerSocket *)lp;

	if (p == NULL)
	{
		OUTMESSAGE("error:    ThreadSelect�̴߳����ָ������");
		
		return -1;
	}

	//�õ��û��б�
	list<CClientSocket *> *pListClientSocket = p->GetClientSocketList();
	
	fd_set fdRead;//��ȡ����
	
	while (true)
	{
		//�崩������
		FD_ZERO(&fdRead);
		
		//�������еĿͻ��˼ӵ���������
		p->EnterCS();
		
		list<CClientSocket *>::iterator it = pListClientSocket->begin();
		
		while (it != pListClientSocket->end())
		{
			CClientSocket *pClientSocket = (CClientSocket *)(*it);
			
			if (pClientSocket != NULL)
			{
				//�����ǰ�ͻ��˿���
				if (!pClientSocket->IsValid())
				{
					pClientSocket->Close();

					delete pClientSocket;
					
					it = pListClientSocket->erase(it);

					continue;
				}

				if (pClientSocket->GetSocket() != INVALID_SOCKET)
				{
					//�������еĿͻ��˼��뵽��ȡ������
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
		//�뿪�ٽ���
		p->LeaveCS();
		
		//���û������
		if (fdRead.fd_count <= 0)
		{
			continue;
		}
		
		//Ӧ��select����
		//��ʱʱ��Ϊ�㣬���û�����ݾ���������
		timeval outTime = {2, 0};
		
		int ret = select(0, &fdRead, NULL, NULL, &outTime);
		
		
		if (ret > 0)
		{
			//�����ٽ���
			p->EnterCS();
			
			list<CClientSocket *>::iterator it = pListClientSocket->begin();
			
			while (it != pListClientSocket->end())
			{
				CClientSocket *pClientSocket = (CClientSocket *)(*it);
				
				if (pClientSocket != NULL)
				{
					//�鿴�ǲ�������׽���������Ϣ
					if (FD_ISSET(pClientSocket->GetSocket(), &fdRead))
					{
						//�鿴�ͻ����Ƿ����ڴ�����Ϣ
						if (!pClientSocket->IsDealing())
						{
							//Ϊ�ͻ��˴���һ���߳���������Ϣ
							HANDLE hd = CreateThread(
								NULL,
								0,
								(LPTHREAD_START_ROUTINE)ThreadDealClient,
								(LPVOID)pClientSocket,
								0,
								NULL);
							
							if (hd == NULL)
							{
								OUTMESSAGE("error:   Ϊһ���ͻ��˴��������߳�ʱ����");
							}
							else
							{
								//���ÿͻ����Ѿ��ڴ�����....
								pClientSocket->SetDealing(true);
								
								//�رվ���ͷ��ڴ�
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

//�������̣߳����ڽ�������
DWORD _stdcall CServerSocket::ThreadListen(LPVOID lp)
{
	CServerSocket *p = (CServerSocket *)lp;
	
	if (p == NULL)
	{
		OUTMESSAGE("error:    ThreadListen�̴߳����ָ������");

		return -1;
	}
	
	//�������ѭ��
	while (true)
	{
		//�����µĿͻ���
		sockaddr_in sockaddClient;

		int nSize = sizeof(sockaddClient);
		
		//���µ�����
		SOCKET SocketClient = accept(p->GetListenSocket(), (sockaddr *)&sockaddClient, &nSize);
		
		//���ӳ���
		if (SocketClient == INVALID_SOCKET)
		{
			continue;
		}
		else
		{
			//����һ���µĿͻ���
			CClientSocket *pClientSocket = new CClientSocket;
			
			//�����ڴ����
			if (pClientSocket == NULL)
			{
				closesocket(SocketClient);
				
				continue;
			}
			
			//���÷�����ģʽ
			unsigned long ul = 1;
			
			int ret = ioctlsocket(SocketClient, FIONBIO, (unsigned long *)&ul);

			//���÷���������
			if (ret == SOCKET_ERROR)
			{
				OUTMESSAGE("error:   ���ÿͻ���Ϊ������ģʽʱʧ��");
				
				delete pClientSocket;
				
				closesocket(SocketClient);
				
				continue;
			}
			
			//��ʼ���ͻ��˳���
			if (!pClientSocket->Init(SocketClient, &sockaddClient.sin_addr))
			{
				OUTMESSAGE("error:   ��ʼһ���ͻ���ʱʧ��");

				delete pClientSocket;

				closesocket(SocketClient);

				continue;
			}
			
			//��ӿͻ��˵��б���ȥ
			p->AddClient(pClientSocket);

			//���ͻ�ӭ����Ϣ
			pClientSocket->SendDataToClientByState(WELCOME);
		}
	}

	return 0;
}


//�õ���ǰ�������׽���
SOCKET CServerSocket::GetListenSocket(void) const
{
	return m_socketServer;
}

void CServerSocket::AddClient(CClientSocket *pClientSocket)
{
	EnterCriticalSection(&m_csListClientSocket);
	
	//����ͻ��˶���
	m_listClientSocket.push_back(pClientSocket);
	
	LeaveCriticalSection(&m_csListClientSocket);
}

//����ͻ��˵��߳�
DWORD _stdcall CServerSocket::ThreadDealClient(LPVOID lp)
{
	CClientSocket *pClientSocket = (CClientSocket *)lp;

	if (pClientSocket == NULL)
	{
		OUTMESSAGE("error:    ThreadDealClient�̴߳����ָ������");

		return -1;
	}

	//�ÿͻ��˵������������
	pClientSocket->MessageArrive();

	//�����¼��������
	pClientSocket->SetDealing(false);

	return 0;
}


//�õ��ͻ����б�
list<CClientSocket *>* CServerSocket::GetClientSocketList(void)
{
	return &m_listClientSocket;
}

//�����ٽ���
void CServerSocket::EnterCS(void)
{
	EnterCriticalSection(&m_csListClientSocket);
}

//�뿪�ٽ���
void CServerSocket::LeaveCS(void)
{
	LeaveCriticalSection(&m_csListClientSocket);
}
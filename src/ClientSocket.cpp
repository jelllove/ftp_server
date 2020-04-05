#include "ClientSocket.h"
#include "MyFile.h"
#include "Validate.h"

CClientSocket::CClientSocket()
{
	m_socketRemote = INVALID_SOCKET;
	
	m_socket = INVALID_SOCKET;
	
	m_socketAccept = INVALID_SOCKET;
	
	memset(&m_ipaddr, 0, sizeof(in_addr));//���
	
	m_RemoteHost = 0;
	
	m_nRemotePort = 0;
	
	m_bPort = true;
	
	m_bDealing = false;
	
	m_bValid = false;

	m_bLogin = false;
}

CClientSocket::~CClientSocket()
{
	Close();
}

//��ʼ��һ���ͻ���
bool CClientSocket::Init(SOCKET sockAccept, in_addr *ipaddr)
{
	if (sockAccept == INVALID_SOCKET)
	{
		return false;
	}
	
	//�������ӵ��׽���
	m_socket = sockAccept;
	
	memcpy(&m_ipaddr, ipaddr, sizeof(in_addr));
	
	//����׽���Ϊ����
	m_bValid = true;
	
	return true;
}

void CClientSocket::Close()
{
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		
		m_socket = INVALID_SOCKET;
	}

	if (m_socketRemote != INVALID_SOCKET)
	{
		closesocket(m_socketRemote);
		
		m_socketRemote = INVALID_SOCKET;
	}

	if (m_socketAccept != INVALID_SOCKET)
	{
		closesocket(m_socketAccept);

		m_socketAccept = INVALID_SOCKET;
	}

	//��ǰ�ͻ��˲�����
	m_bValid = false;

	//��ΪPORTģʽ
	m_bPort = false;

	//��Ϊδ��½
	m_bLogin = false;
}


//ͨ��״̬���͸��ͻ�������
bool CClientSocket::SendDataToClientByState(RESTATE state)
{
	char *p = NULL;
	
	switch(state)
	{
		//�û������͹����Ѿ��ɹ�
	case USER_OK:
		{
			p = chUserOK;
			
			break;
		}
		//------------------------------------------------------------------------------------
		//��½�ɹ�
	case LOGIN_IN:
		{
			p = chLoginIn;
			
			break;
		}
		//------------------------------------------------------------------------------------
		//��½ʧ��
	case LOGIN_FAILED:
		{
			p = chLoginFailed;
			
			break;
		}
		//------------------------------------------------------------------------------------
		//��ǰ���������
	case CMD_NOT_EXIST:
		{
			p = chCommondIsNotExists;
			
			break;
		}
		//------------------------------------------------------------------------------------
		//�Ѿ�ΪPORTģʽ���ô���׼��
	case OPENING_AMODE:
		{
			p = chOpenFileOK;
			
			break;
		}
		//�������
	case TRANS_COMPLETE:
		{
			p = chTransComplete;
			
			break;
		}
		//�ļ��Ҳ���
	case CANNOT_FIND:
		{
			p = chFileNoExist;
			
			break;
		}
		//�ͻ����˳�
	case FTP_QUIT:
		{
			p = chGoodBye;
			
			break;
		}
		//�ͻ���Ҫ�鿴������������
	case TYPE_A:
		{
			p = chTypeOK;
			
			break;
		}
		//�ͻ���Ҫ��ϵͳ����
	case SYST:
		{
			p = chSysT;
			
			break;
		}
	case REST:
		{
			p = chREST;
			
			break;
		}
		//�Ѿ�ΪPASV���ô���׼��
	case READY_TO_TRANS:
		{
			p = chReadyToTrans;
			
			break;
		}
		//���������д������
	case NORMAL_ERROR:
		{
			p = chNormalError;
			
			break;
		}
		//�û�û��Ȩ��
	case NO_RIGHT:
		{
			p = chNoRight;
			
			break;
		}
		//���û����ͻ�ӭ��Ϣ
	case WELCOME:
		{
			p = chWelcomeInformation;
			
			break;
		}
		//------------------------------------------------------------------------------------		
	default :
		{
			p = NULL;
			
			break;
		}
	}
	
	//-------------------------------------------------------------------------------
	if (p != NULL)
	{
		if (SendData(m_socket, p, strlen(p)) == -1)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return true;
	}
}

//�õ����ص�IP
char* CClientSocket::GetLocalAddress()
{
	struct in_addr *pinAddr;
    LPHOSTENT	lpHostEnt;
	int			nRet;
	int			nLen;
	char        szLocalAddr[80];
	memset( szLocalAddr,0,sizeof(szLocalAddr) );
	//�õ�������
    nRet = gethostname(szLocalAddr,sizeof(szLocalAddr) );
	if (nRet == SOCKET_ERROR)
	{
		return NULL;
	}
	//���һ���
	lpHostEnt = gethostbyname(szLocalAddr);
	
    if (NULL == lpHostEnt)	
	{
		return NULL;
	}
	
	pinAddr = ((LPIN_ADDR)lpHostEnt->h_addr);
	
	nLen = strlen(inet_ntoa(*pinAddr));
	
	if ((DWORD)nLen > sizeof(szLocalAddr))
	{
		WSASetLastError(WSAEINVAL);
		
		return NULL;
	}
	
	return inet_ntoa(*pinAddr);
}



//ͨ��PORT�����������ݵõ�IP��ַ�Ͷ˿�
bool CClientSocket::ConvertDotAddress(char* szAddress, LPDWORD pdwIpAddr, LPWORD pwPort)
{
	int idx = 0;
	int i = 0;
	int iCount = 0;
	
	char szIpAddr[MAX_ADDR_LEN];
	
	memset(szIpAddr, 0, sizeof(szIpAddr));
	
	char szPort[MAX_ADDR_LEN];
	
	memset(szPort, 0, sizeof(szPort));
	
	*pdwIpAddr = 0; 
	
	*pwPort = 0;
	
	while (szAddress[idx])
	{
		if(szAddress[idx] == ',')
		{
			iCount++;
			
			szAddress[idx] = '.';
		}
		
		if( iCount < 4 )
		{
			szIpAddr[idx] = szAddress[idx];
		}
		else
		{
			szPort[i++] = szAddress[idx];
		}
		idx++;
	}
	
	if(iCount != 5)
	{
		return false;
	}
	
	*pdwIpAddr = inet_addr(szIpAddr);
	
	if (*pdwIpAddr == INADDR_NONE)
	{
		return false;
	}
	
	char *pToken = strtok(szPort + 1, ".");
	
	if (pToken == NULL)
	{
		return false;
	}
	
	*pwPort = (WORD)(atoi(pToken) * 256);
	
	pToken = strtok(NULL, ".");
	
	if (pToken == NULL)
	{
		return false;
	}
	
	*pwPort += (WORD)atoi(pToken);
	
	return true;
}


//�ÿͻ���ͨ��PORTģʽ���ӵ�Ҫ���ļ����Ǹ�������ȥ
bool CClientSocket::ConnetRemoteHostByPORTcmd()
{
	
	if (m_socketRemote != INVALID_SOCKET)
	{
		closesocket(m_socketRemote);
		
		m_socketRemote = INVALID_SOCKET;
	}
	
	//�����׽���
	m_socketRemote = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	
	if (m_socketRemote == INVALID_SOCKET)
	{
		return false;
	}
	
	//����Ϊ��������20�Ŷ˿�
	struct sockaddr_in inetAddr;
	inetAddr.sin_family = AF_INET;
	inetAddr.sin_port = htons(LOCAL_DATA_TRANS_PORT);
	inetAddr.sin_addr.s_addr = inet_addr(GetLocalAddress());
	
	BOOL optval = TRUE;
	
	if (setsockopt(m_socketRemote, SOL_SOCKET, SO_REUSEADDR, (char*)&optval,sizeof(optval)) == SOCKET_ERROR) 
	{
		closesocket(m_socketRemote);
		
		m_socketRemote = INVALID_SOCKET;
		
		return false;
	}
	
	//���׽��ֵ��˿���
	if (bind(m_socketRemote, (struct sockaddr*)&inetAddr, sizeof(inetAddr)) == SOCKET_ERROR)
	{
		closesocket(m_socketRemote);
		
		m_socketRemote = INVALID_SOCKET;
		
		return false;
	}
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_nRemotePort);
	addr.sin_addr.s_addr = m_RemoteHost;
	
	//���ӵ�FTP�ͻ��˵Ļ�����
	if (connect(m_socketRemote, (const sockaddr*)&addr,sizeof(addr) ) == SOCKET_ERROR) 
	{
		closesocket(m_socketRemote);
		
		m_socketRemote = INVALID_SOCKET;
		
		return false;
	}
	
	return true;
}


//�ÿͻ���ͨ��PASVģʽ���ӵ�Ҫ���ļ����Ǹ�������ȥ
bool CClientSocket::ListenLocalSocketByPASVcmd(WORD listenPort)
{

	if (m_socketRemote != INVALID_SOCKET)
	{
		closesocket(m_socketRemote);
		
		m_socketRemote = INVALID_SOCKET;
	}
	
	//�����׽���
	m_socketRemote = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	
	if (m_socketRemote == INVALID_SOCKET)
	{
		return false;
	}
	
	//�󶨱����׽���
	struct sockaddr_in inetAddr;
	
	inetAddr.sin_family = AF_INET;
	
	inetAddr.sin_port = htons(listenPort);
	
	inetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	BOOL optval = TRUE;
	
	if (setsockopt(m_socketRemote, SOL_SOCKET, SO_REUSEADDR, (char*)&optval,sizeof(optval)) == SOCKET_ERROR) 
	{
		closesocket(m_socketRemote);
		
		m_socketRemote = INVALID_SOCKET;
		
		return false;
	}
	
	//�󶨶˿�
	if (bind(m_socketRemote, (struct sockaddr*)&inetAddr, sizeof(inetAddr)) == SOCKET_ERROR)
	{
		closesocket(m_socketRemote);
		
		m_socketRemote = INVALID_SOCKET;
		
		return false;
	}
	
	//��ʼ����
	if (listen(m_socketRemote, SOMAXCONN ) == SOCKET_ERROR) 
	{
		closesocket(m_socketRemote);
		
		m_socketRemote = INVALID_SOCKET;
		
		return false;
	}
	
	return true;
}


//����PORTģʽ
RESTATE CClientSocket::Deal_PORT(char *lpData)
{
	char chTmp[MAX_DATA_LEN];
	
	//��ȡPORT ������ַ���
	strcpy(chTmp, lpData + strlen("PORT") + 1);
	
	if (!ConvertDotAddress(chTmp, &m_RemoteHost, &m_nRemotePort))
	{
		return CMD_NOT_EXIST;
	}
	
	if (SendData(m_socket, chPortOK, strlen(chPortOK)) == -1)
	{
		return NEED_TO_DEL_CLIENT;
	}
	
	//���ø��û�������PORTģʽ
	
	m_bPort = true;
	
	return CMD_OK;
}


//����PASVģʽ
RESTATE CClientSocket::Deal_PASV()
{
	char chTmp[MAX_DATA_LEN];
	
	//���߿ͻ����Ѿ����ӳɹ�
	sprintf(chTmp, "227 Entering Passive Mode (%s,%d,%d).", GetLocalAddress(), REMOTE_PORT/256, REMOTE_PORT%256);
	
	if (SendData(m_socket, chTmp, strlen(chTmp)) == -1)
	{
		return NEED_TO_DEL_CLIENT;
	}
	
	//���ø��û�������PASVģʽ
	
	m_bPort = false;
	
	return CMD_OK;
}


//����GET����
RESTATE CClientSocket::Deal_RETR(char *lpData)
{
	if (!(RIGHT_READ & m_userInformaion.GetRight()))
	{
		return NO_RIGHT;
	}

	char chTmp[MAX_DATA_LEN];
	
	strcpy(chTmp, DEFAULT_HOME_DIR);
	//��ȡRETR������ַ���
	strcat(chTmp, lpData + strlen("RETR") + 1);
	
	CMyFile myFile;
	
	if (!myFile.OpenFile(chTmp))
	{
		return CANNOT_FIND;
	}
	
	if (!SendDataToClientByState(OPENING_AMODE))
	{	
		myFile.CloseFile();

		return NEED_TO_DEL_CLIENT;
	}
	
	if (m_bPort)
	{	
		if (!ConnetRemoteHostByPORTcmd())
		{
			myFile.CloseFile();

			return NORMAL_ERROR;
		}
	}
	else
	{
		//�����׽��֣����������ͻ��˵�����
		if (!ListenLocalSocketByPASVcmd(REMOTE_PORT))
		{
			myFile.CloseFile();

			return NORMAL_ERROR;
		}
		
		m_socketAccept = accept(m_socketRemote, NULL, NULL);
		
		if (m_socketAccept == INVALID_SOCKET)
		{
			myFile.CloseFile();

			closesocket(m_socketRemote);
			
			m_socketRemote = INVALID_SOCKET;
			
			return NORMAL_ERROR;
		}
	}
	
	//����PORT��PASVģʽ���ò�ͬ��ģʽ
	SOCKET socketSend;
	
	if (m_bPort)
	{
		//PORTģʽ
		socketSend = m_socketRemote;
	}
	else
	{
		//PASVģʽ
		socketSend = m_socketAccept;
	}
	
	int nLen = myFile.GetFileLength();//�ļ��ĳ�
	
	char buffer[MAX_DATA_LEN];
	
	int nLeftToSend = nLen;
	
	int nHaveDone = 0;
	
	int nShouldDo = 0;
	
	//��ȡ���ݲ�����
	while (nLeftToSend > 0)
	{
		nShouldDo = nLeftToSend >= MAX_DATA_LEN ? MAX_DATA_LEN : nLeftToSend;
		
		if (!myFile.ReadBuffer(nHaveDone, nShouldDo, buffer))
		{	
			myFile.CloseFile();
			
			closesocket(m_socketRemote);
			
			m_socketRemote = INVALID_SOCKET;
			
			closesocket(m_socketAccept);
			
			m_socketAccept = INVALID_SOCKET;
			
			return CANNOT_FIND;
		}
		
		int ret;
		//�����ļ���ָ����SOCKET
		if ((ret = SendData(socketSend, buffer, sizeof(char) * nShouldDo)) == -1)
		{	
			myFile.CloseFile();
			
			closesocket(m_socketRemote);
			
			m_socketRemote = INVALID_SOCKET;
			
			closesocket(m_socketAccept);
			
			m_socketAccept = INVALID_SOCKET;
			
			return NEED_TO_DEL_CLIENT;
		}
		else
		{
			nLeftToSend -= ret;
			nHaveDone += ret;
		}
		
	}
	
	//�ر��ļ����׽���
	myFile.CloseFile();
	
	closesocket(m_socketRemote);
	
	m_socketRemote = INVALID_SOCKET;
	
	closesocket(m_socketAccept);
	
	m_socketAccept = INVALID_SOCKET;
	
	return TRANS_COMPLETE;

}


//����STOR����
RESTATE CClientSocket::Deal_STOR(char *lpData)
{
	if (!(RIGHT_WRITE & m_userInformaion.GetRight()))
	{
		return NO_RIGHT;
	}

	char chTmp[MAX_DATA_LEN];
	
	strcpy(chTmp, DEFAULT_HOME_DIR);
	//��ȡRETR������ַ���
	strcat(chTmp, lpData + strlen("STOR") + 1);
	
	if (!SendDataToClientByState(OPENING_AMODE))
	{
			return NEED_TO_DEL_CLIENT;
	}
		
	if (m_bPort)
	{	
		if (!ConnetRemoteHostByPORTcmd())
		{	
			return NORMAL_ERROR;
		}
	}
	else
	{
		//�����׽��֣����������ͻ��˵�����
		if (!ListenLocalSocketByPASVcmd(REMOTE_PORT))
		{
			return NORMAL_ERROR;
		}

		m_socketAccept = accept(m_socketRemote, NULL, NULL);
		
		if (m_socketAccept == INVALID_SOCKET)
		{
			closesocket(m_socketRemote);
			
			m_socketRemote = INVALID_SOCKET;
			
			return NORMAL_ERROR;
		}
	}

	//����PORT��PASVģʽ���ò�ͬ��ģʽ
	SOCKET socketReceive;
	
	if (m_bPort)
	{
		//PORTģʽ
		socketReceive = m_socketRemote;
	}
	else
	{
		//PASVģʽ
		socketReceive = m_socketAccept;
	}

	fstream file;

	file.open(chTmp, ios::out | ios::binary);

	//�����ļ��������ļ�
	if (!file.bad())
	{
		while( TRUE )
		{
			int nBytesRecv = 0;

			char chGet[MAX_DATA_LEN];

			int nLeftToReceive = MAX_DATA_LEN;

			int HaveGet = 0;

			//����ʣ�� ������
			while (nLeftToReceive > 0)
			{
				nBytesRecv = recv(socketReceive, chGet + HaveGet, nLeftToReceive, 0);

				if( nBytesRecv == SOCKET_ERROR ) 
				{
					nBytesRecv = 0;

					break;
				}

				if (nBytesRecv == 0)
				{
					break;
				}
				
				nLeftToReceive -= nBytesRecv;

				HaveGet += nBytesRecv;
			}

			if (HaveGet > 0)
			{
				file.write(chGet, sizeof(char) * HaveGet);
			}

			// ���û�����ݿɽ��գ��˳�ѭ��
			if( nBytesRecv == 0 ) break;
		}
	}

	//�ļ�����
	file.close();

	closesocket(m_socketRemote);
	
	m_socketRemote = INVALID_SOCKET;
	
	closesocket(m_socketAccept);
	
	m_socketAccept = INVALID_SOCKET;	

	return TRANS_COMPLETE;
}


//����鿴����dir,ls
RESTATE CClientSocket::Deal_LIST_NLST(char *lpData)
{
	if (!(RIGHT_VIEW & m_userInformaion.GetRight()))
	{
		return NO_RIGHT;
	}

	char buffer[DATA_BUFSIZE];

	UINT nStrLen;

	bool bList = strstr(lpData, "LIST") ? true:false;
	

	//�Ƚ�������
	if (m_bPort)
	{	
		//ʹ��PORTģʽ
		if (!ConnetRemoteHostByPORTcmd())
		{
			return NORMAL_ERROR;
		}

		//���߿ͻ����Ѿ�׼������
		if (!SendDataToClientByState(OPENING_AMODE))
		{
			closesocket(m_socketRemote);
			
			m_socketRemote = INVALID_SOCKET;
			
			return NEED_TO_DEL_CLIENT;
		}


		//�õ��ļ��б�
		nStrLen = FileListToString(buffer, sizeof(buffer), bList);

		int nLeftToSend = nStrLen;
		
		int nHaveDone = 0;
		
		while (nLeftToSend > 0)
		{
			int ret;
			//�����ļ���ָ����SOCKET
			if ((ret = SendData(m_socketRemote, buffer + nHaveDone, sizeof(char) * nLeftToSend)) == -1)
			{	
				closesocket(m_socketRemote);
				
				m_socketRemote = INVALID_SOCKET;
				
				return NEED_TO_DEL_CLIENT;
			}
			else
			{
				nLeftToSend -= ret;
				nHaveDone += ret;
			}	
		}

		closesocket(m_socketRemote);
		
		m_socketRemote = INVALID_SOCKET;

		return TRANS_COMPLETE;

	}
	else
	{
		if (!ListenLocalSocketByPASVcmd(REMOTE_PORT))
		{
			return NORMAL_ERROR;
		}

		//ʹ��PASVģʽ
		m_socketAccept = accept(m_socketRemote, NULL, NULL);

		if (m_socketAccept == INVALID_SOCKET)
		{
			closesocket(m_socketRemote);
			
			m_socketRemote = INVALID_SOCKET;
			
			return NORMAL_ERROR;
		}

		//���߿ͻ����Ѿ�׼������
		if (!SendDataToClientByState(READY_TO_TRANS))
		{
			closesocket(m_socketRemote);
			
			m_socketRemote = INVALID_SOCKET;

			closesocket(m_socketAccept);
			
			m_socketAccept = INVALID_SOCKET;
			
			return NEED_TO_DEL_CLIENT;
		}

		//�õ��ļ��б�
		nStrLen = FileListToString(buffer, sizeof(buffer), bList);

		int nLeftToSend = nStrLen;
		
		int nHaveDone = 0;
		
		while (nLeftToSend > 0)
		{
			int ret;
			//�����ļ���ָ����SOCKET
			if ((ret = SendData(m_socketRemote, buffer + nHaveDone, sizeof(char) * nLeftToSend)) == -1)
			{	
				closesocket(m_socketRemote);
				
				m_socketRemote = INVALID_SOCKET;
				
				closesocket(m_socketAccept);
				
				m_socketAccept = INVALID_SOCKET;
				
				return NEED_TO_DEL_CLIENT;
			}
			else
			{
				nLeftToSend -= ret;
				nHaveDone += ret;
			}	
		}
		
		closesocket(m_socketRemote);
		
		m_socketRemote = INVALID_SOCKET;
		
		closesocket(m_socketAccept);
		
		m_socketAccept = INVALID_SOCKET;
		
		return TRANS_COMPLETE;

	}
}


//��ȡ�ļ�LIST
int CClientSocket::GetFileList(LIFILE_INF FArray, UINT nAarrySize, const char* szPath)
{
	WIN32_FIND_DATA wPS;
	int index = 0;
	char IpFileName[MAX_PATH];
	strcpy(IpFileName, DEFAULT_HOME_DIR);
	if (IpFileName[strlen(IpFileName) - 1] != '\\')
	{
		strcat(IpFileName,"\\");
	}
	
	strcat(IpFileName,szPath);
	
	HANDLE hFile = FindFirstFile(IpFileName,&wPS);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		lstrcpy(FArray[index].szFileName,wPS.cFileName);
		FArray[index].dwFileAttributes = wPS.dwFileAttributes;
		FArray[index].ftCreationTime = wPS.ftCreationTime;
		FArray[index].ftLastAccessTime = wPS.ftLastAccessTime;
		FArray[index].ftLastWriteTime = wPS.ftLastWriteTime;
		FArray[index].nFileSizeHigh = wPS.nFileSizeHigh;
		FArray[index++].nFileSizeLow = wPS.nFileSizeLow;
		while (FindNextFile(hFile,&wPS) && index < (int)nAarrySize )
		{
			lstrcpy(FArray[index].szFileName,wPS.cFileName);
			FArray[index].dwFileAttributes = wPS.dwFileAttributes;
			FArray[index].ftCreationTime = wPS.ftCreationTime;
			FArray[index].ftLastAccessTime = wPS.ftLastAccessTime;
			FArray[index].ftLastWriteTime = wPS.ftLastWriteTime;
			FArray[index].nFileSizeHigh = wPS.nFileSizeHigh;
			FArray[index++].nFileSizeLow = wPS.nFileSizeLow;
		}
		FindClose( hFile );
	}
	return index;
}


//���ļ��б�ת�����ַ���
UINT CClientSocket::FileListToString(char* buff, UINT nBuffSize,BOOL bDetails)
{
	FileINF fi[MAX_FILE_NUM];
	int nFiles = GetFileList( fi, MAX_FILE_NUM, "*.*" );
	char szTemp[128];
	sprintf( buff,"%s","" );
	if( bDetails ) {
		for( int i=0; i<nFiles; i++) {
			int k = strlen(buff);
			if( strlen(buff)>nBuffSize-128 )   break;
			if(!strcmp(fi[i].szFileName,"."))  continue;
			if(!strcmp(fi[i].szFileName,"..")) continue;
			// ʱ��
			SYSTEMTIME st;
			FileTimeToSystemTime(&(fi[i].ftLastWriteTime), &st);
			char  *szNoon = "AM";
			if( st.wHour > 12 ) 
			{ 
				st.wHour -= 12;
				szNoon = "PM"; 
			}
			if( st.wYear >= 2000 )
				st.wYear -= 2000;
			else st.wYear -= 1900;
			sprintf( szTemp,"%02u-%02u-%02u  %02u:%02u%s       ",
				st.wMonth,st.wDay,st.wYear,st.wHour,st.wMonth,szNoon );
			strcat( buff,szTemp );
			if( fi[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				strcat(buff,"<DIR>");
				strcat(buff,"          ");
			}
			else 
			{
				strcat(buff,"     ");
				// �ļ���С
				sprintf( szTemp,"% 9d ",fi[i].nFileSizeLow );
				strcat( buff,szTemp );
			}
			// �ļ���
			strcat( buff,fi[i].szFileName );
			strcat( buff,"\r\n");
		}
	} 
	else
	{ 
		for( int i=0; i<nFiles; i++)
		{
			if( strlen(buff) + strlen( fi[i].szFileName ) + 2 < nBuffSize )
			{ 
				strcat( buff, fi[i].szFileName );
				strcat( buff, "\r\n");
			} 
			else
				break;
		}
	}
	return strlen( buff );
}


RESTATE CClientSocket::RequestUser(char *lpData)//Ҫ�����û���
{

	//������������û�����Ϣ
	//ʹ�����ַ�����д
	if(strstr(strupr(lpData), "USER") != NULL)
	{
		//�õ��û���
		char chUserName[MAX_USER_NAME_LEN];

		strcpy(chUserName, lpData + strlen("USER") + 1);
		
		//strtok(chUserName,"\r\n");

		m_userInformaion.SetUserName(chUserName);
		
		return USER_OK;
	}
	
	//������������û�����Ϣ
	//ʹ�����ַ�����д
	if(strstr(strupr(lpData), "PASS") != NULL)
	{
		//��ȡ�û���
		char chPassWord[MAX_PASS_WORD_LEN];

		strcpy(chPassWord, lpData + strlen("PASS") + 1);
		
		//strtok(chPassWord,"\r\n");

		m_userInformaion.SetPassWord(chPassWord);
		
		//�Ƚ��û�������
		CValidate val;

		if (!val.GetDataFromFile("Users.txt"))
		{
			return LOGIN_FAILED;
		}
		
		char chRight;

		if (val.IsValidUser(m_userInformaion.GetUsername(), m_userInformaion.GetPassWord(), chRight))
		{
			m_bLogin = true;

			m_userInformaion.SetRight(chRight);
			
			return LOGIN_IN;
		}
		else
		{
			return LOGIN_FAILED;
		}
	}
	
	return CMD_NOT_EXIST;
}


RESTATE CClientSocket::DealCommond(char *lpData)//������ͨ����
{
	
	//��������Ƿ����Ҫ��
	if (strtok(lpData, "\r\n") == NULL)
	{
		return CMD_NOT_EXIST;
	}
	
	
	//�����ݶ���д
	strupr(lpData);
	
	//�������Ҫ�ϴ��������ļ�����ʹ��PORTģʽ
	if (strstr(lpData, "PORT") != NULL)
	{
		return Deal_PORT(lpData);
	}
	
	//�������Ҫ�ϴ��������ļ�����ʹ��PASVģʽ
	if (strstr(lpData, "PASV") != NULL)
	{
		return Deal_PASV();
	}
	
	
	//������û�Ҫ�����ļ�
	if(strstr(lpData, "RETR"))
	{
		return Deal_RETR(lpData);
	}
	
	//������û��˳�
	if (strstr(lpData, "QUIT"))
	{
		SendDataToClientByState(FTP_QUIT);
		
		return NEED_TO_DEL_CLIENT;
	}
	
	if (strstr(lpData,"TYPE") ) 
	{
		return TYPE_A;
	}
	
	if (strstr(lpData, "SYST"))
	{
		return SYST;
	}
	
	if (strstr(lpData, "REST"))
	{
		return REST;
	}
	
	if (strstr(lpData, "LIST") || strstr(lpData, "NLST") || strstr(lpData, "PWD"))
	{
		return Deal_LIST_NLST(lpData);
	}
	
	if (strstr(lpData, "STOR"))
	{
		return Deal_STOR(lpData);
	}
	
	return CMD_NOT_EXIST;
}


//��������
int CClientSocket::ReceiveData(SOCKET sock, char *lpData, int nLen)
{
	while (true)
	{
		int dwDoThisTime = recv(sock, (char *)lpData, nLen, 0);
		
		if (dwDoThisTime == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}
			else
			{
				return -1;
			}
		}

		//�ͻ����Ѿ��ر�
		if (dwDoThisTime == 0)
		{
			return -1;
		}

		return dwDoThisTime;
	}
}


//��������
int CClientSocket::SendData(SOCKET sock, char *lpData, int nLen)
{
	while (true)
	{
		int ret = send(sock, (char *)lpData, nLen, 0);
		
		if (ret == SOCKET_ERROR)
		{
			//��Ϊ·��̫æ���޷�����
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return ret;
		}
	}
}


//�Ƿ����ڴ�������
bool CClientSocket::IsDealing(void) const
{
	return m_bDealing;
}

//���õ�ǰΪ����򲻴���״̬
void CClientSocket::SetDealing(bool bDealing)
{
	m_bDealing = bDealing;
}

//��ǰ�׽����Ƿ����
bool CClientSocket::IsValid(void) const
{
	return m_bValid;
}

//�õ��ͻ��˵��׽���
SOCKET CClientSocket::GetSocket(void) const
{
	return m_socket;
}

//������Ϣ����ʱ���������
void CClientSocket::MessageArrive()
{
	//�����ڴ棬���ڴ��Ҫ���յĶ���
	char chBuffer[DATA_BUFSIZE];

	strcpy(chBuffer, "");
	
	//�������ݣ������صõ����ֽ���
	int nRet = ReceiveData(m_socket, chBuffer, DATA_BUFSIZE);

	//ȥ�������е�������Ϣ
	strtok(chBuffer, "\r\n");

	//��������-1
	if (nRet == -1)
	{
		Close();
		
		return;
	}
	else
	{
		if (nRet == 0)
		{
			Close();

			return;
		}
	}
	
	
	//�ж��Ƿ��Ѿ���½
	if (!m_bLogin)
	{
		//Ҫ���û���ʾ�û���������
		RESTATE re = RequestUser(chBuffer);
						
		//������ʾ���û�
		if (!SendDataToClientByState(re))
		{
			Close();
									
			return;
		}
	}
	else
	{
		//�Ѿ���½
		//����һ�������
		RESTATE re = DealCommond(chBuffer);
		
		//����Ҫɾ���û�
		if (re == NEED_TO_DEL_CLIENT)
		{
			Close();
			
			return;
		}
		else
		{
			//���͸��û���Ӧ����ʾ
			if (!SendDataToClientByState(re))
			{
				Close();
				
				return;
			}
		}
	}
}
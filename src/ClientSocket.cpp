#include "ClientSocket.h"
#include "MyFile.h"
#include "Validate.h"

CClientSocket::CClientSocket()
{
	m_socketRemote = INVALID_SOCKET;
	
	m_socket = INVALID_SOCKET;
	
	m_socketAccept = INVALID_SOCKET;
	
	memset(&m_ipaddr, 0, sizeof(in_addr));//清空
	
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

//初始化一个客户端
bool CClientSocket::Init(SOCKET sockAccept, in_addr *ipaddr)
{
	if (sockAccept == INVALID_SOCKET)
	{
		return false;
	}
	
	//设置连接的套接字
	m_socket = sockAccept;
	
	memcpy(&m_ipaddr, ipaddr, sizeof(in_addr));
	
	//设该套接字为可用
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

	//当前客户端不能用
	m_bValid = false;

	//设为PORT模式
	m_bPort = false;

	//设为未登陆
	m_bLogin = false;
}


//通过状态发送给客户端命令
bool CClientSocket::SendDataToClientByState(RESTATE state)
{
	char *p = NULL;
	
	switch(state)
	{
		//用户名发送过来已经成功
	case USER_OK:
		{
			p = chUserOK;
			
			break;
		}
		//------------------------------------------------------------------------------------
		//登陆成功
	case LOGIN_IN:
		{
			p = chLoginIn;
			
			break;
		}
		//------------------------------------------------------------------------------------
		//登陆失败
	case LOGIN_FAILED:
		{
			p = chLoginFailed;
			
			break;
		}
		//------------------------------------------------------------------------------------
		//当前的命令不存在
	case CMD_NOT_EXIST:
		{
			p = chCommondIsNotExists;
			
			break;
		}
		//------------------------------------------------------------------------------------
		//已经为PORT模式做好传送准备
	case OPENING_AMODE:
		{
			p = chOpenFileOK;
			
			break;
		}
		//传送完成
	case TRANS_COMPLETE:
		{
			p = chTransComplete;
			
			break;
		}
		//文件找不到
	case CANNOT_FIND:
		{
			p = chFileNoExist;
			
			break;
		}
		//客户端退出
	case FTP_QUIT:
		{
			p = chGoodBye;
			
			break;
		}
		//客户端要查看发送内容类型
	case TYPE_A:
		{
			p = chTypeOK;
			
			break;
		}
		//客户端要看系统类型
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
		//已经为PASV做好传送准备
	case READY_TO_TRANS:
		{
			p = chReadyToTrans;
			
			break;
		}
		//服务器上有错误出现
	case NORMAL_ERROR:
		{
			p = chNormalError;
			
			break;
		}
		//用户没有权限
	case NO_RIGHT:
		{
			p = chNoRight;
			
			break;
		}
		//向用户发送欢迎消息
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

//得到本地的IP
char* CClientSocket::GetLocalAddress()
{
	struct in_addr *pinAddr;
    LPHOSTENT	lpHostEnt;
	int			nRet;
	int			nLen;
	char        szLocalAddr[80];
	memset( szLocalAddr,0,sizeof(szLocalAddr) );
	//得到机器名
    nRet = gethostname(szLocalAddr,sizeof(szLocalAddr) );
	if (nRet == SOCKET_ERROR)
	{
		return NULL;
	}
	//查找机器
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



//通过PORT传过来的数据得到IP地址和端口
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


//让客户端通过PORT模式连接到要传文件的那个机器上去
bool CClientSocket::ConnetRemoteHostByPORTcmd()
{
	
	if (m_socketRemote != INVALID_SOCKET)
	{
		closesocket(m_socketRemote);
		
		m_socketRemote = INVALID_SOCKET;
	}
	
	//创建套接字
	m_socketRemote = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	
	if (m_socketRemote == INVALID_SOCKET)
	{
		return false;
	}
	
	//设置为本地侦听20号端口
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
	
	//绑定套接字到端口上
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
	
	//连接到FTP客户端的机器上
	if (connect(m_socketRemote, (const sockaddr*)&addr,sizeof(addr) ) == SOCKET_ERROR) 
	{
		closesocket(m_socketRemote);
		
		m_socketRemote = INVALID_SOCKET;
		
		return false;
	}
	
	return true;
}


//让客户端通过PASV模式连接到要传文件的那个机器上去
bool CClientSocket::ListenLocalSocketByPASVcmd(WORD listenPort)
{

	if (m_socketRemote != INVALID_SOCKET)
	{
		closesocket(m_socketRemote);
		
		m_socketRemote = INVALID_SOCKET;
	}
	
	//创建套接字
	m_socketRemote = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	
	if (m_socketRemote == INVALID_SOCKET)
	{
		return false;
	}
	
	//绑定本地套接字
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
	
	//绑定端口
	if (bind(m_socketRemote, (struct sockaddr*)&inetAddr, sizeof(inetAddr)) == SOCKET_ERROR)
	{
		closesocket(m_socketRemote);
		
		m_socketRemote = INVALID_SOCKET;
		
		return false;
	}
	
	//开始侦听
	if (listen(m_socketRemote, SOMAXCONN ) == SOCKET_ERROR) 
	{
		closesocket(m_socketRemote);
		
		m_socketRemote = INVALID_SOCKET;
		
		return false;
	}
	
	return true;
}


//处理PORT模式
RESTATE CClientSocket::Deal_PORT(char *lpData)
{
	char chTmp[MAX_DATA_LEN];
	
	//获取PORT 后面的字符串
	strcpy(chTmp, lpData + strlen("PORT") + 1);
	
	if (!ConvertDotAddress(chTmp, &m_RemoteHost, &m_nRemotePort))
	{
		return CMD_NOT_EXIST;
	}
	
	if (SendData(m_socket, chPortOK, strlen(chPortOK)) == -1)
	{
		return NEED_TO_DEL_CLIENT;
	}
	
	//设置该用户开起了PORT模式
	
	m_bPort = true;
	
	return CMD_OK;
}


//处理PASV模式
RESTATE CClientSocket::Deal_PASV()
{
	char chTmp[MAX_DATA_LEN];
	
	//告诉客户端已经连接成功
	sprintf(chTmp, "227 Entering Passive Mode (%s,%d,%d).", GetLocalAddress(), REMOTE_PORT/256, REMOTE_PORT%256);
	
	if (SendData(m_socket, chTmp, strlen(chTmp)) == -1)
	{
		return NEED_TO_DEL_CLIENT;
	}
	
	//设置该用户开起了PASV模式
	
	m_bPort = false;
	
	return CMD_OK;
}


//处理GET命令
RESTATE CClientSocket::Deal_RETR(char *lpData)
{
	if (!(RIGHT_READ & m_userInformaion.GetRight()))
	{
		return NO_RIGHT;
	}

	char chTmp[MAX_DATA_LEN];
	
	strcpy(chTmp, DEFAULT_HOME_DIR);
	//获取RETR后面的字符串
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
		//开设套接字，用于侦听客户端的连接
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
	
	//根据PORT与PASV模式设置不同的模式
	SOCKET socketSend;
	
	if (m_bPort)
	{
		//PORT模式
		socketSend = m_socketRemote;
	}
	else
	{
		//PASV模式
		socketSend = m_socketAccept;
	}
	
	int nLen = myFile.GetFileLength();//文件的长
	
	char buffer[MAX_DATA_LEN];
	
	int nLeftToSend = nLen;
	
	int nHaveDone = 0;
	
	int nShouldDo = 0;
	
	//读取数据并发送
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
		//发送文件到指定的SOCKET
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
	
	//关闭文件与套接字
	myFile.CloseFile();
	
	closesocket(m_socketRemote);
	
	m_socketRemote = INVALID_SOCKET;
	
	closesocket(m_socketAccept);
	
	m_socketAccept = INVALID_SOCKET;
	
	return TRANS_COMPLETE;

}


//处理STOR命令
RESTATE CClientSocket::Deal_STOR(char *lpData)
{
	if (!(RIGHT_WRITE & m_userInformaion.GetRight()))
	{
		return NO_RIGHT;
	}

	char chTmp[MAX_DATA_LEN];
	
	strcpy(chTmp, DEFAULT_HOME_DIR);
	//获取RETR后面的字符串
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
		//开设套接字，用于侦听客户端的连接
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

	//根据PORT与PASV模式设置不同的模式
	SOCKET socketReceive;
	
	if (m_bPort)
	{
		//PORT模式
		socketReceive = m_socketRemote;
	}
	else
	{
		//PASV模式
		socketReceive = m_socketAccept;
	}

	fstream file;

	file.open(chTmp, ios::out | ios::binary);

	//创建文件并接收文件
	if (!file.bad())
	{
		while( TRUE )
		{
			int nBytesRecv = 0;

			char chGet[MAX_DATA_LEN];

			int nLeftToReceive = MAX_DATA_LEN;

			int HaveGet = 0;

			//接收剩下 的数据
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

			// 如果没有数据可接收，退出循环
			if( nBytesRecv == 0 ) break;
		}
	}

	//文件保存
	file.close();

	closesocket(m_socketRemote);
	
	m_socketRemote = INVALID_SOCKET;
	
	closesocket(m_socketAccept);
	
	m_socketAccept = INVALID_SOCKET;	

	return TRANS_COMPLETE;
}


//处理查看命令dir,ls
RESTATE CClientSocket::Deal_LIST_NLST(char *lpData)
{
	if (!(RIGHT_VIEW & m_userInformaion.GetRight()))
	{
		return NO_RIGHT;
	}

	char buffer[DATA_BUFSIZE];

	UINT nStrLen;

	bool bList = strstr(lpData, "LIST") ? true:false;
	

	//先建立连接
	if (m_bPort)
	{	
		//使用PORT模式
		if (!ConnetRemoteHostByPORTcmd())
		{
			return NORMAL_ERROR;
		}

		//告诉客户端已经准备接收
		if (!SendDataToClientByState(OPENING_AMODE))
		{
			closesocket(m_socketRemote);
			
			m_socketRemote = INVALID_SOCKET;
			
			return NEED_TO_DEL_CLIENT;
		}


		//得到文件列表
		nStrLen = FileListToString(buffer, sizeof(buffer), bList);

		int nLeftToSend = nStrLen;
		
		int nHaveDone = 0;
		
		while (nLeftToSend > 0)
		{
			int ret;
			//发送文件到指定的SOCKET
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

		//使用PASV模式
		m_socketAccept = accept(m_socketRemote, NULL, NULL);

		if (m_socketAccept == INVALID_SOCKET)
		{
			closesocket(m_socketRemote);
			
			m_socketRemote = INVALID_SOCKET;
			
			return NORMAL_ERROR;
		}

		//告诉客户端已经准备接收
		if (!SendDataToClientByState(READY_TO_TRANS))
		{
			closesocket(m_socketRemote);
			
			m_socketRemote = INVALID_SOCKET;

			closesocket(m_socketAccept);
			
			m_socketAccept = INVALID_SOCKET;
			
			return NEED_TO_DEL_CLIENT;
		}

		//得到文件列表
		nStrLen = FileListToString(buffer, sizeof(buffer), bList);

		int nLeftToSend = nStrLen;
		
		int nHaveDone = 0;
		
		while (nLeftToSend > 0)
		{
			int ret;
			//发送文件到指定的SOCKET
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


//获取文件LIST
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


//把文件列表转化成字符串
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
			// 时间
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
				// 文件大小
				sprintf( szTemp,"% 9d ",fi[i].nFileSizeLow );
				strcat( buff,szTemp );
			}
			// 文件名
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


RESTATE CClientSocket::RequestUser(char *lpData)//要求发送用户名
{

	//如果数据中有用户名信息
	//使所有字符都大写
	if(strstr(strupr(lpData), "USER") != NULL)
	{
		//得到用户名
		char chUserName[MAX_USER_NAME_LEN];

		strcpy(chUserName, lpData + strlen("USER") + 1);
		
		//strtok(chUserName,"\r\n");

		m_userInformaion.SetUserName(chUserName);
		
		return USER_OK;
	}
	
	//如果数据中有用户名信息
	//使所有字符都大写
	if(strstr(strupr(lpData), "PASS") != NULL)
	{
		//提取用户名
		char chPassWord[MAX_PASS_WORD_LEN];

		strcpy(chPassWord, lpData + strlen("PASS") + 1);
		
		//strtok(chPassWord,"\r\n");

		m_userInformaion.SetPassWord(chPassWord);
		
		//比较用户与密码
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


RESTATE CClientSocket::DealCommond(char *lpData)//处理普通命令
{
	
	//检查命令是否符合要求
	if (strtok(lpData, "\r\n") == NULL)
	{
		return CMD_NOT_EXIST;
	}
	
	
	//把数据都大写
	strupr(lpData);
	
	//如果有人要上传或下载文件，并使用PORT模式
	if (strstr(lpData, "PORT") != NULL)
	{
		return Deal_PORT(lpData);
	}
	
	//如果有人要上传或下载文件，并使用PASV模式
	if (strstr(lpData, "PASV") != NULL)
	{
		return Deal_PASV();
	}
	
	
	//如果有用户要下载文件
	if(strstr(lpData, "RETR"))
	{
		return Deal_RETR(lpData);
	}
	
	//如果有用户退出
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


//接收数据
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

		//客户端已经关闭
		if (dwDoThisTime == 0)
		{
			return -1;
		}

		return dwDoThisTime;
	}
}


//发送数据
int CClientSocket::SendData(SOCKET sock, char *lpData, int nLen)
{
	while (true)
	{
		int ret = send(sock, (char *)lpData, nLen, 0);
		
		if (ret == SOCKET_ERROR)
		{
			//因为路由太忙而无法发送
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


//是否正在处理命令
bool CClientSocket::IsDealing(void) const
{
	return m_bDealing;
}

//设置当前为处理或不处理状态
void CClientSocket::SetDealing(bool bDealing)
{
	m_bDealing = bDealing;
}

//当前套接字是否可用
bool CClientSocket::IsValid(void) const
{
	return m_bValid;
}

//得到客户端的套接字
SOCKET CClientSocket::GetSocket(void) const
{
	return m_socket;
}

//当有消息到达时用这个函数
void CClientSocket::MessageArrive()
{
	//设置内存，用于存放要接收的东西
	char chBuffer[DATA_BUFSIZE];

	strcpy(chBuffer, "");
	
	//接收内容，并返回得到的字节数
	int nRet = ReceiveData(m_socket, chBuffer, DATA_BUFSIZE);

	//去除内容中的无用信息
	strtok(chBuffer, "\r\n");

	//出错，返回-1
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
	
	
	//判断是否已经登陆
	if (!m_bLogin)
	{
		//要求用户出示用户名与密码
		RESTATE re = RequestUser(chBuffer);
						
		//发送提示给用户
		if (!SendDataToClientByState(re))
		{
			Close();
									
			return;
		}
	}
	else
	{
		//已经登陆
		//处理一般的命令
		RESTATE re = DealCommond(chBuffer);
		
		//出错，要删除用户
		if (re == NEED_TO_DEL_CLIENT)
		{
			Close();
			
			return;
		}
		else
		{
			//发送给用户相应的提示
			if (!SendDataToClientByState(re))
			{
				Close();
				
				return;
			}
		}
	}
}
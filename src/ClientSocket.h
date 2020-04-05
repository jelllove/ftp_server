#if !defined CLIENT_SOCKET_H_FILE_INCLUDE
#define CLIENT_SOCKET_H_FILE_INCLUDE

#include "User.h"
#include "Define.h"

//客户端类，用于存放一些客户的资料
class CClientSocket
{
public:
	CClientSocket();
	~CClientSocket();
	
	//初始化一个客户端
	bool Init(SOCKET sockAccept, in_addr *ipaddr);

	//关闭客户端
	void Close();

	//是否正在处理命令
	bool IsDealing(void) const;
	
	//是否可用
	bool IsValid(void) const;
	
	//设置当前为处理或不处理状态
	void SetDealing(bool bDealing);

	//得到客户端的套接字
	SOCKET GetSocket(void) const;

	//当有消息到达时用这个函数
	void MessageArrive();
	
	//通过状态发送给客户端命令
	bool SendDataToClientByState(RESTATE state);



private:	
	//得到本地的IP
	char* GetLocalAddress();
	
	
	//通过PORT传过来的数据得到IP地址和端口
	bool ConvertDotAddress(char* szAddress, LPDWORD pdwIpAddr, LPWORD pwPort);
	
	//让客户端通过PORT模式连接到要传文件的那个机器上去
	bool ConnetRemoteHostByPORTcmd();
	
	//让客户端通过PASV模式连接到要传文件的那个机器上去
	bool ListenLocalSocketByPASVcmd(WORD listenPort);
	
	//处理PORT模式
	RESTATE Deal_PORT(char *lpData);
	
	//处理PASV模式
	RESTATE Deal_PASV();
	
	//处理GET命令
	RESTATE Deal_RETR(char *lpData);
	
	//处理STOR命令
	RESTATE Deal_STOR(char *lpData);
	
	//处理查看命令dir,ls
	RESTATE Deal_LIST_NLST(char *lpData);
	
	//获取文件LIST
	int GetFileList(LIFILE_INF FArray, UINT nAarrySize, const char* szPath);
	
	//把文件列表转化成字符串
	UINT FileListToString(char* buff, UINT nBuffSize,BOOL bDetails);
	
	//要求发送用户名
	RESTATE RequestUser(char *lpData);
	
	//处理普通命令
	RESTATE DealCommond(char *lpData);
	
	//接收数据
	int ReceiveData(SOCKET sock, char *lpData, int nLen);
	
	//发送数据
	int SendData(SOCKET sock, char *lpData, int nLen);

private:
	SOCKET m_socket;//套接字
	
	bool m_bLogin;//是否已经登陆成功

	bool m_bDealing;//是否正在处理一条命令

private:
	CUser m_userInformaion;//用户信息
	
	in_addr m_ipaddr;//客户端的IP地址
	
	//----------------------------------------------------
	DWORD m_RemoteHost;//在使用被动模式的主机IP
	
	WORD m_nRemotePort;//远程计算机的端口
	//-------------------------------------------------
	
	bool m_bPort;//是否是PORT模式
	
	SOCKET m_socketRemote;//连接远程计算机的SOCKET
	
	SOCKET m_socketAccept;//用于接收的OSCKET
	
	bool m_bValid;//是否可用
};

#endif
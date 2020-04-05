#if !defined CLIENT_SOCKET_H_FILE_INCLUDE
#define CLIENT_SOCKET_H_FILE_INCLUDE

#include "User.h"
#include "Define.h"

//�ͻ����࣬���ڴ��һЩ�ͻ�������
class CClientSocket
{
public:
	CClientSocket();
	~CClientSocket();
	
	//��ʼ��һ���ͻ���
	bool Init(SOCKET sockAccept, in_addr *ipaddr);

	//�رտͻ���
	void Close();

	//�Ƿ����ڴ�������
	bool IsDealing(void) const;
	
	//�Ƿ����
	bool IsValid(void) const;
	
	//���õ�ǰΪ����򲻴���״̬
	void SetDealing(bool bDealing);

	//�õ��ͻ��˵��׽���
	SOCKET GetSocket(void) const;

	//������Ϣ����ʱ���������
	void MessageArrive();
	
	//ͨ��״̬���͸��ͻ�������
	bool SendDataToClientByState(RESTATE state);



private:	
	//�õ����ص�IP
	char* GetLocalAddress();
	
	
	//ͨ��PORT�����������ݵõ�IP��ַ�Ͷ˿�
	bool ConvertDotAddress(char* szAddress, LPDWORD pdwIpAddr, LPWORD pwPort);
	
	//�ÿͻ���ͨ��PORTģʽ���ӵ�Ҫ���ļ����Ǹ�������ȥ
	bool ConnetRemoteHostByPORTcmd();
	
	//�ÿͻ���ͨ��PASVģʽ���ӵ�Ҫ���ļ����Ǹ�������ȥ
	bool ListenLocalSocketByPASVcmd(WORD listenPort);
	
	//����PORTģʽ
	RESTATE Deal_PORT(char *lpData);
	
	//����PASVģʽ
	RESTATE Deal_PASV();
	
	//����GET����
	RESTATE Deal_RETR(char *lpData);
	
	//����STOR����
	RESTATE Deal_STOR(char *lpData);
	
	//����鿴����dir,ls
	RESTATE Deal_LIST_NLST(char *lpData);
	
	//��ȡ�ļ�LIST
	int GetFileList(LIFILE_INF FArray, UINT nAarrySize, const char* szPath);
	
	//���ļ��б�ת�����ַ���
	UINT FileListToString(char* buff, UINT nBuffSize,BOOL bDetails);
	
	//Ҫ�����û���
	RESTATE RequestUser(char *lpData);
	
	//������ͨ����
	RESTATE DealCommond(char *lpData);
	
	//��������
	int ReceiveData(SOCKET sock, char *lpData, int nLen);
	
	//��������
	int SendData(SOCKET sock, char *lpData, int nLen);

private:
	SOCKET m_socket;//�׽���
	
	bool m_bLogin;//�Ƿ��Ѿ���½�ɹ�

	bool m_bDealing;//�Ƿ����ڴ���һ������

private:
	CUser m_userInformaion;//�û���Ϣ
	
	in_addr m_ipaddr;//�ͻ��˵�IP��ַ
	
	//----------------------------------------------------
	DWORD m_RemoteHost;//��ʹ�ñ���ģʽ������IP
	
	WORD m_nRemotePort;//Զ�̼�����Ķ˿�
	//-------------------------------------------------
	
	bool m_bPort;//�Ƿ���PORTģʽ
	
	SOCKET m_socketRemote;//����Զ�̼������SOCKET
	
	SOCKET m_socketAccept;//���ڽ��յ�OSCKET
	
	bool m_bValid;//�Ƿ����
};

#endif
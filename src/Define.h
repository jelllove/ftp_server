#if !defined DEFINE_H_FILE_INCLUDE
#define DEFINE_H_FILE_INCLUDE

#include <winsock2.h>
#include <iostream>

using namespace std;

#pragma comment(lib, "ws2_32")

#define LOCAL_LISTEN_PORT 21  //���������˿�
#define LOCAL_DATA_TRANS_PORT 20 //�������ݷ��Ͷ˿�

#define REMOTE_PORT 1876//����PASVģʽʱ�Ķ˿�

#define MAX_DATA_LEN 1024//������ݴ��ʹ�С

#define MAX_USER_NAME_LEN 256//�û����ĳ���

#define MAX_PASS_WORD_LEN 256//����ĳ���

#define MAX_ADDR_LEN 80//IP��ַ����󳤶�

#define MAX_FILE_NUM 900//�����ļ�����

#define DATA_BUFSIZE 8192//���Ĵ�������


#define DEFAULT_HOME_DIR    "C:\\TEMP\\"//���ÿͻ��˵���Ŀ¼
//--------------------------------------------------------------------

#define RIGHT_VIEW 0x01//�������Ȩ��
#define RIGHT_READ 0x02//�����ص�Ȩ��
#define RIGHT_WRITE 0x04//���ϴ���Ȩ��


//----------------------------------------------------------------------------------------------
//Ҫ���ʾ�û���������
extern char chWelcomeInformation[];

//�û����Ѿ����ˣ�Ҫ���ʾ����
extern char chUserOK[];

//�û��������붼��ȷ
extern char chLoginIn[];

//�û������������
extern char chLoginFailed[];

//�������
extern char chCommondIsNotExists[];

//�ڷ������Ϸ����������
extern char chNormalError[];

//��û��Ȩ��
extern char chNoRight[];

//�˳��ͻ���
extern char chGoodBye[];

//�����û�Ҫ����PORTģʽ�򿪣����ɹ�ʱ���ظ��û�
extern char chPortOK[];

//���û�Ҫ�������ļ��������ļ��ֲ����ڰٷ��ظ��û�
extern char chFileNoExist[];

//���û�Ҫ�����ļ����Ѿ��������Ѿ�׼�����ͻ���ʱ�������û�,ʹ��PORTģʽʱ��
extern char chOpenFileOK[];


//���û�Ҫ��������ʱ���Ѿ��������Ѿ�׼�����ͻ���ʱ�������û�,ʹ��PASVģʽʱ��
extern char chReadyToTrans[];

//�ļ����ͽ���
extern char chTransComplete[];

//���ص�ǰ�Ĵ�ģʽ
extern char chTypeOK[];

//���ص�ǰ��ϵͳ����
extern char chSysT[];

//����
extern char chREST[];

//------------------------------------------------------------------------------------------------------------
#define OUTMESSAGE(x) cout<<x<<endl;


//��¼ÿ���ļ�������
typedef struct
{
	TCHAR szFileName[MAX_PATH];
	DWORD    dwFileAttributes; 
    FILETIME ftCreationTime; 
    FILETIME ftLastAccessTime; 
    FILETIME ftLastWriteTime; 
    DWORD    nFileSizeHigh; 
    DWORD    nFileSizeLow; 
}FileINF,*LIFILE_INF;




enum RESTATE
{
		USER_OK = 0,//�û�����
		LOGIN_IN,//��½�ɹ�
		LOGIN_FAILED,//��½ʧ�� 
		CMD_OK,//�������
		CMD_NOT_EXIST,//�������
		OPENING_AMODE,//�Ѿ���ASSCII��ʽ
		TRANS_COMPLETE,//�������
		CANNOT_FIND,//�ļ�������
		FTP_QUIT,//�û��˳�
		NORMAL_ERROR,//��ͨ�Ĵ������ָ�벻���ڵ�
		NEED_TO_DEL_CLIENT,//����������Ҫɾ���ͻ���
		TYPE_A,//���ظ��û�ACCIISģʽ
		SYST,//���ظ��û�����������ǰ�Ĳ���ϵͳ
		REST,
		READY_TO_TRANS,//����PASVģʽʱ����׼��������ʱ��
		NO_RIGHT,//û��Ȩ��
		WELCOME//��ӭ����Ϣ
};


#endif
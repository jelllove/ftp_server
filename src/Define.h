#if !defined DEFINE_H_FILE_INCLUDE
#define DEFINE_H_FILE_INCLUDE

#include <winsock2.h>
#include <iostream>

using namespace std;

#pragma comment(lib, "ws2_32")

#define LOCAL_LISTEN_PORT 21  //本地侦听端口
#define LOCAL_DATA_TRANS_PORT 20 //本地数据发送端口

#define REMOTE_PORT 1876//采用PASV模式时的端口

#define MAX_DATA_LEN 1024//最大数据传送大小

#define MAX_USER_NAME_LEN 256//用户名的长度

#define MAX_PASS_WORD_LEN 256//密码的长度

#define MAX_ADDR_LEN 80//IP地址的最大长度

#define MAX_FILE_NUM 900//最大的文件数组

#define DATA_BUFSIZE 8192//最大的传送数据


#define DEFAULT_HOME_DIR    "C:\\TEMP\\"//设置客户端的主目录
//--------------------------------------------------------------------

#define RIGHT_VIEW 0x01//能浏览的权限
#define RIGHT_READ 0x02//能下载的权限
#define RIGHT_WRITE 0x04//能上传的权限


//----------------------------------------------------------------------------------------------
//要求出示用户名与密码
extern char chWelcomeInformation[];

//用户名已经好了，要求出示密码
extern char chUserOK[];

//用户名与密码都正确
extern char chLoginIn[];

//用户名或密码错误
extern char chLoginFailed[];

//命令不存在
extern char chCommondIsNotExists[];

//在服务器上发生常规错误
extern char chNormalError[];

//你没有权限
extern char chNoRight[];

//退出客户端
extern char chGoodBye[];

//当有用户要求以PORT模式打开，并成功时返回给用户
extern char chPortOK[];

//当用户要求下载文件，但是文件又不存在百返回给用户
extern char chFileNoExist[];

//当用户要下载文件，已经服务器已经准备给客户传时，发送用户,使用PORT模式时用
extern char chOpenFileOK[];


//当用户要下载数据时，已经服务器已经准备给客户传时，发送用户,使用PASV模式时用
extern char chReadyToTrans[];

//文件传送结束
extern char chTransComplete[];

//返回当前的打开模式
extern char chTypeOK[];

//返回当前的系统类型
extern char chSysT[];

//返回
extern char chREST[];

//------------------------------------------------------------------------------------------------------------
#define OUTMESSAGE(x) cout<<x<<endl;


//记录每个文件的性质
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
		USER_OK = 0,//用户可以
		LOGIN_IN,//登陆成功
		LOGIN_FAILED,//登陆失败 
		CMD_OK,//命令可以
		CMD_NOT_EXIST,//命令不存在
		OPENING_AMODE,//已经打开ASSCII方式
		TRANS_COMPLETE,//传送完成
		CANNOT_FIND,//文件不存在
		FTP_QUIT,//用户退出
		NORMAL_ERROR,//普通的错误，如果指针不存在等
		NEED_TO_DEL_CLIENT,//出错，并且需要删除客户端
		TYPE_A,//返回给用户ACCIIS模式
		SYST,//返回给用户，服务器当前的操作系统
		REST,
		READY_TO_TRANS,//当用PASV模式时，并准备传数据时用
		NO_RIGHT,//没有权限
		WELCOME//欢迎的消息
};


#endif
#include "Define.h"

//----------------------------------------------------------------------------------------------
//要求出示用户名与密码
char chWelcomeInformation[] = "220 Welcome to our FTP Server.\r\n";

//用户名已经好了，要求出示密码
char chUserOK[] = "331 User name okay, need password.\r\n";

//用户名与密码都正确
char chLoginIn[] = "230 User logged in.\r\n";

//用户名或密码错误
char chLoginFailed[] = "530 User cannot log in.\r\n";

//命令不存在
char chCommondIsNotExists[] = "502 Bad commond.\r\n";

//在服务器上发生常规错误
char chNormalError[] = "502 Have bad error on server.\r\n";

//你没有权限
char chNoRight[] = "502 You have no right to use this serveice.\r\n";

//退出客户端
char chGoodBye[] = "221 Good bye.\r\n";

//当有用户要求以PORT模式打开，并成功时返回给用户
char chPortOK[] = "200 PORT Command successful.\r\n";

//当用户要求下载文件，但是文件又不存在百返回给用户
char chFileNoExist[] = "550 Can not find the file.\r\n";

//当用户要下载文件，已经服务器已经准备给客户传时，发送用户,使用PORT模式时用
char chOpenFileOK[] = "150 Opening ASCII mode data connection for.\r\n";


//当用户要下载数据时，已经服务器已经准备给客户传时，发送用户,使用PASV模式时用
char chReadyToTrans[] = "125 Data connection already open; Transfer starting.\r\n";

//文件传送结束
char chTransComplete[] = "226 Transfer complete.\r\n";

//返回当前的打开模式
char chTypeOK[] = "200 Type set to A.\r\n";

//返回当前的系统类型
char chSysT[] = "215 Windows XP Server Pack 2\r\n";

//返回
char chREST[] = "504 Reply marker must be 0.\r\n";
#if !defined VALIDATE_H_FILE_INCLUDE
#define VALIDATE_H_FILE_INCLUDE

#include <string>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include "User.h"
using namespace std;

//身份验证类，用于登陆时，身份验证
class CValidate  
{
public:
	CValidate();
	~CValidate();

public:

	/******************************************************************
	//函数功能说明：从文件中读取用户名与密码
	//参数说明:
	//   pFileName  文件名
	//返回值：成功返回true,否则返回false
	******************************************************************/
	bool GetDataFromFile(char *pFileName);

    /******************************************************************
	//函数功能说明：用户名与密码是否正确
	//参数说明:
	//   pUserName  用户名
	//   pPassWord  密码
	//   chRight    权限
	//返回值：成功返回true,否则返回false
	******************************************************************/
	bool IsValidUser(char* pUserName,char* pPassWord, char &chRight);

private:
	vector<CUser> m_vec; //用户名向量
};

#endif

#if !defined VALIDATE_H_FILE_INCLUDE
#define VALIDATE_H_FILE_INCLUDE

#include <string>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include "User.h"
using namespace std;

//�����֤�࣬���ڵ�½ʱ�������֤
class CValidate  
{
public:
	CValidate();
	~CValidate();

public:

	/******************************************************************
	//��������˵�������ļ��ж�ȡ�û���������
	//����˵��:
	//   pFileName  �ļ���
	//����ֵ���ɹ�����true,���򷵻�false
	******************************************************************/
	bool GetDataFromFile(char *pFileName);

    /******************************************************************
	//��������˵�����û����������Ƿ���ȷ
	//����˵��:
	//   pUserName  �û���
	//   pPassWord  ����
	//   chRight    Ȩ��
	//����ֵ���ɹ�����true,���򷵻�false
	******************************************************************/
	bool IsValidUser(char* pUserName,char* pPassWord, char &chRight);

private:
	vector<CUser> m_vec; //�û�������
};

#endif

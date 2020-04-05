#include "Validate.h"


CValidate::CValidate()
{
	
}

CValidate::~CValidate()
{
	m_vec.clear();
}


/******************************************************************
//��������˵�������ļ��ж�ȡ�û���������
//����˵��:
//   pFileName  �ļ���
//����ֵ���ɹ�����true,���򷵻�false
******************************************************************/
bool CValidate::GetDataFromFile(char *pFileName)
{
	fstream file;
	
	file.open(pFileName, ios::in);
	
	if (file.good() || !file.bad())
	{
		while (!file.eof())
		{
			CUser user;

			string str;
			
			getline(file, str, '\n');
			user.SetUserName((char *)str.c_str());
	
			getline(file, str, '\n');
			user.SetPassWord((char *)str.c_str());
			
			getline(file, str, '\n');
			user.SetRight(atoi(str.c_str()) - 48);
			
			m_vec.push_back(user);
		}
	}
	else
	{
		return false;
	}
	
	file.close();


	return true;
}



/******************************************************************
//��������˵�����û����������Ƿ���ȷ
//����˵��:
//   pUserName  �û���
//   pPassWord  ����
//   chRight    Ȩ��
//����ֵ���ɹ�����true,���򷵻�false
******************************************************************/
bool CValidate::IsValidUser(char* pUserName,char* pPassWord, char &chRight)
{
	vector<CUser>::iterator it = m_vec.begin();
	
	while (it != m_vec.end())
	{
		if (
			strcmp((*it).GetUsername(), pUserName) == 0 
			&& 
			strcmp((*it).GetPassWord(), pPassWord) == 0)
		{
			chRight = (*it).GetRight();
			
			return true;
		}
		
		it++;
	}
	
	return false;
}


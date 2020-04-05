#include "MyFile.h"


CMyFile::CMyFile()
{
}


CMyFile::~CMyFile()
{
	//�ر��ļ�
	CloseFile();
}

/******************************************************************
//��������˵��������һ���ļ����������ļ��Ƿ����
//����˵��:
//   pFileName �ļ���
//����ֵ��������ڷ���true,���򷵻�false
******************************************************************/
int CMyFile::GetFileLength()
{
	//�򿪳���
	if (m_file.bad() || !m_file.good())
	{
		return -1;
	}
	else
	{
		//�õ�����
		int nLen = 0;
		
        m_file.seekg(ios::beg, ios::end);
		
		nLen = m_file.tellg();

		m_file.seekg(ios::beg);
		
		return nLen;
	}
}


/******************************************************************
//��������˵�����õ��ļ�����
//����˵��:
//   pFileName �ļ���
//����ֵ�������ȷ�򿪣�����ó��ȷ��س��ȣ����򷵻�-1
******************************************************************/
bool CMyFile::ReadBuffer(int iStart,int iLength, char* buffer)
{

	if (m_file.bad() || !m_file.good())
	{
		//�ļ�������
		return false;
	}
	else
	{
		//���ļ�ָ���Ƶ���Ӧ��λ��
        m_file.seekg(iStart);
		
		if (m_file.read(buffer, sizeof(char) * iLength))
		{
			//��ȷ��ȡ����
			return true;
		}
		else
		{
			//��ȡ����ʧ��
			return false;
		}
	}
}




/******************************************************************
//��������˵������һ���ļ�
//����˵��:
//   pFileName  �ļ���
//����ֵ���ɹ�����true, ʧ�ܷ���false
******************************************************************/
bool CMyFile::OpenFile(char *pFileName)
{
	m_file.open(pFileName, ios::in | ios::binary);

	if (m_file.bad() || !m_file.good())
	{
		return false;
	}
	else
	{
		return true;
	}
}


/******************************************************************
//��������˵�����ر��Ѿ��򿪵��ļ�
//����˵��:
//   ��
//����ֵ����
******************************************************************/
void CMyFile::CloseFile(void)
{
	m_file.close();
}
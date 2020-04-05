#include "MyFile.h"


CMyFile::CMyFile()
{
}


CMyFile::~CMyFile()
{
	//关闭文件
	CloseFile();
}

/******************************************************************
//函数功能说明：给出一个文件名，看看文件是否存在
//参数说明:
//   pFileName 文件名
//返回值：如果存在返回true,否则返回false
******************************************************************/
int CMyFile::GetFileLength()
{
	//打开出错
	if (m_file.bad() || !m_file.good())
	{
		return -1;
	}
	else
	{
		//得到长度
		int nLen = 0;
		
        m_file.seekg(ios::beg, ios::end);
		
		nLen = m_file.tellg();

		m_file.seekg(ios::beg);
		
		return nLen;
	}
}


/******************************************************************
//函数功能说明：得到文件长度
//参数说明:
//   pFileName 文件名
//返回值：如果正确打开，并获得长度返回长度，否则返回-1
******************************************************************/
bool CMyFile::ReadBuffer(int iStart,int iLength, char* buffer)
{

	if (m_file.bad() || !m_file.good())
	{
		//文件不存在
		return false;
	}
	else
	{
		//把文件指针移到相应的位置
        m_file.seekg(iStart);
		
		if (m_file.read(buffer, sizeof(char) * iLength))
		{
			//正确读取数据
			return true;
		}
		else
		{
			//读取数据失败
			return false;
		}
	}
}




/******************************************************************
//函数功能说明：打开一个文件
//参数说明:
//   pFileName  文件名
//返回值：成功返回true, 失败返回false
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
//函数功能说明：关闭已经打开的文件
//参数说明:
//   无
//返回值：无
******************************************************************/
void CMyFile::CloseFile(void)
{
	m_file.close();
}
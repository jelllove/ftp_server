#include <iostream>
#include <conio.h>
//�������Լ����ļ�
#include "ServerSocket.h"
//-------------------------------------

int main()
{
	CServerSocket css;

	//��ʼ��
	if (!css.StartServer())
	{
		OUTMESSAGE("error:    ��������ʼ��ʧ��");

		return -1;
	}

	OUTMESSAGE("����ESC���˳�����");

	while (true)
	{
		if (getch() == VK_ESCAPE)
		{
			css.Close();
			
			return 0;
		}
	}
	
	return 0;
}






//----------------------------------------------------------------------------------------------------------







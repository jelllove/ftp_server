#include <iostream>
#include <conio.h>
//以下是自己的文件
#include "ServerSocket.h"
//-------------------------------------

int main()
{
	CServerSocket css;

	//初始化
	if (!css.StartServer())
	{
		OUTMESSAGE("error:    服务器初始化失败");

		return -1;
	}

	OUTMESSAGE("按下ESC键退出服务");

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







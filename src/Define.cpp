#include "Define.h"

//----------------------------------------------------------------------------------------------
//Ҫ���ʾ�û���������
char chWelcomeInformation[] = "220 Welcome to our FTP Server.\r\n";

//�û����Ѿ����ˣ�Ҫ���ʾ����
char chUserOK[] = "331 User name okay, need password.\r\n";

//�û��������붼��ȷ
char chLoginIn[] = "230 User logged in.\r\n";

//�û������������
char chLoginFailed[] = "530 User cannot log in.\r\n";

//�������
char chCommondIsNotExists[] = "502 Bad commond.\r\n";

//�ڷ������Ϸ����������
char chNormalError[] = "502 Have bad error on server.\r\n";

//��û��Ȩ��
char chNoRight[] = "502 You have no right to use this serveice.\r\n";

//�˳��ͻ���
char chGoodBye[] = "221 Good bye.\r\n";

//�����û�Ҫ����PORTģʽ�򿪣����ɹ�ʱ���ظ��û�
char chPortOK[] = "200 PORT Command successful.\r\n";

//���û�Ҫ�������ļ��������ļ��ֲ����ڰٷ��ظ��û�
char chFileNoExist[] = "550 Can not find the file.\r\n";

//���û�Ҫ�����ļ����Ѿ��������Ѿ�׼�����ͻ���ʱ�������û�,ʹ��PORTģʽʱ��
char chOpenFileOK[] = "150 Opening ASCII mode data connection for.\r\n";


//���û�Ҫ��������ʱ���Ѿ��������Ѿ�׼�����ͻ���ʱ�������û�,ʹ��PASVģʽʱ��
char chReadyToTrans[] = "125 Data connection already open; Transfer starting.\r\n";

//�ļ����ͽ���
char chTransComplete[] = "226 Transfer complete.\r\n";

//���ص�ǰ�Ĵ�ģʽ
char chTypeOK[] = "200 Type set to A.\r\n";

//���ص�ǰ��ϵͳ����
char chSysT[] = "215 Windows XP Server Pack 2\r\n";

//����
char chREST[] = "504 Reply marker must be 0.\r\n";
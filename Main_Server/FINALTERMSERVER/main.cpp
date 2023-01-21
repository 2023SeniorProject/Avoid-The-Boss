#include <iostream>
#include <WS2tcpip.h>
#include <tcpioctl.h>
#include "OBDC_MGR.h"

HANDLE h_iocp;
SOCKET g_s_sock;
SOCKET g_c_sock;



int main() {
	USER_DB_MANAGER mgrobdc;
	setlocale(LC_ALL, "korean");
	mgrobdc.AllocateHandles();
	mgrobdc.ConnectDataSource(L"USER_DB");
	mgrobdc.ExecuteStatementDirect(L"EXEC LOGIN_USER_DB abcd,1111");
	mgrobdc.RetrieveResult();
	mgrobdc.DisconnectDataSource();
}
#pragma once
class DummyClient
{

	void MainAction();
	void ProcessPacket(char* packet);
	void RecvPacket();
	void SendPacket();
public:
	SOCKET _sock;
	BYTE _recv_buf[512];
	WSABUF _wsabuf;
};


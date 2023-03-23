#pragma once

class MSession
{
public:
	MSession():_cid(-1),_sock(INVALID_SOCKET),_prev_remain(0) {}
	MSession(SOCKET c_sock, int32 cid) : _sock(c_sock), _cid(cid),_prev_remain(0) { ZeroMemory(&_recv_over._over, sizeof(WSAOVERLAPPED)); }
	~MSession() { closesocket(_sock); }

	void DoSendLoginPacket(bool isSuccess);
	void DoSendRoomPacket(C_ROOM_PACKET_TYPE type);
	void DoRecv();
	void DoSend(void* packet);
public:
	int32     _prev_remain;
	int32	  _cid; // Ŭ���̾�Ʈ id
	SOCKET    _sock; // Ŭ���̾�Ʈ ����
	OVEREXTEN _recv_over; 
	
};


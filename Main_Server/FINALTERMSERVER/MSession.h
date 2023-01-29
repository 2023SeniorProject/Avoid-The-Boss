#pragma once

class MSession
{
public:
	MSession():_cid(-1),_sock(INVALID_SOCKET),_prev_remain(0) {}
	MSession(SOCKET c_sock, int32 cid) : _sock(c_sock), _cid(cid),_prev_remain(0) { ZeroMemory(&_recv_over._over, sizeof(WSAOVERLAPPED)); }
	MSession(const MSession& other) = delete; // ���� ���� ����
	~MSession() { closesocket(_sock); }

	void do_send_login_packet();
	void do_send_room_packet(C_ROOM_PACKET_TYPE type);
	void do_recv_packet();
public:
	int32     _prev_remain;
	int32 _cid; // Ŭ���̾�Ʈ id
	SOCKET _sock; // Ŭ���̾�Ʈ ����
	OVEREXTEN _recv_over; 
	
};


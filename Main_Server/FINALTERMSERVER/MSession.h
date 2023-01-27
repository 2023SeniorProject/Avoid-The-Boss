#pragma once
#include "CorePch.h"
#define BUF_SIZE 512

enum class COMP_TYPE {OP_RECV, OP_SEND, OP_ACCEPT};

class OVER_EXP
{

public:
	OVER_EXP() 
	{
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _buf;
		_comp_type = COMP_TYPE::OP_RECV;
		::ZeroMemory(&_over, sizeof(_over));
	}
	OVER_EXP(char* packet)
	{
		_wsabuf.buf = _buf;
		_wsabuf.len = packet[0];
		::ZeroMemory(&_over, sizeof(_over));
		_comp_type = COMP_TYPE::OP_SEND;
		memcpy(_buf, packet, packet[0]);
	}
	~OVER_EXP() 
	{
	}
public:
	COMP_TYPE _comp_type;
	WSAOVERLAPPED _over;
	WSABUF _wsabuf;
	char _buf[BUF_SIZE];
	
};

class MSession
{
public:
	MSession() {}
	MSession(SOCKET c_sock, int32 cid): _sock(c_sock), _cid(cid) {}
	MSession(const MSession& other) = delete; // 복사 생성 금지
	~MSession() { closesocket(_sock); }

	void do_send_login_packet();
	void do_send_room_packet(C_ROOM_PACKET_TYPE type);
	void do_recv_packet();
	void process_packet(void* packet);
private:
	int32 _cid; // 클라이언트 id
	SOCKET _sock; // 클라이언트 소켓
	OVER_EXP _recv_over; 
};


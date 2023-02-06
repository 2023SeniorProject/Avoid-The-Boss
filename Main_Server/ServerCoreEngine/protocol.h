#pragma once
#include <windows.h>
#define MAX_USER 10
#define PORTNUM 9000
#define CHATBUF 50
// Ŭ�� -> ���� ��Ŷ

enum C_PACKET_TYPE : uint8 { ACQ_LOGIN = 101, ACQ_LOGOUT = 102, ACQ_REGISTER = 103, CCHAT = 104   };
enum C_ROOM_PACKET_TYPE : uint8 { MK_RM = 115, DEL_RM = 116 , ENTER_RM = 117, EXIT_RM = 118}; // �� ����, �� ����, ���� , ���� 
enum S_PACKET_TYPE : uint8 { LOGIN_OK = 201,  LOGIN_FAIL = 202 , SCHAT = 203};


#pragma pack (push, 1)

struct _CHAT
{
	uint8 size;
	uint8 type;
	int16 sid;
	char buf[CHATBUF];
};

struct C2S_LOGIN
{
	uint8 size;
	uint8 type;
	WCHAR name[10];
	WCHAR pw[10];
};

struct C2S_REGISTER
{
	uint8 size;
	uint8 type;
	int8 name[10];
	int32 pw;
};


struct C2S_LOGOUT
{
	uint8 size;
	uint8 type;
	int16 cid;
};

struct C2S_ROOM
{
	uint8 size;
	uint8 type;
};

struct C2S_MOVE
{
	uint8 size;
	uint8 type;
	int16 cid;
	int8 dir;
};

struct C2S_ATTACK
{
	uint8 size;
	uint8 type;
	int16 cid;
	int8 wf; // �߻� ���� ���� ������
};

// =================

struct S2C_LOGIN_OK
{
	uint8 size;
	uint8 type;
	int16 cid;
};

struct S2C_LOGIN_FAIL
{
	uint8 size;
	uint8 type;
	int8 err_code; // �α��� ���� ����
};


struct S2C_MOVE
{
	uint8 size;
	uint8 type;
	int16 cid;
	int8 dir;
	int64 pos_x, pos_y;
};
#pragma pack (pop)
#pragma once

#define MAX_USER 10
#define PORTNUM 9000
#define CHATBUF 50
// Ŭ�� -> ���� ��Ŷ

enum C_PACKET_TYPE : uint8 { ACQ_LOGIN = 101, ACQ_LOGOUT = 102, CCHAT = 103, MOVE = 104   };
enum C_ROOM_PACKET_TYPE : uint8 { ACQ_MK_RM = 115, ACQ_ENTER_RM = 116, EXIT_CRM = 117}; // �� ����, �� ����, ���� , ���� 
enum S_ROOM_PACKET_TYPE : uint8 { MK_RM_OK = 119, MK_RM_FAIL = 120, HIDE_RM = 121, REP_ENTER_RM = 122, REP_EXIT_RM = 123 }; // �� ����, �� ����, ���� , ���� 
enum S_PACKET_TYPE : uint8 { LOGIN_OK = 201,  LOGIN_FAIL = 202 , SCHAT = 203};




#pragma pack (push, 1)

struct _CHAT
{
	uint8 size;
	uint8 type;
	int16 cid;
	char buf[CHATBUF];
};

struct C2S_LOGIN
{
	uint8 size;
	uint8 type;
	wchar_t name[10];
	wchar_t pw[10];
};

struct C2S_LOGOUT
{
	uint8 size;
	uint8 type;
	uint16 sid;
};

struct C2S_ROOM_CREATE
{
	uint8 size;
	uint8 type;
};

struct C2S_ROOM_ENTER
{
	uint8 size;
	uint8 type;
	int32 rmNum;
};

struct C2S_ROOM_EXIT
{
	uint8 size;
	uint8 type;
};

struct C2S_MOVE
{
	uint8 size;
	uint8 type;
	int16 sid;
	float x, y, z;
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
	int16 sid;
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

struct S2C_ROOM_CREATE
{
	uint8 size;
	uint8 type;
};

struct S2C_ROOM_ENTER
{
	uint8 size;
	uint8 type;
	uint8 success;
};

struct S2C_ROOM_EXIT
{
	uint8 size;
	uint8 type;
	int32 rmNum;
};

struct S2C_ROOM
{
	uint8 size;
	uint8 type;
	int32 rmNum;
};

struct S2C_HIDE_ROOM
{
	uint8 size;
	uint8 type;
	int32 rmNum;
};
#pragma pack (pop)
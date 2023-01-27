#pragma once

#define MAX_USER 10
#define PORTNUM 9999
// Ŭ�� -> ���� ��Ŷ

enum class C_PACKET_TYPE { ACQ_LOGIN, ACQ_LOGOUT, ACQ_REGISTER  };
enum class C_ROOM_PACKET_TYPE { MK_RM, DEL_RM, ENTER_RM, EXIT_RM }; // �� ����, �� ����, ���� , ���� 
enum class S_PACKET_TYPE { LOGIN_OK,  LOGIN_FAIL };



struct C2S_LOGIN
{
	int8 size;
	int8 type;
	int8 name[10];
	int32 pw;
};

struct C2S_REGISTER
{
	int8 size;
	int8 type;
	int8 name[10];
	int32 pw;
};


struct C2S_LOGOUT
{
	int8 size;
	int8 type;
	int8 cid;
};

struct C2S_ROOM
{
	int8 size;
	int8 type;
};

struct C2S_MOVE
{
	int8 size;
	int8 type;
	int8 cid;
	int8 dir;
};

struct C2S_ATTACK
{
	int8 size;
	int8 type;
	int8 cid;
	int8 wf; // �߻� ���� ���� ������
};

// =================

struct S2C_LOGIN_OK
{
	int8 size;
	int8 type;
	int8 cid;
};

struct S2C_LOGIN_FAIL
{
	int8 size;
	int8 type;
	int8 err_code; // �α��� ���� ����
};


struct S2C_MOVE
{
	int8 size;
	int8 type;
	int8 cid;
	int8 dir;
	int64 pos_x, pos_y;
};


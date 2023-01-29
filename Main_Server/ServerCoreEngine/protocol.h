#pragma once

#define MAX_USER 10
#define PORTNUM 9000
// 클라 -> 서버 패킷

enum  C_PACKET_TYPE { ACQ_LOGIN = 1, ACQ_LOGOUT = 2, ACQ_REGISTER = 3, CHAT = 4   };
enum  C_ROOM_PACKET_TYPE { MK_RM = 5, DEL_RM = 6 , ENTER_RM = 7, EXIT_RM = 8}; // 방 생성, 방 삭제, 입장 , 종료 
enum  S_PACKET_TYPE { LOGIN_OK = 9,  LOGIN_FAIL = 10};


struct C2S_CHAT
{
	int8 size;
	int8 type;
	char buf[100];
};

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
	int8 wf; // 발생 시점 월드 프레임
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
	int8 err_code; // 로그인 실패 사유
};


struct S2C_MOVE
{
	int8 size;
	int8 type;
	int8 cid;
	int8 dir;
	int64 pos_x, pos_y;
};


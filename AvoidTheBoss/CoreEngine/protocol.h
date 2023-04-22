#pragma once

#define MAX_USER 10
#define PORTNUM 9001
#define CHATBUF 50
// 클라 -> 서버 패킷

enum C_PACKET_TYPE : uint8 { ACQ_LOGIN = 101, ACQ_LOGOUT = 102, CCHAT = 103, CKEY = 104 ,CROT = 105, CMOVE = 106 };
enum S_PACKET_TYPE : uint8 { LOGIN_OK = 201,  LOGIN_FAIL = 202 , SCHAT = 203, SKEY = 204, SROT = 205, SPOS = 206, GAME_START = 207, GAMEEVENT = 208};

enum C_ROOM_PACKET_TYPE : uint8 { ACQ_MK_RM = 115, ACQ_ENTER_RM = 116, EXIT_CRM = 117 }; // 방 생성, 방 삭제, 입장 , 종료 
enum S_ROOM_PACKET_TYPE : uint8 { MK_RM_OK = 119, MK_RM_FAIL = 120, HIDE_RM = 121, REP_ENTER_RM = 122, REP_EXIT_RM = 123 }; // 방 생성, 방 삭제, 입장 , 종료 



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


// ======= 게임 로직 패킷 ==============


struct C2S_KEY
{
	uint8 size;
	uint8 type;
	uint8 key; // 입력 키
	float x,z; // 플레이어 방향 // 키 입력의 변화가 있을 때만 전송
};


struct C2S_ROTATE
{
	uint8 size;
	uint8 type;
	int32 angle;
}; // 마우스 회전 시 , 매 프레임마다 전송

struct C2S_ATTACK
{
	uint8 size;
	uint8 type;
	int16 cid; // 타겟
	int8 wf; // 발생 시점 월드 프레임
};

// =================

struct S2C_LOGIN_OK
{
	uint8 size;
	uint8 type;
	int16 sid;
	int16 cid;
};

struct S2C_LOGIN_FAIL
{
	uint8 size;
	uint8 type;
	int8 err_code; // 로그인 실패 사유
};

struct S2C_GAMESTART
{
	uint8 size;
	uint8 type;
	int16 sids[4];
}; 

struct S2C_KEY
{
	uint8 size;
	uint8 type;
	int16 sid;
	uint8 key;
};

struct S2C_POS
{
	uint8 size;
	uint8 type;
	int16 sid;
	uint8 fidx;
	float x;
	float z;
};

struct S2C_ROTATE
{
	uint8 size;
	uint8 type;
	int16 sid;
	int32 angle;
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

// 클라 / 서버 공용
struct SC_EVENTPACKET
{
	uint8 size;
	uint8 type;
	uint8 eventId; // 0: 발전기 시작 / 1: 발전기 완료 // 2: 사장님 공격 처리 // 3: 사장님 공격 쿨타임 
};
#pragma pack (pop)
#pragma once

#define MAX_USER 10
#define PORTNUM 9001
#define CHATBUF 50
// Ŭ�� -> ���� ��Ŷ

enum C_PACKET_TYPE : uint8 { ACQ_LOGIN = 101, ACQ_LOGOUT = 102, CCHAT = 103, CKEY = 104 ,CROT = 105, CMOVE = 106 };
enum S_PACKET_TYPE : uint8 { LOGIN_OK = 201,  LOGIN_FAIL = 202 , SCHAT = 203, SKEY = 204, SROT = 205, SPOS = 206, GAME_START = 207, SWITCH_ANIM = 209,SWITCH_ANIM_CANCEL = 210};
enum SC_PACKET_TYPE : uint8 { GAMEEVENT = 208};

enum C_ROOM_PACKET_TYPE : uint8 { ACQ_MK_RM = 115, ACQ_ENTER_RM = 116, EXIT_CRM = 117 }; // �� ����, �� ����, ���� , ���� 
enum S_ROOM_PACKET_TYPE : uint8 { MK_RM_OK = 119, MK_RM_FAIL = 120, HIDE_RM = 121, REP_ENTER_RM = 122, REP_EXIT_RM = 123 }; // �� ����, �� ����, ���� , ���� 

enum PLAYER_BEHAVIOR {IDLE = 0, RUN, WALK, SWITCH_INTER, ATTACKED, DOWN, RESCUE, ATTACK, RUN_ATTACK, CRAWL, STAND};

enum class EVENT_TYPE : uint8 
{
	ATTACK_EVENT = 0,
	
	COOLTIME_EVENT = 1,
	
	// ========= ����ġ ���� ��ȣ�ۿ� �̺�Ʈ =========
	SWITCH_ONE_START_EVENT = 2,
	SWITCH_TWO_START_EVENT = 3,
	SWITCH_THREE_START_EVENT = 4,
	SWITCH_ONE_END_EVENT = 5,
	SWITCH_TWO_END_EVENT = 6,
	SWITCH_THREE_END_EVENT = 7,
	SWITCH_ONE_ACTIVATE_EVENT = 8,
	SWITCH_TWO_ACTIVATE_EVENT = 9,
	SWITCH_THREE_ACTIVATE_EVENT = 10,

	// ========  �÷��̾� ����� �̺�Ʈ ==============
	HIDE_PLAYER_ONE = 11,
	HIDE_PLAYER_TWO = 12,
	HIDE_PLAYER_THREE = 13,
	HIDE_PLAYER_FOUR = 14,

	// ======= �÷��̾� �ǰ� �̺�Ʈ =========== // �ǰݽ� ���� �ǰ� ����Ʈ ���� or �ǰ� �ִϸ��̼� ���
	ATTACKED_PLAYER_ONE = 15,
	ATTACKED_PLAYER_TWO = 16,
	ATTACKED_PLAYER_THREE = 17,
	ATTACKED_PLAYER_FOUR = 18,

	// ========= �÷��̾� ����� ��ȣ�ۿ� ============
	RESCUE_PLAYER_ONE = 19,
	RESCUE_PLAYER_TWO = 20,
	RESCUE_PLAYER_THREE = 21,
	RESCUE_PLAYER_FOUR = 22,
	// ======== ����� ================
	RESCUE_CANCEL_PLAYER_ONE = 23,
	RESCUE_CANCEL_PLAYER_TWO = 24,
	RESCUE_CANCEL_PLAYER_THREE = 25,
	RESCUE_CANCEL_PLAYER_FOUR = 26

	// ======== ����� ================
	,
	ALIVE_PLAYER_ONE = 27,
	ALIVE_PLAYER_TWO = 28,
	ALIVE_PLAYER_THREE = 29,
	ALIVE_PLAYER_FOUR = 30
};

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


// ======= ���� ���� ��Ŷ ==============


struct C2S_KEY
{
	uint8 size;
	uint8 type;
	uint8 key; // �Է� Ű
	float x,z; // �÷��̾� ���� // Ű �Է��� ��ȭ�� ���� ���� ����
};


struct C2S_ROTATE
{
	uint8 size;
	uint8 type;
	int32 angle;
}; // ���콺 ȸ�� �� , �� �����Ӹ��� ����

struct C2S_ATTACK
{
	uint8 size;
	uint8 type;
	int16 cid; // Ÿ��
	int8 wf; // �߻� ���� ���� ������
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
	int8 err_code; // �α��� ���� ����
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
	float x, z;
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

// Ŭ�� / ���� ����
struct SC_EVENTPACKET
{
	uint8 size;
	uint8 type;
	uint8 eventId; // 0: ������ ���� / 1: ������ �Ϸ� // 2: ����� ���� ó�� // 3: ����� ���� ��Ÿ�� 
};

struct S2C_SWITCH_ANIM
{
	uint8 size;
	uint8 type;
	uint8 idx;
};

#pragma pack (pop)
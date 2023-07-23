#pragma once

#define MAX_USER 10
#define PORTNUM 9001
#define CHATBUF 50
// Ŭ�� -> ���� ��Ŷ



enum class C_TITLE_PACKET_TYPE : uint8
{
	ACQ_LOGIN = 150,
	ACQ_LOGOUT = 151
};

enum class C_ROOM_PACKET_TYPE : uint8
{
	ACQ_MK_RM = 119, 
	ACQ_ENTER_RM = 120, 
	ACQ_EXIT_ROOM = 121,
	ACQ_READY = 122,
	ACQ_READY_CANCEL = 123
}; // �� ����, �� ����, ���� , ����, ���� ���� 

enum class C_GAME_PACKET_TYPE : uint8
{
	CCHAT = 152,
	CKEY = 153,
	CROT = 154,
	CMOVE = 155,
	CATTACK = 156,
};

// ���� -> Ŭ�� ��Ŷ

enum class S_TITLE_PACKET_TYPE : uint8
{
	LOGIN_OK = 150,
	LOGIN_FAIL = 151,
};
enum class S_ROOM_PACKET_TYPE : uint8
{
	MK_RM_OK = 119, 
	MK_RM_FAIL = 120, 
	REP_ENTER_FAIL = 121, 
	REP_ENTER_OK = 122, 
	REP_EXIT_RM = 123, 
	UPDATE_LIST = 124, 
	ROOM_INFO = 125,
	REP_READY = 126, 
	REP_READY_CANCEL = 127, 
	GAME_START = 128
};

enum class S_GAME_PACKET_TYPE : uint8 
{ 

	SCHAT = 152, 
	SKEY = 153, 
	SROT = 154, 
	SPOS = 155, 
	GAME_START = 156, 
	ANIM = 157,
	FRAME = 158,
};

// ���� ��Ŷ
enum class SC_GAME_PACKET_TYPE : uint8 { GAMEEVENT = 208 };

enum class ANIMTRACK : uint8
{
	GEN_ANIM = 170,
	GEN_ANIM_CANCEL = 171,
	ATTACK_ANIM = 172,
};

 
// �� ���� ����, �� ���� ����, ����

enum class PLAYER_BEHAVIOR {IDLE = 0, RUN, WALK, SWITCH_INTER, ATTACKED, DOWN, RESCUE, ATTACK, RUN_ATTACK, CRAWL, STAND};

enum class EVENT_TYPE : uint8 
{
	ATTACK_EVENT = 0,
	COOLTIME_EVENT = 1,
	
	// ========= ����ġ ���� ��ȣ�ۿ� �̺�Ʈ =========
	SWITCH_ONE_START_EVENT,
	SWITCH_TWO_START_EVENT,
	SWITCH_THREE_START_EVENT,
	
	SWITCH_ONE_END_EVENT,
	SWITCH_TWO_END_EVENT,
	SWITCH_THREE_END_EVENT,

	SWITCH_ONE_ACTIVATE_EVENT,
	SWITCH_TWO_ACTIVATE_EVENT,
	SWITCH_THREE_ACTIVATE_EVENT ,

	// ========  �÷��̾� ����� �̺�Ʈ ==============
	HIDE_PLAYER_ONE,
	HIDE_PLAYER_TWO,
	HIDE_PLAYER_THREE,
	HIDE_PLAYER_FOUR,

	// ======= �÷��̾� �ǰ� �̺�Ʈ =========== // �ǰݽ� ���� �ǰ� ����Ʈ ���� or �ǰ� �ִϸ��̼� ���
	ATTACKED_PLAYER_ONE,
	ATTACKED_PLAYER_TWO,
	ATTACKED_PLAYER_THREE,
	ATTACKED_PLAYER_FOUR,

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
	ALIVE_PLAYER_FOUR = 30,

	ADD_FRAME = 31,
};

#pragma pack (push, 1)

struct _CHAT
{
	uint8 size;
	uint8 type;
	int16 cid;
	char buf[CHATBUF];
};

//============ Ŭ���̾�Ʈ �α��� ============
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
	uint16 sid = -1;
};

// ========== Ŭ���̾�Ʈ �� ���� ��Ŷ ============
struct C2S_ROOM_EVENT // ����, ������, ����
{
	uint8 size;
	uint8 type;
};

struct C2S_ROOM_ENTER // �� ����
{
	uint8 size;
	uint8 type;
	int32 rmNum;
};


// ======= Ŭ���̾�Ʈ ���� ���� ��Ŷ ==============

struct C2S_KEY
{
	uint8 size;
	uint8 type;
	uint8 key; // �Է� Ű
	float x,z; // �÷��̾� ���� // Ű �Է��� ��ȭ�� ���� ���� ����
	int8 idx; //  �÷��̾� �ڽ��� �ε����� �����´�.
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
	int16 tidx; // Ÿ��
	int32 wf; // �߻� ���� ���� ������
};

// ================= ���� �α��� ��Ŷ ==============

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

// ==============  ���� ���� ���� ��Ŷ ==============
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


// ===========  ���� �� ��Ŷ =======================

struct S2C_ROOM_EVENT
{
	uint8 size;
	uint8 type;
};

struct S2C_ROOM_ENTER
{
	uint8 size;
	uint8 type;
	int16 rmNum;
};

struct S2C_ROOM_LIST
{
	uint8 size;
	uint8 type;
	uint8 rmNum;
	int8 member;
};

struct S2C_ROOM_INFO
{
	uint8 size;
	uint8 type;
	int16 sids[4];
};

struct S2C_ROOM_READY
{
	uint8 size;
	uint8 type;
	int16 sid;
};

struct S2C_FRAMEPACKET
{
	uint8 size;
	uint8 type;
	int32 wf;
};


// Ŭ�� / ���� ����
struct SC_EVENTPACKET
{
	uint8 size;
	uint8 type;
	uint8 eventId; // 0: ������ ���� / 1: ������ �Ϸ� // 2: ����� ���� ó�� // 3: ����� ���� ��Ÿ�� 
};

struct S2C_ANIMPACKET
{
	uint8 size;
	uint8 type;
	uint8 idx;
	uint8 track;
};


#pragma pack (pop)
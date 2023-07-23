#pragma once

#define MAX_USER 10
#define PORTNUM 9001
#define CHATBUF 50
// 클라 -> 서버 패킷



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
}; // 방 생성, 방 삭제, 입장 , 종료, 레디 게임 

enum class C_GAME_PACKET_TYPE : uint8
{
	CCHAT = 152,
	CKEY = 153,
	CROT = 154,
	CMOVE = 155,
	CATTACK = 156,
};

// 서버 -> 클라 패킷

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

// 공통 패킷
enum class SC_GAME_PACKET_TYPE : uint8 { GAMEEVENT = 208 };

enum class ANIMTRACK : uint8
{
	GEN_ANIM = 170,
	GEN_ANIM_CANCEL = 171,
	ATTACK_ANIM = 172,
};

 
// 방 생성 응답, 방 입장 응답, 종료

enum class PLAYER_BEHAVIOR {IDLE = 0, RUN, WALK, SWITCH_INTER, ATTACKED, DOWN, RESCUE, ATTACK, RUN_ATTACK, CRAWL, STAND};

enum class EVENT_TYPE : uint8 
{
	ATTACK_EVENT = 0,
	COOLTIME_EVENT = 1,
	
	// ========= 스위치 관련 상호작용 이벤트 =========
	SWITCH_ONE_START_EVENT,
	SWITCH_TWO_START_EVENT,
	SWITCH_THREE_START_EVENT,
	
	SWITCH_ONE_END_EVENT,
	SWITCH_TWO_END_EVENT,
	SWITCH_THREE_END_EVENT,

	SWITCH_ONE_ACTIVATE_EVENT,
	SWITCH_TWO_ACTIVATE_EVENT,
	SWITCH_THREE_ACTIVATE_EVENT ,

	// ========  플레이어 숨기는 이벤트 ==============
	HIDE_PLAYER_ONE,
	HIDE_PLAYER_TWO,
	HIDE_PLAYER_THREE,
	HIDE_PLAYER_FOUR,

	// ======= 플레이어 피격 이벤트 =========== // 피격시 붉은 피격 이펙트 연출 or 피격 애니메이션 재생
	ATTACKED_PLAYER_ONE,
	ATTACKED_PLAYER_TWO,
	ATTACKED_PLAYER_THREE,
	ATTACKED_PLAYER_FOUR,

	// ========= 플레이어 깨우기 상호작용 ============
	RESCUE_PLAYER_ONE = 19,
	RESCUE_PLAYER_TWO = 20,
	RESCUE_PLAYER_THREE = 21,
	RESCUE_PLAYER_FOUR = 22,

	// ======== 깨우기 ================
	RESCUE_CANCEL_PLAYER_ONE = 23,
	RESCUE_CANCEL_PLAYER_TWO = 24,
	RESCUE_CANCEL_PLAYER_THREE = 25,
	RESCUE_CANCEL_PLAYER_FOUR = 26

	// ======== 깨우기 ================
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

//============ 클라이언트 로그인 ============
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

// ========== 클라이언트 방 관련 패킷 ============
struct C2S_ROOM_EVENT // 생성, 나가기, 레디
{
	uint8 size;
	uint8 type;
};

struct C2S_ROOM_ENTER // 방 입장
{
	uint8 size;
	uint8 type;
	int32 rmNum;
};


// ======= 클라이언트 게임 로직 패킷 ==============

struct C2S_KEY
{
	uint8 size;
	uint8 type;
	uint8 key; // 입력 키
	float x,z; // 플레이어 방향 // 키 입력의 변화가 있을 때만 전송
	int8 idx; //  플레이어 자신의 인덱스를 가져온다.
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
	int16 tidx; // 타겟
	int32 wf; // 발생 시점 월드 프레임
};

// ================= 서버 로그인 패킷 ==============

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

// ==============  서버 게임 로직 패킷 ==============
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


// ===========  서버 방 패킷 =======================

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


// 클라 / 서버 공용
struct SC_EVENTPACKET
{
	uint8 size;
	uint8 type;
	uint8 eventId; // 0: 발전기 시작 / 1: 발전기 완료 // 2: 사장님 공격 처리 // 3: 사장님 공격 쿨타임 
};

struct S2C_ANIMPACKET
{
	uint8 size;
	uint8 type;
	uint8 idx;
	uint8 track;
};


#pragma pack (pop)
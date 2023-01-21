#pragma once
#include "types.h"

// 클라 -> 서버 패킷

enum C_PACKET_TYPE { C_LOGIN, C_LOGOUT};
enum S_PACKET_TYPE {S_LOGIN_OK, S_LOGIN_FAIL};

struct C2S_LOGIN_PACKET
{
	int8 size;
	int8 type;
	int8 name[10];
	int32 pw;
};

struct C2S_LOGOUT_PACKET
{
	int8 size;
	int8 type;
	int8 cid;
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
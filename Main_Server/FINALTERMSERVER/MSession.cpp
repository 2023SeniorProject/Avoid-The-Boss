#include "pch.h"
#include "MSession.h"


void MSession::DoSendLoginPacket(bool isSuccess)
{
 
    if (isSuccess)
    {
        S2C_LOGIN_OK loginOkPacket;
        loginOkPacket.cid = _cid;
        loginOkPacket.size = sizeof(S2C_LOGIN_OK);
        loginOkPacket.type = (int8)S_PACKET_TYPE::LOGIN_OK;
        DoSend(&loginOkPacket);
    }
    else
    {
        S2C_LOGIN_OK loginFailPacket;
        loginFailPacket.size = sizeof(S2C_LOGIN_OK);
        loginFailPacket.type = (int8)S_PACKET_TYPE::LOGIN_FAIL;
        DoSend(&loginFailPacket);
    }
}

void MSession::DoSendRoomPacket(C_ROOM_PACKET_TYPE type)
{
}

void MSession::DoRecv()
{
    DWORD recv_flag = 0;
    memset(&_recv_over._over, 0, sizeof(_recv_over._over));
    _recv_over._wsabuf.len = BUFSIZE - _prev_remain;
    _recv_over._wsabuf.buf = _recv_over._buf + _prev_remain;
    WSARecv(_sock, &_recv_over._wsabuf, 1, 0, &recv_flag,
        &_recv_over._over, 0);
}

void MSession::DoSend(void* packet)
{
    OVEREXTEN* sendOver;
    DWORD sendLen(0);
    DWORD flag(0);
    sendOver = new OVEREXTEN(reinterpret_cast<char*>(packet));
    WSASend(_sock, &sendOver->_wsabuf, 1,&sendLen,flag, &sendOver->_over,NULL);
}

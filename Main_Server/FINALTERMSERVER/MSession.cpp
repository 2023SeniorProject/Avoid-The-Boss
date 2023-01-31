#include "pch.h"
#include "MSession.h"


void MSession::DoSendLoginPacket()
{
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

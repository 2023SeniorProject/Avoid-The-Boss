#include "pch.h"
#include "MSession.h"


void MSession::do_send_login_packet()
{
}

void MSession::do_send_room_packet(C_ROOM_PACKET_TYPE type)
{
}

void MSession::do_recv_packet()
{
    DWORD recv_flag = 0;
    memset(&_recv_over._over, 0, sizeof(_recv_over._over));
    _recv_over._wsabuf.len = BUFSIZE - _prev_remain;
    _recv_over._wsabuf.buf = _recv_over._buf + _prev_remain;
    WSARecv(_sock, &_recv_over._wsabuf, 1, 0, &recv_flag,
        &_recv_over._over, 0);
}

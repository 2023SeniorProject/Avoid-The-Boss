#pragma once
enum EVENT_TYPE : int8 { MOVE_EVENT, ATTACK_EVENT, STOP_MOVE_EVENT, WALK_EVENT };
// queue
class queueEvent abstract
{

};

class moveEvent : queueEvent
{
public:
	int8 key;
	int8 type;
};

class attackEvent : queueEvent
{
public:
	int8 type = ATTACK_EVENT;
};
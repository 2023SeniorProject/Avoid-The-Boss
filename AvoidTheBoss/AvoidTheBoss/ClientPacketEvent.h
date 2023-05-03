#pragma once
// 0 1 2

class queueEvent
{
public:
	int64 generateTime = 0.f;
	int32 sid = -1;
public:
	queueEvent() {};
	virtual ~queueEvent() {};
	virtual void Task() {};
};

class posEvent : public queueEvent // 33 ms ���� �����Ѵ�.
{
public:
	posEvent() { };
	virtual ~posEvent() {};
	XMFLOAT3 _pos {0,0,0};
public:
	virtual void Task()
	{
	
	};
	
};
class SwitchInteractionEvent : public queueEvent
{
public:
	SwitchInteractionEvent() {};
	virtual ~SwitchInteractionEvent() {};
	uint8 eventId = -1;
public:
	virtual void Task() 
	{ 
		
		switch ((EVENT_TYPE)eventId)
		{
		case EVENT_TYPE::SWITCH_ONE_START_EVENT:
		case EVENT_TYPE::SWITCH_TWO_START_EVENT:
		case EVENT_TYPE::SWITCH_THREE_START_EVENT:
		{
			
		}
		break;
		case EVENT_TYPE::SWITCH_ONE_END_EVENT: // ��ȣ�ۿ� ���߿� ���� ���
		case EVENT_TYPE::SWITCH_TWO_END_EVENT: // ��ȣ�ۿ� ���߿� ���� ���
		case EVENT_TYPE::SWITCH_THREE_END_EVENT: // ��ȣ�ۿ� ���߿� ���� ���
		
			break;
		default:
			std::cout << "UnKnown Game Event Please Check Your Packet Type\n";
			break;
		}
	};
	
};


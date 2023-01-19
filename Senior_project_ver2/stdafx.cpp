#include "stdafx.h"


//============================================

//====초기화

//----파이프라인 초기화

//디버그 계층을 사용하도록 설정합니다.

//디바이스를 만듭니다.

//명령 큐를 만듭니다.

//스왑 체인을 만듭니다.

//RTV(렌더링 대상 보기) 설명자 힙을 만듭니다.

//프레임 리소스(각 프레임에 대한 렌더링 대상 보기)를 만듭니다.

//명령 할당자를 만듭니다.

//----자산을 초기화 합니다.

//빈 루트 시그니쳐를 만듭니다.

//셰이더를 컴파일합니다.

//꼭짓점 입력 레이아웃을 만듭니다.

//파이프라인 상태 개체 디스크립터를 만든 다음 개체를 만듭니다.

//명령 목록을 만듭니다.

//명령 목록을 닫습니다.

//꼭짓점 버퍼를 만들고 로드합니다.

//꼭짓점 버퍼 뷰를 만듭니다.

//펜스를 만듭니다.(CPU-GPU 동기화 하는데 사용되는 펜스)

//이벤트 핸들을 만듭니다.

//GPU가 완료될 때까지 기다립니다.

//========================================

//====업데이터

//필요에 따라 상수,꼭짓점,인덱스 버퍼 및 다른 모든 항목을 수정합니다.

//OnUpdate

//=======================================

//====렌더링

//----명령 목록을 채웁니다.

//명령 리스트 할당자를 다시 설정합니다.

//명령 리스트를 다시 설정합니다.

//그래픽 루트 시그너쳐를 설정합니다.(현재 명령 리스트에 사용할 그래픽 루트 시그니쳐를 설정)

//뷰포트 및 가위 사각형을 설정합니다.

//렌더링 대상으로 사용할 백 버퍼를 나타내는 리소스 디스펜서를 설정합니다.(리소스 장벽은 리소슷 전환을 관리하는데 사용)

//명령을 명령 리스트에 기록합니다.

//명령 목록이 실행된 후 백 버퍼가 표시하는데 사용됨을 나타냅니다.(리소스 디스펜서를 설정하는 또 다른 호출)

//추가 기록을 위해 명령 리스트를 닫습니다.

//----명령 목록을 실행합니다.

//----프레임을 제공합니다.

//----GPU가 완료될 때까지 기다립니다.(펜스를 계속 업데이트하고 확인하세요)

//OnRender

//=========================================

//====삭제 : 앱을 완전히 닫습니다.

//----GPU가 완료될 때까지 기다립니다.(펜스를 최종적으로 확인하세요.)

//----이벤트 핸들을 닫습니다.

//OnDestroy




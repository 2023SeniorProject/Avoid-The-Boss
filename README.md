# Avoid-The-Boss
2023 졸업작품 Avoid The Boss 팀

--------------------------------------------------------------------------------------------------------------------------------
[게임 소개]
낡은 공장에 갇힌 직원들이 사장님을 피해 공장 속 발전기 3개를 모두 활성화 해 공장을 탈출하면 승리합니다.
사장님은 탈출하는 모든 직원들을 공격해 쓰러뜨려서 승리합니다.


[조작법]
WASD 키 : 상하좌우 이동
스페이스바 : 공격
F 키 : 살리기 및 발전기 상호작용


[프로젝트 빌드 방법]
1. AvoidTheBossServer.sln 열기
2. CorePch.h에서 #define PLAYERNUM 4 //플레이어 수 입력
3. CoreEngine 솔루션 빌드
4. GAMESERVER 솔루션 빌드 후 시작 프로젝트로 설정
5. AvoidTheBoss.sln 열기
6. AvoidTheBoss.cpp의 clientCore.InitConnect("127.0.0.1");의 IP주소 변경
7. AvoidTheBoss.sln에서 CoreEngine 빌드
8. AvoidTheBoss 빌드 후 시작 프로젝트로 설정
9. AvoidTheBoss/Binary/GameServer.exe 실행 파일 생성
10. AvoidTheBoss/Release/AvoidTheBoss.exe 실행 파일을 AvoidTheBoss/AvoidTheBoss/ 로 이동
11. 게임 실행
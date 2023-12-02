#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define PORT 55000
#define PACKET_SIZE 1024
using namespace std;

#include <string>
#include <iostream>
#include <winsock2.h>
#include <thread>
#include <map>

static SOCKET ServSock, ClntSocket1, ClntSocket2;
time_t current_time;
struct tm timeinfo;
map<SOCKET, string> clntInfoMap; // 각 클라이언트의 정보를 소켓과 매핑. 키-값 쌍을 저장하는 배열, 동적으로 할당, 각 키는 중복되지 않음
char buffer[PACKET_SIZE] = {};
char answer[PACKET_SIZE] = {};
bool isGameOver = false;

void showSetting() { // 미구현
    string servIP = "localhost";
    cout << "Server IP 주소 : " << servIP << endl;
}
void servBroadcast() {
    while (!isGameOver) {
        memset(buffer, 0, sizeof(buffer)); // 버퍼 비우기
        cout << "입력 : ";
        fgets(buffer, sizeof(buffer), stdin);
        send(ClntSocket1, buffer, strlen(buffer) - 1, 0);
        send(ClntSocket2, buffer, strlen(buffer) - 1, 0);
    }
}

void settime() {
    current_time = time(NULL); // unix 시간
    localtime_s(&timeinfo, &current_time);
}

void thr_recv(SOCKET clntSocket) {
    string clntMessage;
    string clntInfo;
    memset(buffer, 0, sizeof(buffer)); // 버퍼 비우기
    while (!isGameOver) {
        memset(buffer, 0, sizeof(buffer)); // 버퍼 비우기
        recv(clntSocket, buffer, PACKET_SIZE, 0);
        clntMessage = buffer;
        clntInfo = clntInfoMap[clntSocket];
        settime();
        cout << endl;
        if (strlen(buffer) > 0) {
            cout.width(10);
            cout << clntInfo << " : ";
            cout.width(40);
            cout << buffer;
            cout.width(5);
            cout << timeinfo.tm_hour << ":" << timeinfo.tm_min << ":" << timeinfo.tm_sec << endl << "입력 : ";
        }

        if (clntMessage == "stop") {
            cout << clntInfo << " 가 연결을 종료하였습니다." << endl;
            isGameOver = true;
            break; //클라이언트에서 보낸 message가 "stop"일경우 데이터받아오기 종료
        }

        if (clntSocket == ClntSocket1) {
            send(ClntSocket2, buffer, strlen(buffer), 0);
            if (!strcmp(buffer, answer)) {
                string gameOver = "1번 클라이언트 정답";
                cout << gameOver << endl;
                send(ClntSocket2, gameOver.c_str(), strlen(gameOver.c_str()), 0);
                isGameOver = true;

                break;
            }
        }
        else {
            send(ClntSocket1, buffer, strlen(buffer), 0);
            if (!strcmp(buffer, answer)) {
                string gameOver = "2번 클라이언트 정답";;
                cout << gameOver << endl;
                send(ClntSocket1, gameOver.c_str(), strlen(gameOver.c_str()), 0);
                isGameOver = true;
                break;
            }
        }
    }
}

void buildServ() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa); // Winsock 라이브러리 초기화

    ServSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    cout << "서버 소켓 생성" << endl;


    SOCKADDR_IN servaddr; // 서버소켓의 정보를 담는 구조체
    SOCKADDR_IN clntaddr; // 클라소켓의 정보를 담는 구조체

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY : 모든 가능한 로컬 IP 주소

    bind(ServSock, (SOCKADDR*)&servaddr, sizeof(servaddr)); // bind
    cout << "서버 소켓 바인드" << endl;
    listen(ServSock, 4); // listen 서버 소켓을 대기 상태로 둠
    cout << "클라이언트 접속 대기 중" << endl;

    int clnt_addrsize = sizeof(clntaddr);

    ClntSocket1 = accept(ServSock, (SOCKADDR*)&clntaddr, &clnt_addrsize); // 첫 번째 클라이언트 연결을 수락
    string clntInfo1 = "Client_1 (" + string(inet_ntoa(clntaddr.sin_addr)) + ":" + to_string(ntohs(clntaddr.sin_port)) + ")";
    clntInfoMap[ClntSocket1] = clntInfo1; // ip주소와 포트번호를 기반으로한 클라이언트1 식별자를 할당
    if (ClntSocket1 != INVALID_SOCKET) {
        cout << clntInfo1 << " 가 접속했습니다." << endl;
    }

    ClntSocket2 = accept(ServSock, (SOCKADDR*)&clntaddr, &clnt_addrsize); // 두 번째 클라이언트 연결을 수락
    string clntInfo2 = "Client_2 (" + string(inet_ntoa(clntaddr.sin_addr)) + ":" + to_string(ntohs(clntaddr.sin_port)) + ")";
    clntInfoMap[ClntSocket2] = clntInfo2; // ip주소와 포트번호를 기반으로한 클라이언트2 식별자를 할당
    if (ClntSocket2 != INVALID_SOCKET) {
        cout << clntInfo2 << " 가 접속했습니다." << endl;
    }
    
    cout << "정답을 입력해 주세요 : ";
    cin >> answer;

    char startMessage[] = "게임을 시작합니다.";
    send(ClntSocket1, startMessage, strlen(startMessage) - 1, 0);
    send(ClntSocket2, startMessage, strlen(startMessage) - 1, 0);
    
    thread servThread(servBroadcast);
    thread clntThread1(thr_recv, ClntSocket1); // 첫 번째 클라이언트에 대한 스레드 생성
    thread clntThread2(thr_recv, ClntSocket2); // 두 번째 클라이언트에 대한 스레드 생성
    
    servThread.join();
    clntThread1.join(); // 스레드가 종료될 때까지 대기
    clntThread2.join();


    closesocket(ServSock);
    closesocket(ClntSocket1);
    closesocket(ClntSocket2);

    WSACleanup();
}

int main() {
    buildServ(); // 리시브 하는 스레드 생성
    return 0;
}
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define PORT 55000
#define PACKET_SIZE 1024

#include <string.h>
#include <winsock2.h>
#include <iostream>
#include <thread>
using namespace std;


SOCKET hSocket;
char buffer[PACKET_SIZE] = {}; //char 생성

void thr_recv() {
	string message; //string 생성

	while (!WSAGetLastError()) {
		memset(&buffer, 0, sizeof(buffer)); // 버퍼 비우기
		recv(hSocket, buffer, PACKET_SIZE, 0); // 서버 데이터 받아오기
		message = buffer; //char형 buffer의값이 string형 message에 들어감

		time_t current_time;
		struct tm timeinfo; // 구조체
		current_time = time(NULL); // unix 시간
		localtime_s(&timeinfo, &current_time);
		cout << "Server : " << message << "\t\t" << timeinfo.tm_hour << ":" << timeinfo.tm_min << ":";
		if (timeinfo.tm_sec < 10) {
			cout << "0" << timeinfo.tm_sec << endl;
		}
		else {
			cout << timeinfo.tm_sec << endl;
		}
	}
}
void makeSocket() {
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // 클라이언트 소켓

	SOCKADDR_IN clntaddr;
	clntaddr.sin_family = AF_INET;
	clntaddr.sin_port = htons(PORT);
	clntaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	while (1) { // 접속 할 때 까지 while문 반복
		if (!connect(hSocket, (SOCKADDR*)&clntaddr, sizeof(clntaddr)))
			break;
	}
}

int main() {
	makeSocket();
	thread clntThread(thr_recv);
	char clntMessage[PACKET_SIZE] = { 0 }; // 정적할당 char배열 초기화

	while (!WSAGetLastError()) {
		cout << "입력 : ";
		fgets(clntMessage, sizeof(clntMessage), stdin);
		send(hSocket, clntMessage, strlen(clntMessage) - 1, 0); // send
	}
	clntThread.join();

	closesocket(hSocket); // 클라이언트 소켓 닫음
	WSACleanup();
}


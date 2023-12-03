#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define PORT 55000
#define PACKET_SIZE 1024

#include <string.h>
#include <winsock2.h>
#include <iostream>
#include <thread>
using namespace std;


SOCKET hSocket;
string tag1 = "clntNum1 : ";
string tag2 = "clntNum2 : ";
string recv_ServMsg; // 클라 번호 구분자
char buffer[PACKET_SIZE] = {};
string message; //받는 string 생성 (buffer을 받을거임)

void thr_recv() {
	while (!WSAGetLastError()) {
		memset(&buffer, 0, sizeof(buffer)); // 버퍼 비우기
		recv(hSocket, buffer, PACKET_SIZE, 0); // 서버 데이터 받아오기
		message = buffer; // 서버 : ~~

		recv_ServMsg = buffer;
		if (recv_ServMsg.find(tag1) == 0) { // 서버에게 받은 메세지 중, 번호만 추출
			recv_ServMsg = recv_ServMsg.substr(tag1.length()); // 리시브 서버메세지에는 번호만 남게됨
			cout << recv_ServMsg << endl;
		}
		else if (recv_ServMsg.find(tag2) == 0) {
			recv_ServMsg = recv_ServMsg.substr(tag2.length());
			cout << recv_ServMsg << endl;
		}

		time_t current_time;
		struct tm timeinfo; // 구조체
		current_time = time(NULL); // unix 시간
		localtime_s(&timeinfo, &current_time);
		cout << message << "\t\t" << timeinfo.tm_hour << ":" << timeinfo.tm_min << ":";
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
void Sending() {

}

int main() {
	makeSocket();
	thread clntThread(thr_recv);

	string clientCheck_msg = "클라이언트1 : ";

	while (!WSAGetLastError()) {
		fgets(buffer, sizeof(buffer), stdin);
		clientCheck_msg += buffer;
		send(hSocket, clientCheck_msg.c_str(), clientCheck_msg.size() - 1, 0); // send
		memset(&buffer, 0, sizeof(buffer)); // 메세지 배열 초기화
		clientCheck_msg = "클라이언트1 : ";
	}
	clntThread.join();

	closesocket(hSocket); // 클라이언트 소켓 닫음
	WSACleanup();
}


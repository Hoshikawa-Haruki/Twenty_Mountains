#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define PORT 55000
#define PACKET_SIZE 1024

#include <string.h>
#include <winsock2.h>
#include <iostream>
#include <thread>
using namespace std;


SOCKET hSocket;
char buffer[PACKET_SIZE] = {}; //char ����

void thr_recv() {
	string message; //string ����

	while (!WSAGetLastError()) {
		memset(&buffer, 0, sizeof(buffer)); // ���� ����
		recv(hSocket, buffer, PACKET_SIZE, 0); // ���� ������ �޾ƿ���
		message = buffer; //char�� buffer�ǰ��� string�� message�� ��

		time_t current_time;
		struct tm timeinfo; // ����ü
		current_time = time(NULL); // unix �ð�
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

	hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // Ŭ���̾�Ʈ ����

	SOCKADDR_IN clntaddr;
	clntaddr.sin_family = AF_INET;
	clntaddr.sin_port = htons(PORT);
	clntaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	while (1) { // ���� �� �� ���� while�� �ݺ�
		if (!connect(hSocket, (SOCKADDR*)&clntaddr, sizeof(clntaddr)))
			break;
	}
}

int main() {
	makeSocket();
	thread clntThread(thr_recv);
	char clntMessage[PACKET_SIZE] = { 0 }; // �����Ҵ� char�迭 �ʱ�ȭ

	string clientCheck_msg = "Ŭ���̾�Ʈ1 : ";

	while (!WSAGetLastError()) {
		fgets(clntMessage, sizeof(clntMessage), stdin);
		clientCheck_msg += clntMessage;
		send(hSocket, clientCheck_msg.c_str(), clientCheck_msg.size() - 1, 0); // send
		memset(&clntMessage, 0, sizeof(clntMessage)); // �޼��� �迭 �ʱ�ȭ
		clientCheck_msg = "Ŭ���̾�Ʈ1:";
	}
	clntThread.join();

	closesocket(hSocket); // Ŭ���̾�Ʈ ���� ����
	WSACleanup();
}


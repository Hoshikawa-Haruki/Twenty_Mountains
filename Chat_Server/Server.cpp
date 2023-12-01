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
map<SOCKET, string> clntInfoMap; // �� Ŭ���̾�Ʈ�� ������ ���ϰ� ����. Ű-�� ���� �����ϴ� �迭, �������� �Ҵ�, �� Ű�� �ߺ����� ����
char buffer[PACKET_SIZE] = {};

void showSetting() { // �̱���
    string servIP = "localhost";
    cout << "Server IP �ּ� : " << servIP << endl;
}
void servBroadcast() {
    while (TRUE) {
        memset(buffer, 0, sizeof(buffer)); // ���� ����
        fgets(buffer, sizeof(buffer), stdin);
        send(ClntSocket1, buffer, strlen(buffer) - 1, 0);
        send(ClntSocket2, buffer, strlen(buffer) - 1, 0);
    }
}

void makeserthread() {
    thread servThread(servBroadcast);
    servThread.detach();
}

void settime() {
    current_time = time(NULL); // unix �ð�
    localtime_s(&timeinfo, &current_time);
}

void thr_recv(SOCKET clntSocket) {
    string clntMessage;
    string clntInfo;
    memset(buffer, 0, sizeof(buffer)); // ���� ����

    cout << "check " << buffer << endl;
    while (true) {
        memset(buffer, 0, sizeof(buffer)); // ���� ����
        recv(clntSocket, buffer, PACKET_SIZE, 0);
        clntMessage = buffer;
        clntInfo = clntInfoMap[clntSocket];
        settime();

        if (strlen(buffer) > 0) {
            cout.width(10);
            cout << clntInfo << " : ";
            cout.width(40);
            cout << buffer;
            cout.width(5);
            cout << timeinfo.tm_hour << ":" << timeinfo.tm_min << ":" << timeinfo.tm_sec << endl;
            //memset(buffer, 0, sizeof(buffer)); // ���� ����

        }

        if (clntMessage == "stop") {
            cout << clntInfo << " �� ������ �����Ͽ����ϴ�." << endl;
            break; //Ŭ���̾�Ʈ���� ���� message�� "stop"�ϰ�� �����͹޾ƿ��� ����
        }

    }
    closesocket(clntSocket);
}

void buildServ() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa); // Winsock ���̺귯�� �ʱ�ȭ

    ServSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    cout << "���� ���� ����" << endl;


    SOCKADDR_IN servaddr; // ���������� ������ ��� ����ü
    SOCKADDR_IN clntaddr; // Ŭ������� ������ ��� ����ü

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY : ��� ������ ���� IP �ּ�

    bind(ServSock, (SOCKADDR*)&servaddr, sizeof(servaddr)); // bind
    cout << "���� ���� ���ε�" << endl;
    listen(ServSock, 4); // listen ���� ������ ��� ���·� ��
    cout << "Ŭ���̾�Ʈ ���� ��� ��" << endl;

    int clnt_addrsize = sizeof(clntaddr);

    ClntSocket1 = accept(ServSock, (SOCKADDR*)&clntaddr, &clnt_addrsize); // ù ��° Ŭ���̾�Ʈ ������ ����
    thread clntThread1(thr_recv, ClntSocket1); // ù ��° Ŭ���̾�Ʈ�� ���� ������ ����
    string clntInfo1 = "Client_1 (" + string(inet_ntoa(clntaddr.sin_addr)) + ":" + to_string(ntohs(clntaddr.sin_port)) + ")";
    clntInfoMap[ClntSocket1] = clntInfo1; // ip�ּҿ� ��Ʈ��ȣ�� ��������� Ŭ���̾�Ʈ1 �ĺ��ڸ� �Ҵ�
    if (ClntSocket1 != INVALID_SOCKET) {
        cout << clntInfo1 << " �� �����߽��ϴ�." << endl;
    }

    ClntSocket2 = accept(ServSock, (SOCKADDR*)&clntaddr, &clnt_addrsize); // �� ��° Ŭ���̾�Ʈ ������ ����
    thread clntThread2(thr_recv, ClntSocket2); // �� ��° Ŭ���̾�Ʈ�� ���� ������ ����
    string clntInfo2 = "Client_2 (" + string(inet_ntoa(clntaddr.sin_addr)) + ":" + to_string(ntohs(clntaddr.sin_port)) + ")";
    clntInfoMap[ClntSocket2] = clntInfo2; // ip�ּҿ� ��Ʈ��ȣ�� ��������� Ŭ���̾�Ʈ2 �ĺ��ڸ� �Ҵ�
    if (ClntSocket2 != INVALID_SOCKET) {
        cout << clntInfo2 << " �� �����߽��ϴ�." << endl;
    }
    clntThread1.join(); // �����尡 ����� ������ ���
    clntThread2.join();


    closesocket(ServSock);
    closesocket(ClntSocket1);
    closesocket(ClntSocket2);

    WSACleanup();
}

int main() {
    makeserthread(); // ��ε�ĳ��Ʈ
    buildServ(); // ���ú� �ϴ� ������ ����
    return 0;
}
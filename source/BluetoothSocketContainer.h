#pragma once

#include <WinSock2.h>
#include <ws2bth.h>

#define WM_BTH_SOCK_CREATED 0xafaf


class BluetoothSocketContainer
{
private:

	HWND hWnd;
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	SOCKADDR_BTH bt_addr;
	SOCKADDR_BTH client_bt_addr;


	int iSendResult;
	static const int DEFAULT_BUFLEN = 512;
	const char* MESSAGE_COMPLETE = "done";
	const char* MESSAGE_STOP = "stop";
	int recvbuflen;
	char* recvbuf; 

	bool shouldStop;
	bool newData;

	int onRecieve();

public:

	BluetoothSocketContainer(HWND hWnd, const int& recvbuflen);

	~BluetoothSocketContainer();

	int onCreate();

	void onThreadLoop();

	int onClear();

	char* getRecieveBuffer();

	void setShouldStop(const bool& shouldStop);

	int getBuffLen();

	bool getNewData();
};


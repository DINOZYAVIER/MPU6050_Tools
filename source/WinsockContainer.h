#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#define WM_SOCK_CREATED 0xfafa

class WinsockContainer
{
private:
	WSADATA wsaData;
	int iResult;
	SOCKET ListenSocket;
	SOCKET ClientSocket;

	struct addrinfo* result;
	struct addrinfo hints;

	HWND hWnd;
	char* port;

	static const int DEFAULT_BUFLEN = 512;
	const char *MESSAGE_COMPLETE = "done";
	const char *MESSAGE_STOP = "stop";
	int recvbuflen;
	char* recvbuf;

	bool shouldStop;
	bool newData;

	int onRecieve();

public:

	WinsockContainer(HWND hWnd, const int& recvbuflen);

	~WinsockContainer();

	void setPort(char* port);

	int onCreate();

	void onThreadLoop();

	int onClear();
	
	char* getRecieveBuffer();

	void setShouldStop(const bool& shouldStop);

	int getBuffLen();

	bool getNewData();
};


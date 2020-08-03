#include "WinsockContainer.h"

int WinsockContainer::onRecieve()
{
	int Result = 1;
	int iSendResult = 1;
	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		//printf("Bytes received: %d\n", iResult);

		// Echo the buffer back to the sende
		iSendResult = send(ClientSocket, MESSAGE_COMPLETE, strlen(MESSAGE_COMPLETE), 0);
		if (iSendResult == SOCKET_ERROR) {
			//printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
		//printf("Bytes sent: %d\n", iSendResult);

	}
	else if (iResult == 0) {
		//printf("Connection closing...\n");
		DestroyWindow(hWnd);
		return 1;
	}
	else {
		//printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	InvalidateRect(hWnd, NULL, FALSE);
	newData = true;
	return shouldStop;
}

WinsockContainer::WinsockContainer(HWND hWnd, const int& recvbuflen)
{
	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;
	result = NULL;
	shouldStop = false;
	this->hWnd = hWnd;
	this->recvbuflen = recvbuflen;
	recvbuf = new char[recvbuflen];
	memset(recvbuf, 0, recvbuflen);
}

WinsockContainer::~WinsockContainer()
{
	delete recvbuf;
}

void WinsockContainer::setPort(char* port)
{
	this->port = port;
}

int WinsockContainer::onCreate()
{
	int iResult = 1;
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		//printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, port, &hints, &result);
	if (iResult != 0) {
		//printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		//printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		//printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		//printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		//printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// No longer need server socket
	closesocket(ListenSocket);
	return 0;
}

void WinsockContainer::onThreadLoop()
{
	if (onCreate())
	{
		DestroyWindow(hWnd);
	}
	SendMessage(hWnd, WM_SOCK_CREATED, NULL, NULL);
	while (!onRecieve());
}

int WinsockContainer::onClear()
{
	int Result = 1;
	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		//printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();
	return 0;
}

char* WinsockContainer::getRecieveBuffer()
{
	return recvbuf;
}

void WinsockContainer::setShouldStop(const bool& shouldStop)
{
	this->shouldStop = shouldStop;
}

int WinsockContainer::getBuffLen()
{
	return recvbuflen;
}

bool WinsockContainer::getNewData()
{
	bool result = newData;
	if (newData) newData = false;
	return result;
}

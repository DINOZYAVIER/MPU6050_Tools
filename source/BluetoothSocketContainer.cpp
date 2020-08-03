#include "BluetoothSocketContainer.h"

int BluetoothSocketContainer::onRecieve()
{
	int Result = 1;
	int iSendResult = 1;
	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {

		iSendResult = send(ClientSocket, MESSAGE_COMPLETE, strlen(MESSAGE_COMPLETE), 0);
		if (iSendResult == SOCKET_ERROR) {
			//printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	}
	else if (iResult == 0) {
		DestroyWindow(hWnd);
		return 1;
	}
	else {
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	InvalidateRect(hWnd, NULL, FALSE);
	newData = true;
	return shouldStop;
}

BluetoothSocketContainer::BluetoothSocketContainer(HWND hWnd, const int& recvbuflen)
{
	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;

	memset(&bt_addr, 0, sizeof(bt_addr));
	bt_addr.addressFamily = AF_BTH;
	bt_addr.btAddr = 0;
	bt_addr.port = 0x0011;

	this->hWnd = hWnd;
	this->recvbuflen = recvbuflen;
	recvbuf = new char[recvbuflen];
	memset(recvbuf, 0, recvbuflen);

}

BluetoothSocketContainer::~BluetoothSocketContainer()
{
	delete recvbuf;
}

int BluetoothSocketContainer::onCreate()
{

	// Initialize Winsock
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		return 1;
	}

	memset(&bt_addr, 0, sizeof(bt_addr));
	bt_addr.addressFamily = AF_BTH;
	bt_addr.btAddr = 0;
	bt_addr.port = 0x0011;

	// Create a SOCKET for connecting to server
	ListenSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if (INVALID_SOCKET == ListenSocket) {
		WSACleanup();
		return 1;
	}

	// Setup the listening socket
	if (SOCKET_ERROR == bind(ListenSocket, (SOCKADDR*)&bt_addr, sizeof(bt_addr))) {
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	if (SOCKET_ERROR == listen(ListenSocket, SOMAXCONN)) {
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Accept a client socket
	int client_len = sizeof(client_bt_addr);
	ClientSocket = accept(ListenSocket, (SOCKADDR*)&client_bt_addr, &client_len);
	if (ClientSocket == INVALID_SOCKET) {
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// No longer need server socket
	closesocket(ListenSocket);
	return 0;
}

void BluetoothSocketContainer::onThreadLoop()
{
	if (onCreate())
	{
		DestroyWindow(hWnd);
	}
	SendMessage(hWnd, WM_BTH_SOCK_CREATED, NULL, NULL);
	while (!onRecieve());
}

int BluetoothSocketContainer::onClear()
{
	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (SOCKET_ERROR == shutdown(ClientSocket, SD_SEND)) {
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();
	return 0;
}

char* BluetoothSocketContainer::getRecieveBuffer()
{
	return recvbuf;
}

void BluetoothSocketContainer::setShouldStop(const bool& shouldStop)
{
	this->shouldStop = shouldStop;
}

int BluetoothSocketContainer::getBuffLen()
{
	return recvbuflen;
}

bool BluetoothSocketContainer::getNewData()
{
	bool result = newData;
	if (newData) newData = false;
	return result;
}
#pragma once
#include <windows.h>
#include <windowsx.h>
#define IDB_SUBMIT 322
#define IDB_SUBMIT_BT 228

class info
{
public:
	static constexpr const char BT_SUBMIT[] = "Press \"Submit\" to start the transmission...";
	static constexpr const char PORT_SUBMIT[] = "Input your port and press \"Submit\"...";
	static constexpr const char WAIT_CLIENT[] = "Waiting for client...";
	static constexpr const char TRANSFER[] = "Enjoying yourself?";
};

class ChildWnd
{
private:
	HWND hWnd;
	HWND hEdit;
	HWND hButton;
	HWND hEditInfo;
	HINSTANCE hInst;

	HWND hButton_bt;
	HWND hEditInfo_bt;
	char* port;
	int portlen;


public:
	static const int SOCK_SUBMIT = 1;
	static const int BT_SOCK_SUBMIT = 2;

	ChildWnd(HWND hWnd, HINSTANCE hInst);

	~ChildWnd();

	void onCreate();

	HWND createEdit();

	HWND createButton();

	HWND createEditInfo();

	HWND createButton_bt();

	HWND createEditInfo_bt();

	void getEditText();

	int onCommand(WPARAM wParam);

	void setTransfer();

	void setTransferBth();

	char* getPort();

	int getLen();
};
#include "ChildWnd.h"

ChildWnd::ChildWnd(HWND hWnd, HINSTANCE hInst)
{
	this->hWnd = hWnd;
	this->hInst = hInst;
}

ChildWnd::~ChildWnd()
{
	if (port)
		delete[] port;
}

void ChildWnd::onCreate()
{
	hEdit = createEdit();
	hButton = createButton();
	hEditInfo = createEditInfo();

	hButton_bt = createButton_bt();
	hEditInfo_bt = createEditInfo_bt();
}

HWND ChildWnd::createEdit()
{
	return CreateWindow("Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE,
		10, 20, 100, 20, hWnd, NULL, hInst, NULL);
}

HWND ChildWnd::createButton()
{
	return CreateWindow("Button", "Submit", WS_CHILD | WS_VISIBLE,
		10, 45, 100, 20, hWnd, (HMENU)IDB_SUBMIT, hInst, NULL);
}

HWND ChildWnd::createEditInfo()
{
	return CreateWindow("Edit", info::PORT_SUBMIT, WS_CHILD | WS_VISIBLE,
		10, 85, 250, 20, hWnd, NULL, hInst, NULL);
}


HWND ChildWnd::createButton_bt()
{
	return CreateWindow("Button", "Submit", WS_CHILD | WS_VISIBLE,
		GetSystemMetrics(SM_CXSCREEN) / 2, 45, 100, 20, hWnd, (HMENU)IDB_SUBMIT_BT, hInst, NULL);
}

HWND ChildWnd::createEditInfo_bt()
{
	return CreateWindow("Edit", info::BT_SUBMIT, WS_CHILD | WS_VISIBLE,
		GetSystemMetrics(SM_CXSCREEN) / 2, 85, 300, 20, hWnd, NULL, hInst, NULL);
}

void ChildWnd::getEditText()
{
	portlen = GetWindowTextLength(hEdit);
	port = new char[portlen + 1];
	SendMessage(hEdit, WM_GETTEXT, (WPARAM)(portlen + 1), (LPARAM)port);
	port[portlen] = '\0';
}

int ChildWnd::onCommand(WPARAM wParam)
{
	switch (LOWORD(wParam))
	{
	case IDB_SUBMIT:
		getEditText();
		Edit_Enable(hEdit, false);
		Button_Enable(hButton, false);
		Edit_SetText(hEditInfo, info::WAIT_CLIENT);
		return SOCK_SUBMIT;

	case IDB_SUBMIT_BT:
		Button_Enable(hButton_bt, false);
		Edit_SetText(hEditInfo_bt, info::WAIT_CLIENT);
		return BT_SOCK_SUBMIT;
	}

	return 0;
}

void ChildWnd::setTransfer()
{
	Edit_SetText(hEditInfo, info::TRANSFER);
}

void ChildWnd::setTransferBth()
{
	Edit_SetText(hEditInfo_bt, info::TRANSFER);
}

char* ChildWnd::getPort()
{
	return port;
}

int ChildWnd::getLen()
{
	return portlen;
}
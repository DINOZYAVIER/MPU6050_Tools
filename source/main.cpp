#include "main.h"

const int WINDOW_SIZEH = 600;
const int WINDOW_SIZEV = 450;

HINSTANCE hInst;
LPCTSTR szWindowClass = "mpu6050_tools";
LPCTSTR szTitle = "MPU6050 Tools";

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
	int nCmdShow)
{
	MSG msg;
	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow)) { return FALSE; }
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	hInst = hInstance;
	hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_CAPTION | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_SYSMENU | WS_MAXIMIZEBOX | WS_CLIPCHILDREN,
		0,
		0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN) - 40,
		NULL,
		NULL,
		hInstance,
		NULL);
	if (!hWnd) { return FALSE; }
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

char dataset[7][21] = {
	"10 13 14 1 2 3 100",
	"20 14 15 1 2 3 100",
	"30 13 14 1 2 3 100",
	"40 14 15 1 2 3 100",
	"30 13 14 1 2 3 100",
	"20 14 15 1 2 3 100",
	"10 13 14 1 2 3 100",
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rt;

	static std::thread* thr = nullptr;
	static std::thread* thr_bth = nullptr;
	int recvbuflen = DEFAULT_BUFLEN;
	static char *port;
	static WinsockContainer wSock(hWnd, recvbuflen);
	static BluetoothSocketContainer bthSock(hWnd, recvbuflen);
	static ChildWnd chldWnd(hWnd, hInst);
	static WinPaint wndPnt(hWnd);
	int result;

	char* recvbuf = wSock.getRecieveBuffer();
	char* recvbuf_bth = bthSock.getRecieveBuffer();
	static int init_flag = WinPaint::INIT;
	static int socket_flag;
	static int socket_flag_bth;

	switch (message)
	{
	case WM_CREATE:

		chldWnd.onCreate();
		break;

	case WM_PAINT:

		socket_flag = wSock.getNewData() ? WinPaint::SOCKET : 0;
		socket_flag_bth = bthSock.getNewData() ? WinPaint::BLUETOOTH : 0;

		wndPnt.onPaint(init_flag | socket_flag | socket_flag_bth, recvbuf, recvbuf_bth);

		init_flag = 0;

		break;

	case WM_MOVE:
	//case WM_SYSCOMMAND:

		init_flag = WinPaint::INIT;

	case WM_COMMAND:
		result = chldWnd.onCommand(wParam);
		if (result & chldWnd.SOCK_SUBMIT)
		{
			wSock.setPort(chldWnd.getPort());

			thr = new std::thread(&WinsockContainer::onThreadLoop, &wSock);
		}
		if (result & chldWnd.BT_SOCK_SUBMIT)
		{
			thr_bth = new std::thread(&BluetoothSocketContainer::onThreadLoop, &bthSock);
		}
		else
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;


	case WM_SOCK_CREATED:
		chldWnd.setTransfer();
		break;

	case WM_BTH_SOCK_CREATED:
		chldWnd.setTransferBth();
		break;

	case WM_DESTROY:

		wSock.setShouldStop(true);
		wSock.onClear();
		bthSock.setShouldStop(true);
		bthSock.onClear();
		thr->join();
		thr_bth->join();
		if (thr)
			delete thr;
		if (thr_bth)
			delete thr_bth;
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}


	return 0;
}

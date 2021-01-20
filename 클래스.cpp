#include <windows.h>



LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_hInst;

HWND hWndMain;

LPCTSTR lpszClass = TEXT("Class");

//진입점 Entry point main()

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance

	, LPSTR lpszCmdParam, int nCmdShow)

{



	HWND hWnd;

	MSG Message;

	WNDCLASS WndClass;

	g_hInst = hInstance;



	WndClass.cbClsExtra = 0;

	WndClass.cbWndExtra = 0;

	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);

	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	WndClass.hInstance = hInstance;

	WndClass.lpfnWndProc = WndProc;

	WndClass.lpszClassName = lpszClass;

	WndClass.lpszMenuName = NULL;

	WndClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&WndClass);



	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,

		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);



	while (GetMessage(&Message, NULL, 0, 0)) {

		TranslateMessage(&Message);

		DispatchMessage(&Message);

	}

	return (int)Message.wParam;

}

class pt{//클래스
public:
	int x;
	int y;
};

class mypoints :public pt
{
public:
	pt p[1000];
	int iCount;
};
mypoints points;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)

{//Window Procedure

	HDC hdc;

	PAINTSTRUCT ps;

	int x, y;

	HBRUSH hBrush;

	HBRUSH hbrush;

	//Message Driven Architecture

	switch (iMessage) {

	case WM_CREATE:

		hWndMain = hWnd;

		return 0;

	case 0x0201://WM_LBUTTONDOWN://왼쪽버튼을 누르면

		hdc = GetDC(hWnd); //윈도우 달려있는 그리기도구상자(DC) 얻기 hdc->핸들

		//x = LOWORD(lParam);//lParam->어디에서 눌렀는지를 알려줌

		x = (WORD)(lParam);

		//y = HIWORD(lParam);

		y = (WORD)((lParam) >> 16);
		points.p[points.iCount].x = x;
		points.p[points.iCount].y = y;
		points.iCount++;

		hBrush = CreateSolidBrush(RGB(255, 0, 0));

		SelectObject(hdc, hBrush);

		Ellipse(hdc, x - 10, y - 10, x + 10, y + 10);//그리기 api, 원(Ellipse),사각형(Rectangle)을 그려라

		return 0;

	case WM_PAINT:

		hdc = BeginPaint(hWnd, &ps);

		for (int i = 0; i < points.iCount; i++) {

			hBrush = CreateSolidBrush(RGB(255, 0, 0));

			SelectObject(hdc, hBrush);

			Ellipse(hdc, points.p[i].x - 10, points.p[i].y - 10,

				points.p[i].x + 10, points.p[i].y + 10);

		}



		EndPaint(hWnd, &ps);

		return 0;

	case WM_DESTROY:

		PostQuitMessage(0);

		return 0;

	}

	return(DefWindowProc(hWnd, iMessage, wParam, lParam));

}
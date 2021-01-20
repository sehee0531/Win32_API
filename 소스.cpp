#include <windows.h>



LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_hInst;

HWND hWndMain;

LPCTSTR lpszClass = TEXT("Class");

//������ Entry point main()

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

typedef struct tagmyPoint //����ü

{

	LONG x;

	LONG y;

} myPoint;



myPoint p[1000];

int iCount; //�׷��� ���� ����




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

	case 0x0201://WM_LBUTTONDOWN://���ʹ�ư�� ������

		hdc = GetDC(hWnd); //������ �޷��ִ� �׸��⵵������(DC) ��� hdc->�ڵ�

		//x = LOWORD(lParam);//lParam->��𿡼� ���������� �˷���

		x = (WORD)(lParam);

		//y = HIWORD(lParam);

		y = (WORD)((lParam) >> 16);
		p[iCount].x = x;
		p[iCount].y = y;

		iCount++;

		hBrush = CreateSolidBrush(RGB(255, 0, 0));

		SelectObject(hdc, hBrush);

		Ellipse(hdc, x - 10, y - 10, x + 10, y + 10);//�׸��� api, ��(Ellipse),�簢��(Rectangle)�� �׷���

		return 0;

	case WM_PAINT:

		hdc = BeginPaint(hWnd, &ps);

		for (int i = 0; i < iCount; i++) {

			hBrush = CreateSolidBrush(RGB(255, 0, 0));

			SelectObject(hdc, hBrush);

			Ellipse(hdc, p[i].x - 10, p[i].y - 10,

				p[i].x + 10, p[i].y + 10);

		}



		EndPaint(hWnd, &ps);

		return 0;

	case WM_DESTROY:

		PostQuitMessage(0);

		return 0;

	}

	return(DefWindowProc(hWnd, iMessage, wParam, lParam));

}
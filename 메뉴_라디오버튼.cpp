#include <windows.h>
#include "resource.h";

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Mouse");

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;// | CS_DBLCLKS;
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

typedef struct _line {
	POINT p[500];
	int iCount;
	COLORREF pColor;
} line;

line lines[1000];

//int iTempCount;
int iLines;

//COLORREF iCurrent
COLORREF iCurrentPenColor = RGB(255, 0, 0);
enum { ID_R1 = 101, ID_R2, ID_R3 };
HWND r1, r2, r3;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static int x;
	static int y;
	//static BOOL bNowDraw=FALSE;
	HPEN hPen;

	switch (iMessage) {
	case WM_CREATE:
		CreateWindow(TEXT("button"), TEXT("Color"), WS_CHILD | WS_VISIBLE |
			BS_GROUPBOX, 5, 5, 120, 110, hWnd, (HMENU)0, g_hInst, NULL);// 그룹박스는 테두리, 실행에 전혀 영향을 미치지않음
		
		r1 = CreateWindow(TEXT("button"), TEXT("Red"), WS_CHILD | WS_VISIBLE |
			BS_AUTORADIOBUTTON | WS_GROUP,//라디오버튼은 하나만 선택할 수 있으므로 GWS_ROUP 옵션 사용
			10, 20, 100, 30, hWnd, (HMENU)ID_R1, g_hInst, NULL);
		r2 = CreateWindow(TEXT("button"), TEXT("Green"), WS_CHILD | WS_VISIBLE |
			BS_AUTORADIOBUTTON,
			10, 50, 100, 30, hWnd, (HMENU)ID_R2, g_hInst, NULL);
		r3 = CreateWindow(TEXT("button"), TEXT("Blue"), WS_CHILD | WS_VISIBLE |
			BS_AUTORADIOBUTTON,
			10, 80, 100, 30, hWnd, (HMENU)ID_R3, g_hInst, NULL);
	
			
		CheckRadioButton(hWnd, ID_R1, ID_R3, ID_R1);//처음과 마지막, 선택되어 있어야하는 박스

		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDM_RED:
		case ID_R1:
			iCurrentPenColor = RGB(255, 0, 0);
			SetFocus(hWnd);//메인윈도우로 포커스를 바꾸어준다
			CheckRadioButton(hWnd, ID_R1, ID_R3, ID_R1);
			break;
		case IDM_GREEN:
		case ID_R2:
			CheckRadioButton(hWnd, ID_R1, ID_R3, ID_R2);
			iCurrentPenColor = RGB(0, 255, 0, 0);
			SetFocus(hWnd);
			break;
		case IDM_BLUE:
		case ID_R3:
			CheckRadioButton(hWnd, ID_R1, ID_R3, ID_R3);
			iCurrentPenColor = RGB(0, 0, 255);
			SetFocus(hWnd);
			break;
		}
		return 0;

	case WM_INITMENU:
		if (iCurrentPenColor == RGB(255, 0, 0)){
			
			CheckMenuItem((HMENU)wParam, IDM_RED, MF_BYCOMMAND | MF_CHECKED);//빨간색만 체크
			CheckMenuItem((HMENU)wParam, IDM_GREEN, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, IDM_BLUE, MF_BYCOMMAND | MF_UNCHECKED);
		
		}
		else if (iCurrentPenColor == RGB(0, 255, 0)){
			CheckMenuItem((HMENU)wParam, IDM_RED, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, IDM_GREEN, MF_BYCOMMAND | MF_CHECKED);//초록색만 체크
			CheckMenuItem((HMENU)wParam, IDM_BLUE, MF_BYCOMMAND | MF_UNCHECKED);
			
		}
		else if (iCurrentPenColor == RGB(0, 0, 255)){
			
			CheckMenuItem((HMENU)wParam, IDM_RED, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, IDM_GREEN, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, IDM_BLUE, MF_BYCOMMAND | MF_CHECKED);//파란색만 체크
			
		}
		return 0;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_BACK:
			if (iLines > 0)
			{
				iLines--; lines[iLines].iCount = 0;
			}
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
		return 0;

	case WM_LBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		lines[iLines].pColor = iCurrentPenColor;
		lines[iLines].p[lines[iLines].iCount].x = x;
		lines[iLines].p[lines[iLines].iCount].y = y;
		lines[iLines].iCount++;
		return 0;
	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON) {
			hdc = GetDC(hWnd);
			hPen = CreatePen(PS_SOLID, 3, iCurrentPenColor);

			hPen = (HPEN)SelectObject(hdc, hPen);

			MoveToEx(hdc, x, y, NULL);
			x = LOWORD(lParam);
			y = HIWORD(lParam);
			lines[iLines].p[lines[iLines].iCount].x = x;
			lines[iLines].p[lines[iLines].iCount].y = y;
			lines[iLines].iCount++;
			LineTo(hdc, x, y);

			DeleteObject(SelectObject(hdc, hPen));

			ReleaseDC(hWnd, hdc);
		}
		return 0;
	case WM_LBUTTONUP:
		//lines[iLines].iCount = iTempCount;
		//iTempCount = 0;
		iLines++;
		return 0;
		// 내가 생성한 GDI Object는 사용후반드시 삭제한다, 단 선택해제후
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		for (int i = 0; i < iLines; i++)
		{
			hPen = CreatePen(PS_SOLID, 3, lines[i].pColor);
			hPen = (HPEN)SelectObject(hdc, hPen);  //선택한다.
			MoveToEx(hdc, lines[i].p[0].x,
				lines[i].p[0].y, NULL);
			for (int j = 0; j < lines[i].iCount - 1; j++)
			{

				LineTo(hdc, lines[i].p[j + 1].x,
					lines[i].p[j + 1].y);
			}
			//선택해제...
			DeleteObject(SelectObject(hdc, hPen));  //그리고 삭제한다...
		}
		return 0;

	case WM_LBUTTONDBLCLK:
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}


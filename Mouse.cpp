#include <windows.h>
#include <tchar.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass=TEXT("Mouse");

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance
		  ,LPSTR lpszCmdParam,int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;
	
	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style=CS_HREDRAW | CS_VREDRAW;// | CS_DBLCLKS;
	RegisterClass(&WndClass);

	hWnd=CreateWindow(lpszClass,lpszClass,WS_OVERLAPPEDWINDOW,
		  CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
		  NULL,(HMENU)NULL,hInstance,NULL);
	ShowWindow(hWnd,nCmdShow);
	
	while (GetMessage(&Message,NULL,0,0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}
/*
void PaintWindow(HWND hwnd, int iColor, int iWidth)
{
	RECT            rect;
	HDC  hdc;
	HPEN hPen;
	hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rect);

	hPen = CreatePen(PS_SOLID, iWidth, iColor);
	hPen = (HPEN)SelectObject(hdc, hPen);
	MoveToEx(hdc, rect.left + 20, rect.bottom / 2, NULL);
	LineTo(hdc, rect.right - 20, rect.bottom / 2);
	DeleteObject(SelectObject(hdc, hPen));
	ReleaseDC(hwnd, hdc);
}
*/
void PaintWindow(HWND hwnd, int iColor, int iWidth)
{
	RECT rect;
	HDC  hdc, memdc;
	HPEN hPen;
	HBITMAP MyBitmap, OldBitmap;
	HBRUSH hBackBrush, oldBackBrush;
	hdc = GetDC(hwnd);
	memdc = CreateCompatibleDC(hdc);
	GetClientRect(hwnd, &rect);//*hwnd 즉 static text 의 left , top , botton , right  를 받아옴 

	MyBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom); // *원래 도화지 색깔은 검은색
	OldBitmap = (HBITMAP)SelectObject(memdc, MyBitmap);
	
	//hBackBrush = (HBRUSH)(LONG_PTR)GetClassLongPtr(hwnd, GCLP_HBRBACKGROUND);
	//FillRect(memdc, &rect, hBackBrush);
	//안됨, 값을 가져오지 못함 직접 등록한 class에 대해서만 작동 p.346
	//SetRect(&rect, rect.left, rect.top, rect.right, rect.bottom);//* 검은색 , 흰색 없애고 싶음 여기 1000,700으로 해주면 된다
	
	hBackBrush = CreateSolidBrush(RGB(240, 240, 240));
	oldBackBrush = (HBRUSH)SelectObject(memdc, hBackBrush);
	FillRect(memdc, &rect, hBackBrush);
	hPen = CreatePen(PS_SOLID, iWidth, iColor);//* 펜통에 펜을 넣는다
	hPen = (HPEN)SelectObject(memdc, hPen);// *펜을 가져온다
	MoveToEx(memdc, rect.left, rect.bottom / 2, NULL);
	LineTo(memdc, rect.right, rect.bottom / 2);
	BitBlt(hdc, rect.left, rect.top, rect.right, rect.bottom, memdc, 0, 0, SRCCOPY);

	SelectObject(memdc, oldBackBrush);
	DeleteObject(hBackBrush);
	SelectObject(memdc, OldBitmap);
	DeleteObject(MyBitmap);
	DeleteObject(SelectObject(memdc, hPen));
	DeleteDC(memdc);
	
	ReleaseDC(hwnd, hdc);
}

void PaintTheBlock(HWND hCtrl, int iColor, int iWidth)
{
	InvalidateRect(hCtrl, NULL, TRUE); // Dialog box의 윈도우 프로스져인 AboutDlgProc이 아니라 
	UpdateWindow(hCtrl);               // static control의 숨겨진 WndProc으로 WM_PAINT 전달
	PaintWindow(hCtrl, iColor, iWidth);
}



typedef struct _line {
	POINT p[500];
	int iCount;
	COLORREF pColor;
	int pWidth;
} line;

line lines[1000];

//int iTempCount;
int iLines;

int iCurrentPenWidth = 3;
COLORREF iCurrentPenColor = RGB(0, 0, 255);

TCHAR *Items[] = { TEXT("3"), TEXT("6"), TEXT("9"), TEXT("12") };
HWND hCombo, hRed, hGreen, hBlue;

BOOL CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static HWND hCtrlBlock;
	int TempPos;
	static int iRed, iGreen, iBlue, iWidth;
	switch (iMessage) {
	case WM_INITDIALOG:
		iWidth = iCurrentPenWidth;
		hCtrlBlock = GetDlgItem(hDlg, IDC_PAINT);


		iRed = GetRValue(iCurrentPenColor);
		iGreen = GetGValue(iCurrentPenColor);
		iBlue = GetBValue(iCurrentPenColor);
		hRed = GetDlgItem(hDlg, IDC_SCROLLBAR1);
		hGreen = GetDlgItem(hDlg, IDC_SCROLLBAR2);
		hBlue = GetDlgItem(hDlg, IDC_SCROLLBAR3);
		SetScrollRange(hRed, SB_CTL, 0, 255, TRUE);
		SetScrollPos(hRed, SB_CTL, iRed, TRUE);
		SetScrollRange(hGreen, SB_CTL, 0, 255, TRUE);
		SetScrollPos(hGreen, SB_CTL, iGreen, TRUE);
		SetScrollRange(hBlue, SB_CTL, 0, 255, TRUE);
		SetScrollPos(hBlue, SB_CTL, iBlue, TRUE);

		SetDlgItemInt(hDlg, IDC_SRED, iRed, FALSE);
		SetDlgItemInt(hDlg, IDC_SGREEN, iGreen, FALSE);
		SetDlgItemInt(hDlg, IDC_SBLUE, iBlue, FALSE);

		hCombo = GetDlgItem(hDlg, IDC_COMBO1);
		for (int i = 0; i < 4; i++) {
			SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)Items[i]);
		}
		//SendMessage(hCombo, CB_SETCURSEL, (iCurrentPenWidth / 3) - 1, 0);
		SetDlgItemInt(hDlg, IDC_COMBO1, iWidth, FALSE);
		return TRUE;

	case WM_HSCROLL:
		//if ((HWND)lParam == hRed) TempPos = Red;
		//if ((HWND)lParam == hGreen) TempPos = Green;
		//if ((HWND)lParam == hBlue) TempPos = Blue;
		TempPos = GetScrollPos((HWND)lParam, SB_CTL);
		switch (LOWORD(wParam)) {
		case SB_LINELEFT:
			TempPos = max(0, TempPos - 1);
			break;
		case SB_LINERIGHT:
			TempPos = min(255, TempPos + 1);
			break;
		case SB_PAGELEFT:
			TempPos = max(0, TempPos - 10);
			break;
		case SB_PAGERIGHT:
			TempPos = min(255, TempPos + 10);
			break;
		case SB_THUMBTRACK:
			TempPos = HIWORD(wParam);
			break;
		}

		SetScrollPos((HWND)lParam, SB_CTL, TempPos, TRUE);
		if ((HWND)lParam == hRed)
		{
			iRed = TempPos;
			SetDlgItemInt(hDlg, IDC_SRED, iRed, FALSE);
		}
		if ((HWND)lParam == hGreen)
		{
			iGreen = TempPos;
			SetDlgItemInt(hDlg, IDC_SGREEN, iGreen, FALSE);
		}
		if ((HWND)lParam == hBlue)
		{
			iBlue = TempPos;
			SetDlgItemInt(hDlg, IDC_SBLUE, iBlue, FALSE);
		}



		//InvalidateRect(hWnd, NULL, FALSE);
		PaintTheBlock(hCtrlBlock, RGB(iRed, iGreen, iBlue),
			iWidth);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			//iCurrentPenWidth =
			//	(SendMessage(hCombo,CB_GETCURSEL, 0, 0) + 1) * 3;
			iCurrentPenWidth = GetDlgItemInt(hDlg, IDC_COMBO1, NULL, FALSE);
			iCurrentPenColor = RGB(iRed, iGreen, iBlue);
			EndDialog(hDlg, IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;

		case IDC_COMBO1:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				iWidth =(SendMessage(hCombo, CB_GETCURSEL, 0, 0) + 1) * 3;
				PaintTheBlock(hCtrlBlock, RGB(iRed, iGreen, iBlue), iWidth);
				break;
			case CBN_EDITCHANGE:
				iWidth = GetDlgItemInt(hDlg, IDC_COMBO1, NULL, FALSE);
				PaintTheBlock(hCtrlBlock, RGB(iRed, iGreen, iBlue), iWidth);
				break;
			}
			return TRUE;
		}
	case WM_PAINT:
		PaintTheBlock(hCtrlBlock, RGB(iRed, iGreen, iBlue),iWidth);
		break;
	}
	return FALSE;
}


LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static int x;
	static int y;
	//static BOOL bNowDraw=FALSE;
	HPEN hPen;
	TCHAR str[128];
	switch (iMessage) {
	case WM_CREATE:
	
	/*	for (int i = 0; i<4; i++) {
			SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)Items[i]);
		}
		SendMessage(hCombo, CB_SETCURSEL, (iCurrentPenWidth/3)-1, 0);
*/	
	return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDM_DIALOG:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), 
				hWnd, DlgProc);
			break;

			/*
		case ID_COMBOBOX:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				iCurrentPenWidth = 
					(SendMessage(hCombo, CB_GETCURSEL, 0, 0) + 1) * 3;
				SetFocus(hWnd);
				break;
			case CBN_EDITCHANGE:
				GetWindowText(hCombo, str, 128);
				iCurrentPenWidth = _ttoi(str); //Neutral
				//iCurrentPenWidth = atoi(str);  //C함수-MBCS
				//iCurrentPenWidth = _wtoi(str); //C함수-UNICODE
				//SetFocus(hWnd);  //두글자 이상 입력이 안됨
				break;
			}
			*/
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
		SetFocus(hWnd);
		x=LOWORD(lParam);
		y=HIWORD(lParam);
		 
		lines[iLines].pColor =iCurrentPenColor ;
		lines[iLines].pWidth =iCurrentPenWidth;

		lines[iLines].p[lines[iLines].iCount].x = x;
		lines[iLines].p[lines[iLines].iCount].y = y;
		lines[iLines].iCount++;  ///
		return 0;
	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON) {
			hdc=GetDC(hWnd);
			hPen = CreatePen(PS_SOLID, iCurrentPenWidth, iCurrentPenColor);
			
			hPen=(HPEN)SelectObject(hdc, hPen);
			
			MoveToEx(hdc,x,y,NULL);
			x=LOWORD(lParam);
			y=HIWORD(lParam);
			lines[iLines].p[lines[iLines].iCount].x = x;
			lines[iLines].p[lines[iLines].iCount].y = y;
			lines[iLines].iCount++;
			LineTo(hdc,x,y);

			DeleteObject(SelectObject(hdc, hPen));

			ReleaseDC(hWnd,hdc);
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
			hPen = CreatePen(PS_SOLID, lines[i].pWidth, lines[i].pColor);
			hPen = (HPEN)SelectObject(hdc, hPen);  //선택한다.
			MoveToEx(hdc, lines[i].p[0].x, 
					lines[i].p[0].y, NULL);
			for (int j = 0; j < lines[i].iCount-1; j++)
			{
				
				LineTo(hdc, lines[i].p[j+1].x,
					lines[i].p[j+1].y);
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
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

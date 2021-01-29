#include <windows.h>

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass=TEXT("ReadDIB");

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance
	  ,LPSTR lpszCmdParam,int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;

	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=NULL;
	WndClass.style=CS_HREDRAW | CS_VREDRAW;
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

BITMAPFILEHEADER *fh=NULL;
BITMAPINFOHEADER *ih;
int bx,by;
BYTE *pRaster;
void LoadDIB(LPCTSTR Path)
{
	HANDLE hFile;
	DWORD FileSize, dwRead;

	hFile=CreateFile(Path,GENERIC_READ,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE) {
		return;
	}

	if (fh) {
		free(fh);
		fh=NULL;
	}
	FileSize=GetFileSize(hFile,NULL);//파일을 쫙 읽어들인다
	fh=(BITMAPFILEHEADER *)malloc(FileSize);
	ReadFile(hFile,fh,FileSize,&dwRead,NULL);
	CloseHandle(hFile);

	pRaster=(PBYTE)fh+fh->bfOffBits;//할당받은 메모리 fh //pRaster는 실제 데이터의 시작 주소 
	ih=(BITMAPINFOHEADER *)((PBYTE)fh+sizeof(BITMAPFILEHEADER));
	bx=ih->biWidth;//bx는 가로길이
	by=ih->biHeight;//by는 세로길이
	//pRaster,px,py 는 그림을 읽어들일 때 필요한 전역변수
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	OPENFILENAME OFN;
	TCHAR lpstrFile[MAX_PATH]="";

	switch (iMessage) {
	case WM_LBUTTONDOWN:
		memset(&OFN, 0, sizeof(OPENFILENAME));
		OFN.lStructSize = sizeof(OPENFILENAME);
		OFN.hwndOwner=hWnd;
		OFN.lpstrFilter="Bitmap File(*.bmp)\0*.bmp\0";
		OFN.lpstrFile=lpstrFile;
		OFN.nMaxFile=MAX_PATH;
		if (GetOpenFileName(&OFN)!=0) {
			LoadDIB(lpstrFile);//파일을 윈도우에 그림, 하드디스크에 있는 파일을 메인메모리로 올린다
			InvalidateRect(hWnd, NULL, TRUE);
		}
		return 0;
	case WM_PAINT:
		hdc=BeginPaint(hWnd, &ps);
		if (fh) {
			SetDIBitsToDevice(hdc,0,0,bx,by,0,0,0,by, //그림의 폭과 넓이 시작위치//SetDIBitsToDevice가 올리는 주요 함수
				pRaster,(BITMAPINFO *)ih,DIB_RGB_COLORS);
			//StretchDIBits(hdc,0,0,bx*2,by*2,0,0,bx,by,pRaster,
			//	(BITMAPINFO *)ih,DIB_RGB_COLORS,SRCCOPY);
		}
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		if (fh)
			free(fh);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}


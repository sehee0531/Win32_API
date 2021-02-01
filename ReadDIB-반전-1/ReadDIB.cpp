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

//...\Study-Lang\WINAPI\ApiPrj2003\[ skk ]\ch29-비트맵 추가\ReadDIB-반전
//...\Study-Lang\WINAPI\ApiPrj2003\[ skk ]\ch29-비트맵 추가\ReadDIB-GrayLevel

BITMAPFILEHEADER *fh=NULL;
BITMAPINFOHEADER *ih;
int bx,by, bitCount;
BYTE *pRaster;
HANDLE hFile ;
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
	FileSize=GetFileSize(hFile,NULL);
	fh=(BITMAPFILEHEADER *)malloc(FileSize);
	ReadFile(hFile,fh,FileSize,&dwRead,NULL);
	CloseHandle(hFile);

	pRaster=(PBYTE)fh+fh->bfOffBits;
	ih=(BITMAPINFOHEADER *)((PBYTE)fh+sizeof(BITMAPFILEHEADER));
	bx=ih->biWidth;
	by=ih->biHeight;
	bitCount = ih->biBitCount;
}

void InvertGrey()
{
	BYTE *pBitmap;
	int cx, cy, i, j;
	pBitmap = (BYTE *)pRaster;
	cx = bx; cy = by;

	for (i = 0; i < cy; i++){
		for (j = 0; j < cx; j++)
		{
			*(pBitmap + j + i*cx) = ~*(pBitmap + j + i*cx);
		}
	}

}
void InvertColor()
{
	BYTE *pBitmap;
	int cx, cy, i, j;
	pBitmap = (BYTE *)pRaster;
	cx = bx; cy = by;

	//Blue Invert
	for (i = 0; i < cy; i++){
		for (j = 0; j < (3 * cx); j += 3)
		{

			*(pBitmap + j + i*(3 * cx)) = ~*(pBitmap + j + i*(3 * cx));
		}
	}
	//Green Invert
	for (i = 0; i < cy; i++){
		for (j = 1; j < (3 * cx); j += 3)
		{

			*(pBitmap + j + i*(3 * cx)) = ~*(pBitmap + j + i*(3 * cx));
		}
	}

	//Red Invert
	for (i = 0; i < cy; i++){
		for (j = 2; j < (3 * cx); j += 3)
		{

			*(pBitmap + j + i*(3 * cx)) = ~*(pBitmap + j + i*(3 * cx));
		}
	}
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
			LoadDIB(lpstrFile);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		return 0;
	
	case WM_RBUTTONDOWN:
		if (fh == NULL) {
			return 0;
		}
		
		switch (bitCount) {
		case 8:
			InvertGrey();
			break;
		case 24:
			InvertColor();
			break;
		}
		
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	case WM_PAINT:
		hdc=BeginPaint(hWnd, &ps);
		if (fh) {
			SetDIBitsToDevice(hdc,0,0,bx,by,0,0,0,by,
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


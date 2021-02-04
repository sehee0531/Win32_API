#include <windows.h>

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass=TEXT("BackThread");

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
	hWndMain=hWnd;
	
	while (GetMessage(&Message,NULL,0,0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

int Count;
int arData[100];
int Num;
int FileNum;

int GetAddNum()
{
	/*__declspec(thread) */static int m=0;

	if (m == 5000) {
		m=0;
	}
	m+=1000;
	return m;
}

void MakeAddNum(int *m)
{
	if (*m == 5000) {
		*m=0;
	}
	*m+=1000;
}

DWORD WINAPI CalcThread(LPVOID temp)
{
	int *arWork;
	arWork=(int *)temp;
	int i;
	HANDLE hFile;
	DWORD dwWritten;
	TCHAR Path[MAX_PATH],Result[100];
	int m=0;

	// arWork를 가공한다. 예를 들어 통계, 정렬 등등
	for (i=0;i<100;i++) {
		arWork[i]*=2;
		arWork[i]+=GetAddNum();
//		MakeAddNum(&m);
//		arWork[i]+=m;
		Sleep(50);
	}

	// arWork를 파일이나 DB에 저장한다. 또는 네트워크로 전송한다.
	wsprintf(Path,"Result%d.txt",++FileNum);
	hFile=CreateFile(Path,GENERIC_WRITE,0,NULL,
		CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	for (i=0;i<100;i++) {
		wsprintf(Result,"[%d]=%d\r\n",i,arWork[i]);
		WriteFile(hFile,Result,lstrlen(Result),&dwWritten,NULL);
	}
	CloseHandle(hFile);
	MessageBeep(0);
	free(arWork);

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static TCHAR Mes[256];
	DWORD ThreadID;
	HANDLE hThread;
	int *arCopy;

	switch (iMessage) {
	case WM_CREATE:
		Num=0;
//		SetTimer(hWnd,1,20,NULL);
		SetTimer(hWnd,1,100,NULL);
		return 0;
	// 네트워크나 사용자에 의해 데이터가 끊임없이 입력된다.
	case WM_TIMER:
		arData[Num++]=++Count;
		if (Num == 100) {
			arCopy=(int *)malloc(sizeof(arData));
			memcpy(arCopy,arData,sizeof(arData));
			hThread=CreateThread(NULL,0,CalcThread,arCopy,0,&ThreadID);
			CloseHandle(hThread);
			wsprintf(Mes,"데이터 100개 입력 완료. 스레드 호출");
			InvalidateRect(hWnd,NULL,TRUE);
			UpdateWindow(hWnd);
			Sleep(1000);
			Num=0;
		} else {
			InvalidateRect(hWnd,NULL,TRUE);
			wsprintf(Mes,"%d번째 데이터(%d) 입력중",Num,Count);
		}
		return 0;
	case WM_PAINT:
		hdc=BeginPaint(hWnd, &ps);
		TextOut(hdc,10,10,Mes,lstrlen(Mes));
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		KillTimer(hWnd,1);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}


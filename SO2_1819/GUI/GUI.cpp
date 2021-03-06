// GUI.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "GUI.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

GameInfo masterGameInfo;
User masterUser;

DWORD GameThreadID;
HANDLE GameThreadHandle;
HWND globalhWnd;

TCHAR displayData[100] = TEXT("");


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Login(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    HighScore(HWND, UINT, WPARAM, LPARAM);

void setGame(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	if (!OpenSharedMemory()) {
		_tprintf(TEXT("shit went wrong opening shared memory\n"));
	}
	else {
		_tprintf(TEXT("Everything is ok mate\n"));
	}

	if (!OpenSync()) {
		_tprintf(TEXT("shit went wrong opening semaphores\n"));
	}
	else {
		_tprintf(TEXT("Green light mate\n"));
	}

	masterGameInfo.isGame = FALSE;

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GUI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GUI));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GUI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+5);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GUI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, RIGHT_LIMIT+20, LOWER_LIMIT+100, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   globalhWnd = hWnd;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	static HBRUSH bg = NULL;
	static int nx = 0, ny = 0;

	static HDC hdc = NULL;
	static HDC auxDC = NULL;
	static HBITMAP auxBM = NULL;

	//Tijolo NORMAL
	static HBITMAP hBlock = NULL;
	static BITMAP bmBlock;
	static HDC hdcBlock;

	//RESISTENTE
	static HBITMAP hBlockR1 = NULL;
	static BITMAP bmBlockR1;
	static HDC hdcBlockR1;

	//RESISTENTE
	static HBITMAP hBlockR2 = NULL;
	static BITMAP bmBlockR2;
	static HDC hdcBlockR2;

	//RESISTENTE
	static HBITMAP hBlockR3 = NULL;
	static BITMAP bmBlockR3;
	static HDC hdcBlockR3;

	//RESISTENTE
	static HBITMAP hBlockR4 = NULL;
	static BITMAP bmBlockR4;
	static HDC hdcBlockR4;


	//MAGICO
	static HBITMAP hBlockM = NULL;
	static BITMAP bmTijoloM;
	static HDC hdcBlockM;


	//Barreira
	static HBITMAP hBarrier = NULL;
	static BITMAP bmBarreira;
	static HDC hdcBarrier;

	//Brindes
	//SPEED-UP
	static HBITMAP hSpeed;
	static BITMAP bmSpup;
	static HDC hdcSpeed;

	//SLOW-DOWN 
	static HBITMAP hSlow;
	static BITMAP bmSldwn;
	static HDC hdcSlow;

	//Vida-Extra
	static HBITMAP hExtra;
	static BITMAP bmVextra;
	static HDC hdcExtra;

	//Triple
	static HBITMAP hTriple;
	static BITMAP bmTriple;
	static HDC hdcTriple;

	switch (message)
	{
	case WM_CREATE:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_LOGIN), hWnd, Login);

		//Obter as dimensões do Ecra
		bg = CreateSolidBrush(RGB(153, 153, 0));
		nx = GetSystemMetrics(SM_CYSCREEN);
		ny = GetSystemMetrics(SM_CYSCREEN);


		// Preparação de 'BITMAP'
		hdc = GetDC(hWnd);
		auxDC = CreateCompatibleDC(hdc);
		auxBM = CreateCompatibleBitmap(hdc, nx, ny);
		SelectObject(auxDC, auxBM);
		SelectObject(auxDC, bg);
		PatBlt(auxDC, 0, 0, nx, ny, PATCOPY);
		ReleaseDC(hWnd, hdc);
		// Carregar "BITMAP's"
		hBlock = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BLOCK), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hBlockR1 = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BLOCK_R4), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hBlockR2 = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BLOCK_R3), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hBlockR3 = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BLOCK_R2), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hBlockR4 = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BLOCK_R1), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hBlockM = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BLOCK_M), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hBarrier = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BARRIER), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hSpeed = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_PRIZE_SPEED), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hSlow = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_PRIZE_SLOW), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hExtra = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_PRIZE_EXTRA), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hTriple = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_PRIZE_TRIPLE), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);

		hdc = GetDC(hWnd);

		// GETOBJECT ** Returns a reference to an object provided by an ActiveX component.
		GetObject(hBlock, sizeof(bmBlock), &bmBlock);
		GetObject(hBlockR1, sizeof(bmBlockR1), &bmBlockR1);
		GetObject(hBlockR2, sizeof(bmBlockR2), &bmBlockR2);
		GetObject(hBlockR3, sizeof(bmBlockR3), &bmBlockR3);
		GetObject(hBlockR4, sizeof(bmBlockR4), &bmBlockR4);
		GetObject(hBlockM, sizeof(bmTijoloM), &bmTijoloM);
		GetObject(hBarrier, sizeof(bmBarreira), &bmBarreira);
		GetObject(hSpeed, sizeof(bmSpup), &bmSpup);
		GetObject(hSlow, sizeof(bmSldwn), &bmSldwn);
		GetObject(hExtra, sizeof(bmVextra), &bmVextra);
		GetObject(hTriple, sizeof(bmTriple), &bmTriple);

		// The CreateCompatibleDC function creates a memory device context (DC) compatible with the specified device.
		hdcBlock = CreateCompatibleDC(hdc);
		hdcBlockR1 = CreateCompatibleDC(hdc);
		hdcBlockR2 = CreateCompatibleDC(hdc);
		hdcBlockR3 = CreateCompatibleDC(hdc);
		hdcBlockR4 = CreateCompatibleDC(hdc);
		hdcBlockM = CreateCompatibleDC(hdc);
		hdcBarrier = CreateCompatibleDC(hdc);
		hdcSpeed = CreateCompatibleDC(hdc);
		hdcSlow = CreateCompatibleDC(hdc);
		hdcExtra = CreateCompatibleDC(hdc);
		hdcTriple = CreateCompatibleDC(hdc);


		// The SelectObject function selects an object into the specified device context (DC). The new object replaces the previous object of the same type.
		SelectObject(hdcBlock, hBlock);
		SelectObject(hdcBlockR1, hBlockR1);
		SelectObject(hdcBlockR2, hBlockR2);
		SelectObject(hdcBlockR3, hBlockR3);
		SelectObject(hdcBlockR4, hBlockR4);
		SelectObject(hdcBlockM, hBlockM);
		SelectObject(hdcBarrier, hBarrier);
		SelectObject(hdcSpeed, hSpeed);
		SelectObject(hdcSlow, hSlow);
		SelectObject(hdcExtra, hExtra);
		SelectObject(hdcTriple, hTriple);

		ReleaseDC(hWnd, hdc);

		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
			/*case ID_FILE_LOGIN:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_LOGIN), hWnd, Login);
			break;*/
		case ID_FILE_STARTGAME:
			setGame(hWnd);
			break;
		case ID_FILE_HIGHSCORE:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_HIGHSCORE), hWnd, HighScore);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_KEYDOWN:
		//Lidar com as teclas primidas
		Info info;
		ZeroMemory(&info, sizeof(info));
		info.user = masterUser;
		switch (wParam) {
		case VK_LEFT:
			if (masterGameInfo.isGame) {
				_tcscpy_s(info.comand, sizeof(TCHAR[25]), TEXT("move_left"));
				WriteMemory(info);
			}
			break;
		case VK_RIGHT:
			if (masterGameInfo.isGame) {
				_tcscpy_s(info.comand, sizeof(TCHAR[25]), TEXT("move_right"));
				WriteMemory(info);
			}
			break;
		case 0x1B: //Process an escape
				   //Tecnicamente carregando escape o jogador baza :O
			break;
		default:
			break;
		}
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		// **The PatBlt function** paints the specified rectangle using the brush that is currently 
		//selected into the specified device context. The brush color and the surface color 
		//or colors are combined by using the specified raster operation.
		PatBlt(auxDC, 0, 0, nx, ny, BLACKNESS);


		//The SetStretchBltMode function sets the bitmap stretching mode in the specified device context.
		//BLACKONWHITE Perserva os valores pretos acima dos brancos (Pixeis)
		SetStretchBltMode(auxDC, BLACKONWHITE);
		if (masterGameInfo.isGame) {
			//Imprimir block enum Tipo_Tijolo{normal, resistente, magico};
			for (int i = 0; i < MAX_BLOCKS; i++) {
				if (masterGameInfo.block[i].life != 0) {

					switch (masterGameInfo.block[i].tipo) {
					case normal:
						StretchBlt(auxDC, masterGameInfo.block[i].coord.X, masterGameInfo.block[i].coord.Y, BLOCK_WIDTH, BLOCK_HEIGHT, hdcBlock, 0, 0, bmBlock.bmWidth, bmBlock.bmHeight, SRCCOPY);
						break;
					case resistente:
						switch (masterGameInfo.block[i].life) {
						case 1:
							StretchBlt(auxDC, masterGameInfo.block[i].coord.X, masterGameInfo.block[i].coord.Y, BLOCK_WIDTH, BLOCK_HEIGHT, hdcBlockR2, 0, 0, bmBlockR2.bmWidth, bmBlockR2.bmHeight, SRCCOPY);
							break;
						case 2:
							StretchBlt(auxDC, masterGameInfo.block[i].coord.X, masterGameInfo.block[i].coord.Y, BLOCK_WIDTH, BLOCK_HEIGHT, hdcBlockR3, 0, 0, bmBlockR3.bmWidth, bmBlockR3.bmHeight, SRCCOPY);
							break;
						case 3:
							StretchBlt(auxDC, masterGameInfo.block[i].coord.X, masterGameInfo.block[i].coord.Y, BLOCK_WIDTH, BLOCK_HEIGHT, hdcBlockR4, 0, 0, bmBlockR4.bmWidth, bmBlockR4.bmHeight, SRCCOPY);
							break;
						case 4:
							StretchBlt(auxDC, masterGameInfo.block[i].coord.X, masterGameInfo.block[i].coord.Y, BLOCK_WIDTH, BLOCK_HEIGHT, hdcBlockR1, 0, 0, bmBlockR1.bmWidth, bmBlockR1.bmHeight, SRCCOPY);
							break;
						}
						break;
					case magico:
						StretchBlt(auxDC, masterGameInfo.block[i].coord.X, masterGameInfo.block[i].coord.Y, BLOCK_WIDTH, BLOCK_HEIGHT, hdcBlockM, 0, 0, bmTijoloM.bmWidth, bmTijoloM.bmHeight, SRCCOPY);
						break;

					}
				}
				//StretchBlt(auxDC, masterGameInfo.bola.coord.X + 10 , masterGameInfo.bola.coord.Y + 10, BLOCK_HEIGHT, BLOCK_HEIGHT, hdcBlock, 0, 0, bmTijolo.bmWidth, bmTijolo.bmHeight, SRCCOPY);
			}

			//Barreira
			for (int i = 0; i < MAX_USERS; i++) {
				if (masterGameInfo.barrier[i].id != -1) {
					StretchBlt(auxDC, masterGameInfo.barrier[i].coord.X, masterGameInfo.barrier[i].coord.Y, masterGameInfo.barrier[i].size, BARRIER_HEIGHT, hdcBarrier, 0, 0, bmBarreira.bmWidth, bmBarreira.bmHeight, SRCCOPY);
				}
			}

			//prize enum Tipo_Brinde{speed_up, slow_down, vida_extra, triple, barreira}; //Adicionar outros prize consoante a originalidade

			for (int i = 0; i < MAX_PRIZES; i++) {
				if (masterGameInfo.prize[i].isActive) {
					switch (masterGameInfo.prize[i].tipo)
					{
					case speed_up:
						StretchBlt(auxDC, masterGameInfo.prize[i].coord.X, masterGameInfo.prize[i].coord.Y, masterGameInfo.prize[i].size, PRIZE_HEIGHT, hdcSpeed, 0, 0, bmSpup.bmWidth, bmSpup.bmHeight, SRCCOPY);
						break;
					case slow_down:
						StretchBlt(auxDC, masterGameInfo.prize[i].coord.X, masterGameInfo.prize[i].coord.Y, masterGameInfo.prize[i].size, PRIZE_HEIGHT, hdcSlow, 0, 0, bmSldwn.bmWidth, bmSldwn.bmHeight, SRCCOPY);
						break;
					case extra_life:
						StretchBlt(auxDC, masterGameInfo.prize[i].coord.X, masterGameInfo.prize[i].coord.Y, masterGameInfo.prize[i].size, PRIZE_HEIGHT, hdcExtra, 0, 0, bmVextra.bmWidth, bmVextra.bmHeight, SRCCOPY);
						break;
					case triple:
						StretchBlt(auxDC, masterGameInfo.prize[i].coord.X, masterGameInfo.prize[i].coord.Y, masterGameInfo.prize[i].size, PRIZE_HEIGHT, hdcTriple, 0, 0, bmTriple.bmWidth, bmTriple.bmHeight, SRCCOPY);
						break;
					default:
						//IMPRIMIR UM NORMAL!
						break;
					}

				}
			}


			//BOLA
			for (int i = 0; i < MAX_BALLS; i++) {
				if (masterGameInfo.ball[i].isBall)
					Ellipse(auxDC, masterGameInfo.ball[i].coord.X, masterGameInfo.ball[i].coord.Y, masterGameInfo.ball[i].coord.X + masterGameInfo.ball[i].radius, masterGameInfo.ball[i].coord.Y + masterGameInfo.ball[i].radius);
			}
			
			for (int i = 0; i < MAX_USERS; i++)
			{
				if (masterGameInfo.info.high.users[i].id == masterUser.id)
				{
					swprintf_s(displayData, TEXT("User Name : %s, Lifes : %d, Score : %d\n"), masterUser.username, masterGameInfo.info.high.users[i].lifes, masterGameInfo.info.high.users[i].score);
				}
			}
			TextOut(auxDC, LEFT_LIMIT + 10, LOWER_LIMIT + 20, displayData, _tcslen(displayData));

			BitBlt(hdc, 0, 0, nx, ny, auxDC, 0, 0, SRCCOPY);

		}
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Login(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{

			Info info;
			GameInfo gameInfo;

			HWND edit;
			TCHAR buff[1024];

			ZeroMemory(&info, sizeof(info));
			ZeroMemory(&gameInfo, sizeof(gameInfo));
			edit = GetDlgItem(hDlg, IDC_EDIT1);
			GetWindowText(edit, buff, 1024);
			_tcscpy_s(info.user.username, sizeof(TCHAR[25]), buff);
			_tcscpy_s(info.comand, sizeof(TCHAR[25]), TEXT("login"));
			WriteMemory(info);
			ReadGameMemory(&gameInfo);
			//if (_tcsncmp(gameInfo.info.comand, TEXT("loginConfirmed"), 14) == 0) {
				masterUser = gameInfo.info.user;
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			//}
			
		}
		else if (LOWORD(wParam) == IDCANCEL){
			DestroyWindow(hDlg);
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK HighScore(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	Info info;
	GameInfo gameInfo;

	TCHAR str[100] = TEXT("");
	HWND hWndList;

	switch (message)
	{
	case WM_INITDIALOG:

		hWndList = GetDlgItem(hDlg, IDC_LIST1);

		ZeroMemory(&info, sizeof(info));
		ZeroMemory(&gameInfo, sizeof(gameInfo));

		_tcscpy_s(info.comand, sizeof(TCHAR[25]), TEXT("highScore"));

		WriteMemory(info);
		ReadGameMemory(&gameInfo);
		if (_tcsncmp(gameInfo.info.comand, TEXT("highScoreConfirmed"), 18) == 0) {
			for (int i = 0; i < 10; i++) {
				swprintf_s(str, TEXT("%2.d º - Nome: %s Score: %d"), i + 1, gameInfo.info.high.users[i].username, gameInfo.info.high.users[i].score);
				SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)str);
			}
		}

		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

DWORD WINAPI GameThread(LPVOID param) {
	do {
		ReadGameMemory(&masterGameInfo);
		InvalidateRect(globalhWnd, NULL, FALSE);
	} while (masterGameInfo.isGame);
	return 0;
}

void setGame(HWND hWnd) {

	Info info;
	GameInfo gameInfo;

	ZeroMemory(&info, sizeof(Info));
	ZeroMemory(&gameInfo, sizeof(gameInfo));

	info.user = masterUser;
	_tcscpy_s(info.comand, sizeof(TCHAR[25]), TEXT("start"));
	WriteMemory(info);
	ReadGameMemory(&gameInfo);
	if (_tcsncmp(gameInfo.info.comand, TEXT("startConfirmed"), 14) == 0) {
		GameThreadHandle = CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			GameThread,       // thread function name
			NULL,          // argument to thread function 
			0,                      // use default creation flags 
			&GameThreadID);

	}
	else {
		DialogBox(hInst, MAKEINTRESOURCE(IDD_START_FAILED), hWnd, About);
	}
}
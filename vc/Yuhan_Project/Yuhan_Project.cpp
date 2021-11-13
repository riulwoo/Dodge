// Yuhan_Project.cpp : 애플리케이션에 대한 진입점을 정의합니다.
// 유한대학교 컴퓨터소프트웨어공학과 2학년 1반 VC++ 텀프로젝트
// 주제 : 닷지(Dodge)
// 화면 : 처음 화면(게임시작) 게임 화면 (날아오는적 피하기) 게임오버(다시시작, 종료)
// 게임 설명 : 마우스로 날아오는 적들을 피해서 점수를 얻는 방식
// 
// Z키 : 폭탄 ( 3 개 ) 마우스와 같은 좌표를 가지다가 Z키 누르면 장막의 크기가 커졌다 작아짐
//       폭탄과 적이 부딪히면 적은 사라진다.
// X키 : 치트 X키 누르면 플레이어 주변에 동그란 원이 생김 사라지지않고 폭탄과 같은 기능을 가짐
// 
// 난이도 조절
// 일정한 시간이 지나면 랜덤한 위치에 도달해야 하는 포인트가 생긴다.
// 제한 시간내에 포인트를 찍지 못하면 패배

#include "framework.h"
#include "Yuhan_Project.h"

#include <stdlib.h>     // 랜덤 값 획득 함수 호출
#include <time.h>       // SEED 값 조절을 위해 현재 시간 값 이용

#define MAX_LOADSTRING  100
#define IDM_BTN_START   123

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_YUHANPROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_YUHANPROJECT));

    MSG msg;

    // 기본 메시지 루프입니다:
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
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_YUHANPROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_YUHANPROJECT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

// 윈도우 크기 변수
int Canvas_X = 1440;
int Canvas_Y = 800;

// 버튼을 위한 윈도우 핸들 선언
HWND b_start;           // 게임을 시작하기 위한 버튼

HWND g_hWnd;

// 폭탄
int bomb = 3;

// 생존 시간
int atime = 0;

// 플레이어와 폭탄의 좌표 선언
RECT g_me, g_bomb;
RECT g_enemy; // 테스트

// 마우스의 이전 좌표를 받아 오기 위한 변수
int g_x, g_y;

// b_start가 생성된 상태인지 체크
bool b_flag = true; 

// 게임오버인지 체크
bool b_gmover = false;

WCHAR cheat = 'X';
//타이머 아이디를 선언
#define TIMER_ID_1          1   // 생존 시간을 위한 타이머 아이디

// decidewall 0 = top 1 = right 2 = bottom 3 = left

HANDLE g_mux;
int dstX, dstY;
DWORD WINAPI enemyspawn(LPVOID Param)
{
    // SEED 값 초기화
    srand(time(NULL));
    int decidewall = rand() % 4;
    int x, y;

    WaitForSingleObject(g_mux, INFINITE);
        switch(decidewall)
        {
            case 0:
                // 적이 위에서 생성되어 아래로 이동
                x = 0;
                y = rand() % Canvas_X;
                
                // 생성된 적의 경로 설정
                dstX = rand() % Canvas_X;
                dstY = Canvas_Y;
                break;
            case 1:
                // 적이 오른쪽에서 생성되어 왼쪽으로 이동
                x = Canvas_X;
                y = rand() % Canvas_Y;
                
                // 생성된 적의 경로 설정
                dstX = 0;
                dstY = rand()%Canvas_Y;
                break;
            case 2:
                // 적이 아래에서 생성되어 위로 이동
                x = rand() % Canvas_X;
                y = Canvas_Y;

                // 생성된 적의 경로 설정
                dstX = rand() % Canvas_X;
                dstY = 0;
                break;
            case 3:
                // 적이 왼쪽에서 생성되어 오른쪽으로 이동
                x = 0;
                y = rand() % Canvas_Y;
                
                // 생성된 적의 경로 설정
                dstX = Canvas_X;
                dstY = rand()%Canvas_Y;
                break;
        }
        g_enemy.left = x;
        g_enemy.top = y;
        g_enemy.right = g_enemy.left + 15;
        g_enemy.bottom = g_enemy.top + 15;

        if (g_enemy.left < dstX)
        {
            g_enemy.left += 5;
            g_enemy.right += 5;
        }
        else
        {
            g_enemy.left -= 5;
            g_enemy.right -= 5;
        }
        if (g_enemy.top < dstY)
        {
            g_enemy.top -= 5;
            g_enemy.bottom -= 5;
        }
        else
        {
            g_enemy.top += 5;
            g_enemy.bottom += 5;
        }
        WCHAR Bufferw[] = { 0, };
        wsprintfW(Bufferw, L"%d %d", dstX, dstY);
        MessageBox((HWND)Param, Bufferw, Bufferw, 0);
        ReleaseMutex(g_mux);
        ExitThread(0);
        return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_TIMER:
        switch (wParam)
        {
            case TIMER_ID_1:
                atime++;
                if(atime % 3 == 0) // 3초마다 enemyspawn 실행
                {
                    // SEED 값 초기화
                    srand(time(NULL));
                        CreateThread(NULL, 0, enemyspawn, (LPVOID)hWnd, 0, NULL);
                }
                if (b_gmover == true)
                {
                    KillTimer(hWnd, TIMER_ID_1);

                }
                break;
        }
        break;
    case WM_KEYDOWN:
        switch (wParam)
        {
            case 0x5A: // Z키 입력시 g_bomb의 크기가 일시적으로 커졌다 원래 크기로 돌아감
                if(bomb > 0)
                {
                    bomb--;

                }
                break;
            case 0x58: // X키 입력시 g_bomb이 활성화 / invisible 상태
                if (cheat == 'X')
                    cheat = 'O';
                else
                    cheat = 'X';
                break;
        }

    case WM_MOUSEMOVE:
    {
        RECT dst;
        // START 버튼이 없다면 플레이어의 좌표를 마우스 좌표로 설정
        if (b_flag == false)
        {
            ShowCursor(false); // 시작시 커서를 숨김
            int x, y;
            x = LOWORD(lParam);
            y = HIWORD(lParam);

            g_me.left = x - 10;
            g_me.top = y - 10;
            g_me.right = x + 10;
            g_me.bottom = y + 10;
            if (TRUE == IntersectRect(&dst, &g_me, &g_enemy))     // 좌표 겹침이 존재한다면
            {
                b_gmover = true;
                ShowCursor(true);
            }
            InvalidateRect(hWnd, NULL, TRUE);
        }
        
    }
        break;
    
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_BTN_START:
                DestroyWindow(b_start);
                b_flag = false;
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

    case WM_CREATE:
        g_mux = CreateMutex(NULL, FALSE, NULL);
        

        // 초기 화면값 설정
        MoveWindow(hWnd, 100, 100, Canvas_X, Canvas_Y, true);
        // 게임 플레이어의 초기 좌표
        g_me.left = -1000;
        g_me.top  = -1000;
        g_me.right = g_me.left + 20;
        g_me.bottom = g_me.top + 20;

        g_enemy.left = 700;
        g_enemy.top = 700;
        g_enemy.right = g_enemy.left + 20;
        g_enemy.bottom = g_enemy.top + 20;
        
        // 게임시작 버튼
            b_start = CreateWindow(L"button", L"START", WS_CHILD | WS_VISIBLE, 600, 300, 200, 60, hWnd, (HMENU)IDM_BTN_START, hInst, NULL);

            SetTimer(hWnd, TIMER_ID_1, 1000, NULL);
        break;
    case WM_PAINT:
        {
            HFONT hFont, OldFont;
            PAINTSTRUCT ps;
            WCHAR string[32] = { 0, };
            HDC hdc = BeginPaint(hWnd, &ps);
            hFont = CreateFont(50, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("궁서"));
            OldFont = (HFONT)SelectObject(hdc, hFont);
            if (b_flag == true)
            {
                TextOut(hdc, 615, 200, L"DODGE", lstrlenW(L"DODGE"));
            }
            if (b_gmover == true)
            {
                ShowCursor(true);
                TextOut(hdc, 550, 200, L"GAME OVER", lstrlenW(L"GAME OVER"));
                b_start = CreateWindow(L"button", L"START", WS_CHILD | WS_VISIBLE, 600, 400, 200, 60, hWnd, (HMENU)IDM_BTN_START, hInst, NULL);
            }
            hFont = CreateFont(14, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("HY나무M"));
            OldFont = (HFONT)SelectObject(hdc, hFont);
            if(b_gmover == true)
                wsprintfW(string, L"생존시간 %d : %d", atime / 60, atime % 60);
                TextOut(hdc, 650, 300, string, lstrlenW(string));
            if (b_flag == true)
            {
                TextOut(hdc, 615, 700, L"날아오는 운석을 피해라..!", lstrlenW(L"날아오는 운석을 피해라..!"));
            }
            if (b_flag == false) {
                if(b_gmover == false){
                wsprintfW(string, L"생존 시간 %d : %d", atime/60, atime%60);
                TextOut(hdc, 10, 10, string, lstrlenW(string));
                wsprintfW(string, L"폭탄(Z) : %d", bomb);
                TextOut(hdc, 10, 700, string, lstrlenW(string));
                wsprintfW(string, L"치트(?) : %lc", cheat);
                TextOut(hdc, 1300, 700, string, lstrlenW(string));
                Ellipse(hdc, g_enemy.left, g_enemy.top, g_enemy.right, g_enemy.bottom);
                }
            }
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            // 플레이어의 생성
            Rectangle(hdc, g_me.left, g_me.top, g_me.right, g_me.bottom);
            SelectObject(hdc, OldFont);
            DeleteObject(hFont);
            
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        CloseHandle(g_mux);
        DestroyWindow(b_start);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return true;
}

// 정보 대화 상자의 메시지 처리기입니다.
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

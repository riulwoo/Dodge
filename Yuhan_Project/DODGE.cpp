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

WCHAR qqqq[128] = L"DODGE";
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

   HWND hWnd = CreateWindowW(szWindowClass, qqqq , WS_OVERLAPPEDWINDOW,
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

RECT canvas;

// 버튼을 위한 윈도우 핸들 선언
HWND b_start;           // 게임을 시작하기 위한 버튼

HWND g_hWnd;

// 폭탄
int bomb = 3;                       // 개수
bool bombst = false;                // 현재 상황이 폭탄인지 체크
int btime = 5;                      // 폭탄일때 제한시간

// 생존 시간
int atime = 0;

// 난이도 증가시 필요한 웨이포인트 시간
int needtime = 6;

// 타이머 스피드 조절 변수
int timesp1 = 1000;
int timesp2 = 35;

// 점수
int score = 0;                      // 점수
int enemynum;                       // 적의 랜덤 생성 수 ( 사용 x )

// RECT
RECT g_me;                          // 나
RECT g_enemy[3][50] = { 0, };       // 적 (150개)
RECT g_point;                       // 웨이포인트
RECT g_item;                        // 아이템

RECT RE, EX;                        // 게임종료시 RESTART / EXIT 버튼용 RECT

RECT p;                             // 처음화면에 DrawFocusRect를 그리기 위한 RECT

// 마우스의 이전 좌표를 받아 오기 위한 변수
int g_x, g_y;

// b_start가 생성된 상태인지 체크
bool b_flag = true; 

// 게임오버인지 체크
bool b_gmover = false;

// 레벨업
bool levelup = false;

// 초기 치트값
WCHAR cheat = 'X';

//타이머 아이디를 선언
#define TIMER_ID_1          1   // 생존 시간을 위한 타이머 아이디
#define TIMER_ID_2          2   // 폭탄이 수행되는 시간
#define TIMER_ID_ENEMYMOVE  3   // 적의 이동하는 속도
#define TIMER_ID_PTTM       4   // 웨이 포인트
// decidewall 0 = top 1 = right 2 = bottom 3 = left

HANDLE g_mux;                   // 어디서 본건 있어서 써보려고 만듬 ( 사용 x )

// 적의 생성위치
int x[50] = { 0 };
int y[50] = { 0 };

// 적의 이동좌표
int dstX[50] = { 0 }, dstY[50] = { 0 };

// IntersectRect 할때 지역변수로 선언하는거 구찮아서 쓰는 변수입니다..
RECT dst;

DWORD WINAPI enemyspawn(LPVOID Param)
{
    srand((unsigned int)time(NULL));
    int decidewall;

    for(int t =0 ; t < 100 ; t++)
    {
        for (int i = 0; i < 50; i++)
        {
            decidewall = rand() % 16;
            if (decidewall == 0)
            {
                // 적이 위에서 생성되어 아래로 이동
                x[i] = rand() % Canvas_X;
                y[i] = -150;

                // 생성된 적의 경로 설정
                dstX[i] = rand() % Canvas_X;
                dstY[i] = Canvas_Y;
            }
            if (decidewall == 1)
            {
                // 적이 위에서 생성되어 오른쪽으로 이동
                x[i] = rand() % Canvas_X;
                y[i] = -150;

                // 생성된 적의 경로 설정
                dstX[i] = Canvas_X;
                dstY[i] = rand() % Canvas_Y;
            }
            if (decidewall == 2)
            {
                // 적이 위에서 생성되어 왼쪽으로 이동
                x[i] = rand() % Canvas_X;
                y[i] = -150;

                // 생성된 적의 경로 설정
                dstX[i] = -150;
                dstY[i] = rand() % Canvas_Y;
            }
            else if (decidewall == 3)
            {
                // 적이 오른쪽에서 생성되어 위쪽으로 이동
                x[i] = Canvas_X;
                y[i] = rand() % Canvas_Y;

                // 생성된 적의 경로 설정
                dstX[i] = rand() % Canvas_X;
                dstY[i] = -150;
            }
            else if (decidewall == 4)
            {
                // 적이 오른쪽에서 생성되어 왼쪽으로 이동
                x[i] = Canvas_X;
                y[i] = rand() % Canvas_Y;

                // 생성된 적의 경로 설정
                dstX[i] = -150;
                dstY[i] = rand() % Canvas_Y;
            }
            else if (decidewall == 5)
            {
                // 적이 오른쪽에서 생성되어 오른쪽으로 이동
                x[i] = Canvas_X;
                y[i] = rand() % Canvas_Y;

                // 생성된 적의 경로 설정
                dstX[i] = Canvas_X;
                dstY[i] = rand() % Canvas_Y;
            }
            else if (decidewall == 6)
            {
                // 적이 아래에서 생성되어 위로 이동
                x[i] = rand() % Canvas_X;
                y[i] = Canvas_Y;

                // 생성된 적의 경로 설정
                dstX[i] = rand() % Canvas_X;
                dstY[i] = -150;
            }
            else if (decidewall == 7)
            {
                // 적이 아래에서 생성되어 오른쪽으로 이동
                x[i] = rand() % Canvas_X;
                y[i] = Canvas_Y;

                // 생성된 적의 경로 설정
                dstX[i] = Canvas_X;
                dstY[i] = rand() % Canvas_Y;
            }
            else if (decidewall == 8)
            {
                // 적이 아래에서 생성되어 왼쪽으로 이동
                x[i] = rand() % Canvas_X;
                y[i] = Canvas_Y;

                // 생성된 적의 경로 설정
                dstX[i] = -150;
                dstY[i] = rand() % Canvas_Y;
            }
            else if (decidewall == 9)
            {
                // 적이 왼쪽에서 생성되어 오른쪽으로 이동
                x[i] = -150;
                y[i] = rand() % Canvas_Y;

                // 생성된 적의 경로 설정
                dstX[i] = Canvas_X;
                dstY[i] = rand() % Canvas_Y;
            }
            else if (decidewall == 10)
            {
                // 적이 왼쪽에서 생성되어 위쪽으로 이동
                x[i] = -150;
                y[i] = rand() % Canvas_Y;

                // 생성된 적의 경로 설정
                dstX[i] = rand() % Canvas_X;
                dstY[i] = -150;
            }
            else if (decidewall == 11)
            {
                // 적이 왼쪽에서 생성되어 아래쪽으로 이동
                x[i] = -150;
                y[i] = rand() % Canvas_Y;

                // 생성된 적의 경로 설정
                dstX[i] = rand() % Canvas_X;
                dstY[i] = Canvas_Y;
            }
            else if (decidewall == 12)
            {
                // 적이 랜덤에서 생성되어 아래쪽으로 이동
                x[i] = rand() % Canvas_X;
                y[i] = rand() % Canvas_Y;

                // 생성된 적의 경로 설정
                dstX[i] = rand() % Canvas_X;
                dstY[i] = Canvas_Y;
            }
            else if (decidewall == 13)
            {
            // 적이 랜덤에서 생성되어 위쪽으로 이동
            x[i] = rand() % Canvas_X;
            y[i] = rand() % Canvas_Y;

            // 생성된 적의 경로 설정
            dstX[i] = rand() % Canvas_X;
            dstY[i] = -150;
            }
            else if (decidewall == 14)
            {
            // 적이 랜덤에서 생성되어 오른쪽으로 이동
            x[i] = rand() % Canvas_X;
            y[i] = rand() % Canvas_Y;

            // 생성된 적의 경로 설정
            dstX[i] = Canvas_X;
            dstY[i] = rand() % Canvas_Y;
            }
            else if (decidewall == 15)
            {
            // 적이 랜덤에서 생성되어 왼쪽으로 이동
            x[i] = rand() % Canvas_X;
            y[i] = rand() % Canvas_Y;

            // 생성된 적의 경로 설정
            dstX[i] = -150;
            dstY[i] = rand() % Canvas_Y;
            }
        }

        // enemy의 각 개체마다 위에서 얻어온 랜덤 좌표와 랜덤 경로 설정
        for (int j = 0; j < 3; j++)
        {
            for (int i = 0; i < 50; i++)
            {
                g_enemy[j][i].left = x[i];
                g_enemy[j][i].top = y[i];
                g_enemy[j][i].right = g_enemy[j][i].left + 20;
                g_enemy[j][i].bottom = g_enemy[j][i].top + 20;
            }
        }
    }
    ExitThread(0);
    return 0;
}
WCHAR buf1[128] = { 0, };


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        g_hWnd = hWnd;

        RE.left = 500;
        RE.top = 380;
        RE.right = 680;
        RE.bottom = 450;

        EX.left = 720;
        EX.top = 380;
        EX.right = 900;
        EX.bottom = 450;

        srand((unsigned int)time(NULL));
        // 초기 화면값 설정
        MoveWindow(hWnd, 0, 0, Canvas_X, Canvas_Y, true);
        // 게임 플레이어의 초기 좌표
        g_me.left = -1000;
        g_me.top = -1000;
        g_me.right = g_me.left + 20;
        g_me.bottom = g_me.top + 20;

        for (int j = 0; j < 3; j++)
        {
            for (int i = 0; i < 50; i++)
            {
                g_enemy[j][i].left = -500;
                g_enemy[j][i].top = -500;
                g_enemy[j][i].right = g_enemy[j][i].left + 20;
                g_enemy[j][i].bottom = g_enemy[j][i].top + 20;

                g_enemy[j][i].left = -500;
                g_enemy[j][i].top = -500;
                g_enemy[j][i].right = g_enemy[j][i].left + 20;
                g_enemy[j][i].bottom = g_enemy[j][i].top + 20;

                g_enemy[j][i].left = -500;
                g_enemy[j][i].top = -500;
                g_enemy[j][i].right = g_enemy[j][i].left + 20;
                g_enemy[j][i].bottom = g_enemy[j][i].top + 20;
            }
        }

        g_point.left = -1000;
        g_point.top = -1000;
        g_point.right = g_point.left + 40;
        g_point.bottom = g_point.top + 40;


        // 게임시작 버튼
        b_start = CreateWindow(L"button", L"START", WS_CHILD | WS_VISIBLE, 600, 300, 200, 60, hWnd, (HMENU)IDM_BTN_START, hInst, NULL);



        // 처음 시작시 쓰레드로 적의 위치를 한번 재설정
        if (b_flag == false)
        {
            CreateThread(NULL, 0, enemyspawn, (LPVOID)enemynum, 0, NULL);
        }

        SetTimer(hWnd, TIMER_ID_1, timesp1, NULL);
        SetTimer(hWnd, TIMER_ID_ENEMYMOVE, timesp2, NULL);

        p.left = 660; p.top = 530; p.right = 690; p.bottom = 560;
        break;
    case WM_LBUTTONDOWN:
        if(b_gmover)
        {
            if (TRUE == IntersectRect(&dst, &g_me, &EX))
            {
                HWND YUHANPROJECT;
                YUHANPROJECT = FindWindow(L"YUHANPROJECT", NULL);
                SendMessage(YUHANPROJECT, WM_CLOSE, wParam, lParam);
            }
            if (TRUE == IntersectRect(&dst, &g_me, &RE))
            {
                b_gmover = false;
                b_flag = false;
                levelup = false;
                score = 0;
                atime = 0;
                bomb = 3;
                needtime = 6;
                timesp1 = 1000;
                timesp2 = 35;

                g_item.left = rand() % 1200 + 30;
                g_item.top = rand() % 600 + 30;
                g_item.right = g_item.left + 30;
                g_item.bottom = g_item.top + 30;

                g_point.left = -1000;
                g_point.top = -1000;
                g_point.right = g_point.left + 40;
                g_point.bottom = g_point.top + 40;

                KillTimer(hWnd, TIMER_ID_1);
                KillTimer(hWnd, TIMER_ID_2);
                KillTimer(hWnd, TIMER_ID_ENEMYMOVE);
                KillTimer(hWnd, TIMER_ID_PTTM);
                CreateThread(NULL, 0, enemyspawn, (LPVOID)enemynum, 0, NULL);
                SetTimer(hWnd, TIMER_ID_1, timesp1, NULL);
                SetTimer(hWnd, TIMER_ID_ENEMYMOVE, timesp2, NULL);

            }
        }
        break;

    case WM_RBUTTONDOWN:
        ShowCursor(true);
        break;
    case WM_TIMER:
        switch (wParam)
        {
            case TIMER_ID_1:
            {
                // 게임종료시
                if (b_gmover == true)
                {
                    timesp1 = 1;
                    timesp2 = 1;
                    KillTimer(hWnd, TIMER_ID_PTTM);
                    KillTimer(hWnd, TIMER_ID_1);
                    ShowCursor(true);
                }

                // 5초마다 적들 위치 바뀜
                if (atime % 5 == 0)
                {
                    CreateThread(NULL, 0, enemyspawn, (LPVOID)enemynum, 0, NULL);
                }

                // 아이템 구현
                if (atime == 0)
                {
                    g_item.left = rand() % 1200 + 30;
                    g_item.top = rand() % 600 + 30;
                    g_item.right = g_item.left + 30;
                    g_item.bottom = g_item.top + 30;
                    Sleep(40);
                }

                // 웨이포인트 구현
                if (atime == 15)
                {
                    levelup = true;
                    g_point.left = rand() % 1200 + 60;
                    g_point.top = rand() % 600 + 60;
                    g_point.right = g_point.left + 40;
                    g_point.bottom = g_point.top + 40;
                    SetTimer(hWnd, TIMER_ID_PTTM, 1000, NULL);
                }
                atime++;
                score += atime;
            }
            break;

            case TIMER_ID_2:
            {
                btime--;

                // 폭탄이 켜질시
                if (btime <= 0)
                {
                    g_me.left += 20;
                    g_me.top += 20;
                    g_me.right -= 20;
                    g_me.bottom -= 20;
                    bombst = false;
                    btime = 5;
                    KillTimer(hWnd, TIMER_ID_2);
                }
            }
            break;

            case TIMER_ID_ENEMYMOVE:
            {
                if (b_flag && b_gmover)
                    ShowCursor(true);
                // 적의 움직임 구현
                for (int j = 0; j < 3; j++)
                {
                    for (int i = 0; i < 50; i++)
                    {
                        // x 좌표에 대한 비교 : 나와 상대 값을 이용
                        if (g_enemy[j][i].left > dstX[i])     // 참: 나는 상대의 왼쪽에 있다!
                        {
                            // 상대는 추적하기 위해 왼쪽으로 이동해야 한다.
                            // x좌표의 감소
                            g_enemy[j][i].left -= 10;
                            g_enemy[j][i].right -= 10;
                        }
                        else // 거짓 : 나는 상대의 오른쪽에 있다.
                        {
                            // 상대는 추적하기 위해 오른쪽으로 이동해야 한다.
                            // x좌표의 증가
                            g_enemy[j][i].left += 10;
                            g_enemy[j][i].right += 10;
                        }

                        // y 좌표에 대한 비교 : 나와 상대 값을 이용
                        if (g_enemy[j][i].top > dstY[i])        // 참: 나는 상대의 위쪽에 있다.
                        {
                            // 상대는 추적을 위해 위쪽으로 이동
                            g_enemy[j][i].top -= 10;
                            g_enemy[j][i].bottom -= 10;
                        }
                        else
                        {
                            g_enemy[j][i].top += 10;
                            g_enemy[j][i].bottom += 10;
                        }
                        InvalidateRect(hWnd, NULL, TRUE);
                    }
                }

            }
                break;
            case TIMER_ID_PTTM:
            {
                needtime--;

                if (needtime <= 0)
                {
                    needtime = 6;
                    b_gmover = true;
                    timesp1 = 1;
                    timesp2 = 1;
                    ShowCursor(true);
                }
            }
                break;
        }
    break;
    case WM_KEYDOWN:
        switch (wParam)
        {
            case 0x5A: // Z키 입력시 g_bomb의 크기가 일시적으로 커졌다 원래 크기로 돌아감
                if(cheat == 'X')
                {
                    if (bomb > 0)
                    {
                        bomb--;
                        bombst = true;
                    }
                }
                SetTimer(hWnd, TIMER_ID_2, 1000, NULL);
                break;

            case 0x58: // X키 입력시 g_bomb이 활성화 / invisible 상태
                if (cheat == 'X')
                {
                    cheat = 'O';
                    bombst = true;
                }
                else
                {
                    cheat = 'X';
                    bombst = false;
                }
                    break;
        }

    case WM_MOUSEMOVE:
    {
        RECT dst;
        // START 버튼이 없다면 플레이어의 좌표를 마우스 좌표로 설정
        int x, y;
        x = LOWORD(lParam);
        y = HIWORD(lParam);

        g_me.left = x - 12;
        g_me.top = y - 12;
        g_me.right = x + 12;
        g_me.bottom = y + 12;

        if (b_flag)
            ShowCursor(true);
        else if (b_flag == false)
        {
            ShowCursor(false); // 시작시 커서를 숨김


            if (bombst)
            {
                g_me.left -= 30;
                g_me.top -= 30;
                g_me.right = g_me.left + 60;
                g_me.bottom = g_me.top + 60;
            }

            // 적의 배열들을 돌면서 플레이어와 닿았는지 체크
            for(int i = 0 ; i < 3 ; i++)
            {
                for(int j = 0 ; j < 50 ; j++)
                {
                    // 일반 상태일때 적과 좌표 겹치는 때의 처리
                    if (TRUE == IntersectRect(&dst, &g_me, &g_enemy[i][j]))    
                    {
                        if(bombst == false)
                        {
                            b_gmover = true;
                            timesp1 = 1;
                            timesp2 = 1;
                            b_flag = true;
                            KillTimer(hWnd, TIMER_ID_PTTM);
                            KillTimer(hWnd, TIMER_ID_1);
                        }
                    }
                    // 폭탄 상태일때 적과 좌표 겹치는 때의 처리
                    if(bombst)
                    {
                        if (TRUE == IntersectRect(&dst, &g_me, &g_enemy[i][j]))
                        {
                            dstX[j] = 1800;
                            dstY[j] = 1800;
                            g_enemy[i][j].left = 1800 + rand() % 50;
                            g_enemy[i][j].top =  1800 + rand() % 3;
                            g_enemy[i][j].right = g_enemy[i][j].left + 20;
                            g_enemy[i][j].bottom = g_enemy[i][j].right + 20;
                        }
                    }
                }
            }



            // 플레이어와 아이템이 겹치는지 체크
            if (TRUE == IntersectRect(&dst, &g_me, &g_item)) // 겹친다면 아이템 위치 재설정, 점수 증가
            {
                g_item.left = rand() % 1200 + 100;
                g_item.top = rand() % 600 + 100;
                g_item.right = g_item.left + 30;
                g_item.bottom = g_item.top + 30;

                score += 100;
            }
            if (TRUE == IntersectRect(&dst, &g_me, &g_point))
            {
                g_point.left = rand() % 1200 + 100;
                g_point.top = rand() % 600 + 100;
                g_point.right = g_point.left + 40;
                g_point.bottom = g_point.top + 40;

                needtime = 6;
            }
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

    
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            WCHAR string[128] = { 0, };
            HDC hdc = BeginPaint(hWnd, &ps);
            HFONT hFont, OldFont;
            


            if (b_flag == true)
            {
                hFont = CreateFont(50, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("궁서"));
                OldFont = (HFONT)SelectObject(hdc, hFont);
                TextOut(hdc, 615, 200, L"DODGE", lstrlenW(L"DODGE"));
                SelectObject(hdc, OldFont);
                DeleteObject(hFont);
                wsprintfW(string, L"게임 방법 : 날아오는 적을 피하고 아이템을 먹어 점수를 획득");
                TextOut(hdc, 490, 500, string, lstrlenW(string));
                Rectangle(hdc, 460, 530, 490, 560); TextOut(hdc, 500, 535, L" << 나", lstrlenW(L" << 나"));
                Ellipse(hdc, 560, 530, 590, 560); TextOut(hdc, 600, 535, L" << 적", lstrlenW(L" << 적"));
                DrawFocusRect(hdc, &p); TextOut(hdc, 700, 535, L" << 웨이 포인트", lstrlenW(L" << 웨이 포인트"));
                RoundRect(hdc, 820, 530, 850, 560,12,12); TextOut(hdc, 860, 535, L" << 아이템", lstrlenW(L" << 아이템"));
                TextOut(hdc, 605, 700, L"( ? ) 키를 누르면 신기한일이..", lstrlenW(L"( ? ) 키를 누르면 신기한일이.."));
            }

            if (b_flag == false) {
                if (b_gmover == false) {
                    wsprintfW(string, L"생존 시간 %d : %d", atime / 60, atime % 60);
                    TextOut(hdc, 10, 10, string, lstrlenW(string));
                    wsprintfW(string, L"스코어 : %d", score);
                    TextOut(hdc, 10, 30, string, lstrlenW(string));
                    wsprintfW(string, L"폭탄(Z) : %d", bomb);
                    TextOut(hdc, 10, 700, string, lstrlenW(string));
                    wsprintfW(string, L"치트(?) : %lc", cheat);
                    TextOut(hdc, 1300, 700, string, lstrlenW(string));
                    Rectangle(hdc, g_point.left, g_point.top, g_point.right, g_point.bottom);
                    RoundRect(hdc, g_item.left, g_item.top, g_item.right, g_item.bottom, 12, 12);
                    for(int j = 0 ; j <3; j++)
                    {
                        for (int i = 0; i < 50; i++)
                        {
                            Ellipse(hdc, g_enemy[j][i].left, g_enemy[j][i].top, g_enemy[j][i].right, g_enemy[j][i].bottom);
                            Ellipse(hdc, g_enemy[j][i].left, g_enemy[j][i].top, g_enemy[j][i].right, g_enemy[j][i].bottom);
                            Ellipse(hdc, g_enemy[j][i].left, g_enemy[j][i].top, g_enemy[j][i].right, g_enemy[j][i].bottom);
                        }
                    }
                    if (levelup)
                    {
                        wsprintfW(string, L"웨이포인트 : %d", needtime);
                        TextOut(hdc, 200, 10, string, lstrlenW(string));
                        DrawFocusRect(hdc, &g_point);
                    }
                }
            }

            if (b_gmover == true)
            {
                ShowCursor(true);
                TextOut(hdc, 660, 280, L"GAME OVER", lstrlenW(L"GAME OVER"));
                wsprintfW(string, L"생존시간 %d : %d", atime / 60, atime % 60);
                TextOut(hdc, 650, 300, string, lstrlenW(string));
                wsprintfW(string, L"스코어 : %d", score);
                TextOut(hdc, 660, 320, string, lstrlenW(string));
                Rectangle(hdc, 500, 380, 680, 450);
                wsprintfW(string, L"RESTART");
                TextOut(hdc, 560, 410, string, lstrlenW(string));
                Rectangle(hdc, 720, 380, 900, 450);
                wsprintfW(string, L"EXIT");
                TextOut(hdc, 800, 410, string, lstrlenW(string));
                wsprintfW(string, L"커서가 안나올시 우클릭 몇번 하고 기다리면 나옵니다(아마도)");
                TextOut(hdc, 480, 350, string, lstrlenW(string));
            }
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            // 플레이어의 생성
            if(b_gmover != true && b_flag != true)
            {
                    Rectangle(hdc, g_me.left, g_me.top, g_me.right, g_me.bottom);
            }
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_DESTROY:
        DestroyWindow(b_start);
        PostQuitMessage(0);
        break;
    case WM_CLOSE:
        MessageBox(hWnd, L"게임을 종료합니다.", L"ㅋ", MB_OK);
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
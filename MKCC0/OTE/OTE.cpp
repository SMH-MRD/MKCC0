// OTE.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"

#include "OTE.h"
#include "CBasicControl.h"
#include "CSharedMem.h"	    //共有メモリクラス

#include "CEnvironment.h"
#include "CClientService.h"
#include "CScada.h"
#include "CPolicy.h"
#include "CAgent.h"

using namespace OTE;

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

CSharedMem* pOte_Obj;

static ST_KNL_MANAGE_SET    knl_manage_set;     //マルチスレッド管理用構造体
static ST_OTE_WND           st_work_wnd;        //センサーウィンドウ管理用構造体   

vector<CBasicControl*>	    VectCtrlObj;	    //スレッドオブジェクトのポインタ
static vector<HANDLE>	    VectHevent;		    //マルチスレッド用イベントのハンドル
static vector<HWND>	        VectTweetHandle;	//メインウィンドウのスレッドツイートメッセージ表示Staticハンドル
static HIMAGELIST	        hImgListTaskIcon;	//タスクアイコン用イメージリスト


LPWSTR              pszInifile;					// iniファイルのパス

static HWND         hTabWnd;                    //操作パネル用タブコントロールウィンドウのハンドル

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


/// スレッド実行のためのゲート関数
/// 引数　pObj タスククラスインスタンスのポインタ
static unsigned __stdcall thread_gate_func(void* pObj)
{
    CBasicControl* pthread_obj = (CBasicControl*)pObj;
    return pthread_obj->run(pObj);
}
//# ウィンドウにステータスバーを追加
HWND CreateStatusbarMain(HWND hWnd);

HWND	            CreateTaskSettingWnd(HWND hWnd); //個別タスク設定タブウィンドウ生成用関数
LRESULT CALLBACK    TaskTabDlgProc(HWND, UINT, WPARAM, LPARAM); //個別タスク設定タブウィンドウ用 メッセージハンドリング関数
DWORD	            knlTaskStartUp(); //実行させるタスクスレッドの起動処理

//# マルチメディアタイマイベントコールバック関数
VOID CALLBACK alarmHandlar(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // 共有メモリオブジェクトのインスタンス化
    pOte_Obj = new CSharedMem;

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OTE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OTE));

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}


//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OTE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_OTE);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

    knl_manage_set.num_of_task = (unsigned)ENUM_THREAD::NUM_OF_THREAD;			//スレッド数

    ///#メインウィンドウ生成
    st_work_wnd.hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        OTE_WND_X,
        OTE_WND_Y,
        TAB_POS_X + TAB_DIALOG_W + TAB_SPACE,
        OTE_WND_MIN_H + (MSG_WND_H + MSG_WND_Y_SPACE) * knl_manage_set.num_of_task + TAB_DIALOG_H,
        nullptr, nullptr, hInstance, nullptr);
    if (!st_work_wnd.hWnd)  return FALSE;
    ShowWindow(st_work_wnd.hWnd, nCmdShow);
    UpdateWindow(st_work_wnd.hWnd);

    /// アプリケーション固有の初期化処理を追加します。

   ///-ini file path設定##################
    static WCHAR dstpath[_MAX_PATH], szDrive[_MAX_DRIVE], szPath[_MAX_PATH], szFName[_MAX_FNAME], szExt[_MAX_EXT];
    //--exe failのpathを取得
    GetModuleFileName(NULL, dstpath, (sizeof(WCHAR) * _MAX_PATH / 2));
    //--取得したpathを分割
    _wsplitpath_s(dstpath, szDrive, sizeof(szDrive) / 2, szPath, sizeof(szPath) / 2, szFName, sizeof(szFName) / 2, szExt, sizeof(szExt) / 2);
    //--フォルダのパスとiniファイルのパスに合成
    _wmakepath_s(dstpath, sizeof(dstpath) / 2, szDrive, szPath, NAME_OF_INIFILE, EXT_OF_INIFILE);
    pszInifile = dstpath;

    ///-共有メモリ割付&設定##################
    if (OK_SHMEM != pOte_Obj->create_smem(SMEM_SENSOR_NAME, sizeof(ST_SENSOR_IO), MUTEX_SENSOR_NAME)) return(FALSE);

    HBITMAP hBmp;
    CBasicControl* pobj;
    int task_index = 0;
    WCHAR taskname[BC_TASK_NAME_BUF_SIZE];
 
    InitCommonControls();//コモンコントロール初期化
    hImgListTaskIcon = ImageList_Create(32, 32, ILC_COLOR | ILC_MASK, 2, 0);//タスクアイコン表示用イメージリスト設定

    ///# VectCtrlObjへの登録順は、ENUM_THREADに準じてください
    ///##Task1 設定 Environment
    {
        /// -タスクインスタンス作成->リスト登録
        pobj = new CEnvironment;
        VectCtrlObj.push_back(pobj);
        pobj->inf.index = (int32_t)ENUM_THREAD::ENV;
        /// -イベントオブジェクトクリエイト->リスト登録	
        VectHevent.push_back(pobj->inf.hevents[BC_EVENT_TYPE_TIME] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

        /// -スレッド起動周期セット
        pobj->inf.cycle_ms = ENV_SCAN_MS;       //タスクスキャンタイムmsec

        /// -ツイートメッセージ用iconセット
        hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_ENV");//ビットマップ割り当て
        ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
        DeleteObject(hBmp);

        ///オブジェクト名セット
        DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, ENV_KEY_OF_INIFILE, L"Environment", taskname, BC_TASK_NAME_BUF_SIZE, PATH_OF_INIFILE);
        pobj->inf.name.clear(); pobj->inf.name.append(taskname);
        str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, ENV_KEY_OF_INIFILE, L"ENV", taskname, BC_TASK_NAME_BUF_SIZE, PATH_OF_INIFILE);
        pobj->inf.sname.clear(); pobj->inf.sname.append(taskname);
        ///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
        pobj->inf.n_active_events = 1;
        pobj->inf.status = BC_CODE_STAT_INIT_REQ;

    }

    ///##Task2 設定 Client Service
    {
        /// -タスクインスタンス作成->リスト登録
        pobj = new CClientService;
        VectCtrlObj.push_back(pobj);
        pobj->inf.index = (int32_t)ENUM_THREAD::CS;
        /// -イベントオブジェクトクリエイト->リスト登録	
        VectHevent.push_back(pobj->inf.hevents[BC_EVENT_TYPE_TIME] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

        /// -スレッド起動周期セット
        pobj->inf.cycle_ms = CS_SCAN_MS;       //タスクスキャンタイムmsec

        /// -ツイートメッセージ用iconセット
        hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_CS");//ビットマップ割り当て
        ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
        DeleteObject(hBmp);

        ///オブジェクト名セット

        DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, CS_KEY_OF_INIFILE, L"Client Service", taskname, BC_TASK_NAME_BUF_SIZE, PATH_OF_INIFILE); //(DWORD)pobj->inf.name.length() * 2, PATH_OF_INIFILE);
        pobj->inf.name.clear(); pobj->inf.name.append(taskname);
        str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, CS_KEY_OF_INIFILE, L"CS", taskname, BC_TASK_NAME_BUF_SIZE, PATH_OF_INIFILE);
        pobj->inf.sname.clear(); pobj->inf.sname.append(taskname);

        ///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
        pobj->inf.n_active_events = 1;
        pobj->inf.status = BC_CODE_STAT_INIT_REQ;
    }
    ///##Task3 設定 SCADA
    {
        /// -タスクインスタンス作成->リスト登録
        pobj = new CScada;
        VectCtrlObj.push_back(pobj);
        pobj->inf.index = (int32_t)ENUM_THREAD::SCAD;
        /// -イベントオブジェクトクリエイト->リスト登録	
        VectHevent.push_back(pobj->inf.hevents[BC_EVENT_TYPE_TIME] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

        /// -スレッド起動周期セット
        pobj->inf.cycle_ms = SCADA_SCAN_MS;       //タスクスキャンタイムmsec

        /// -ツイートメッセージ用iconセット
        hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_SCAD");//ビットマップ割り当て
        ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
        DeleteObject(hBmp);

        ///オブジェクト名セット
        DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, SCAD_KEY_OF_INIFILE, L"Scada", taskname, BC_TASK_NAME_BUF_SIZE, PATH_OF_INIFILE);
        pobj->inf.name.clear(); pobj->inf.name.append(taskname);
        str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, SCAD_KEY_OF_INIFILE, L"SCAD", taskname, BC_TASK_NAME_BUF_SIZE, PATH_OF_INIFILE);
        pobj->inf.sname.clear(); pobj->inf.sname.append(taskname);
 
        ///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
        pobj->inf.n_active_events = 1;
        pobj->inf.status = BC_CODE_STAT_INIT_REQ;
    }

    ///##Task4 設定 Policy
    {
        /// -タスクインスタンス作成->リスト登録
        pobj = new CPolicy;
        VectCtrlObj.push_back(pobj);
        pobj->inf.index = (int32_t)ENUM_THREAD::POL;
        /// -イベントオブジェクトクリエイト->リスト登録	
        VectHevent.push_back(pobj->inf.hevents[BC_EVENT_TYPE_TIME] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

        /// -スレッド起動周期セット
        pobj->inf.cycle_ms = POLICY_SCAN_MS;       //タスクスキャンタイムmsec

        /// -ツイートメッセージ用iconセット
        hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_POL");//ビットマップ割り当て
        ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
        DeleteObject(hBmp);

        ///オブジェクト名セット
        DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, POL_KEY_OF_INIFILE, L"Policy", taskname, BC_TASK_NAME_BUF_SIZE, PATH_OF_INIFILE);
        pobj->inf.name.clear(); pobj->inf.name.append(taskname);
        str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, POL_KEY_OF_INIFILE, L"POL", taskname, BC_TASK_NAME_BUF_SIZE, PATH_OF_INIFILE);
        pobj->inf.sname.clear(); pobj->inf.sname.append(taskname);
        ///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
        pobj->inf.n_active_events = 1;
        pobj->inf.status = BC_CODE_STAT_INIT_REQ;
    }
    ///##Task5 設定 Agent
    {
        /// -タスクインスタンス作成->リスト登録
        pobj = new CAgent;
        VectCtrlObj.push_back(pobj);
        pobj->inf.index = (int32_t)ENUM_THREAD::AGENT;
        /// -イベントオブジェクトクリエイト->リスト登録	
        VectHevent.push_back(pobj->inf.hevents[BC_EVENT_TYPE_TIME] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

        /// -スレッド起動周期セット
        pobj->inf.cycle_ms = AGENT_SCAN_MS;       //タスクスキャンタイムmsec

        /// -ツイートメッセージ用iconセット
        hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_AGENT");//ビットマップ割り当て
        ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
        DeleteObject(hBmp);

        ///オブジェクト名セット
        DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, AGENT_KEY_OF_INIFILE, L"Agent", taskname, BC_TASK_NAME_BUF_SIZE, PATH_OF_INIFILE);
        pobj->inf.name.clear(); pobj->inf.name.append(taskname);
        str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, AGENT_KEY_OF_INIFILE, L"AGENT", taskname, BC_TASK_NAME_BUF_SIZE, PATH_OF_INIFILE);
        pobj->inf.sname.clear(); pobj->inf.sname.append(taskname);
        ///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
        pobj->inf.n_active_events = 1;
        pobj->inf.status = BC_CODE_STAT_INIT_REQ;
    }
    ///##タスク設定ウィンドウ作成
    pobj = VectCtrlObj[1];

    hTabWnd = CreateTaskSettingWnd(st_work_wnd.hWnd);

    ///##タスクパラメータセット

    for (unsigned i = 0; i < knl_manage_set.num_of_task; i++) {
        CBasicControl* pobj = (CBasicControl*)VectCtrlObj[i];
        pobj->inf.hwnd_parent = st_work_wnd.hWnd;//親ウィンドウのハンドルセット
        pobj->inf.hInstance = hInst;//親ウィンドウのハンドルセット

        // -ツイートメッセージ用Static window作成->リスト登録	
        pobj->inf.hwnd_msgstatics = CreateWindow(L"STATIC", L"...", WS_CHILD | WS_VISIBLE, MSG_WND_ORG_X, MSG_WND_ORG_Y + MSG_WND_H * i + i * MSG_WND_Y_SPACE, MSG_WND_W, MSG_WND_H, st_work_wnd.hWnd, (HMENU)(IDC_OBJECT_BASE + i), hInst, NULL);
        VectTweetHandle.push_back(pobj->inf.hwnd_msgstatics);

        //その他設定
        pobj->inf.psys_counter = &knl_manage_set.sys_counter;
        pobj->inf.act_count = 0;//起動チェック用カウンタリセット
        //起動周期カウント値
        if (pobj->inf.cycle_ms >= SYSTEM_TICK_ms)	pobj->inf.cycle_count = pobj->inf.cycle_ms / SYSTEM_TICK_ms;
        else pobj->inf.cycle_count = 1;

        //ステータスを初期化要求にセット
        pobj->inf.status = BC_CODE_STAT_INIT_REQ;
    }

    InvalidateRect(st_work_wnd.hWnd, NULL, FALSE);            //WM_PAINTを発生させてアイコンを描画させる
    UpdateWindow(st_work_wnd.hWnd);

    // タスクオブジェクトスレッド起動	
    knlTaskStartUp();

    ///#タスクループ処理起動マルチメディアタイマ起動
    {
        // --マルチメディアタイマ精度設定
        TIMECAPS wTc;//マルチメディアタイマ精度構造体
        if (timeGetDevCaps(&wTc, sizeof(TIMECAPS)) != TIMERR_NOERROR) 	return((DWORD)FALSE);
        knl_manage_set.mmt_resolution = MIN(MAX(wTc.wPeriodMin, TARGET_RESOLUTION), wTc.wPeriodMax);
        if (timeBeginPeriod(knl_manage_set.mmt_resolution) != TIMERR_NOERROR) return((DWORD)FALSE);

        _RPT1(_CRT_WARN, "MMTimer Period = %d\n", knl_manage_set.mmt_resolution);

        // --マルチメディアタイマセット
        knl_manage_set.KnlTick_TimerID = timeSetEvent(knl_manage_set.cycle_base, knl_manage_set.mmt_resolution, (LPTIMECALLBACK)alarmHandlar, 0, TIME_PERIODIC);

        // --マルチメディアタイマー起動失敗判定　メッセージBOX出してFALSE　returen
        if (knl_manage_set.KnlTick_TimerID == 0) {	 //失敗確認表示
            LPVOID lpMsgBuf;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                0, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language*/(LPTSTR)&lpMsgBuf, 0, NULL);
            MessageBox(NULL, (LPCWSTR)lpMsgBuf, L"MMT Failed!!", MB_OK | MB_ICONINFORMATION);// Display the string.
            LocalFree(lpMsgBuf);// Free the buffer.
            return((DWORD)FALSE);
        }
    }

    return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case WM_CREATE:
    {

        //メインウィンドウにステータスバー付加
        st_work_wnd.hWnd_status_bar = CreateStatusbarMain(hWnd);
        SendMessage(st_work_wnd.hWnd_status_bar, SB_SETTEXT, 0, (LPARAM)L"-");

    } break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 選択されたメニューの解析:
        switch (wmId)
        {
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

    case WM_SIZE:
    {
        //ステータスバーにサイズ変更を通知
        SendMessage(st_work_wnd.hWnd_status_bar, WM_SIZE, wParam, lParam);
    }break;

    case WM_NOTIFY://コモンコントロールでイベントが起こった場合、およびコモンコントロールが情報を親ウィンドウに要求する場合に、コモンコントロールの親ウィンドウに送信されます。
    {
        int tab_index = TabCtrl_GetCurSel(((NMHDR*)lParam)->hwndFrom);//タブコントロールの選択タブID取得

        for (unsigned i = 0; i < VectCtrlObj.size(); i++)//全タスクオブジェクト確認
        {
            CBasicControl* pObj = (CBasicControl*)VectCtrlObj[i];
            //タスク設定パネルウィンドウ位置調整
            MoveWindow(pObj->inf.hwnd_opepane, TAB_POS_X, TAB_POS_Y + TAB_SIZE_H + (MSG_WND_H + MSG_WND_Y_SPACE) * knl_manage_set.num_of_task, TAB_DIALOG_W, TAB_DIALOG_H - TAB_SIZE_H, TRUE);

            if ((VectCtrlObj.size() - 1 - pObj->inf.index) == tab_index) //タブ選択されているタスクのみ処理
            {
                ShowWindow(pObj->inf.hwnd_opepane, SW_SHOW);
            }
            else
            {
                ShowWindow(pObj->inf.hwnd_opepane, SW_HIDE);
            }
        }
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        //タスクツィートメッセージアイコン描画
        for (unsigned i = 0; i < knl_manage_set.num_of_task; i++) ImageList_Draw(hImgListTaskIcon, i, hdc, 0, i * (MSG_WND_H + MSG_WND_Y_SPACE), ILD_NORMAL);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:

        timeKillEvent(knl_manage_set.KnlTick_TimerID);//マルチメディアタイマ停止
        ///-タスクスレッド終了##################
        for (unsigned i = 0; i < VectCtrlObj.size(); i++) {

            CBasicControl* pobj = (CBasicControl*)VectCtrlObj[i];
            pobj->inf.command = BC_CODE_COM_TERMINATE;		// 基本ティックのカウンタ変数クリア
            pobj->close();
        }
        Sleep(1000);//スレッド終了待機

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
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

///#　******************************************************************************************
//  関数: CreateStatusbarMain(HWND)
//
//  目的: メイン ウィンドウ下部にアプリケーションの状態を表示用のステータスバーを配置します。
//　******************************************************************************************  
HWND CreateStatusbarMain(HWND hWnd)
{
    HWND hSBWnd;
    int sb_size[] = { 60,120,180,240,300,360,420,480,615 };//ステータス区切り位置

    InitCommonControls();
    hSBWnd = CreateWindowEx(
        0,                          //拡張スタイル
        STATUSCLASSNAME,            //ウィンドウクラス
        NULL,                       //タイトル
        WS_CHILD | SBS_SIZEGRIP,    //ウィンドウスタイル
        0, 0, //位置
        0, 0, //幅、高さ
        hWnd, //親ウィンドウ
        (HMENU)ID_STATUS_BAR,       //ウィンドウのＩＤ
        hInst,                      //インスタンスハンドル
        NULL);
    SendMessage(hSBWnd, SB_SETPARTS, (WPARAM)9, (LPARAM)(LPINT)sb_size);//6枠で各枠の仕切り位置をパラーメータ指定
    ShowWindow(hSBWnd, SW_SHOW);
    return hSBWnd;
}

///#　*****************************************************************************************************************
//  関数: CreateTaskSettingWnd(HWND hWnd)
//
//  目的: アプリケーションタスク調整用パネルウィンドウを生成します。
//  
HWND CreateTaskSettingWnd(HWND hWnd)
{
    RECT rc;
    TC_ITEM tc[static_cast<uint32_t>(ENUM_THREAD::NUM_OF_THREAD)];//タブコントロール設定構造体

    //タブコントロールウィンドウの生成
    GetClientRect(hWnd, &rc);
    HWND hTab = CreateWindowEx(0, WC_TABCONTROL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        rc.left + TAB_POS_X, rc.top + TAB_POS_Y + (MSG_WND_H + MSG_WND_Y_SPACE) * knl_manage_set.num_of_task, TAB_DIALOG_W, TAB_DIALOG_H, hWnd, (HMENU)ID_TASK_SET_TAB, hInst, NULL);

    //Task Setting用パネルウィンドウタブ作成
    for (unsigned i = 0; i < VectCtrlObj.size(); i++) {
        CBasicControl* pObj = (CBasicControl*)VectCtrlObj[i];

        //タブコントロールにタブの追加
        tc[i].mask = (TCIF_TEXT | TCIF_IMAGE);//テキストとイメージ配置
        tc[i].pszText = const_cast<LPWSTR>(pObj->inf.sname.c_str());
        tc[i].iImage = pObj->inf.index;       //イメージのID
        SendMessage(hTab, TCM_INSERTITEM, (WPARAM)0, (LPARAM)&tc[i]);

        //パネルウィンドウの生成
        pObj->inf.hwnd_opepane = CreateDialog(hInst, L"IDD_DIALOG_TASK_SET", hWnd, (DLGPROC)TaskTabDlgProc);
        
        HWND hname_static = GetDlgItem(pObj->inf.hwnd_opepane, IDC_TAB_TASKNAME);
        SetWindowText(hname_static, pObj->inf.name.c_str());
        pObj->set_func_pb_txt();
        MoveWindow(pObj->inf.hwnd_opepane, TAB_POS_X, TAB_POS_Y + TAB_SIZE_H + (MSG_WND_H + MSG_WND_Y_SPACE) * knl_manage_set.num_of_task, TAB_DIALOG_W, TAB_DIALOG_H - TAB_SIZE_H, TRUE);

        //初期値はindex 0のウィンドウを表示
        if (i == 0) {
            ShowWindow(pObj->inf.hwnd_opepane, SW_SHOW);
            SetWindowText(GetDlgItem(pObj->inf.hwnd_opepane, IDC_TAB_TASKNAME), const_cast<LPWSTR>(pObj->inf.name.c_str()));//タスク名をスタティックテキストに表示
        }
        else ShowWindow(pObj->inf.hwnd_opepane, SW_HIDE);
    }

    //タブコントロールにイメージリストセット
    SendMessage(hTab, TCM_SETIMAGELIST, (WPARAM)0, (LPARAM)hImgListTaskIcon);

    return hTab;
}

///#　*****************************************************************************************************************
//  関数:TaskTabDlgProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: アプリケーションタスク調整用タブウィンドウのメッセージハンドリング関数。
//  
LRESULT CALLBACK TaskTabDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_INITDIALOG: {

        InitCommonControls();

        //メッセージ用リストコントロールの設定
        LVCOLUMN lvcol;

        //列ラベル設定
        LPTSTR strItem0[] = { (LPTSTR)(L"time"), (LPTSTR)(L"message") };//列ラベル
        int CX[] = { 60, 600 };//列幅
        HWND hList = GetDlgItem(hDlg, IDC_LIST1);
        lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        lvcol.fmt = LVCFMT_LEFT;
        for (int i = 0; i < 2; i++) {
            lvcol.cx = CX[i];             // 表示位置
            lvcol.pszText = strItem0[i];  // 見出し
            lvcol.iSubItem = i;           // サブアイテムの番号
            ListView_InsertColumn(hList, i, &lvcol);
        }
        //リスト行追加
        LVITEM item;
        item.mask = LVIF_TEXT;
        for (int i = 0; i < PRM_N_TASK_MSGLIST_ROW; i++) {
            item.pszText = (LPWSTR)L".";   // テキスト
            item.iItem = i;               // 番号
            item.iSubItem = 0;            // サブアイテムの番号
            ListView_InsertItem(hList, &item);
        }

        return TRUE;
    }break;
    case WM_COMMAND: {
        CBasicControl* pObj = VectCtrlObj[VectCtrlObj.size() - TabCtrl_GetCurSel(hTabWnd) - 1];

        //タスクオブジェクト固有の処理
        pObj->PanelProc(hDlg, msg, wp, lp);
    }break;
    }
    return FALSE;
}

///#　*****************************************************************************************************************
//  関数: knlTaskStartUp()
//
//  目的    :登録されているタスクオブジェクトのスレッドを起動します。
//  機能	:[KNL]システム/ユーザタスクスタートアップ関数
//  処理	:自プロセスのプライオリティ設定，カーネルの初期設定,タスク生成，基本周期設定
//  戻り値  :Win32APIエラーコード
//  
DWORD knlTaskStartUp()
{
    HANDLE	myPrcsHndl;	// 本プログラムのプロセスハンドル

    //-プロセスハンドル取得
    if ((myPrcsHndl = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_SET_INFORMATION, FALSE, _getpid())) == NULL)	return(GetLastError());
    _RPT1(_CRT_WARN, "KNL Priority For Windows(before) = %d \n", GetPriorityClass(myPrcsHndl));

    //-自プロセスのプライオリティを最優先ランクに設定
    if (SetPriorityClass(myPrcsHndl, REALTIME_PRIORITY_CLASS) == 0)	return(GetLastError());
    _RPT1(_CRT_WARN, "KNL Priority For NT(after) = %d \n", GetPriorityClass(myPrcsHndl));

    //-アプリケーションタスク数が最大数を超えた場合は終了
    if (VectCtrlObj.size() > (size_t)ENUM_THREAD::NUM_OF_THREAD)	return((DWORD)ERROR_BAD_ENVIRONMENT);

    //- アプリケーションスレッド生成処理	
    for (unsigned i = 0; i < VectCtrlObj.size(); i++) {

        CBasicControl* pobj = (CBasicControl*)VectCtrlObj[i];

        // タスク生成(スレッド生成)
        // 他ﾌﾟﾛｾｽとの共有なし,スタック初期サイズ　デフォルト, スレッド実行関数　引数で渡すオブジェクトで対象切り替え,スレッド関数の引数（対象のオブジェクトのポインタ）, 即実行Createflags, スレッドID取り込み
        pobj->inf.hndl = (HANDLE)_beginthreadex(    //戻り値：スレッドハンドル
            (void*)NULL,					        //スレッドの属性
            0,								        //Stack初期Size
            thread_gate_func,				        //スレッドプロシジャー
            VectCtrlObj[i],				            //スレッドのパラメータ
            (unsigned)0,					        //スレッド作成フラグ
            (unsigned*)&(pobj->inf.id)		        //スレッドIDを受け取る変数のアドレス
        );

        // タスクプライオリティ設定
        if (pobj->inf.hndl != 0) {
            HANDLE hTh = pobj->inf.hndl;
            if (SetThreadPriority(hTh, pobj->inf.priority) == 0)
                return(GetLastError());
        }
        else {
            return(GetLastError());
        }
        _RPT2(_CRT_WARN, "Task[%d]_priority = %d\n", i, GetThreadPriority(pobj->inf.hndl));

        pobj->inf.act_count = 0;		// 基本ティックのカウンタ変数クリア
        pobj->inf.time_over_count = 0;	// 予定周期オーバーカウントクリア
    }

    return L_ON;
}

///#　****************************************************************************************
//  関数: alarmHandlar(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
//  目的: アプリケーションメイン処理
// 　マルチメディアタイマーイベント処理関数
//  ******************************************************************************************
VOID	CALLBACK    alarmHandlar(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
    knl_manage_set.sys_counter++;

    LONG64 tmttl;

    //スレッドループ再開イベントセット処理
    for (unsigned i = 0; i < knl_manage_set.num_of_task; i++) {
        CBasicControl* pobj = VectCtrlObj[i];
        pobj->inf.act_count++;
        if (pobj->inf.act_count >= pobj->inf.cycle_count) {
            PulseEvent(VectHevent[i]);
            pobj->inf.period = pobj->inf.act_count * knl_manage_set.cycle_base;
            pobj->inf.act_count = 0;
            pobj->inf.total_act++;

            TCHAR tbuf[32];
            wsprintf(tbuf, L"\t%4d", (int)pobj->inf.period);
            SendMessage(st_work_wnd.hWnd_status_bar, SB_SETTEXT, i, (LPARAM)tbuf);
        }
    }

    //Statusバーに経過時間表示
    if (knl_manage_set.sys_counter % 40 == 0) {// 1sec毎

        //起動後経過時間計算
        tmttl = (long long)knl_manage_set.sys_counter * knl_manage_set.cycle_base;//アプリケーション起動後の経過時間msec
        knl_manage_set.Knl_Time.wMilliseconds = (WORD)(tmttl % 1000); tmttl /= 1000;
        knl_manage_set.Knl_Time.wSecond = (WORD)(tmttl % 60); tmttl /= 60;
        knl_manage_set.Knl_Time.wMinute = (WORD)(tmttl % 60); tmttl /= 60;
        knl_manage_set.Knl_Time.wHour = (WORD)(tmttl % 60); tmttl /= 24;
        knl_manage_set.Knl_Time.wDay = (WORD)(tmttl % 24);

        TCHAR tbuf[32];
        wsprintf(tbuf, L"\t%3dD %02d:%02d:%02d", knl_manage_set.Knl_Time.wDay, knl_manage_set.Knl_Time.wHour, knl_manage_set.Knl_Time.wMinute, knl_manage_set.Knl_Time.wSecond);
        SendMessage(st_work_wnd.hWnd_status_bar, SB_SETTEXT, 8, (LPARAM)tbuf);
    }
    return;
}

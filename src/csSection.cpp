#include "csSection.h"
#include "csUIFx.h"

#include<mutex>

std::mutex _mutex;

float xdimFact = 1.0;
float ydimFact = 1.0;
extern wchar_t* appSizesFilePath;
extern vector<RECT> RF;
bool saveAppSizes;

extern wchar_t* appTitleFilePath;
extern vector<wchar_t*> TITLEFILE;
extern vector<bool> setTitleInit;
extern bool saveAppTitles;

extern wchar_t* appTipsFilePath;
extern bool saveAppTips;

vector<HWND> SECTION;
vector<HWND> PAR;
vector<int> PARID;
vector<BOOL_RECT> RESIZE_EDGE;
vector<RECT> RECTPARREF;
vector<RECT> RECTPARREFSAVED;
vector<RECT> RECTWND;
vector<RECT> RECTCL;
vector<RECT> RECTWNDSAVED;
vector<RECT> RECTRESTORE;
vector<SIZE> tmpSize;
vector<SIZE> DELTASIZE;
vector<RECT> deltaSizeMoveWaitList;
vector<bool> isWindowInSizeMoveWaitList;
vector<POINT> DELTAPOS;
vector<bool> BINDTOPAR;
vector<COLORREF> backgroundColor;
vector<COLORREF> borderColor;
vector<vector<BIND_DIM_GEOM_PARAMS>> rSizeBind;
vector<vector<BIND_DIM_GEOM_PARAMS>> lSizeBind;
vector<vector<BIND_DIM_GEOM_PARAMS>> tSizeBind;
vector<vector<BIND_DIM_GEOM_PARAMS>> bSizeBind;
vector<int> resizingThreadId;
vector<vector<bool>> HIDEGROUPMSG;
vector<WNDCLASSEXW*> wndClass;
vector<bool> SECTIONSTYLE;
vector<bool> moveMsgConnexion;
vector<bool> enterSizeMove;
vector<bool> isMaximized;
vector<bool> wereMinimized;
vector<int> isMaximizedCount;
vector<bool> initBool;
vector<HDC> hdcontext;
vector<HDC> hdBkgContext;
vector<HDC> hdStackContext;
vector<HBITMAP> hBkgBmp;
vector<HBITMAP> hStackBmp;
vector<SIZE> hdcSize;
vector<HDC> hdcontextExt;
vector<HBITMAP> hdBmpExt;
vector<POINT> hdcontextExtInPos;
vector<POINT> hdcontextExtOutPos;
vector<SIZE> hdcontextExtSize;
vector<int> lockSizeMoveId;
vector<bool> inCaptionArea;
vector<int> THREAD_END;
vector<int> BORDERTHICK;
vector<CSRGBA> hdcontextExtBkgColor;
vector<CSRGBA> hdcontextExtBrdColor;
vector<vector<CSGRAPHIC_CONTEXT>> imageGradients;
vector<vector<CSGRAPHIC_ENTITY>> entity;
vector<CSENTITY_ID_MAP> entityMap;
vector<bool> bltUpdate;
vector<bool> attached;

vector<bool> WAITANIMEENDSIN;
vector<bool> WAITANIMEENDSOUT;
vector<int> MOUSEHOVERCOUNT;
vector<int> MOUSELEAVECOUNT;
vector<int> MOUSEHOVERSIGNAL;
vector<CSIMAGE_GRADIENT> mhgradient;
vector<CSLOCKED_MODE> lockedMode;
vector<CSBKG_CLICK_GRADIENT> clkGradient;
vector<int> CLICKGRADIENTCOUNT;

vector<bool> mouseLeaveRemoveBkg;
vector<TIPS_POPUP_PARAMS> TipsPopupParams;
vector<AUTO_TRANSFORMATION> AutoTransform;
vector<int> AUTOTRANSCOUNT;
vector<HCURSOR> cursor;

vector<CSTEXT> TITLE;

vector<int> withHScroll;
vector<int> withVScroll;
vector<int> viewedAreaRightMarging;
vector<int> viewedAreaBottomMarging;
vector<RECT> bltRect;
vector<float> hZoom;
vector<float> vZoom;

vector<bool> halftoneMode;
vector<CSTIMER_PARAMS> TIMER_PARAMS;

vector<vector<CSLOADED_IMAGE>> loadedImage;

vector<vector<void(*)(CSARGS)>> GROUPED_EVENTS_FUNC;
vector<vector<CSARGS>> GROUPED_EVENTS_ARGS;

vector<CSDYNAMIC_SIMPLE_TEXT> dynSimpleText;
vector<CSZOOM_PARAMS> zoomParams;
vector<bool> updateAfterResizeMsg;
vector<CSAPP_ICON> appIcon;
extern vector<bool> updateTitleSectionBool;

extern bool __translateTitles();
extern bool __translateTips(char* inputLanguage);



HINSTANCE _hInstance;
HWND hwndBtnDown;
int CURSOR_NCHITTEST_POS = 0;
bool EXECUTE_SIZEMOVE_WAIT_LIST_SIGNAL = 0;
POINT TIMER_POINT, LBD_POINT;
TASKBAR_INFO taskbarInfo;
WNDCLASS wc0 = { 0 };
RECT LAST_WORKAREA;

float dimFact = 1.0;
int CAPTION_AREA_SIZE = GetSystemMetrics(SM_CYCAPTION);

UINT LAST_TASKBAR_POS = 0;
int HIDDEN_WINDOW_PART[4];

void geometryBinding(int& id);
void sizeMoveWaitListExecute(int id);
void sizeMoveAll(int _id, bool automatic=1, SIZE _deltaSize= {0}, POINT _deltaPos= {0});
void _blitDynamicSimpleText(int id);

using namespace CSUIMAN;
using namespace CSUTILS;
using namespace CSFILESMAN;

int SMX, SMY;

#include "csTranslator.h"

int CSUIMAN::createSection(int id, RECT _geom, COLORREF color, BOOL_RECT edgeResize, bool show, bool isRoot, bool attach)
{
    int i = SECTION.size();
    RECT geom = r(_geom.left*dimFact, _geom.top*dimFact, _geom.right*dimFact, _geom.bottom*dimFact, i);
    
    if(i == 1)
    {
        if((saveAppTitles && !CSFILESMAN::fileExists(appTitleFilePath)))
        {
            if(__translateTitles())
                __getAppTitles(); // executer apres la creation de la premiere fenete pour eviter les bogues
        }
        else if((saveAppTitles && CSFILESMAN::fileExists(appTitleFilePath)))
        {
            __getAppTitles();
        }

        if((saveAppTips && !CSFILESMAN::fileExists(appTipsFilePath)))
        {
            __translateTips("en");
        }
        else if((saveAppTips && CSFILESMAN::fileExists(appTipsFilePath)))
        {
            __getAppTips();
        }

        __getAppSizes();

        if(saveAppSizes && CSFILESMAN::fileExists(appSizesFilePath))
        {
            MoveWindow(SECTION[0], RF[0].left, RF[0].top, RF[0].right, RF[0].bottom, 1);
        }
    }
    CSUIMAN::printRect(geom);
    RESIZE_EDGE.push_back(edgeResize);
    RECTPARREF.push_back({0});
    RECTCL.push_back({0,0,geom.right, geom.bottom});
    if(i == 0)
    {
        RECTWND.push_back({geom.left, geom.top, geom.right+geom.left, geom.bottom+geom.top});
    }
    else
        RECTWND.push_back({0});
    RECTRESTORE.push_back({0});
    tmpSize.push_back({0});
    DELTASIZE.push_back({0});
    DELTAPOS.push_back({0});
    deltaSizeMoveWaitList.push_back({0});
    isWindowInSizeMoveWaitList.push_back(0);
    PARID.push_back(id);
    RECTPARREFSAVED.push_back({geom.left, geom.top, geom.right+geom.left, geom.bottom+geom.top});
    RECTWNDSAVED.push_back({0});
    BINDTOPAR.push_back(1);
    backgroundColor.push_back(color);
    borderColor.push_back(color);
    HIDEGROUPMSG.push_back(newVector<bool>());
    moveMsgConnexion.push_back(0);
    enterSizeMove.push_back(0);
    isMaximized.push_back(0);
    wereMinimized.push_back(0);
    isMaximizedCount.push_back(0);
    initBool.push_back(0);
    lockSizeMoveId.push_back(-1);
    hdcSize.push_back({SMX=GetSystemMetrics(SM_CXSCREEN), SMY=GetSystemMetrics(SM_CYSCREEN)});
    inCaptionArea.push_back(0);
    THREAD_END.push_back(0);
    BORDERTHICK.push_back(0);
    vector<CSGRAPHIC_CONTEXT> gc;
    imageGradients.push_back(gc);

    WAITANIMEENDSIN.push_back(0);
    WAITANIMEENDSOUT.push_back(0);
    MOUSEHOVERCOUNT.push_back(0);
    MOUSELEAVECOUNT.push_back(INT_MAX);//--------------------------------------------- care
    MOUSEHOVERSIGNAL.push_back(0);
    CSIMAGE_GRADIENT mhg = {0};
    mhgradient.push_back(mhg);
    mouseLeaveRemoveBkg.push_back(0);
    lockedMode.push_back({0});
    //TITLE.push_back(CSTEXT{.Text=csAlloc<wchar_t>(1,'\0')}); // dont do that
    TITLE.push_back({0});
    setTitleInit.push_back(0);
    hdStackContext.push_back(0);
    hStackBmp.push_back(0);
    clkGradient.push_back({0});
    CLICKGRADIENTCOUNT.push_back(INT_MAX);
    cursor.push_back(LoadCursor(0,IDC_ARROW));
    entity.push_back(newVector<CSGRAPHIC_ENTITY>());
    entityMap.push_back({newVector<long>(),0,0});
    bltUpdate.push_back(0);


    vector<void(*)(CSARGS)> funcList;
    GROUPED_EVENTS_FUNC.push_back(funcList);
    vector<CSARGS> argList;
    GROUPED_EVENTS_ARGS.push_back(argList);

    rSizeBind.push_back(newVector<BIND_DIM_GEOM_PARAMS>());
    lSizeBind.push_back(newVector<BIND_DIM_GEOM_PARAMS>());
    tSizeBind.push_back(newVector<BIND_DIM_GEOM_PARAMS>());
    bSizeBind.push_back(newVector<BIND_DIM_GEOM_PARAMS>());

    TipsPopupParams.push_back({newVector<int>(), newVector<RECT>(), newVector<POS_BOOL>(), newVector<CSDYNAMIC_SIMPLE_TEXT>(),0,0,0,0});
    AutoTransform.push_back({0});
    AUTOTRANSCOUNT.push_back(INT_MAX);

    withHScroll.push_back(0);
    withVScroll.push_back(0);
    viewedAreaRightMarging.push_back(0);
    viewedAreaBottomMarging.push_back(0);
    bltRect.push_back(geom);
    hZoom.push_back({1});
    vZoom.push_back({1});
    
    hdcontextExtBkgColor.push_back({0});
    hdcontextExtBrdColor.push_back({0});

    halftoneMode.push_back(1);
    TIMER_PARAMS.push_back({0});
    loadedImage.push_back(newVector<CSLOADED_IMAGE>());

    updateAfterResizeMsg.push_back(0);

    dynSimpleText.push_back({newVector<CSTEXT>(),newVector<int>(),0});

    updateTitleSectionBool.push_back(0);
    zoomParams.push_back({0.2, 15, 0.2, 15, 1, 1, 100, {GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)}});

    HWND par = id < 0 ? 0 : SECTION[id];

    PAR.push_back(par);
    //cout<<par<<"par\n";

    if(id == -1)
    {
        WNDCLASSEXW* wc = (WNDCLASSEXW*)malloc(sizeof(WNDCLASSEXW));
        wc->hCursor = LoadCursor (NULL, IDC_ARROW);
        wc->lpfnWndProc = sectionProc;
        wc->hInstance = _hInstance;
        wc->lpszClassName = L"CSigmaWindow\0";
        wc->style = CS_DBLCLKS;                 /* Catch double-clicks */
        //wc->hbrBackground = CreateSolidBrush(color);
        wc->cbSize = sizeof(WNDCLASSEX);
        RegisterClassExW(wc);
        wndClass.push_back(wc);

    }
    
    HWND hPopup;
    if(id == -1 || isRoot)
    {
        SECTIONSTYLE.push_back(0);
        hPopup = CreateWindowExW(
                     WS_EX_APPWINDOW, wndClass[0]->lpszClassName, L"CSigma Root\0", WS_POPUPWINDOW|WS_BORDER|WS_THICKFRAME|WS_MINIMIZEBOX/*|WS_MAXIMIZEBOX*/,
                     geom.left, geom.top, geom.right, geom.bottom,
                     par, 0, _hInstance, nullptr
                 );

    }
    else
    {

        SECTIONSTYLE.push_back(1);
        hPopup = CreateWindowExW(
                     0,               // fenêtre sans bouton barre des tâches
                     wndClass[0]->lpszClassName,              // nom de la classe
                     NULL,                           // titre (aucun)
                     WS_CHILD|WS_POPUP,      // style popup + redimensionnable
                     geom.left, geom.top, geom.right, geom.bottom,             // position et taille initiale
                     par, 0, _hInstance, NULL
                 );

    }
    if(attach && par != 0)
    {
        SetParent(hPopup, par);
        attached.push_back(1);
    }
    else
    {
        attached.push_back(0);
    }

    RegisterTouchWindow(hPopup, TWF_WANTPALM);

    
    InvalidateRect(hPopup,0,1);
    if(show)
        ShowWindow(hPopup, SW_SHOWNORMAL);
    UpdateWindow(hPopup);
    SECTION.push_back(hPopup);
    hdcontext.push_back(GetDC(hPopup));
    hdcontextExt.push_back(0);
    hdcontextExtInPos.push_back({0});
    hdcontextExtOutPos.push_back({0});
    hdcontextExtSize.push_back({0});
    hdBmpExt.push_back(0);
    SetTimer(hPopup, 0, 50, 0);


    hdBkgContext.push_back(CreateCompatibleDC(hdcontext[i]));
    hBkgBmp.push_back(CreateCompatibleBitmap(hdcontext[i],hdcSize[i].cx, hdcSize[i].cy));
    SelectBitmap(hdBkgContext[i], hBkgBmp[i]);
    HBRUSH hb = CreateSolidBrush(color);
    SelectBrush(hdBkgContext[i],hb);
    HPEN hp = CreatePen(0,0,color);
    SelectPen(hdBkgContext[i], hp);
    Rectangle(hdBkgContext[i], 0,0, hdcSize[i].cx, hdcSize[i].cy);

    BitBlt(hdcontext[i],0,0,hdcSize[i].cx, hdcSize[i].cy, hdBkgContext[i], 0,0, SRCCOPY);

    /*
    BOOL isDwmEnabled = FALSE;
    MARGINS shadow = {1, 1, 1, 1}; // petite extension = effet d’ombre subtil
    if (SUCCEEDED(DwmIsCompositionEnabled(&isDwmEnabled)) && isDwmEnabled) {
        DwmExtendFrameIntoClientArea(hPopup, &shadow);
    }*/
 
    /*LONG_PTR style = GetWindowLongPtr(hPopup, GWL_STYLE);
    style &= ~(WS_CHILD);
    style |= WS_POPUP | WS_THICKFRAME;
    SetWindowLongPtr(hPopup, GWL_STYLE, style);
    SetWindowPos(hPopup, NULL, 0, 0, 0, 0,
             SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE);*/

    //if(SECTIONSTYLE[i])
    {
        GetWindowRect(SECTION[i], &RECTWND[i]);
        GetWindowRect(SECTION[i], &RECTWNDSAVED[i]);
        GetClientRect(SECTION[i], &RECTCL[i]);
        RECTPARREFSAVED[i] = CSUTILS::rectInParentRef(i);
        RECTPARREF[i] = RECTPARREFSAVED[i];
    }
             
    return SECTION.size()-1;
}

int getId(HWND hwnd)
{
    int n = SECTION.size();
    for(int i=0; i<n; i++)
    {
        if(hwnd == SECTION[i])
        {
            return i;
        }
    }
    return -1;
}


int pid = 0;
void _blitExtDc(int id);
extern void __getHiddenWindowPart(int id, RECT r);
LRESULT CALLBACK sectionProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    int id = getId(hwnd);
    static RECT rwsave;
    pid = id;

    static bool bCreate = 0;
    static int xsframe = GetSystemMetrics(SM_CXSIZEFRAME);
    static int ysframe = GetSystemMetrics(SM_CYSIZEFRAME);
    static int capt = GetSystemMetrics(SM_CYCAPTION);
    
    if(id > -1)
    {

        //csGraphics::titleBarDesign(hwnd, msg, wParam, lParam, id);

        static bool bact = 0;

        /*if(msg == WM_GETMINMAXINFO)
        {
            MINMAXINFO* mmi = (MINMAXINFO*)lParam;

            MONITORINFO mi = { sizeof(mi) };
            HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            if (GetMonitorInfo(monitor, &mi))
            {
                RECT rcWork = mi.rcWork;   // zone utilisable (sans la barre des tâches)
                RECT rcMonitor = mi.rcMonitor; // zone totale de l'écran

                // Si rcWork != rcMonitor, cela signifie qu’on est potentiellement en train de snapper ou maximiser
                if (!EqualRect(&rcWork, &rcMonitor))
                {
                    // La fenêtre va probablement être redimensionnée automatiquement
                    // Tu peux réagir ici
                    //cout<<"4554\n";
                }
            }

        }*/

        /*if(msg == WM_ACTIVATEAPP && id == 0)
        //if(msg == WM_APPCOMMAND)
        {
            //cout<<"act\n";
            // if(LOWORD(wParam) != WA_INACTIVE)
            // {
            //     if(IsMinimized(hwnd))
            //         ShowWindow(hwnd, SW_RESTORE);
            //     else
            //         ShowWindow(hwnd, SW_MINIMIZE);
            // }
           POINT p = TIMER_POINT;
           RECT r = taskbarInfo.rect;

           if(PtInRect(&r, p))
           {
                if(!IsMinimized(SECTION[0]) && !bact)
                {
                    ShowWindow(SECTION[0], SW_MINIMIZE);
                    bact = 1;
                }
                else if(bact)
                {
                    ShowWindow(SECTION[0], SW_RESTORE);
                    bact = 0;
                }
           }
            
        } */
        /*if(msg == WM_WINDOWPOSCHANGING)
        {
            WINDOWPOS* wp = (WINDOWPOS*)lParam;
            if (!(wp->flags & SWP_NOSIZE)) {
                // La fenêtre va changer de taille automatiquement
                // wp->cx et wp->cy contiennent la nouvelle taille
                cout<<"uhjhjk\n";
            }

        }
        if(msg == WM_NCCALCSIZE)
        {
            cout<<"hey !\n";
        }*/

        if (msg == WM_NCHITTEST)
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ScreenToClient(hwnd, &pt);

            RECT rc;
            GetClientRect(hwnd, &rc);
            const int BORDER_SIZE = 8;

            bool left = pt.x < BORDER_SIZE;
            bool right = pt.x > rc.right - BORDER_SIZE;
            bool top = pt.y < BORDER_SIZE;
            bool bottom = pt.y > rc.bottom - BORDER_SIZE;

            if(SECTIONSTYLE[id] == 1)
            {
                if (RESIZE_EDGE[id].tl && top && left)
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_TOPLEFT;
                    return HTTOPLEFT;
                }
                else if (RESIZE_EDGE[id].tr && top && right)
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_TOPRIGHT;
                    return HTTOPRIGHT;
                }
                else if (RESIZE_EDGE[id].bl && bottom && left)
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_BOTTOMLEFT;
                    return HTBOTTOMLEFT;
                }
                else if (RESIZE_EDGE[id].br && bottom && right)
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_BOTTOMRIGHT;
                    return HTBOTTOMRIGHT;
                }
                else if (RESIZE_EDGE[id].l && left)
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_LEFT;
                    return HTLEFT;
                }
                else if (RESIZE_EDGE[id].r && right)
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_RIGHT;
                    return HTRIGHT;
                }
                else if (RESIZE_EDGE[id].t && top)
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_TOP;
                    return HTTOP;
                }
                else if (RESIZE_EDGE[id].b && bottom)
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_BOTTOM;
                    return HTBOTTOM;
                }
                else
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_CLIENT;
                }
                return HTCLIENT; // partie centrale
            }
            else
            {

                if (RESIZE_EDGE[id].tl && top && left)
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_TOPLEFT;
                }
                else if (RESIZE_EDGE[id].tr && top && right)
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_TOPRIGHT;
                }
                else if (RESIZE_EDGE[id].bl && bottom && left)
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_BOTTOMLEFT;
                }
                else if (RESIZE_EDGE[id].br && bottom && right)
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_BOTTOMRIGHT;
                }
                else if (RESIZE_EDGE[id].l && left)
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_LEFT;
                }
                else if (RESIZE_EDGE[id].r && right)
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_RIGHT;
                }
                else if (RESIZE_EDGE[id].t && top)
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_TOP;
                }
                else if (RESIZE_EDGE[id].b && bottom)
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_BOTTOM;
                }
                else
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_CLIENT;
                }

                if(pt.x > BORDER_SIZE && !right && pt.y > BORDER_SIZE && pt.y< CAPTION_AREA_SIZE+BORDER_SIZE)
                {
                    CURSOR_NCHITTEST_POS = CURSOR_NCHITTEST_POS_CAPTION;
                    return HTCAPTION;
                }
            }

        }
        else if(msg == WM_SETCURSOR)
        {
            LRESULT hc;
            if(CURSOR_NCHITTEST_POS == CURSOR_NCHITTEST_POS_CLIENT)
            {
                hc = (LRESULT)SetCursor(cursor[id]);
            
                return hc;
            }
        }
if(msg == WM_GESTURE)
    {
cout<<"gesture\n";
    }
        

        if(msg == WM_LBUTTONDOWN)
        {
            GetCursorPos(&LBD_POINT);
        }

        else if(msg == WM_NCLBUTTONDOWN)
        {
            POINT p;
            GetCursorPos(&p);
            LRESULT hit = SendMessage(hwnd, WM_NCHITTEST, 0, MAKELPARAM(p.x, p.y));
            if(hit == HTCAPTION)
                inCaptionArea[id] = 1;
            else
                inCaptionArea[id] = 0;
        }
        static int test = 1;
        /*if(msg == WM_NCLBUTTONDOWN)
        {
            cout<<"54654";
        }*/
        if(msg == WM_SYSCOMMAND)
        {
            //long l = (wParam & 0xFFF0);
            cout<<"sys\n";
            if ((wParam & 0xFFF0) == SC_MAXIMIZE)
            {
                // L'utilisateur a cliqué sur le bouton "Maximiser" ou appuyé sur Win+Flèche haut
                // Tu peux refuser ou intercepter ici
                //cout<<SendMessage(hwnd, WM_ENTERSIZEMOVE, 0, 0)<<"\n";
                GetWindowRect(hwnd, &RECTWNDSAVED[id]);
                RECTPARREFSAVED[id] = CSUTILS::rectInParentRef(id);
                printRect(RECTWNDSAVED[id], " RECTWNDSAVED : ");

            }
            else if ((wParam & 0xFFF0) == SC_RESTORE)
            {
                GetWindowRect(hwnd, &RECTWNDSAVED[id]);
                RECTPARREFSAVED[id] = CSUTILS::rectInParentRef(id);
                printRect(RECTWNDSAVED[id], " RECTWNDSAVED : ");

                //wereMinimized[id] = 0;
            }

            else if((wParam & 0xFFF0) == SC_MINIMIZE)
            {
                wereMinimized[id] = 1;
            }


        }
        static RECT rsv;
        static bool moving = 0;
        static bool Restored = 0;

        if(msg == WM_ENTERSIZEMOVE)
        {
            // L'utilisateur commence a déplacer ou redimensionner
            //GetWindowRect(hwnd, &RECTWND[id]);
            //GetWindowRect(hwnd, &RECTWNDSAVED[id]);
            printRect(RECTWNDSAVED[id], " Saved 1 : ");
            //RECTPARREFSAVED[id] = rectInParentRef(id);
            hwndBtnDown = hwnd;
            rsv = RECTWNDSAVED[id];
            if((inCaptionArea[id] && moveMsgConnexion[id]) || !inCaptionArea[id])
            {

                test = 1;
                cout<<"++---------------enter-----------------------\n";
                enterSizeMove[id] = 1;
                EXECUTE_SIZEMOVE_WAIT_LIST_SIGNAL = 1;
                geometryBinding(id);
                sizeMoveWaitListExecute(id);
            }
        }
        if(msg == WM_SIZE)
        {
            GetClientRect(hwnd, &RECTCL[id]);
            GetWindowRect(hwnd, &RECTWND[id]);
            RECTPARREF[id] = CSUTILS::rectInParentRef(id);

            if(withVScroll[id])
                SendMessage(SECTION[withVScroll[id]],WM_TIMER,0,0); //autoresize
            if(withHScroll[id])
                SendMessage(SECTION[withHScroll[id]],WM_TIMER,0,0); //autoresize


            if(BORDERTHICK[id] || TITLE[id].Text)
                InvalidateRect(sHandle(id), 0, 1);
            //InvalidateRect(sHandle(id), &TITLE[id].TextRect, 1);
            if(dynSimpleText[id].view)
            {
                bltUpdate[id] = 1;
            }

        }



        else if(msg == WM_MOVE)
        {
            GetClientRect(hwnd, &RECTCL[id]);
            GetWindowRect(hwnd, &RECTWND[id]);
            RECTPARREF[id] = CSUTILS::rectInParentRef(id);
            if(moveMsgConnexion[id])
            {
                DELTAPOS[id] = {RECTWND[id].left-RECTWNDSAVED[id].left, RECTWND[id].top-RECTWNDSAVED[id].top};
                DELTASIZE[id] = {RECTWND[id].right-RECTWNDSAVED[id].right, RECTWND[id].bottom-RECTWNDSAVED[id].bottom};
            }
            /*if((BORDERTHICK[id] || TITLE[id].Text) && ((CURSOR_NCHITTEST_POS == CURSOR_NCHITTEST_POS_TOP ||
                                CURSOR_NCHITTEST_POS == CURSOR_NCHITTEST_POS_LEFT ||
                                CURSOR_NCHITTEST_POS == CURSOR_NCHITTEST_POS_TOPLEFT)))
                InvalidateRect(sHandle(id), 0, 1);*/
        }
        else if(msg == WM_SIZING)
        {
            //if(!isMaximized[id])
            {
                DELTAPOS[id] = {RECTWND[id].left-RECTWNDSAVED[id].left, RECTWND[id].top-RECTWNDSAVED[id].top};
                DELTASIZE[id] = {RECTWND[id].right-RECTWNDSAVED[id].right, RECTWND[id].bottom-RECTWNDSAVED[id].bottom};
            }

        }
        else if(msg == WM_MOVING)
        {
            moving = 1;
        }

        else if(msg == WM_EXITSIZEMOVE)
        {

            // L'utilisateur a fini de déplacer ou redimensionner
            //DELTASIZE[id] = {0};
            //DELTAPOS[id] = {0};
            enterSizeMove[id] = 0;
            EXECUTE_SIZEMOVE_WAIT_LIST_SIGNAL = 0;
            hwndBtnDown = 0;
            printRect(RECTWNDSAVED[id], " Saved 2 exit -------------: ");

            if(moving)
            {
                int n = RECTWND.size();
                for(int i=0; i<n; i++)
                {
                    GetWindowRect(SECTION[i], &RECTWND[i]);
                    GetWindowRect(SECTION[i], &RECTWNDSAVED[i]);
                    RECTPARREFSAVED[i] = CSUTILS::rectInParentRef(i);
                }
                moving = 0;
                
                // redessiner la partie cachee pour eviter le noir
                int d = HIDDEN_WINDOW_PART[0];
                RECT r = {0,0, d, RECTCL[id].bottom};
                InvalidateRect(hwnd, &r,1);
                HIDDEN_WINDOW_PART[0] = {0};

                d = HIDDEN_WINDOW_PART[1];
                r = {0,0, RECTCL[id].right, d};
                InvalidateRect(hwnd, &r,1);
                HIDDEN_WINDOW_PART[1] = {0};

                d = HIDDEN_WINDOW_PART[2];
                r = {RECTCL[id].right-d, 0, RECTCL[id].right, RECTCL[id].bottom};
                InvalidateRect(hwnd, &r,1);
                HIDDEN_WINDOW_PART[2] = {0};

                d = HIDDEN_WINDOW_PART[3];
                r = {0,RECTCL[id].bottom-d, RECTCL[id].right, RECTCL[id].bottom};
                InvalidateRect(hwnd, &r,1);
                HIDDEN_WINDOW_PART[3] = {0};
                
                __getHiddenWindowPart(id, RECTWND[id]);
            }
            //cout<<"++---------------exit-----------------------\n";
            /*if(!test)
            {

                int a = 0, b = 0;
                if(!SECTIONSTYLE[id])
                {
                    a = xsframe;;
                    b = ysframe;
                }
                GetWindowRect(hwnd, &RECTWND[id]);
                printRect(RECTPARREFSAVED[id]);
                printRect(RECTWND[id]);
                DELTAPOS[id] = {RECTWND[id].left-RECTWNDSAVED[id].left,
                                RECTWND[id].top-RECTWNDSAVED[id].top};
                DELTASIZE[id] = {RECTWND[id].right-RECTWNDSAVED[id].right+RECTPARREFSAVED[id].left+a,
                                RECTWND[id].bottom-RECTWNDSAVED[id].bottom+RECTPARREFSAVED[id].top+a};
                sizeMoveAll(id);
                test = 0;
                //cout<<"45464----------------------\n";
            }*/

        }
        else if(msg ==  WM_NCLBUTTONUP) // USE EXITSIZEMOVE
        {
            rwsave = {0};
            hwndBtnDown = 0;
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }

        else if(msg == WM_NCLBUTTONDOWN)
        {
            rwsave = RECTWND[id];
            hwndBtnDown = hwnd;
            //cout<<"54656";
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }

        else if(msg == WM_DESTROY)
        {
            KillTimer(hwnd, 0);
            CSUIMAN::__setAllRects();
            CSFILESMAN::__saveAppSizes();
            CSFILESMAN::__saveAppTitles();
            CSFILESMAN::__saveAppTips();
            PostQuitMessage(0);
            return 0;
        }


        if(msg == WM_TIMER)
        {
            GetCursorPos(&TIMER_POINT);
            if(isMaximizedCount[id] > 0)
            {

                isMaximizedCount[id] = isMaximizedCount[id] - 1;
                if(isMaximizedCount[id] == 0)
                {
                    enterSizeMove[id] = 1;
                    EXECUTE_SIZEMOVE_WAIT_LIST_SIGNAL = 1;
                    SendMessage(hwnd, WM_EXITSIZEMOVE, 0,0);
                    //cout<<enterSizeMove[id]<<"         kjkljkl\n";
                }

            }

            if(initBool[id] == 0)
            {
                GetWindowRect(hwnd, &RECTWND[id]);
                GetWindowRect(hwnd, &RECTWNDSAVED[id]);
                GetClientRect(hwnd, &RECTCL[id]);
                RECTPARREFSAVED[id] = CSUTILS::rectInParentRef(id);
                initBool[id] = 1;
            }

            if(THREAD_END[id])
            {

                THREAD_END[id]--;
                if(((inCaptionArea[id] && moveMsgConnexion[id]) || !inCaptionArea[id]) && THREAD_END[id]==0)
                {
                    cout<<"++---------------out-----------------------\n";

                    GetClientRect(hwnd, &RECTCL[id]);
                    GetWindowRect(hwnd, &RECTWND[id]);
                    RECTPARREF[id] = CSUTILS::rectInParentRef(id);
                    RECTWNDSAVED[id] = rsv;
                    DELTAPOS[id] = {RECTWND[id].left-RECTWNDSAVED[id].left, RECTWND[id].top-RECTWNDSAVED[id].top};
                    DELTASIZE[id] = {RECTWND[id].right-RECTWNDSAVED[id].right, RECTWND[id].bottom-RECTWNDSAVED[id].bottom};
                    //DELTAPOS[id] = {RECTWND[id].left-RECTWNDSAVED[id].left, RECTWND[id].top-RECTWNDSAVED[id].top};
                    printRect(rsv, " Saved 2 : ");
                    printRect(RECTWND[id], " Now : ");
                    sizeMoveAll(id);
                }
                //THREAD_END[id] = 0;

            }

            CSUIFX::_mouseHover_colorGradient(hwnd, id); 
            if(mhgradient[id].init == 0)
            {
                InvalidateRect(hwnd, 0,1);
                mhgradient[id].init = 1;
            }

            if(id == 0)
            {
                taskbarInfo = CSUIMAN::getTaskbarInfo();
                if(taskbarInfo.autoHide) taskbarInfo.edge = 10;
            }

            if(LAST_TASKBAR_POS != taskbarInfo.edge && !SECTIONSTYLE[id])
            {
                if(isMaximized[id])
                {
                    RECT rc;
                    SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
                    RECT r = {rc.left-xsframe, rc.top-ysframe, rc.right+2*xsframe, rc.bottom+2*ysframe};

                    RECT rr = LAST_WORKAREA;
                    
                    sizeMoveAll(id, 0, {rc.right-rr.right, rc.bottom-rr.bottom}, {0});
                    SetWindowPos(hwnd, 0, r.left, r.top, r.right, r.bottom, SWP_NOZORDER);
                    
                    LAST_WORKAREA = r;
                }
                LAST_TASKBAR_POS = taskbarInfo.edge;
            }

            ///***************************************************************** */
            /*RECT rc;

            GetClientRect(hwnd, &rc);

            RECT rp;
            if(GetParent(hwnd) == 0)
            {
                rp.right = GetSystemMetrics(SM_CXSCREEN);
                rp.bottom = GetSystemMetrics(SM_CYSCREEN);
            }
            else
            {
                GetClientRect(GetParent(hwnd), &rp);
            }
            if(RECTPARREF[id].left >= rp.right || RECTWND[id].top >= rp.bottom ||
                RECTPARREF[id].right <= 0 || RECTWND[id].bottom <= 0)
            {
                

                SetWindowPos(hwnd,0, (rp.right-rc.right)/2, (rp.bottom-rc.bottom)/2, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
            }*/
            /***************************************************************************** */

            CSUIMAN::_updateApp(0);

        }

        CSUIFX::_mouseHoverLeave(hwnd, msg, id);
        CSUIFX::_mouseHover_clickEffect(hwnd, msg, id);
        CSUIFX::_autoTransformation(hwnd, msg, wParam, lParam, id);
        CSUIFX::_mouseHover_movePopup(hwnd, msg, wParam, lParam, id);


        if(msg == WM_PAINT && !moving)
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &RECTCL[id]);
            RECT r = RECTCL[id];
            long penCol = borderColor[id];
            int thick2 = BORDERTHICK[id]/2;
            POINT p[] =
            {
                {thick2,thick2},
                {r.right-thick2,thick2},
                {r.right-thick2,r.bottom-thick2},
                {thick2,r.bottom-thick2},
                {thick2,thick2},
            };

            HPEN hp;

            if(mhgradient[id].TimeStep)
            {
                
                if(SECTIONSTYLE[id])
                {
                    
                    if(mhgradient[id].TitleGradient)
                        TITLE[id].Color = mhgradient[id].ActiveTitleColor;
                    if(mhgradient[id].BorderGradient)
                    {
                        CSRGBA c = mhgradient[id].ActiveBorderColor;
                        penCol = RGB(c.r, c.g, c.b);
                    }

                    if(mhgradient[id].BackgroundGradient)
                    {
                        CSRGBA col = mhgradient[id].ActiveColor;
                        long c = RGB(col.r, col.g, col.b);
                        HBRUSH hb = CreateSolidBrush(c);
                        HPEN hpp = CreatePen(0,1,c);
                        SelectBrush(hdBkgContext[id], hb);
                        SelectPen(hdBkgContext[id], hpp);
                        Rectangle(hdBkgContext[id], 0,0,RECTCL[id].right, RECTCL[id].bottom);
                        DeleteBrush(hb);
                        DeletePen(hpp);
                    }
                }

            }


            if(SECTIONSTYLE[id] && TITLE[id].Text && TITLE[id].Show)
            {
                //HDC
                BitBlt(hdStackContext[id],0,0,RECTCL[id].right, RECTCL[id].bottom, hdBkgContext[id], 0,0, SRCCOPY);
                
                if(mhgradient[id].ImageGradient)
                {
                    HDC idc = imageGradients[id][mhgradient[id].ActiveImageIndex].dc;
                    POINT pos = mhgradient[id].Position;
                    SIZE sz = mhgradient[id].Size;
                    //StretchBlt(hdStackContext[id],pos.x*xdimFact,pos.y*ydimFact,sz.cx*xdimFact, sz.cy*ydimFact, idc, 0,0,sz.cx, sz.cy, SRCCOPY);
                    BitBlt(hdStackContext[id],pos.x,pos.y,sz.cx, sz.cy, idc, 0,0, SRCCOPY);
                }
                _blitExtDc(id);
                _drawTitle(id, hdStackContext[id]);

                if(BORDERTHICK[id])
                {
                    hp = CreatePen(0,2*thick2, penCol);
                    SelectPen(hdStackContext[id], hp);
                    Polyline(hdStackContext[id], p, 5);
                    DeletePen(hp);
                }
                BitBlt(hdc,0,0,RECTCL[id].right, RECTCL[id].bottom, hdStackContext[id], 0,0, SRCCOPY);

            }
            else if(BORDERTHICK[id])
            {
                BitBlt(hdStackContext[id],0,0,RECTCL[id].right, RECTCL[id].bottom, hdBkgContext[id], 0,0, SRCCOPY);
                if(mhgradient[id].ImageGradient)
                {
                    HDC idc = imageGradients[id][mhgradient[id].ActiveImageIndex].dc;
                    POINT pos = mhgradient[id].Position;
                    SIZE sz = mhgradient[id].Size;
                    //StretchBlt(hdStackContext[id],pos.x*xdimFact,pos.y*ydimFact,sz.cx*xdimFact, sz.cy*ydimFact, idc, 0,0,sz.cx, sz.cy, SRCCOPY);
                    BitBlt(hdStackContext[id],pos.x,pos.y,sz.cx, sz.cy, idc, 0,0, SRCCOPY);
                }
                _blitExtDc(id);
                hp = CreatePen(0,2*thick2, penCol);
                SelectPen(hdStackContext[id], hp);
                Polyline(hdStackContext[id], p, 5);
                DeletePen(hp);

                BitBlt(hdc,0,0,RECTCL[id].right, RECTCL[id].bottom, hdStackContext[id], 0,0, SRCCOPY);
            }
            else if(mhgradient[id].ImageGradient)
            {
                BitBlt(hdStackContext[id],0,0,RECTCL[id].right, RECTCL[id].bottom, hdBkgContext[id], 0,0, SRCCOPY);
                HDC idc = imageGradients[id][mhgradient[id].ActiveImageIndex].dc;
                POINT pos = mhgradient[id].Position;
                SIZE sz = mhgradient[id].Size;
                //cout<<xdimFact<<"\n";
                _blitExtDc(id);
                //StretchBlt(hdStackContext[id],pos.x*xdimFact,pos.y*ydimFact,sz.cx*xdimFact, sz.cy*ydimFact, idc, 0,0,sz.cx, sz.cy, SRCCOPY);
                BitBlt(hdStackContext[id],pos.x,pos.y,sz.cx, sz.cy, idc, 0,0, SRCCOPY);
                BitBlt(hdc,0,0,RECTCL[id].right, RECTCL[id].bottom, hdStackContext[id], 0,0, SRCCOPY);
            }
            else if(hdcontextExt[id])
            {
                BitBlt(hdStackContext[id],0,0,RECTCL[id].right, RECTCL[id].bottom, hdBkgContext[id], 0,0, SRCCOPY);
                _blitExtDc(id);
                BitBlt(hdc,0,0,RECTCL[id].right, RECTCL[id].bottom, hdStackContext[id], 0,0, SRCCOPY);
            }
            else
            {
                BitBlt(hdc,0,0,RECTCL[id].right, RECTCL[id].bottom, hdBkgContext[id], 0,0, SRCCOPY);
                _drawTitle(id, hdc);
            }
            //_blitDynamicSimpleText(id);
            //BitBlt(hdcontext[id],0,0,hdcSize[id].cx, hdcSize[id].cy, hdBkgContext[id], 0,0, SRCCOPY);


            EndPaint(hwnd, &ps);
            return 0;
        }
        else if(msg == WM_ERASEBKGND)
        {
            return 1;
        }

        else if(msg == WM_CTLCOLOREDIT)
        {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, RGB(220, 220, 220));      // texte clair
            SetBkColor(hdcStatic, RGB(20,20,20));            // fond fonc�
            //SetBkMode(hdcStatic, TRANSPARENT);
            static HBRUSH hBrush = CreateSolidBrush(RGB(20,20,20));
            return (LRESULT)hBrush;

        }

        int n = GROUPED_EVENTS_ARGS[id].size();
        for(int i=0; i<n; i++)
        {
            if(HIDEGROUPMSG[id][i] == 0)
            {
                GROUPED_EVENTS_ARGS[id][i].setProcParams(&hwnd,&msg,&wParam,&lParam,&pid);
                (GROUPED_EVENTS_FUNC[id][i])(GROUPED_EVENTS_ARGS[id][i]);
                LRESULT lr = *(LRESULT*)GROUPED_EVENTS_ARGS[id][i].getLRESULT();
                if(lr)
                {
                    *(LRESULT*)GROUPED_EVENTS_ARGS[id][i].getLRESULT() = 0;
                    return lr;
                }
            }
        }
        
        if(msg == WM_LBUTTONDOWN || msg == WM_NCLBUTTONDOWN)
        {
            //SetForegroundWindow(SECTION[0]);
            /*HWND topWnd = GetWindow(SECTION[0], GW_HWNDLAST);
            HWND fwnd = GetForegroundWindow();
            char className[10000];
            GetClassNameA(fwnd, (LPSTR)className, 10000);
            cout<<SECTION[0]<<" "<<className<<"\n";*/
            
            SetWindowPos(SECTION[0], HWND_TOP, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
            
        }
        /*if(msg == WM_NCLBUTTONDBLCLK && CURSOR_NCHITTEST_POS == CURSOR_NCHITTEST_POS_CAPTION && !SECTIONSTYLE[id])
        {
            // L'utilisateur commence a déplacer ou redimensionner
            
            if(isMaximized[id] == 0)
            {
                RECTRESTORE[id] = RECTWND[id];
                RECT rc;
                SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
                RECT r = {rc.left-xsframe, rc.top-ysframe, rc.right+2*xsframe, rc.bottom+2*ysframe};
                LAST_WORKAREA = rc;
                
                RECT rr;
                GetClientRect(hwnd, &rr);
                sizeMoveAll(id, 0, {rc.right-rr.right, rc.bottom-rr.bottom}, {0});
                SetWindowPos(hwnd, 0, r.left, r.top, r.right, r.bottom, SWP_NOZORDER);
                //AnimateWindow(hwnd, 200, AW_BLEND | AW_ACTIVATE);

                isMaximized[id] = 1;
            }
            else
            {
                rwsave = RECTRESTORE[id];
                RECT rc = LAST_WORKAREA;

                sizeMoveAll(id, 0, {rwsave.right-rwsave.left-rc.right-2*xsframe, rwsave.bottom-rwsave.top-rc.bottom-2*ysframe}, {0});
                SetWindowPos(hwnd, 0, rwsave.left, rwsave.top, rwsave.right-rwsave.left, rwsave.bottom-rwsave.top, SWP_NOZORDER);

                isMaximized[id] = 0;
            }
        }*/

       /*GESTUREINFO gi;
       ZeroMemory(&gi, sizeof(GESTUREINFO));
       gi.cbSize = sizeof(GESTUREINFO);

       BOOL bres = GetGestureInfo((HGESTUREINFO)lParam, &gi);
       BOOL bHandle = 0;

       if(bres)
       {cout<<"hey |||||\n";
            switch (gi.dwID)
            {
            case GID_ZOOM:
                
                bHandle = 1;
                break;
            
            default:
                break;
            }
       }*/

    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void _blitImage(int id, int i);
void _blitEntities(int id);
void _blitExtDc(int id)
{
    if(hdcontextExt[id])
    {
        POINT pin = hdcontextExtInPos[id];
        POINT pout = hdcontextExtOutPos[id];
        //cout<<id<<"  "<<RECTCL[id].right<<"  "<<RECTCL[id].bottom<<"\n";
        if(!withHScroll[id] && !withVScroll[id])
        {
            _blitEntities(id);
            BitBlt(hdStackContext[id],pout.x,pout.y,RECTCL[id].right, RECTCL[id].bottom, hdcontextExt[id], pin.x,pin.y, SRCCOPY);
        }
        else
        {
            if(halftoneMode[id])
                SetStretchBltMode(hdStackContext[id], HALFTONE);
            else
                SetStretchBltMode(hdStackContext[id], COLORONCOLOR);

            _blitEntities(id);
            StretchBlt(hdStackContext[id],pout.x,pout.y,bltRect[id].right, bltRect[id].bottom, 
                        hdcontextExt[id], pin.x + zoomParams[id].focus.x, pin.y + zoomParams[id].focus.y, 
                        bltRect[id].right/hZoom[id], bltRect[id].bottom/vZoom[id], SRCCOPY);
        }
    }
}

void _blitEntities(int id)
{
    if(bltUpdate[id])
    {
        int n = entity[id].size();
        for(int i=0; i<n; i++)
        {
            int idt = entity[id][i].tid;
            if(entity[id][i].type == CSG_ENTITY_IMAGE)
            {
                _blitImage(id, idt);
            }
        }
        _blitDynamicSimpleText(id);
        bltUpdate[id] = 0;
    }
}

void _blitImage(int id, int i)
{
    CSLOADED_IMAGE li = loadedImage[id][i];
    if(li.show)
    {
        if(li.eraseBkg)
        TransparentBlt(hdcontextExt[id], li.outPos.x, li.outPos.y, li.outSize.cx, li.outSize.cy, 
        li.outDc, li.inPos.x, li.inPos.y, li.inSize.cx, li.inSize.cy, li.bkgColor);
        else
        StretchBlt(hdcontextExt[id], li.outPos.x, li.outPos.y, li.outSize.cx, li.outSize.cy, 
        li.outDc, li.inPos.x, li.inPos.y, li.inSize.cx, li.inSize.cy, SRCCOPY);
    }
}

extern void viewDynamicSimpleText(int id, vector<CSTEXT> paragraph, vector<int> pSpace, RECT marg, bool updateGASize);
void _blitDynamicSimpleText(int id)
{
    if(dynSimpleText[id].view)
    {
        viewDynamicSimpleText(id, dynSimpleText[id].paragraph, dynSimpleText[id].pSpace, dynSimpleText[id].marg, dynSimpleText[id].updateGASize);
        
    }
}

void lockSizeMove(std::vector<std::vector<BIND_DIM_GEOM_PARAMS>> sizeBind, int id)
{
    int n = sizeBind[id].size();
    for(int i=0; i<n; i++)
    {
        int iid = sizeBind[id][i].id;
        if(lockSizeMoveId[iid] == -1)
            lockSizeMoveId[iid] = id;
    }
}
/*void unlockSizeMove(std::vector<std::vector<BIND_DIM_GEOM_PARAMS>> sizeBind, int id)
{
    int n = sizeBind[id].size();
    for(int i=0; i<n; i++)
    {
        int iid = sizeBind[id][i].id;
        //if(lockSizeMoveId[iid] == id)
            lockSizeMoveId[iid] = -1;
    }
}*/
void unlockSizeMove(int id, std::vector<BIND_DIM_GEOM_PARAMS> sizeBind, int n)
{
    for(int i=0; i<n; i++)
    {
        int iid = sizeBind[i].id;
        lockSizeMoveId[iid] = -1;
    }
}

void setSizeMoveWaitList(std::vector<BIND_DIM_GEOM_PARAMS> sizeBind, int id)
{
    int n = sizeBind.size();
    for(int i=0; i<n; i++)
    {
        int iid = sizeBind[i].id;
        if(lockSizeMoveId[iid] == id)
        {
            isWindowInSizeMoveWaitList[iid] = 1;
        }
    }
}
void unSetSizeMoveWaitList(std::vector<BIND_DIM_GEOM_PARAMS> sizeBind)
{
    int n = sizeBind.size();
    for(int i=0; i<n; i++)
    {
        int iid = sizeBind[i].id;
        isWindowInSizeMoveWaitList[iid] = 0;
        deltaSizeMoveWaitList[iid] = {0,0,0,0};
    }
}
void geometryBinding(int& _id);
void sendGeometryBindingToDest(std::vector<std::vector<BIND_DIM_GEOM_PARAMS>> sizeBind, int id)
{
    int n = sizeBind[id].size();
    for(int i=0; i<n; i++)
    {
        int iid = sizeBind[id][i].id;
        //GetWindowRect(SECTION[iid], &RECTWND[iid]);
        //GetWindowRect(SECTION[iid], &RECTWNDSAVED[iid]);
        //RECTPARREFSAVED[iid] = rectInParentRef(iid);
        //enterSizeMove[iid] = 1;

        //geometryBinding(iid);
        //SendMessage(SECTION[lSizeBind[_id][i].id], WM_ENTERSIZEMOVE, 0, 0);
    }
}

int getRightParam(int id, int iLoop, char edge, BIND_DIM_GEOM_PARAMS& bd)
{
    if(edge == 'l')
    {
        bd = lSizeBind[id][iLoop];
        return DELTAPOS[id].x;
    }
    if(edge == 't')
    {
        bd = tSizeBind[id][iLoop];
        return DELTAPOS[id].y;
    }
    if(edge == 'r')
    {
        bd = rSizeBind[id][iLoop];
        return DELTASIZE[id].cx;
    }
    if(edge == 'l')
    {
        bd = bSizeBind[id][iLoop];
        return DELTASIZE[id].cy;
    }
}

void sizeMoveCore(int id, std::vector<BIND_DIM_GEOM_PARAMS> sizeBind, int n, int delta)
{
    if(delta)
        for(int i=0; i<n; i++)
        {
            BIND_DIM_GEOM_PARAMS bd = sizeBind[i];
            int diff = delta*bd.cf;
            HWND hwnd = SECTION[bd.id];
            RECT r = RECTPARREFSAVED[bd.id];

            if(lockSizeMoveId[bd.id] != id)
            {
                continue;
            }
//cout<<bd.flag<<"   --flag\n";
            if(bd.flag == BIND_DEST_LEFT_EDGE)
            {
                deltaSizeMoveWaitList[bd.id].left = diff;
                //SetWindowPos(hwnd, 0, r.left + diff, r.top, 0,0, SWP_NOSIZE|SWP_NOZORDER);
            }
            else if(bd.flag == (BIND_DEST_LEFT_EDGE|BIND_DEST_RIGHT_EDGE_ONE))
            {
                deltaSizeMoveWaitList[bd.id].left = diff;
                deltaSizeMoveWaitList[bd.id].right = (1.0-bd.cf)*delta;
                //SetWindowPos(hwnd, 0, r.left + diff, r.top, 0,0, SWP_NOSIZE|SWP_NOZORDER);
            }
            else if(bd.flag == (BIND_DEST_LEFT_EDGE_OPP|BIND_DEST_RIGHT_EDGE_ONE))
            {
                deltaSizeMoveWaitList[bd.id].left = -diff;
                deltaSizeMoveWaitList[bd.id].right = (1.0-bd.cf)*delta;
                //SetWindowPos(hwnd, 0, r.left + diff, r.top, 0,0, SWP_NOSIZE|SWP_NOZORDER);
            }
            else if(bd.flag == (BIND_DEST_LEFT_EDGE|BIND_DEST_RIGHT_EDGE))
            {
                deltaSizeMoveWaitList[bd.id].left = diff;
                deltaSizeMoveWaitList[bd.id].right = -diff;
                //SetWindowPos(hwnd, 0, r.left + diff, r.top, r.right-r.left - diff, r.bottom-r.top,  SWP_NOZORDER);
            }
            else if(bd.flag == BIND_DEST_RIGHT_EDGE)
            {
                deltaSizeMoveWaitList[bd.id].right = diff;
                //SetWindowPos(hwnd, 0, 0, 0, r.right-r.left + diff, r.bottom-r.top, SWP_NOZORDER|SWP_NOMOVE);
            }
            else if(bd.flag == (BIND_DEST_RIGHT_EDGE|BIND_DEST_LEFT_EDGE_ONE))
            {
                deltaSizeMoveWaitList[bd.id].left = (1.0-bd.cf)*delta;
                deltaSizeMoveWaitList[bd.id].right = diff;
                //SetWindowPos(hwnd, 0, 0, 0, r.right-r.left + diff, r.bottom-r.top, SWP_NOZORDER|SWP_NOMOVE);
            }
            else if(bd.flag == (BIND_DEST_RIGHT_EDGE_ONE|BIND_DEST_LEFT_EDGE_ONE))//---------------------
            {
                deltaSizeMoveWaitList[bd.id].left = (1.0-bd.cf)*delta;
                deltaSizeMoveWaitList[bd.id].right = -diff;
                //SetWindowPos(hwnd, 0, 0, 0, r.right-r.left + diff, r.bottom-r.top, SWP_NOZORDER|SWP_NOMOVE);
            }
            else if(bd.flag == (BIND_DEST_RIGHT_EDGE_OPP|BIND_DEST_LEFT_EDGE))//-----------------------
            {
                deltaSizeMoveWaitList[bd.id].left = diff;
                deltaSizeMoveWaitList[bd.id].right = diff ;
                //SetWindowPos(hwnd, 0, 0, 0, r.right-r.left + diff, r.bottom-r.top, SWP_NOZORDER|SWP_NOMOVE);
            }
            else if(bd.flag == BIND_DEST_TOP_EDGE)
            {
                deltaSizeMoveWaitList[bd.id].top = diff;
                //SetWindowPos(hwnd, 0, r.left, r.top + diff, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
            }
            else if(bd.flag == (BIND_DEST_TOP_EDGE|BIND_DEST_BOTTOM_EDGE_ONE))
            {
                deltaSizeMoveWaitList[bd.id].top = diff;
                deltaSizeMoveWaitList[bd.id].bottom = (1.0-bd.cf)*delta;
                //SetWindowPos(hwnd, 0, r.left, r.top + diff, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
            }
            else if(bd.flag == (BIND_DEST_TOP_EDGE_OPP|BIND_DEST_BOTTOM_EDGE_ONE))
            {
                deltaSizeMoveWaitList[bd.id].top = -diff;
                deltaSizeMoveWaitList[bd.id].bottom = (1.0-bd.cf)*delta;
                //SetWindowPos(hwnd, 0, r.left, r.top + diff, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
            }
            else if(bd.flag == (BIND_DEST_TOP_EDGE|BIND_DEST_BOTTOM_EDGE))
            {
                deltaSizeMoveWaitList[bd.id].top = diff;
                deltaSizeMoveWaitList[bd.id].bottom = -diff;
                //SetWindowPos(hwnd, 0, r.left, r.top + diff,r.right-r.left, r.bottom-r.top - diff, SWP_NOZORDER);
            }
            else if(bd.flag == BIND_DEST_BOTTOM_EDGE)
            {
                deltaSizeMoveWaitList[bd.id].bottom = diff;
                //SetWindowPos(hwnd, 0, 0, 0,r.right-r.left, r.bottom-r.top + diff, SWP_NOZORDER|SWP_NOMOVE);
            }
            else if(bd.flag == (BIND_DEST_BOTTOM_EDGE|BIND_DEST_TOP_EDGE_ONE))
            {
                deltaSizeMoveWaitList[bd.id].top = (1.0-bd.cf)*delta;
                deltaSizeMoveWaitList[bd.id].bottom = diff;
                //SetWindowPos(hwnd, 0, r.left, r.top + diff, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
            }
            else if(bd.flag == (BIND_DEST_BOTTOM_EDGE_OPP|BIND_DEST_TOP_EDGE))//--------------------------
            {
                deltaSizeMoveWaitList[bd.id].top = diff;
                deltaSizeMoveWaitList[bd.id].bottom = diff;
                //SetWindowPos(hwnd, 0, r.left, r.top + diff, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
            }

        }

}

void delay(int del = 10)
{
    std::this_thread::sleep_for(std::chrono::microseconds(del));
}

void endThread(int*id, std::vector<BIND_DIM_GEOM_PARAMS> sizeBind, int n, int& additionalIter)
{
    if(enterSizeMove[*id] == 0)
    {
        additionalIter--;
        if(additionalIter == 0)
        {
            for(int i=0; i<n; i++)
            {
                enterSizeMove[sizeBind[i].id] = 0;
            }
            unlockSizeMove(*id, sizeBind, n);

            //DELTASIZE[*id] = {0};
            //DELTAPOS[*id] = {0};
            //GetWindowRect(SECTION[*id], &RECTWNDSAVED[*id]);
            unSetSizeMoveWaitList(sizeBind);
            free(id);
        }
    }
}

void endSizeMove(int id, std::vector<BIND_DIM_GEOM_PARAMS> sizeBind, int n)
{
    for(int i=0; i<n; i++)
    {
        int iid = sizeBind[i].id;
        enterSizeMove[iid] = 0;
        GetWindowRect(SECTION[iid], &RECTWND[iid]);
        GetWindowRect(SECTION[iid], &RECTWNDSAVED[iid]);
        GetClientRect(SECTION[iid], &RECTCL[iid]);
        RECTPARREFSAVED[iid] = CSUTILS::rectInParentRef(iid);
        if(updateAfterResizeMsg[iid])
            InvalidateRect(SECTION[iid], 0, 1);
    }
    unlockSizeMove(id, sizeBind, n);
    //DELTASIZE[id] = {0};
    //DELTAPOS[id] = {0};
    //GetWindowRect(SECTION[id], &RECTWNDSAVED[id]);
    unSetSizeMoveWaitList(sizeBind);
}

int delayVal = 750;

void geometryBinding(int& _id)
{
    if(SECTION[_id] == hwndBtnDown)
    {
        if(lSizeBind[_id].size() && ((CURSOR_NCHITTEST_POS == CURSOR_NCHITTEST_POS_LEFT ||
                                      CURSOR_NCHITTEST_POS == CURSOR_NCHITTEST_POS_TOPLEFT ||
                                      CURSOR_NCHITTEST_POS == CURSOR_NCHITTEST_POS_BOTTOMLEFT) || isMaximizedCount[_id]) )
        {
            lockSizeMove(lSizeBind, _id);
            sendGeometryBindingToDest(lSizeBind, _id);
            setSizeMoveWaitList(lSizeBind[_id], _id);

            int* id = csAlloc<int>(1,_id);

            thread t(
                [id]()
            {
                int n = lSizeBind[*id].size();
                int a = 1;
                while (a > 0)
                {
                    SIZE deltaSize = DELTASIZE[*id];
                    POINT deltaPos = DELTAPOS[*id];

                    sizeMoveCore(*id, lSizeBind[*id], n, deltaPos.x);
                    endThread(id, lSizeBind[*id], n, a);

                    delay(delayVal);
                }

            }
            );
            t.detach();
        }

        if(tSizeBind[_id].size() && ((CURSOR_NCHITTEST_POS == CURSOR_NCHITTEST_POS_TOP ||
                                      CURSOR_NCHITTEST_POS == CURSOR_NCHITTEST_POS_TOPRIGHT ||
                                      CURSOR_NCHITTEST_POS == CURSOR_NCHITTEST_POS_TOPLEFT) || isMaximizedCount[_id]) )
        {
            lockSizeMove(tSizeBind, _id);
            sendGeometryBindingToDest(tSizeBind, _id);
            setSizeMoveWaitList(tSizeBind[_id], _id);

            int* id = csAlloc<int>(1,_id);

            thread t(
                [id]()
            {
                int n = tSizeBind[*id].size();
                int a = 1;
                while (a > 0)
                {
                    SIZE deltaSize = DELTASIZE[*id];
                    POINT deltaPos = DELTAPOS[*id];

                    sizeMoveCore(*id, tSizeBind[*id], n, deltaPos.y);
                    endThread(id, tSizeBind[*id], n, a);
                    delay(delayVal+5);
                }

            }
            );
            t.detach();
        }

        if(rSizeBind[_id].size() && ((CURSOR_NCHITTEST_POS == CURSOR_NCHITTEST_POS_RIGHT ||
                                      CURSOR_NCHITTEST_POS == CURSOR_NCHITTEST_POS_TOPRIGHT ||
                                      CURSOR_NCHITTEST_POS == CURSOR_NCHITTEST_POS_BOTTOMRIGHT) || isMaximizedCount[_id]) )
        {
            lockSizeMove(rSizeBind, _id);
            sendGeometryBindingToDest(rSizeBind, _id);
            setSizeMoveWaitList(rSizeBind[_id], _id);

            int* id = csAlloc<int>(1,_id);

            thread t(
                [id]()
                {
                    int a = 1;
                    int n = rSizeBind[*id].size();
                    while (a > 0)
                    {
                        SIZE deltaSize = DELTASIZE[*id];
                        POINT deltaPos = DELTAPOS[*id];

                        sizeMoveCore(*id, rSizeBind[*id], n, deltaSize.cx);
                        endThread(id, rSizeBind[*id], n, a);
                        delay(delayVal-1);
                    }

                }
            );
            t.detach();
        }
        //cout<<CURSOR_NCHITTEST_POS<<" "<<bSizeBind[_id].size()<<"  -----------------------\n";
        if(bSizeBind[_id].size() && ((CURSOR_NCHITTEST_POS == CURSOR_NCHITTEST_POS_BOTTOM ||
                                      CURSOR_NCHITTEST_POS == CURSOR_NCHITTEST_POS_BOTTOMLEFT ||
                                      CURSOR_NCHITTEST_POS == CURSOR_NCHITTEST_POS_BOTTOMRIGHT) || isMaximizedCount[_id]) )
        {
            lockSizeMove(bSizeBind, _id);
            sendGeometryBindingToDest(bSizeBind, _id);
            setSizeMoveWaitList(bSizeBind[_id], _id);

            int* id = csAlloc<int>(1,_id);

            thread t(
                [id]()
            {
                int a = 1;
                int n = bSizeBind[*id].size();
                while (a > 0)
                {
                    SIZE deltaSize = DELTASIZE[*id];
                    POINT deltaPos = DELTAPOS[*id];
                    sizeMoveCore(*id, bSizeBind[*id], n, deltaSize.cy);
                    endThread(id, bSizeBind[*id], n, a);
                    delay(delayVal-2);
                }

            }
            );
            t.detach();
        }
    }

}

void sizeMoveWaitListExecute(int id)
{
    int *_id = csAlloc(1,id);
    thread t(
        [_id]()
    {
        int a = 1, x, y, cx, cy;
        int n = SECTION.size();
        while (a > 0)
        {
            for(int i=0; i<n; i++)
            {
                if(isWindowInSizeMoveWaitList[i])
                {
                    RECT dr = deltaSizeMoveWaitList[i];
                    RECT r = RECTPARREFSAVED[i];
                    //printRect(dr, " dr : ");
                    //cout<<i<<" ---i\n";
                    x = r.left + dr.left;
                    y = r.top + dr.top;
                    cx = r.right-r.left + dr.right + tmpSize[i].cx;
                    cy = r.bottom-r.top + dr.bottom + tmpSize[i].cy;

                    if(cx > -1 && cy > -1)
                    {
                        SetWindowPos(SECTION[i], 0, x, y, cx, cy,  SWP_NOZORDER);
                    }
                }
                //delay(10);
            }

            if(EXECUTE_SIZEMOVE_WAIT_LIST_SIGNAL == 0)
            {
                a--;
                if(a == 0)
                {
                    THREAD_END[*_id] = 1;
                    free(_id);
                    break;
                }
            }
            delay(delayVal+1);
        }
    }
    );
    t.detach();
}

void sizeMoveAll(int _id, bool automatic, SIZE _deltaSize, POINT _deltaPos)
{
    lockSizeMove(lSizeBind, _id);
    sendGeometryBindingToDest(lSizeBind, _id);
    setSizeMoveWaitList(lSizeBind[_id], _id);

    lockSizeMove(tSizeBind, _id);
    sendGeometryBindingToDest(tSizeBind, _id);
    setSizeMoveWaitList(tSizeBind[_id], _id);

    lockSizeMove(rSizeBind, _id);
    sendGeometryBindingToDest(rSizeBind, _id);
    setSizeMoveWaitList(rSizeBind[_id], _id);

    lockSizeMove(bSizeBind, _id);
    sendGeometryBindingToDest(bSizeBind, _id);
    setSizeMoveWaitList(bSizeBind[_id], _id);

    SIZE deltaSize = (automatic == 1) ? DELTASIZE[_id] : _deltaSize;
    POINT deltaPos = (automatic == 1) ? DELTAPOS[_id] : _deltaPos;

    //cout<<"tSizeBind[_id].size() = "<<deltaPos.y<<"\n";
    sizeMoveCore(_id, lSizeBind[_id], lSizeBind[_id].size(), deltaPos.x);
    sizeMoveCore(_id, tSizeBind[_id], tSizeBind[_id].size(), deltaPos.y);
    sizeMoveCore(_id, rSizeBind[_id], rSizeBind[_id].size(), deltaSize.cx);
    sizeMoveCore(_id, bSizeBind[_id], bSizeBind[_id].size(), deltaSize.cy);


    int n = SECTION.size();
    //cout<<"============================\n";
    int x, y, cx0, cy0, cx, cy;
    for(int i=0; i<n; i++)
    {
        if(isWindowInSizeMoveWaitList[i])
        {
            RECT dr = deltaSizeMoveWaitList[i];
            RECT r = RECTPARREFSAVED[i];
            //printRect(dr, " dr : ");
            //cout<<i<<" ---i\n";
            x = r.left + dr.left;
            y = r.top + dr.top;
            cx0 = r.right-r.left + dr.right;
            cy0 = r.bottom-r.top + dr.bottom;

            cx = cx0 + tmpSize[i].cx;
            cy = cy0 + tmpSize[i].cy;

            tmpSize[i].cx = (cx < 0 ? cx : 0);
            tmpSize[i].cy = (cy < 0 ? cy : 0);

            //if(cx > -1 && cy > -1)
            {
                SetWindowPos(SECTION[i], 0, x, y, cx, cy,  SWP_NOZORDER);
            }
        }
    }

    endSizeMove(_id, lSizeBind[_id], lSizeBind[_id].size());
    endSizeMove(_id, tSizeBind[_id], tSizeBind[_id].size());
    endSizeMove(_id, rSizeBind[_id], rSizeBind[_id].size());
    endSizeMove(_id, bSizeBind[_id], bSizeBind[_id].size());

    DELTASIZE[_id] = {0};
    DELTAPOS[_id] = {0};
    GetWindowRect(SECTION[_id], &RECTWNDSAVED[_id]);
    GetWindowRect(SECTION[_id], &RECTWND[_id]);
    GetClientRect(SECTION[_id], &RECTCL[_id]);
    RECTPARREFSAVED[_id] = CSUTILS::rectInParentRef(_id);
    if(updateAfterResizeMsg[_id])
        InvalidateRect(SECTION[_id], 0, 1);
}

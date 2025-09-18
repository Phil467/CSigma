#include "csUtils.h"
#include "math.h"
//#include <shellscalingapi.h>
extern float xdimFact;
extern float ydimFact;
vector<RECT> RF;
extern vector<vector<void(*)(CSARGS)>> GROUPED_EVENTS_FUNC;
extern vector<vector<CSARGS>> GROUPED_EVENTS_ARGS;
extern vector<vector<bool>> HIDEGROUPMSG;
extern vector<RECT> RECTPARREF;
extern vector<RECT> RECTWNDSAVED;
extern vector<RECT> RECTWND;
extern vector<RECT> RECTPARREFSAVED;
extern vector<RECT> RECTCL;
extern vector<SIZE> DELTASIZE;
extern vector<POINT> DELTAPOS;
extern vector<HWND> SECTION;
extern vector<HDC> hdcontext;
extern vector<bool> SECTIONSTYLE;

extern vector<vector<CSGRAPHIC_CONTEXT>> imageGradients;
extern vector<CSIMAGE_GRADIENT> mhgradient;
extern vector<CSBKG_CLICK_GRADIENT> clkGradient;
extern vector<CSZOOM_PARAMS> zoomParams;

extern vector<bool> attached;
extern vector<bool> BINDTOPAR;
extern vector<int> PARID;
extern vector<CSTEXT> TITLE;
extern vector<SIZE> hdcSize;
extern vector<HDC> hdStackContext;
extern vector<HBITMAP> hStackBmp;
extern vector<int> BORDERTHICK;
extern vector<COLORREF> borderColor;
extern vector<CSTIMER_PARAMS> TIMER_PARAMS;
extern vector<CSLOCKED_MODE> lockedMode;

vector<void(*)(CSARGS)> forceMsgFunc;
vector<CSARGS> forceMsgArgs;
MSG Messages;

extern HINSTANCE _hInstance;
extern vector<HWND> SECTION;
extern int CAPTION_AREA_SIZE;
extern void sizeMoveAll(int _id, bool automatic, SIZE _deltaSize, POINT _deltaPos);

extern RECT LAST_WORKAREA;
extern vector<RECT> RECTRESTORE;
extern vector<bool> isMaximized;

extern vector<vector<BIND_DIM_GEOM_PARAMS>> rSizeBind;
extern vector<vector<BIND_DIM_GEOM_PARAMS>> lSizeBind;
extern vector<vector<BIND_DIM_GEOM_PARAMS>> tSizeBind;
extern vector<vector<BIND_DIM_GEOM_PARAMS>> bSizeBind;

extern vector<bool> updateAfterResizeMsg;

extern vector<CSDYNAMIC_SIMPLE_TEXT> dynSimpleText;
extern vector<TIPS_POPUP_PARAMS> TipsPopupParams;

extern vector<HDC> hdStackContextExt;
extern vector<HBITMAP> hStackBmp;

extern vector<HDC> hdcontextExt;
extern vector<HBITMAP> hdBmpExt;
extern vector<POINT> hdcontextExtInPos;
extern vector<POINT> hdcontextExtOutPos;
extern vector<SIZE> hdcontextExtSize;
extern vector<COLORREF> backgroundColor;
extern vector<COLORREF> borderColor;

HINSTANCE hReditLib;

extern int CURSOR_NCHITTEST_POS;
extern float dimFact;

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
HHOOK setHook();

typedef enum
{
    PROCESS_DPI_UNAWARE = 0,
    PROCESS_SYSTEM_DPI_AWARE = 1,
    PROCESS_PER_MONITOR_DPI_AWARE = 2
} PROCESS_DPI_AWAERNESS;

/*typedef enum DPI_AWARENESS_CONTEXT_VALUE {
    DPI_AWARENESS_CONTEXT_UNAWARE = -1,
    DPI_AWARENESS_CONTEXT_SYSTEM_AWARE = -2,
    DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE = -3,
    DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 = -4
} DPI_AWARENESS_CONTEXT;*/

extern vector<CSTEXT> TITLEFILE;
extern vector<bool> setTitleInit;

void CSUIMAN::_CSIGMA_APP_INIT_(HINSTANCE hInstance, void(*forceEventFunc)(CSARGS), CSARGS *forceEventArgs)
{
    _hInstance = hInstance;

    xdimFact = 1.0*GetSystemMetrics(SM_CXSCREEN)/(1920);
    ydimFact = 1.0*GetSystemMetrics(SM_CYSCREEN)/(1080);
    

    hReditLib = LoadLibraryW(L"riched20.dll");

    HMODULE hShcore = LoadLibraryW(L"Shcore.dll");
    if(hShcore)
    {
        typedef HRESULT(WINAPI* SetProcessDpiAwarenessProc)(PROCESS_DPI_AWAERNESS);
        //typedef BOOL(WINAPI* SetProcessDpiAwarenessProc)(DPI_AWARENESS_CONTEXT);
        SetProcessDpiAwarenessProc PSetProcessDpiAwareness = (SetProcessDpiAwarenessProc)GetProcAddress(hShcore, "SetProcessDpiAwareness");

        if(PSetProcessDpiAwareness)
        {
            cout<<L" Activation de la gestion avancee du rendu.. \n";
            PSetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
            //PSetProcessDpiAwareness(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        }
        FreeLibrary(hShcore);
    }

    if(forceEventFunc)
    {
        forceMsgFunc.push_back(forceEventFunc);
        forceMsgArgs.push_back(*forceEventArgs);
    }
}

extern bool CLICK_EFFECT_BOOL;

int CSUIMAN::_CSIGMA_APP_CREATE_()
{
    HHOOK mhook = setHook();

    while (GetMessage(&Messages, nullptr, 0, 0))
    {
        TranslateMessage(&Messages);
        /*if(Messages.message == 96)
        {
            wchar_t className[1000];
            GetClassNameW(Messages.hwnd, className, 1000);
            wcout<<className<<"  +++++++++++++--------------message \n";
        }*/
        if(Messages.message == WM_NCLBUTTONUP || Messages.message == WM_LBUTTONUP)
        {
            CLICK_EFFECT_BOOL = 1;
        }
        if(Messages.message == WM_NCLBUTTONDOWN)
        {
            __setAllRects();
        }
        int n = forceMsgArgs.size();
        for(int i=0; i<n; i++)
        {
            forceMsgArgs[i].setProcParams((HWND*)&Messages.hwnd, (UINT*)&Messages.message, (WPARAM*)&Messages.lParam, (LPARAM*)&Messages.wParam, nullptr);
            (*forceMsgFunc[i])(forceMsgArgs[i]);
        }
        DispatchMessage(&Messages);
    }

    UnhookWindowsHookEx(mhook);

    return (int) Messages.wParam;
}

void CSUIMAN::__setAllRects()
{
    int n = RECTWND.size();
    for(int i=0; i<n; i++)
    {
        GetWindowRect(SECTION[i], &RECTWND[i]);
        GetWindowRect(SECTION[i], &RECTWNDSAVED[i]);
        RECTPARREFSAVED[i] = CSUTILS::rectInParentRef(i);
    }
}

HWND HOOK_HWND_1;
extern vector<HWND> richEdits;
extern vector<bool> hookRichEditSignal;

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if(nCode>=0)
    {
        if(wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_NCLBUTTONDOWN || wParam == WM_NCLBUTTONDBLCLK
         ||wParam == WM_NCRBUTTONDOWN || wParam == WM_NCRBUTTONDBLCLK)
        {
            int n = hookRichEditSignal.size();
            for(int i=0; i<n; i++)
            {
                hookRichEditSignal[i] = 1;
            }
        }

    }
    return CallNextHookEx(0,nCode,wParam,lParam);
}

HHOOK setHook()
{
    LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
    HHOOK mhook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, 0, 0);
    return mhook;
}

bool CSUIMAN::addAction(int id, void(*f)(CSARGS), CSARGS& args)
{
    GROUPED_EVENTS_FUNC[id].push_back(f);
    GROUPED_EVENTS_ARGS[id].push_back(args);
    HIDEGROUPMSG[id].push_back(0);
    return 1;
}

bool CSUIMAN::addAction(int id, void(*f)(CSARGS), int nbArgs, ...)
{
    CSARGS args(nbArgs);
    va_list adArgs;
    va_start (adArgs, nbArgs);
    for (int i=0 ; i<nbArgs ; i++)
    {
        args.setArg(i,va_arg (adArgs, void*));
    }
    va_end(adArgs);

    GROUPED_EVENTS_FUNC[id].push_back(f);
    GROUPED_EVENTS_ARGS[id].push_back(args);
    HIDEGROUPMSG[id].push_back(0);
    return 1;
}

extern vector<CSAPP_ICON> appIcon;

int CSUIMAN::setIcon(int id, wchar_t*pathSmallIcon, wchar_t*pathBigIcon)
{
    HICON hIcon = (HICON)LoadImageW(
                      NULL,
                      pathSmallIcon,       // Chemin relatif ou absolu vers ton fichier ICO
                      IMAGE_ICON,
                      0, 0,                   // Taille (0 = taille par d�faut)
                      LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED
                  );
    HICON hIcon2 = (HICON)LoadImageW(
                       NULL,
                       pathBigIcon,       // Chemin relatif ou absolu vers ton fichier ICO
                       IMAGE_ICON,
                       0, 0,                   // Taille (0 = taille par d�faut)
                       LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED
                   );

    if (hIcon && hIcon2)
    {
        SendMessage(SECTION[id], WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        SendMessage(SECTION[id], WM_SETICON, ICON_BIG, (LPARAM)hIcon2);

        appIcon.push_back({hIcon, hIcon2});

    }
    
    return appIcon.size()-1;
}

void CSUIMAN::setIcon(int id, int idIcon)
{
    SendMessage(SECTION[id], WM_SETICON, ICON_SMALL, (LPARAM)appIcon[idIcon].smallIcon);
    SendMessage(SECTION[id], WM_SETICON, ICON_BIG, (LPARAM)appIcon[idIcon].bigIcon);
}

HWND CSUIMAN::sHandle(int id)
{
    return SECTION[id];
}

CSTEXT& CSUIMAN::sTitle(int id)
{
    return TITLE[id];
}

CSDYNAMIC_SIMPLE_TEXT& CSUIMAN::sDynSimpleText(int id)
{
    return dynSimpleText[id];
}

RECT CSUIMAN::sRectParentContext(int id)
{
    return RECTPARREF[id];
}

RECT CSUIMAN::sRectParentContextStatic(int id)
{
    return RECTPARREFSAVED[id];
}

RECT CSUIMAN::sRectClient(int id)
{
    return RECTCL[id];
}

POINT CSUIMAN::sDeltaPos(int id)
{
    return DELTAPOS[id];
}

SIZE CSUIMAN::sDeltaSize(int id)
{
    return DELTASIZE[id];
}

void CSUIMAN::printRect(RECT r, char*title)
{
    wcout<<title<<L" : "<<r.left<<L" "<<r.top<<L" "<<r.right<<L" "<<r.bottom<<L"\n";
}


void CSUIMAN::bindGeometry_(int id, BIND_GEOM_PARAMS*& Args, int& n)
{

}

void CSUIMAN::bindGeometry(int id, BIND_GEOM_PARAMS* bgps, int n)
{
    for(int i=0; i<n; i++)
    {
        BIND_GEOM_PARAMS bgp = bgps[i];
        if(int(bgp.coef.lcf*10000) != 0)
        {
            lSizeBind[id].push_back({bgp.id, bgp.coef.lcf, bgp.flag.lFlag});
        }
        if(int(bgp.coef.rcf*10000) != 0)
        {
            rSizeBind[id].push_back({bgp.id, bgp.coef.rcf, bgp.flag.rFlag});
        }
        if(int(bgp.coef.tcf*10000) != 0)
        {
            tSizeBind[id].push_back({bgp.id, bgp.coef.tcf, bgp.flag.tFlag});
        }
        if(int(bgp.coef.bcf*10000) != 0)
        {
            bSizeBind[id].push_back({bgp.id, bgp.coef.bcf, bgp.flag.bFlag});
        }
    }
}

void CSUIMAN::bindGeometry(int id, int n, ...)
{
    va_list adArgs ;
    BIND_GEOM_PARAMS* tab = csAlloc<BIND_GEOM_PARAMS>(n);
    va_start (adArgs, n);
    for (int i=0 ; i<n ; i++)
    {
        tab[i] = va_arg (adArgs, BIND_GEOM_PARAMS) ;

    }
    va_end(adArgs);

    CSUIMAN::bindGeometry(id, tab, n);

    free(tab);
}

void CSUIMAN::setSizeFactor(float factor)
{
    dimFact = factor;
}

void CSUIMAN::setBorderThick(int id, int thick)
{
    if(!hdStackContext[id])
    {
        hdStackContext[id] = (CreateCompatibleDC(hdcontext[id]));
        hStackBmp[id] = (CreateCompatibleBitmap(hdcontext[id],hdcSize[id].cx, hdcSize[id].cy));
        SelectBitmap(hdStackContext[id], hStackBmp[id]);
    }
    BORDERTHICK[id] = thick;
}

void CSUIMAN::setBorderColorAndThick(int id, COLORREF color, int thick)
{
    if(!hdStackContext[id])
    {
        hdStackContext[id] = (CreateCompatibleDC(hdcontext[id]));
        hStackBmp[id] = (CreateCompatibleBitmap(hdcontext[id],hdcSize[id].cx, hdcSize[id].cy));
        SelectBitmap(hdStackContext[id], hStackBmp[id]);
    }
    borderColor[id] = color;
    BORDERTHICK[id] = thick;
}

void CSUIMAN::setTitle(int id, CSTEXT title, bool textOnly)
{
    if(title.Text)
    {
        if(TITLE[id].Text);
        {
            free(TITLE[id].Text);
            free(TITLE[id].Font);
        }
        if(TITLEFILE.size())
        {
            if(!setTitleInit[id])
            {
                TITLE[id] = TITLEFILE[id];
                setTitleInit[id] = 1;
            }
            else
            {
                if(!textOnly)
                    TITLE[id] = title;
                int l;
                TITLE[id].Text = csAlloc<wchar_t>((l=wcslen(title.Text)+1));
                wcscpy_s(TITLE[id].Text, l, title.Text);
                TITLE[id].Font = csAlloc<wchar_t>((l=wcslen(title.Font)+1));
                wcscpy_s(TITLE[id].Font, l, title.Font);
            }
        }
        else
        {
            if(!textOnly)
                TITLE[id] = title;
            int l;
            TITLE[id].Text = csAlloc<wchar_t>((l=wcslen(title.Text)+1));
            wcscpy_s(TITLE[id].Text, l, title.Text);
            TITLE[id].Font = csAlloc<wchar_t>((l=wcslen(title.Font)+1));
            wcscpy_s(TITLE[id].Font, l, title.Font);

        }
        
        if(!hdStackContext[id])
        {
            hdStackContext[id] = (CreateCompatibleDC(hdcontext[id]));
            hStackBmp[id] = (CreateCompatibleBitmap(hdcontext[id],hdcSize[id].cx, hdcSize[id].cy));
            SelectBitmap(hdStackContext[id], hStackBmp[id]);
        }
        SetWindowTextW(SECTION[id], title.Text);
    }
}

void CSUIMAN::inert(int id, BYTE alphaLevel)
{
    SetWindowLongW(sHandle(id), GWL_EXSTYLE, GetWindowLongW(sHandle(id), GWL_EXSTYLE)|WS_EX_LAYERED|WS_EX_TRANSPARENT);
    SetLayeredWindowAttributes(sHandle(id), 0, alphaLevel, LWA_ALPHA);

    /*RECT r = sRectClient(id);
    SetWindowPos(SECTION[id],0, 0,0,r.right+2, r.bottom, SWP_NOMOVE|SWP_NOZORDER|SWP_DRAWFRAME);
    SetWindowPos(SECTION[id],0, 0,0,r.right, r.bottom, SWP_NOMOVE|SWP_NOZORDER);*/
}

void CSUIMAN::setTransparent(int id)
{
    SetWindowLongW(sHandle(id), GWL_EXSTYLE, GetWindowLongW(sHandle(id), GWL_EXSTYLE)|WS_EX_LAYERED);
}

void CSUIMAN::setTransparency(int id, char level)
{
    SetLayeredWindowAttributes(sHandle(id), 0, level, LWA_ALPHA);
    InvalidateRect(sHandle(id), 0, 1);
}

void CSUIMAN::_drawTitle(int id, HDC dc)
{
   CSTEXT& Title = TITLE[id];
   if(Title.Text && Title.Show)
    {

        RECT rc = RECTCL[id];
        //GetClientRect(SECTION[id], &rc);
        //printRect(rc, "rc = ");
        //HDC dc = GetDC(sHandle(id));
        SetBkMode(dc,TRANSPARENT);
        HFONT hf = CreateFontW(CSUTILS::getAdjustedFontSizeX(Title.FontSize.cx),
                              CSUTILS::getAdjustedFontSizeY(Title.FontSize.cy),
                              Title.Orientation, 0, Title.Bold,Title.Italic,Title.Underline,0,DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS,CLIP_STROKE_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH|FF_DONTCARE, Title.Font);
        SelectFont(dc,hf);
        SetTextColor(dc,RGB(Title.Color.r, Title.Color.g,Title.Color.b));

        int b = (Title.FontSize.cy>0)?Title.FontSize.cy:Title.FontSize.cx;

        LPSIZE lps;
        wchar_t*title;
        if(!Title.ShowEntierText)
            title = CSUTILS::truncateHorizontalTextW(Title.Text, hf, rc.right-20, lps);
        else
        {
            title = Title.Text;
            lps = CSUTILS::textExtentW(0,hf,title);
        }

        Title.TextRect.right = {lps->cx};
        Title.TextRect.bottom = {lps->cy};

        int textAlign = Title.Align;

        if(textAlign == CS_TA_TOP_CENTER)
        {
            Title.TextRect.left = (rc.right-lps->cx)/2;
            Title.TextRect.top = Title.Marging.top*dimFact*ydimFact;
        }
        if(textAlign == CS_TA_CENTER)
        {
            if(Title.Orientation == -900)
            {
                Title.TextRect.left = (rc.right+lps->cy)/2 + Title.Marging.left*dimFact*xdimFact;
                Title.TextRect.top = (rc.bottom-lps->cx)/2 + Title.Marging.top*dimFact*ydimFact;
            }
            else if(Title.Orientation == 900)
            {
                Title.TextRect.left = (rc.right-lps->cy)/2 + Title.Marging.left*dimFact*xdimFact;
                Title.TextRect.top = (rc.bottom+lps->cx)/2 + Title.Marging.top*dimFact*ydimFact;
            }
            else
            {
                //cout<<Title.Marging.left<<" id ="<<id<<"\n";
                Title.TextRect.left = (rc.right-lps->cx)/2 + Title.Marging.left*dimFact*xdimFact;
                Title.TextRect.top = (rc.bottom-lps->cy)/2 + Title.Marging.top*dimFact*ydimFact;
            }
        }
        else if(textAlign == CS_TA_TOP_LEFT)
        {
            Title.TextRect.left = Title.Marging.left*dimFact*xdimFact;
            Title.TextRect.top = Title.Marging.top*dimFact*ydimFact;
        }
        else if(textAlign == CS_TA_CENTER_LEFT)
        {
            Title.TextRect.left = Title.Marging.left*dimFact*xdimFact;
            Title.TextRect.top = (rc.bottom-lps->cy)/2 + Title.Marging.top*dimFact*ydimFact;
        }
        else if(textAlign == CS_TA_TOP_RIGHT)
        {
            Title.TextRect.left = (rc.right-lps->cx) + Title.Marging.left*dimFact*xdimFact;
            Title.TextRect.top = Title.Marging.top*dimFact*ydimFact;
        }
        else if(textAlign == CS_TA_CENTER_RIGHT)
        {
            Title.TextRect.left = (rc.right-lps->cx) + Title.Marging.left*dimFact*xdimFact;
            Title.TextRect.top = (rc.bottom-lps->cy)/2 + Title.Marging.top*dimFact*ydimFact;
        }

        TextOutW(dc,Title.TextRect.left,Title.TextRect.top,title,
                 wcslen(title));

        Title.TextRect.left -= 10;
        Title.TextRect.top -= 10;
        Title.TextRect.right += (Title.TextRect.left + 20);
        Title.TextRect.bottom += (Title.TextRect.top + 20);

        DeleteFont(hf);
        free(lps);

        //wcout<<TITLE[id].Text<<"\n";

        if(!Title.ShowEntierText)
            free(title);
        //ReleaseDC(sHandle(id), dc);
    }
}

void CSUIMAN::setAsCloseButton(int id, int& id_close)
{
    auto f = [](CSARGS Args)
    {
        if(UINT(Args) == WM_LBUTTONUP)
        {
            if(*(int*)Args[0] == 0)
            {
                SendMessage(SECTION[0], WM_DESTROY, 0, 0);
            }
            else
                ShowWindow(SECTION[*(int*)Args[0]], 0);
        }
    };

    addAction(id, f, 1, &id_close);
}

void CSUIMAN::setAsMinButton(int id, int& id_minimize)
{
    auto f = [](CSARGS Args)
    {
        if(UINT(Args) == WM_LBUTTONUP)
        {
            int idMin = *(int*)Args[0];
            if(!attached[idMin])
                SendMessage(SECTION[idMin], WM_SYSCOMMAND, SC_MINIMIZE, 0);
            else
                ShowWindow(SECTION[idMin], 0);
        }
    };

    addAction(id, f, 1, &id_minimize);
}

extern int HIDDEN_WINDOW_PART[4];

void __getHiddenWindowPart(int id, RECT r)
{
    RECT rc;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);

    if(RECTWND[id].left < rc.left)
    {
        HIDDEN_WINDOW_PART[0] = rc.left-RECTWND[id].left;
    }
    if(RECTWND[id].top < rc.top)
    {
        HIDDEN_WINDOW_PART[1] = rc.top-RECTWND[id].top;
    }
    if(RECTWND[id].right > GetSystemMetrics(SM_CXSCREEN))
    {
        HIDDEN_WINDOW_PART[2] = RECTWND[id].right - rc.right;
    }
    if(RECTWND[id].bottom > GetSystemMetrics(SM_CYSCREEN))
    {
        HIDDEN_WINDOW_PART[3] = RECTWND[id].bottom - rc.bottom;
    }
}

void CSUIMAN::setAsMaxButton(int& id, int& id_maximize)
{
    auto f = [](CSARGS Args)
    {
        if((UINT)Args == WM_LBUTTONUP)
        {
            // L'utilisateur commence a déplacer ou redimensionner
            int id = *(int*)Args[0];
            HWND hwnd = SECTION[id];
            int xsframe = GetSystemMetrics(SM_CXSIZEFRAME);
            int ysframe = GetSystemMetrics(SM_CYSIZEFRAME);

            if(isMaximized[id] == 0)
            {
                RECT rc;
                if(!attached[id])
                {
                    RECTRESTORE[id] = RECTWND[id];
                    SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
                }
                else
                {
                    RECTRESTORE[id] = CSUTILS::rectInParentRef(id);;
                    rc = RECTCL[PARID[id]];
                }
                RECT r = {rc.left-xsframe, rc.top-ysframe, rc.right+2*xsframe, rc.bottom+2*ysframe};
                LAST_WORKAREA = rc;

                RECT rr;
                GetClientRect(hwnd, &rr);
                sizeMoveAll(id, 0, {rc.right-rr.right, rc.bottom-rr.bottom}, {0});
                SetWindowPos(hwnd, 0, r.left, r.top, r.right, r.bottom, SWP_NOZORDER);

                isMaximized[id] = 1;
            }
            else
            {
                RECT rrest = RECTRESTORE[id];
                RECT rc = LAST_WORKAREA;
                //SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);

                sizeMoveAll(id, 0, {rrest.right-rrest.left-rc.right-2*xsframe, rrest.bottom-rrest.top-rc.bottom-2*ysframe}, {0});
                SetWindowPos(hwnd, 0, rrest.left, rrest.top, rrest.right-rrest.left, rrest.bottom-rrest.top, SWP_NOZORDER);

                isMaximized[id] = 0;

// redessiner la partie cachee pour eviter le noir
                __getHiddenWindowPart(id, RECTRESTORE[id]);
            }
            __setAllRects();
        }
    };

    addAction(id, f, 1, &id_maximize);

    auto f2 = [](CSARGS Args)
    {
        int id = (int)Args;
        if(UINT(Args) == WM_NCLBUTTONDBLCLK && CURSOR_NCHITTEST_POS == CURSOR_NCHITTEST_POS_CAPTION && !SECTIONSTYLE[id])
        {
            int idbtn = *(int*)Args[0];
            SendMessage(SECTION[idbtn], WM_LBUTTONUP, 0, 0);
        }
        /*else if(UINT(Args) == WM_MOVING && isMaximized[id])
        {
            int idbtn = *(int*)Args[0];
            SendMessage(SECTION[idbtn], WM_LBUTTONUP, 0, 0);
        }*/
    };

    addAction(id_maximize, f2, 1, &id);
}

RECT CSUIMAN::getTaskbarRect()
{
    APPBARDATA abd = {sizeof(APPBARDATA)};
    SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
    return abd.rc;
}

TASKBAR_INFO CSUIMAN::getTaskbarInfo()
{
    TASKBAR_INFO ti = {0};
    APPBARDATA abd = {sizeof(APPBARDATA)};
    SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
    ti.rect = abd.rc;
    ti.edge = abd.uEdge;

    APPBARDATA abdState = {sizeof(APPBARDATA)};

    abdState.lParam = ABS_AUTOHIDE;
    ti.autoHide = (SHAppBarMessage(ABM_GETSTATE, &abdState) & ABS_AUTOHIDE);

    return ti;
}

extern int TIPS_POPUP;
extern bool saveAppTips;
extern vector<vector<vector<wchar_t*>>> TIPSFILE;


void CSUIMAN::addTips(int id, RECT rTips, POS_BOOL pb, int delay, bool locked, CSDYNAMIC_SIMPLE_TEXT message)
{
    TIPS_POPUP_PARAMS tpp;
    tpp.Ids.push_back(TIPS_POPUP);
    tpp.Geometry.push_back({0,0,rTips.right*dimFact, rTips.bottom*dimFact});
    tpp.Bpos.push_back(pb);
    if(TIPSFILE.size() && saveAppTips)
    {
        int n = message.paragraph.size();
        for(int i=0; i<n; i++)
        {
            //wcout<<TIPSFILE[id][TipsPopupParams[id].message.size()][i]<<L"\n";
            message.paragraph[i].Text = CSSTRUTILS::makeWcharString(TIPSFILE[id][TipsPopupParams[id].message.size()][i]);
        }
    }
    else
    {
        int n = message.paragraph.size();
        for(int i=0; i<n; i++)
        {
            message.paragraph[i].Text = CSSTRUTILS::makeWcharString(message.paragraph[i].Text);
        }
    }
    tpp.message.push_back(message);
    tpp.Lock = locked;
    tpp.Delay = delay;
    tpp.MouseHoverCount = 0;
    tpp.MouseLeaveCount = 0;

    TipsPopupParams[id] = tpp;

    if(!hdStackContext[TIPS_POPUP])
    {
        hdStackContext[TIPS_POPUP] = CreateCompatibleDC(hdcontext[0]);
        hStackBmp[TIPS_POPUP] = CreateCompatibleBitmap(hdcontext[0], GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
        SelectBitmap(hdStackContext[TIPS_POPUP], hStackBmp[TIPS_POPUP]);
    }

    if(!hdcontextExt[TIPS_POPUP])
    {
        int c = backgroundColor[TIPS_POPUP];
        csGraphics::setGraphicAreaPosition(TIPS_POPUP,{0,0});
        csGraphics::setGraphicAreaColor(TIPS_POPUP,{GetRValue(c),GetGValue(c),GetBValue(c)},{0});
        csGraphics::setGraphicAreaSize(TIPS_POPUP,{rTips.right, rTips.bottom});
        csGraphics::updateGraphicArea(TIPS_POPUP, 1);
        /*CSSCROLLBAR hscroll1 = CSUICONTROLS::addHScrollBar(&MIDDLE_BOTTOM_SECTION, &MIDDLE_BOTTOM_SECTION, 0, 20);
        CSSCROLLBAR vscroll1 = CSUICONTROLS::addVScrollBar(&MIDDLE_BOTTOM_SECTION, &MIDDLE_BOTTOM_SECTION, 0, 20);

        hscroll1.setViewFrameBottomMarging(20);
        vscroll1.setViewFrameRightMarging(20);*/
    }
}

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
void CSUIMAN::enableDarkEdge(int id)
{
    BOOL useDark = TRUE;

    // Windows 10 1809+ supporte l'attribut 19
    const DWORD DWMWA_USE_IMMERSIVE_DARK_MODE = 20; // ou 19 selon version

    HRESULT hr = DwmSetWindowAttribute(
                     SECTION[id],
                     DWMWA_USE_IMMERSIVE_DARK_MODE,
                     &useDark,
                     sizeof(useDark)
                 );
}


extern int SMX;
extern int SMY;
void CSUIMAN::_updateApp(int id)
{
    //imageGradients
    //mhgradient
    //clkGradient
    //zoomParams
    static int count = 0;

    if(SMX != GetSystemMetrics(SM_CXSCREEN) && id == 0)
    {
        count++;

        if(count == 1000)
        {
            count = 0;
            float xcf = 1.0*GetSystemMetrics(SM_CXSCREEN)/SMX;
            float ycf = 1.0*GetSystemMetrics(SM_CYSCREEN)/SMY;

        cout<<"smx = "<<SMX<<"\n";

            SMX = GetSystemMetrics(SM_CXSCREEN);
            SMY = GetSystemMetrics(SM_CYSCREEN);
            int N = SECTION.size();

            vector<RECT> rv;

            for(int i=0; i<N; i++)
            {
                hdcSize[i] = {SMX, SMY};
                csGraphics::updateBackground(i);

                zoomParams[i].targetContextSize = hdcSize[i];

                if(hdStackContext[i])
                {
                    DeleteDC(hdStackContext[i]);
                    DeleteBitmap(hStackBmp[i]);
                    hdStackContext[i] = (CreateCompatibleDC(hdcontext[i]));
                    hStackBmp[i] = (CreateCompatibleBitmap(hdcontext[i],hdcSize[i].cx, hdcSize[i].cy));
                    SelectBitmap(hdStackContext[i], hStackBmp[i]);
                }

                if(imageGradients[i].size())
                {
                    mhgradient[i].ImageGradient = 0;
                    mhgradient[i].Position.x*=xcf;
                    mhgradient[i].Position.y*=ycf;
                    mhgradient[i].Size.cx*=xcf;
                    mhgradient[i].Size.cy*=ycf;
                    int n = imageGradients[i].size();

                    for(int j=0; j<n; j++)
                    {
                        csGraphics::releaseGraphicContext(imageGradients[i][j]);
                    }
                    imageGradients[i].clear();
                    imageGradients[i] = csGraphics::getImageGradient(mhgradient[i], CSUTILS::toRGBA(backgroundColor[i]));
                    mhgradient[i].ImageGradient = 1;
                }

                RECT r = RECTPARREFSAVED[i];
                if(i == 0)
                CSUIMAN::printRect(r, "r ");
                r.left *= xcf; r.top *= ycf; r.right *= xcf; r.bottom *= ycf;
                if(i == 0)
                CSUIMAN::printRect(r, "r ");
                rv.push_back(r);

            }

            for(int i=0; i<N; i++)
            {
                RECT r = rv[i];
                //if(i == 0)
                {
                    MoveWindow(SECTION[i], r.left, r.top, r.right-r.left, r.bottom-r.top, 1);
                    
                }
                InvalidateRect(SECTION[i],0,1);
            }

            __setAllRects();

            rv.clear();
        }
    }
}



/*********************************************************************************************************** */

void _manageTimers(CSARGS Args)
{
    if(UINT(Args) == WM_SYSCOMMAND)
    {
        int id = int(Args);
        WPARAM wp = WPARAM(Args);
        if(wp == SC_RESTORE)
        {
            CSUIMAN::setAllTimers();
        }
        else if(wp == SC_MINIMIZE)
        {
            CSUIMAN::killAllTimers();
        }
    }
}

void CSUIMAN::manageTimers(int id)
{
    addAction(id, _manageTimers,0);
}

void CSUIMAN::killAllTimers()
{
    int n = TIMER_PARAMS.size();
    for(int i=1; i<n; i++)
    {
        if(TIMER_PARAMS[i].closeWhenMinimize)
        {
            KillTimer(SECTION[i], 0);
        }
    }
}

void CSUIMAN::setAllTimers()
{
    int n = TIMER_PARAMS.size();
    for(int i=1; i<n; i++)
    {
        if(TIMER_PARAMS[i].closeWhenMinimize)
        {
            SetTimer(SECTION[i], 0, TIMER_PARAMS[i].delay,0);
        }
    }
}

void CSUIMAN::setLockable(int id, CSLOCKED_MODE lm)
{
    lockedMode[id] = lm;
}

void CSUIMAN::updateAfterReceivingResizeMessage(int id)
{
    updateAfterResizeMsg[id] = 1;
}

/**************************************************************************************************************** */

int  CSUTILS::getAdjustedFontSizeXY(int size)
{
    return ceil(size*GetSystemMetrics(SM_CXSCREEN)*GetSystemMetrics(SM_CYSCREEN)/(1366.0*768));
    //return xSize;
}

RECT  CSUTILS::r(int x, int y, int cx, int cy, int id)
{
    if(!RF.size())
        return {x, y, cx, cy};
    else
        return RF[id];
}
SIZE  CSUTILS::s(int cx, int cy)
{

    return {cx, cy};
}
int  CSUTILS::l(int i)
{

    return i*xdimFact;
}

int  CSUTILS::getAdjustedFontSizeX(int xbaseFontSize)
{
    /*// Obtenir le DPI de l'écran
    HDC hdc = hdcontext[0];
    HWND hwnd = SECTION[0];
    int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
    
    // Calculer la taille adaptée (96 DPI = référence 100%)
    int adaptedSize = MulDiv(xbaseFontSize, dpi, 96);
    
    // Ajustement supplémentaire selon résolution
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    int screenWidth = workArea.right - workArea.left;
    
    if (screenWidth >= 1920) {
        adaptedSize = (int)(adaptedSize * 1.1);  // 10% plus grand sur grands écrans
    } else if (screenWidth <= 1366) {
        adaptedSize = (int)(adaptedSize * 1.1);  // 10% plus petit sur petits écrans
    }*/
   //cout<<ceil(xbaseFontSize*GetSystemMetrics(SM_CXSCREEN)/1366.0)<< " "<< adaptedSize<<"\n";
    //return ceil(xbaseFontSize*GetSystemMetrics(SM_CXSCREEN)/1366.0);
    return ceil(xbaseFontSize*xdimFact*dimFact*0.7);
    //return adaptedSize;
}
int  CSUTILS::getAdjustedFontSizeY(int ySize)
{
    //return ceil(dimFact*ySize*GetSystemMetrics(SM_CYSCREEN)/768.0);
    return ceil(ySize*ydimFact*dimFact*0.7);
    //return ySize;
}

char* CSUTILS::truncateHorizontalText(char*str, HFONT hf, int maxLength, LPSIZE& lps)
{
    int n = strlen(str);
    char* res = csAlloc<char>(n + 4), chr[2];
    sprintf(res,"");
    sprintf(chr,"");
    int i = 0;

    lps=0;
    lps = (LPSIZE)calloc(1,sizeof(SIZE));

    LPSIZE lps0 = CSUTILS::textExtent(0,hf, "...");

    while((lps->cx + lps0->cx < maxLength) && i < n)
    {
        free(lps);
        sprintf(chr, "%c", str[i]);
        strcat(res,chr);
        lps = CSUTILS::textExtent(0,hf, res);
        i++;
    }

    int j = i;
    if(n > i)
    {
        strcat(res, "...");
        j = i + 4;
        lps->cx += lps0->cx;
    }


    free(lps0);

    //res = (char*)realloc(res, j);

    return res;
}

wchar_t* CSUTILS::truncateHorizontalTextW(wchar_t* str, HFONT hf, int maxLength, LPSIZE& lps)
{
    int n = wcslen(str);
    wchar_t* res = csAlloc<wchar_t>((n + 4)*sizeof(wchar_t)), chr[2];
    wsprintf((LPWSTR)res,L"");
    wsprintf((LPWSTR)chr,L"");
    int i = 0;

    lps=0;
    lps = (LPSIZE)calloc(1,sizeof(SIZE));

    LPSIZE lps0 = CSUTILS::textExtentW(0,hf, L"...");

    while((lps->cx + lps0->cx < maxLength) && i < n)
    {
        free(lps);
        wsprintf((LPWSTR)chr, L"%c", str[i]);
        wcscat(res,chr);
        lps = CSUTILS::textExtentW(0,hf, res);
        //cout<<"lps->cx = "<<lps->cx<<"\n";
        i++;
    }

    int j = i;
    if(n > i)
    {
        wcscat(res, L"...");
        j = i + 4;
        lps->cx += lps0->cx;
    }


    free(lps0);

    //res = (char*)realloc(res, j);

    return res;
}

char* CSUTILS::truncateHorizontalText(char*str, HFONT hf, int maxLength)
{
    LPSIZE sz;
    char*ret = truncateHorizontalText(str, hf, maxLength, sz);
    free(sz);
    return ret;
}

wchar_t* CSUTILS::truncateHorizontalTextW(wchar_t* str, HFONT hf, int maxLength)
{
    LPSIZE sz;
    wchar_t*ret = truncateHorizontalTextW(str, hf, maxLength, sz);
    free(sz);
    return ret;
}

RECT CSUTILS::rectInParentRef(int id)
{
    RECT rwp = {0};
    int a = 0, b = 0;
    if(PARID[id] < 0)
    {
        rwp = {0};
    }
    else
    {
        rwp = RECTWND[PARID[id]];

        if(!SECTIONSTYLE[PARID[id]])
        {
            a = GetSystemMetrics(SM_CXSIZEFRAME);
            //b = GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYCAPTION);
            b = GetSystemMetrics(SM_CYSIZEFRAME);
        }
    }
    RECT rw = RECTWND[id];
    if(BINDTOPAR[id])//-pour un changement de parent, verifier ce parametre-----------
    {
        rw.left -= (rwp.left + a);
        rw.top -= (rwp.top + b);
        rw.right -= (rwp.left + a);
        rw.bottom -= (rwp.top + b);
    }
    return rw;
}
RECT CSUTILS::rectInParentRef_saved(int id)
{
    RECT rwp = {0};
    int a = 0, b = 0;
    if(PARID[id] < 0)
    {
        rwp = {0};
    }
    else
    {
        rwp = RECTWNDSAVED[PARID[id]];

        if(!SECTIONSTYLE[PARID[id]])
        {
            a = GetSystemMetrics(SM_CXSIZEFRAME);
            //b = GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYCAPTION);
            b = GetSystemMetrics(SM_CYSIZEFRAME);
        }
    }
    RECT rw = RECTWNDSAVED[id];
    if(BINDTOPAR[id])//-pour un changement de parent, verifier ce parametre-----------
    {
        rw.left -= (rwp.left + a);
        rw.top -= (rwp.top + b);
        rw.right -= (rwp.left + a);
        rw.bottom -= (rwp.top + b);
    }
    return rw;
}

LPSIZE CSUTILS::textExtent(int id, HFONT font, char*text)
{
    LPSIZE lps=(LPSIZE)malloc(sizeof(LPSIZE));
    HDC dc= hdcontext[id];
    SelectFont(dc, font);
    GetTextExtentPoint32A(dc, (LPCSTR)text, strlen((LPCSTR)text), lps);
    return lps;
}

LPSIZE CSUTILS::textExtentW(int id, HFONT font, wchar_t* text)
{
    LPSIZE lps=(LPSIZE)malloc(sizeof(LPSIZE));
    HDC dc= hdcontext[id];
    SelectFont(dc, font);
    GetTextExtentPoint32W(dc, (LPCWSTR)text, wcslen(text), lps);
    return lps;
}

LPSIZE CSUTILS::textExtent(HWND hwnd, HFONT font, char*text)
{
    LPSIZE lps=(LPSIZE)malloc(sizeof(LPSIZE));
    HDC dc= GetDC(hwnd);
    SelectFont(dc, font);
    GetTextExtentPoint32A(dc, (LPCSTR)text, strlen((LPCSTR)text), lps);
    ReleaseDC(hwnd, dc);
    return lps;
}

LPSIZE CSUTILS::textExtentW(HWND hwnd, HFONT font, wchar_t*text)
{
    LPSIZE lps=(LPSIZE)malloc(sizeof(LPSIZE));
    HDC dc= GetDC(hwnd);
    SelectFont(dc, font);
    GetTextExtentPoint32W(dc, (LPCWSTR)text, wcslen(text), lps);
    ReleaseDC(hwnd, dc);
    return lps;
}

LPSIZE CSUTILS::numExtent(HWND hwnd, int num, int maxNumDigits)
{
    HFONT hFont = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
    char* str = csAlloc<char>(maxNumDigits);
    sprintf(str,"%d",num);
    LPSIZE lps = CSUTILS::textExtent(hwnd, hFont, str);
    free(str);
    return lps;
}

LPSIZE CSUTILS::numExtent(HWND hwnd, HFONT font, int num, int maxNumDigits)
{
    char* str = csAlloc<char>(maxNumDigits);
    sprintf(str,"%d",num);
    LPSIZE lps = CSUTILS::textExtent(hwnd, font, str);
    free(str);
    return lps;
}


// Fonction qui compte le nombre de chiffres d'un nombre entier
int CSUTILS::numDigits(long number)
{
    // Gérer le cas du zéro (un chiffre)
    if (number == 0)
    {
        return 1;
    }

    // Gérer les nombres négatifs en prenant la valeur absolue
    if (number < 0)
    {
        number = -number;
    }

    // Compter les chiffres
    int inc = 0;
    while (number > 0)
    {
        inc++;
        number /= 10;
    }

    return inc;
}
// Fonction qui compte le nombre de chiffres d'un nombre entier
int CSUTILS::numDigits(unsigned long number)
{

    // Compter les chiffres
    int inc = 0;

    if(number == 0)
    {
        return 1;
    }
    while (number > 0)
    {
        inc++;
        number /= 10;
    }

    return inc;
}



int CSUTILS::unilineTextToPolylineText(wchar_t*_str, HFONT hf, int maxLength, vector<wchar_t*>& lines)
{
    int n = wcslen(_str);
    wchar_t*str = csAlloc<wchar_t>(n+1);
    wcscpy_s(str, n+1, _str);
    //wsprintf(str, L"%s\0", _str); // trunc to 1024 char

    wchar_t chr[2];
    wsprintf(chr,L"");
    int i = 0, j=0;
    LPSIZE lps = (LPSIZE)calloc(1,sizeof(SIZE));
    wchar_t* res;

    while (i<n)
    {
        res = csAlloc<wchar_t>(n+1);
        wsprintf(res,L"");
        j = 0;

        while((lps->cx < maxLength) && i < n)
        {
            free(lps);
            wsprintf(chr, L"%c", str[i]);
            wcscat(res,chr);
            lps = CSUTILS::textExtentW(0,hf, res);
            i++;
            j++;
        }
        lps->cx = 0;
        if(i<n)
        {
            i -= 1;
            res = (wchar_t*)realloc(res, (j)*sizeof(wchar_t));
            res[j-1] = '\0';
        }
        else
        {
            res = (wchar_t*)realloc(res, (j+1)*sizeof(wchar_t));
            res[j] = '\0';
        }
        lines.push_back(res);
    }

    int l = lps->cy;
    free(lps);
    free(str);
    return l;
}

void removeLeft(char*&nb, size_t& size, size_t remLen)
{
    size_t diff = size - remLen;
    char *ret = csAlloc<char>(diff+1);
    ret[diff] = '\0';
    for(size_t i=remLen, j=0; i<size; i++, j++)
    {
        ret[j] = nb[i];
    }
    //cout<<" mid ---- \n";
    free(nb); // --------------------------------------------------------------------------- to check for memory !!!!
    nb = ret;
    size = diff;
}

char* removeLeft(const char*nb, size_t& size, size_t remLen)
{
    size_t diff = size - remLen;
    char *ret = csAlloc<char>(diff+1);
    ret[diff] = '\0';
    for(size_t i=remLen, j=0; i<size; i++, j++)
    {
        ret[j] = nb[i];
    }
    //cout<<" mid ---- \n";
    size = diff;
    return ret;
}

bool CSUTILS::signExtraction(char*& strNumeric)
{
    bool sign = 0;
    if(strNumeric[0] == '-' || strNumeric[0] == '+')
    {
        if(strNumeric[0] == '-')
            sign = 1;

        size_t n = strlen(strNumeric);
        char* ch = removeLeft((const char*)strNumeric, n, 1);
        free(strNumeric);
        strNumeric = ch;

    }
    return sign;
}

bool CSUTILS::signExtraction(const char* strNumeric, char*& ret)
{
    bool sign = 0;
    if(strNumeric[0] == '-' || strNumeric[0] == '+')
    {
        if(strNumeric[0] == '-')
            sign = 1;

        size_t n = strlen(strNumeric);
        ret = removeLeft(strNumeric, n, 1);

    }
    else
    {
        ret = csAlloc<char>(strlen(strNumeric)+1);
        sprintf(ret, "%s\0", strNumeric);
    }
    return sign;
}


void CSUTILS::drawGDIRectangle(HDC dc, COLORREF brush, COLORREF pen, int border, RECT r)
{
    HBRUSH hb = CreateSolidBrush(brush);
    HPEN hp = CreatePen(0,border,pen);
    SelectBrush(dc,hb);
    SelectPen(dc,hp);
    Rectangle(dc,r.left, r.top, r.right, r.bottom);
    DeletePen(hp);
    DeleteBrush(hb);
}


CSRGBA CSUTILS::toRGBA(long color)
{
    return {color&0xffff, (color>>8)&0xffff, (color>>16)&0xffff};
}

bool CSUTILS::directoryExists(const wchar_t* dirname) 
{
    DWORD attributes = GetFileAttributesW(dirname);
    
    return (attributes != INVALID_FILE_ATTRIBUTES && 
            (attributes & FILE_ATTRIBUTE_DIRECTORY));
}

bool CSUTILS::pathExists(const wchar_t* path) 
{
    DWORD attributes = GetFileAttributesW(path);
    return (attributes != INVALID_FILE_ATTRIBUTES);
}

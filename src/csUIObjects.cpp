#include "csUIObjects.h"
#include "readwfile.h"
#include "csArithmetic/csArithUtils.h"

extern vector<HWND> SECTION;
extern vector<CSTEXT> TITLE;
extern vector<RECT> RECTCL;
extern vector<int> PARID;
extern vector<int> withHScroll;
extern vector<int> withVScroll;
extern vector<POINT> hdcontextExtInPos;
extern vector<POINT> hdcontextExtOutPos;
extern vector<SIZE> hdcontextExtSize;

vector<bool> updateTitleSectionBool;
extern vector<HWND> richEdits;
extern vector<RECT> richEditMarging;
extern vector<CS_RICHEDIT_NUMERIC_BOUNDS> richEditNumericBounds;
extern vector<void(*)(CSARGS)> richEditUpdatingFunction;
extern vector<CSARGS*> richEditUpdatingFunctionArgs;

int TIPS_POPUP;

extern float dimCoef;
void smoothScrolling(CSARGS Args);

CSSCROLLBAR CSUIOBJECTS::addHScrollBar(int* idp, int* idClient, int* idMask, int thick, int autoHideIntensity)
{
    RECT r = RECTCL[*idp];
    int*idc = idClient;
    if(!idc) idc = idp;
    CSSCROLLBAR hscroll(*idp, {0,r.bottom-thick,r.right,thick});
    
    if(idMask)
    {
        hscroll.setClient(*idc,*idMask);
    }
    else
    {
        hscroll.setClient(*idc,*idp);
    }
    
    hscroll.mouseLeaveHide(autoHideIntensity);
    SetTimer(SECTION[hscroll.getId()],0,17,0);
    CSSECMAN::addAction(*idc,smoothScrolling,1,hscroll.getIdPtr());
    withHScroll[*idc] = hscroll.getId();
    return hscroll;
}

CSSCROLLBAR CSUIOBJECTS::addVScrollBar(int* idp, int* idClient, int* idMask, int thick, int autoHideIntensity)
{
    RECT r = RECTCL[*idp];
    int*idc = idClient;
    if(!idc) idc = idp;
    CSSCROLLBAR vscroll(*idp, {r.right-thick,0,thick,r.bottom}, 1, CS_SBAR_VERTICAL);
    if(idMask)
    {
        vscroll.setClient(*idc,*idMask);
    }
    else
    {
        vscroll.setClient(*idc,*idp);
    }
    vscroll.mouseLeaveHide(autoHideIntensity);
    SetTimer(SECTION[vscroll.getId()],0,18,0);
    CSSECMAN::addAction(*idc,smoothScrolling,1,vscroll.getIdPtr());
    withVScroll[*idc] = vscroll.getId();
    return vscroll;
}

void smoothScrolling(CSARGS Args)
{
    if(UINT(Args) == WM_MOUSEWHEEL)
    {
        int idSb = *(int*)Args[0];
        SendMessage(SECTION[idSb], WM_TIMER,0,0);
    }
    else if(UINT(Args) == WM_SIZE)
    {
        /*int edge = 5;
        int idSb = *(int*)Args[0];
        int thick = RECTCL[idSb].bottom;
        int a = withVScroll[idp] ? thick : 0;
        int a = withVScroll[idp] ? thick : 0;
        hdcontextExtSize[int(Args)] = {cx - 2*edge - hdcontextExtOutPos[idp].x, cy - 2*edge - thick - hdcontextExtOutPos[idp].y};*/
        //int id = int(Args);
        //hdcontextExtSize[id] = {RECTCL[id].right - 2*hdcontextExtOutPos[id].x, RECTCL[id].bottom - 2*hdcontextExtOutPos[id].y};
    }
}


int CSUIOBJECTS::iconButton01(int idp, char* path1, char* path2, RECT r)
{

    int BUTTON = CSSECMAN::createSection(idp, r,  RGB(30,30,30), {0,0,0,0});
    CSBIND_GEOM_PARAMS bd = {BUTTON, {0,0,1,0}, {0,0,BIND_DEST_LEFT_EDGE,0}};
    CSSECMAN::bindGeometry(idp, bd);
    CSUIFX::setImageGradient(BUTTON, path1, path2, {0}, {r.bottom*dimCoef,r.bottom*dimCoef}, 0.1, 4,1);

    return BUTTON;
}

int CSUIOBJECTS::iconButton02(int idp, char* path1, char* path2, RECT r)
{

    int BUTTON = CSSECMAN::createSection(idp, r,  RGB(30,30,30), {0,0,0,0});
    /*CSBIND_GEOM_PARAMS bd = {BUTTON, {0,0,1,0}, {0,0,BIND_DEST_LEFT_EDGE,0}};
    CSSECMAN::bindGeometry(idp, bd);*/
    CSUIFX::setImageGradient(BUTTON, path1, path2, {0}, {r.bottom*dimCoef,r.bottom*dimCoef}, 0.1, 4,1);

    return BUTTON;
}

int CSUIOBJECTS::darkTextButton01(int idp, wchar_t*title, RECT r, int fontSize)
{

    int BUTTON = CSSECMAN::createSection(idp, r,  RGB(30,30,30), {0,0,0,0});
    CSBIND_GEOM_PARAMS bd = {BUTTON, {0,0,1,0}, {0,0,BIND_DEST_RIGHT_EDGE,0}};
    CSSECMAN::bindGeometry(idp, bd);
    CSUIFX::setBackgroundColorGradient(BUTTON, {50,50,50}, {20,20,20}, 2, 2);
    CSUIFX::setBackgroundColorClickEffect(BUTTON, {60,60,60});
    CSUIFX::setTitleColorGradient(BUTTON, {220,200,100}, {200,200,200}, 2, 2);
    CSUIFX::setTitleColorClickEffect(BUTTON, {220,220,120});
    CSUIFX::setBorderColorGradient(BUTTON, {100,100,100}, {25,25,25}, 2, 2);
    CSUIFX::setBorderColorClickEffect(BUTTON, {120,120,120});
    CSSECMAN::setBorderThick(BUTTON, 2);
    CSSECMAN::setTitle(BUTTON, CSTEXT{.Text=title, .Font=L"Segoe UI", .FontSize=fontSize, .Color={150,150,150},
                            .Marging={0,0}, .Align = CS_TA_CENTER, .Show=1});

    return BUTTON;
}

int CSUIOBJECTS::darkImgTextButton01(int idp, wchar_t*title, char* path1, char* path2, RECT r, int fontSize)
{

    int BUTTON = CSSECMAN::createSection(idp, r,  RGB(30,30,30), {0,0,0,0});
    CSBIND_GEOM_PARAMS bd = {BUTTON, {0,0,1,0}, {0,0,BIND_DEST_RIGHT_EDGE,0}};
    CSSECMAN::bindGeometry(idp, bd);
    CSUIFX::setImageGradient(BUTTON, path1, path2, {0}, {26,26}, 0.1, 2, 2);
    CSUIFX::setBackgroundColorGradient(BUTTON, {5,5,5}, {20,20,20}, 2, 2);
    CSUIFX::setBackgroundColorClickEffect(BUTTON, {10,10,10});
    CSUIFX::setTitleColorGradient(BUTTON, {220,200,100}, {200,200,200}, 2, 2);
    CSUIFX::setTitleColorClickEffect(BUTTON, {220,220,120});
    CSUIFX::setBorderColorGradient(BUTTON, {10,10,10}, {25,25,25}, 2, 2);
    CSUIFX::setBorderColorClickEffect(BUTTON, {20,20,20});
    CSSECMAN::setBorderThick(BUTTON, 2);
    CSSECMAN::setTitle(BUTTON, CSTEXT{.Text=title, .Font=L"Segoe UI", .FontSize=fontSize, .Color={150,150,140},
                            .Marging={32/dimCoef,0}, .Align = CS_TA_CENTER_LEFT, .Show=1});

    return BUTTON;
}

int CSUIOBJECTS::darkTextButton02(int idp, wchar_t*title, RECT r, int fontSize)
{

    int BUTTON = CSSECMAN::createSection(idp, r,  RGB(30,30,30), {0,0,0,0});
    CSBIND_GEOM_PARAMS bd = {BUTTON, {0,0,1,0}, {0,0,BIND_DEST_RIGHT_EDGE,0}};
    CSSECMAN::bindGeometry(idp, bd);
    CSUIFX::setBackgroundColorGradient(BUTTON, {50,50,50}, {20,20,20}, 2, 2);
    CSUIFX::setBackgroundColorClickEffect(BUTTON, {60,60,60});
    CSUIFX::setTitleColorGradient(BUTTON, {220,220,220}, {200,200,200}, 2, 2);
    CSUIFX::setTitleColorClickEffect(BUTTON, {190,190,190});
    CSUIFX::setBorderColorGradient(BUTTON, {25,25,25}, {25,25,25}, 2, 2);
    CSUIFX::setBorderColorClickEffect(BUTTON, {120,120,120});
    CSSECMAN::setBorderThick(BUTTON, 1);
    CSSECMAN::setTitle(BUTTON, CSTEXT{.Text=title, .Font=L"Segoe UI", .FontSize=fontSize, .Color={200,200,200},
                            .Marging={0,0}, .Align = CS_TA_CENTER, .Show=1});

    return BUTTON;
}

CSSYSCOMMAND_SECTION CSUIOBJECTS::addSysCommand(int& id, POINT pos)
{
    CSSYSCOMMAND_SECTION sc;
    int width = (GetSystemMetrics(SM_CYCAPTION)-2);
    if(dimCoef < 1.5)
        sc.SYSCOMMAND_SECTION = CSSECMAN::createSection(id, {RECTCL[id].right/dimCoef-3*width-2-6,pos.y,3*width+3,width},  RGB(5,5,5), {0,0,0,0});
    else
        sc.SYSCOMMAND_SECTION = CSSECMAN::createSection(id, {RECTCL[id].right/dimCoef-3*width-2,pos.y,3*width,width},  RGB(5,5,5), {0,0,0,0});
    CSBIND_GEOM_PARAMS bd = {sc.SYSCOMMAND_SECTION, {-1,0,1,0}, {BIND_DEST_LEFT_EDGE,0,BIND_DEST_LEFT_EDGE,0}};
    CSSECMAN::bindGeometry(id, bd);
    CSSECMAN::setBorderColorAndThick(sc.SYSCOMMAND_SECTION, RGB(20,20,20), 1);

    width -= 2;
    if(dimCoef < 1.5)
    {
        width+=3;
        sc.SYS_MIN = CSUIOBJECTS::iconButton01(sc.SYSCOMMAND_SECTION, "resources/min20.bmp\0", "resources/min20.bmp\0", {-1,-1,width,width});
        sc.SYS_MAX = CSUIOBJECTS::iconButton01(sc.SYSCOMMAND_SECTION, "resources/max20.bmp\0", "resources/max20.bmp\0", {-1+width,-1,width,width});
        sc.SYS_CLOSE = CSUIOBJECTS::iconButton01(sc.SYSCOMMAND_SECTION, "resources/close20.bmp\0", "resources/close20.bmp\0", {-1+width*2,-1,width,width});
    }
    else
    {
        sc.SYS_MIN = CSUIOBJECTS::iconButton01(sc.SYSCOMMAND_SECTION, "resources/_min02.bmp\0", "resources/_min01.bmp\0", {1,1,width,width});
        sc.SYS_MAX = CSUIOBJECTS::iconButton01(sc.SYSCOMMAND_SECTION, "resources/_max02.bmp\0", "resources/_max01.bmp\0", {1+width,1,width,width});
        sc.SYS_CLOSE = CSUIOBJECTS::iconButton01(sc.SYSCOMMAND_SECTION, "resources/_close02.bmp\0", "resources/_close01.bmp\0", {1+width*2,1,width,width});
    }
    CSSECMAN::setAsMinButton(sc.SYS_MIN, id);
    CSSECMAN::setAsMaxButton(sc.SYS_MAX, id);
    CSSECMAN::setAsCloseButton(sc.SYS_CLOSE, id);

    return sc;
}

/*int CSUIOBJECTS::addTitle(int& id, wchar_t*title, SIZE size, char*iconPath, int fontSize, wchar_t*fontName)
{

    if(size.cx == 0 || size.cy == 0)
    {
        HFONT hf = CreateFontW(CSUTILS::getAdjustedFontSizeX(fontSize),
                              0,
                              0, 0, 0, 0,0,0,DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS,CLIP_STROKE_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH|FF_DONTCARE, fontName);
        LPSIZE lps = CSUTILS::textExtentW(SECTION[id], hf, title);
        size = {lps->cx, lps->cy};
        free(lps);
        DeleteFont(hf);

        if(iconPath)
            size.cx += 26;
    }
    
    int TITLE_SECTION = CSSECMAN::createSection(id, {2,0,size.cx,size.cy},  RGB(5,5,5), {0,0,0,0});
    
    CSSECMAN::setBorderColorAndThick(TITLE_SECTION, RGB(20,20,20), 1);
    if(iconPath)
        CSUIFX::setImageGradient(TITLE_SECTION, iconPath, iconPath, {2,2}, {26,26}, 0.05, 2,1);
    CSSECMAN::setTitle(TITLE_SECTION, CSTEXT{.Text=title, .Font=fontName, .FontSize = fontSize, .Italic=0,
                                   .Bold=0, .Color={150,150,150},
                                   .Marging={35/dimCoef,0}, .Align = CS_TA_CENTER_LEFT, .Show=1});
    
    CSSECMAN::inert(TITLE_SECTION,190);
    SetWindowTextW(SECTION[id], title);

    CSSECMAN::autoFitToTitle(TITLE_SECTION, 10);

    auto f = [](CSARGS Args)
    {
        if(UINT(Args) == WM_TIMER)
        {
            int id = (int)Args;
            if(!updateTitleSectionBool[id] && IsWindowVisible(GetParent(HWND(Args))))
            {
                InvalidateRect(HWND(Args),0,1);
                updateTitleSectionBool[id] = 1;
                RECT r = TITLE[id].TextRect;
                SetWindowPos(SECTION[id], 0, 0,0,r.right, 20*dimCoef, SWP_NOMOVE|SWP_NOZORDER);
            }
        }
    };

    CSSECMAN::addAction(TITLE_SECTION, f, 0);
    
    return TITLE_SECTION;
}*/

extern std::vector<CSAPP_ICON> appIcon;
extern vector<HDC> hdcontext;
extern vector<HDC> hdStackContext;
extern vector<int> ICONID;

int CSUIOBJECTS::addTitle(int& id, wchar_t*title, SIZE size, int fontSize, wchar_t*fontName, int iconId, int alphaLevel)
{

    if(size.cx == 0 || size.cy == 0)
    {
        HFONT hf = CreateFontW(CSUTILS::getAdjustedFontSizeX(fontSize),
                              0,
                              0, 0, 0, 0,0,0,DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS,CLIP_STROKE_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH|FF_DONTCARE, fontName);
        LPSIZE lps = CSUTILS::textExtentW(SECTION[id], hf, title);
        size = {lps->cx, lps->cy};
        free(lps);
        DeleteFont(hf);

        if(iconId > -1) // remplace par autoFitToTitle()
            size.cx += appIcon[iconId].rectSmall.left+appIcon[iconId].rectSmall.right;
    }
    
    int TITLE_SECTION = CSSECMAN::createSection(id, {2,0,size.cx,size.cy},  RGB(5,5,5), {0,0,0,0});

    CSSECMAN::setBorderColorAndThick(TITLE_SECTION, RGB(20,20,20), 1);
    CSSECMAN::setTitle(TITLE_SECTION, CSTEXT{.Text=title, .Font=fontName, .FontSize = fontSize, .Italic=0,
                                   .Bold=0, .Color={150,150,150},
                                   .Marging={(iconId>-1?appIcon[iconId].rectSmall.left+appIcon[iconId].rectSmall.right:0)+5,0}, .Align = CS_TA_CENTER_LEFT, .Show=1});
    ICONID[TITLE_SECTION] = iconId;
    
    
    CSSECMAN::inert(TITLE_SECTION,255-alphaLevel);
    SetWindowTextW(SECTION[id], title);

    CSSECMAN::autoFitToTitle(TITLE_SECTION, 10);

    auto f = [](CSARGS Args)
    {
        if(UINT(Args) == WM_TIMER)
        {
            int id = (int)Args;
            if(!updateTitleSectionBool[id] && IsWindowVisible(GetParent(HWND(Args))))
            {
                InvalidateRect(HWND(Args),0,1);
                updateTitleSectionBool[id] = 1;
                RECT r = TITLE[id].TextRect;
                SetWindowPos(SECTION[id], 0, 0,0,r.right, 20*dimCoef, SWP_NOMOVE|SWP_NOZORDER);
            }
        }
    };

    CSSECMAN::addAction(TITLE_SECTION, f, 0);
    
    return TITLE_SECTION;
}


extern int csCreateRichEdit(int idpar, RECT marging, const wchar_t* defaultText, int style, int hMenuId);

CS_NUMERIC_INCREMENTER_PARAMS CSUIOBJECTS::numericIncrementer(int idp, RECT r, double value, double incr, bool intNum)
{
    CS_NUMERIC_INCREMENTER_PARAMS nip;
    nip.idSection = CSSECMAN::createSection(idp, r,  RGB(15,15,15), {0,0,0,0});
    //bd = {idp, {-0.5,0,0.5,0}, {BIND_DEST_LEFT_EDGE,0,BIND_DEST_LEFT_EDGE,0}};
    //bindGeometry(ROOT, bd);
    int l = (r.bottom-4);
    //CSUIFX::setBorderColorGradient(nip.idSection, {100,100,100}, {40,40,40}, 2, 2);
    CSSECMAN::setBorderThick(nip.idSection, 2);
    CSSECMAN::setTitle(nip.idSection, CSTEXT{.Text=L"\0", .Font=L"Arial", .FontSize=14, .Italic=1, .Color={100,100,100},
                                   .Marging={-45,0}, .Align = CS_TA_CENTER, .Show=1});

    nip.idUp = CSUIOBJECTS::iconButton02(nip.idSection, "resources/increase.bmp\0", "resources/increase2.bmp\0", {r.right-l*3-2,2,l,l});
    nip.idDown = CSUIOBJECTS::iconButton02(nip.idSection, "resources/decrease.bmp\0", "resources/decrease2.bmp\0", {r.right-l*2-2,2,l,l});
    nip.idValidate = CSUIOBJECTS::iconButton02(nip.idSection, "resources/go.bmp\0", "resources/go2.bmp\0", {r.right-l-2,2,l,l});


    wchar_t num[5000];
    if(intNum)
        wsprintf(num, L"%s\0", to_wstring((long)value).c_str());
    else
        wsprintf(num, L"%s\0", to_wstring(value).c_str());
    
    nip.idText = csCreateRichEdit(nip.idSection, {4,4,((l+1)*3)*dimCoef,4}, (const wchar_t*)num, 0, 0);
    setRichEditColors(nip.idText, 0, RGB(120,120,120));

    return nip;
}

extern void csSetRichEditFormat(int id, short format);
void incrementFunction(CSARGS Args);
void decrementFunction(CSARGS Args);

void CS_NUMERIC_INCREMENTER_PARAMS::setMinBound(const char* num)
{
    min = num;
    useMinBound = 1;
    richEditNumericBounds[idText].minBound = (char*)num;
}

void CS_NUMERIC_INCREMENTER_PARAMS::setMaxBound(const char* num)
{
    max = num;
    useMaxBound = 1;
    richEditNumericBounds[idText].maxBound = (char*)num;
}

CS_NUMERIC_INCREMENTER_PARAMS CSUIOBJECTS::numericIncrementerExt(int idp, RECT r, wchar_t* value, wchar_t* step, short format)
{
    CS_NUMERIC_INCREMENTER_PARAMS nip = {0};
    nip.idSection = CSSECMAN::createSection(idp, r,  RGB(25,25,25), {0,0,0,0});
    //bd = {idp, {-0.5,0,0.5,0}, {BIND_DEST_LEFT_EDGE,0,BIND_DEST_LEFT_EDGE,0}};
    //bindGeometry(ROOT, bd);
    int l = (r.bottom-4);
    //CSUIFX::setBorderColorGradient(nip.idSection, {100,100,100}, {40,40,40}, 2, 2);
    CSSECMAN::setBorderThick(nip.idSection, 2);
    CSSECMAN::setTitle(nip.idSection, CSTEXT{.Text=L"\0", .Font=L"Arial", .FontSize=12, .Italic=1, .Bold=FW_BOLD, .Color={100,100,100},
                                   .Marging={-((l+1)*3)*dimCoef/2,0}, .Align = CS_TA_CENTER, .Show=1, .ShowEntierText=1});

    nip.idUp = CSUIOBJECTS::iconButton02(nip.idSection, "resources/increase.bmp\0", "resources/increase2.bmp\0", {r.right-l*3-2,2,l,l});
    nip.idDown = CSUIOBJECTS::iconButton02(nip.idSection, "resources/decrease.bmp\0", "resources/decrease2.bmp\0", {r.right-l*2-2,2,l,l});
    nip.idValidate = CSUIOBJECTS::iconButton02(nip.idSection, "resources/go.bmp\0", "resources/go2.bmp\0", {r.right-l-2,2,l,l});

    nip.idText = csCreateRichEdit(nip.idSection, {4,4,((l+1)*3)*dimCoef,4}, (const wchar_t*)value, 0, 0);
    setRichEditColors(nip.idText, 0, RGB(120,120,120));
    csSetRichEditFormat(nip.idText, format);
    char*nb = (char*)wcharPtrToCharPtr((const wchar_t*)step).c_str();
    bool sign = CSUTILS::signExtraction(nb); 
    nip.step.set(nb,0,sign);
    //free(nb);

    bool*titleAutoRepos = csAlloc<bool>(1,1);
    CSSECMAN::addAction(nip.idUp, incrementFunction, 2, &nip, titleAutoRepos);
    CSSECMAN::addAction(nip.idDown, decrementFunction, 2, &nip, titleAutoRepos);
    return nip;
}

void incrementFunction(CSARGS Args)
{
    UINT msg = UINT(Args);
    int timerid = 2;
    static int delay = 5, count = 0;
    bool titleAutoRepos = *(bool*)Args[1];
    if(msg == WM_LBUTTONDOWN||msg == WM_LBUTTONDBLCLK)
    {
        CS_NUMERIC_INCREMENTER_PARAMS nip = *(CS_NUMERIC_INCREMENTER_PARAMS*)Args[0];
        const char*nb = wcharPtrToCharPtr((const wchar_t*)TITLE[nip.idSection].Text).c_str();

        CSARITHMETIC::csRNUMBER res(0,nb);
        if(nip.useMaxBound)
        {
            if(res < nip.max)
                res = res + nip.step;
            else
                res.copy(nip.max);
        }
        else
        {
            res = res + nip.step;
        }
        free(TITLE[nip.idSection].Text);
        char*printFormat = CSARITHMETIC::getPrintFormat(res);
        wchar_t* text = csAlloc<wchar_t>((strlen(printFormat)+1));
        wsprintf(text, L"%s\0",  (wchar_t*)charPtrtoWcharPtr(printFormat).c_str());

        TITLE[nip.idSection].Text = text;
        InvalidateRect(SECTION[nip.idSection],0,1);
        res.clear();

        SetWindowTextW(richEdits[nip.idText], (LPCWSTR)text);

        if(richEditUpdatingFunction[nip.idText])
            richEditUpdatingFunction[nip.idText](*(richEditUpdatingFunctionArgs[nip.idText]));

        if(msg == WM_LBUTTONDOWN)
        {
            SetTimer((HWND)Args, timerid, 100, 0);
            SetCapture(HWND(Args));
            count = 0;
        }
    }

    else if(msg == WM_TIMER && WPARAM(Args) == timerid)
    {
        if(count < delay)
        {
            count++;
        }

        if(count == delay)
        SendMessage(HWND(Args), WM_LBUTTONDBLCLK, 0, 0);
    }

    else if(msg == WM_LBUTTONUP)
    {
        KillTimer(HWND(Args), timerid);
        ReleaseCapture();

        if(titleAutoRepos)
        {
            CS_NUMERIC_INCREMENTER_PARAMS nip = *(CS_NUMERIC_INCREMENTER_PARAMS*)Args[0];
            RECT rt = TITLE[nip.idSection].TextRect;

            if(rt.right-rt.left - TITLE[nip.idSection].Marging.left > RECTCL[nip.idSection].right)
            {
                TITLE[nip.idSection].Align = CS_TA_CENTER_RIGHT;
                TITLE[nip.idSection].Marging.left = -richEditMarging[nip.idText].right;
            }
            else
            {
                TITLE[nip.idSection].Align = CS_TA_CENTER;
                TITLE[nip.idSection].Marging.left = -richEditMarging[nip.idText].right/2;
            }
        }
        //InvalidateRect(SECTION[nip.idSection],0,1);
    }

}

void decrementFunction(CSARGS Args)
{
    UINT msg = UINT(Args);
    int timerid = 2;
    static int delay=5, count = 0;
    bool titleAutoRepos = *(bool*)Args[1];
    if(msg == WM_LBUTTONDOWN||msg == WM_LBUTTONDBLCLK)
    {
        CS_NUMERIC_INCREMENTER_PARAMS nip = *(CS_NUMERIC_INCREMENTER_PARAMS*)Args[0];
        const char*nb = wcharPtrToCharPtr((const wchar_t*)TITLE[nip.idSection].Text).c_str();
        CSARITHMETIC::csRNUMBER res(0, nb);

        if(nip.useMinBound)
        {
            if(res > nip.min)
                res = res - nip.step;
            else
                res.copy(nip.min);
        }
        else
        {
            res = res - nip.step;
        }
        
        free(TITLE[nip.idSection].Text);
        char*printFormat = CSARITHMETIC::getPrintFormat(res);
        
        wchar_t* text = csAlloc<wchar_t>((strlen(printFormat)+1));
        wsprintf(text, L"%s\0",  (wchar_t*)charPtrtoWcharPtr(printFormat).c_str());

        TITLE[nip.idSection].Text = text;
        InvalidateRect(SECTION[nip.idSection],0,1);
        res.clear();

        SetWindowTextW(richEdits[nip.idText], (LPCWSTR)text);

        if(richEditUpdatingFunction[nip.idText])
            richEditUpdatingFunction[nip.idText](*(richEditUpdatingFunctionArgs[nip.idText]));

        if(msg == WM_LBUTTONDOWN)
        {
            SetTimer((HWND)Args, timerid, 100, 0);
            SetCapture(HWND(Args));
            count = 0;
        }
    }

    else if(msg == WM_TIMER && WPARAM(Args) == timerid)
    {
        if(count < delay)
        {
            count++;
        }
        
        if(count == delay)
        SendMessage(HWND(Args), WM_LBUTTONDBLCLK, 0, 0);
    }

    else if(msg == WM_LBUTTONUP)
    {
        KillTimer(HWND(Args), timerid);
        ReleaseCapture();
        
        if(titleAutoRepos)
        {
            CS_NUMERIC_INCREMENTER_PARAMS nip = *(CS_NUMERIC_INCREMENTER_PARAMS*)Args[0];
            RECT rt = TITLE[nip.idSection].TextRect;

            if(rt.right-rt.left - TITLE[nip.idSection].Marging.left > RECTCL[nip.idSection].right)
            {
                TITLE[nip.idSection].Align = CS_TA_CENTER_RIGHT;
                TITLE[nip.idSection].Marging.left = -richEditMarging[nip.idText].right;
            }
            else
            {
                TITLE[nip.idSection].Align = CS_TA_CENTER;
                TITLE[nip.idSection].Marging.left = -richEditMarging[nip.idText].right/2;
            }
        }
        //InvalidateRect(SECTION[nip.idSection],0,1);
        
    }

};


CS_NUMERIC_INCREMENTER_PARAMS CSUIOBJECTS::numericIncrementerExt1(int idp, RECT r, wchar_t* value, wchar_t* step, short format, COEFS4 bindCoefs, FLAGS4 bindFlags)
{
    CS_NUMERIC_INCREMENTER_PARAMS nip = {0};
    nip.idSection = CSSECMAN::createSection(idp, r,  RGB(25,25,25), {0,0,0,0});
    CSSECMAN::setBorderColorAndThick(nip.idSection, RGB(40,40,40), 1);
    CSBIND_GEOM_PARAMS bd;
    bd = {nip.idSection, bindCoefs, bindFlags};
    CSSECMAN::bindGeometry(idp, bd);
    int l = (r.bottom-4);
    //CSUIFX::setBorderColorGradient(nip.idSection, {100,100,100}, {40,40,40}, 2, 2);
    CSSECMAN::setBorderThick(nip.idSection, 2);
    CSSECMAN::setTitle(nip.idSection, CSTEXT{.Text=L"\0", .Font=L"Arial", .FontSize=12, .Italic=1, .Bold=FW_BOLD, .Color={100,100,100},
                                   .Marging={-5,0}, .Align = CS_TA_CENTER_RIGHT, .Show=1, .ShowEntierText=1});

    nip.idUp = CSUIOBJECTS::iconButton02(nip.idSection, "resources/u1.bmp\0", "resources/u1.bmp\0", {2,2,l,l});
    nip.idDown = CSUIOBJECTS::iconButton02(nip.idSection, "resources/d1.bmp\0", "resources/d1.bmp\0", {2+l,2,l,l});

    
    nip.idText = csCreateRichEdit(nip.idSection, {(2+l*2+2)*dimCoef,4,4,4}, (const wchar_t*)value, 0, 0);
    setRichEditColors(nip.idText, 0, RGB(120,120,120));
    csSetRichEditFormat(nip.idText, format);
    char*nb = (char*)wcharPtrToCharPtr((const wchar_t*)step).c_str();
    bool sign = CSUTILS::signExtraction(nb); 
    nip.step.set(nb,0,sign);
    //free(nb);


    bool*titleAutoRepos = csAlloc<bool>(1,0);
    CSSECMAN::addAction(nip.idUp, incrementFunction, 2, &nip, titleAutoRepos);
    CSSECMAN::addAction(nip.idDown, decrementFunction, 2, &nip, titleAutoRepos);
    return nip;
}

void reposIncBtn(CSARGS Args);
CS_NUMERIC_INCREMENTER_PARAMS CSUIOBJECTS::numericIncrementerExt2(int idp, RECT r, wchar_t* value, wchar_t* step, short format)
{
    CS_NUMERIC_INCREMENTER_PARAMS nip = {0};
    nip.idSection = CSSECMAN::createSection(idp, r,  RGB(30,30,30), {0,0,0,0});
    CSBIND_GEOM_PARAMS bd;
    bd = {nip.idSection, {0,0,1,0}, {0,0,BIND_DEST_RIGHT_EDGE,0}};
    CSSECMAN::bindGeometry(idp, bd);
    int l = (r.bottom-4);
    //CSUIFX::setBorderColorGradient(nip.idSection, {100,100,100}, {40,40,40}, 2, 2);
    CSSECMAN::setBorderThick(nip.idSection, 2);
    CSSECMAN::setTitle(nip.idSection, CSTEXT{.Text=L"\0", .Font=L"Arial", .FontSize=12, .Italic=1, .Bold=FW_BOLD, .Color={100,100,100},
                                   .Marging={0,0}, .Align = CS_TA_CENTER, .Show=1, .ShowEntierText=1});

    nip.idUp = CSUIOBJECTS::iconButton02(nip.idSection, "resources/next2.bmp\0", "resources/next.bmp\0", {r.right-l-2,2,l,l});
    nip.idDown = CSUIOBJECTS::iconButton02(nip.idSection, "resources/back2.bmp\0", "resources/back.bmp\0", {2,2,l,l});

    
    int c = 2*dimCoef;
    nip.idText = csCreateRichEdit(nip.idSection, {(2+l+2)*dimCoef,c,(2+l+2)*dimCoef,c}, (const wchar_t*)value, 0, 0);
    setRichEditColors(nip.idText, 0, RGB(120,120,120));
    csSetRichEditFormat(nip.idText, format);
    char*nb = (char*)wcharPtrToCharPtr((const wchar_t*)step).c_str();
    bool sign = CSUTILS::signExtraction(nb); 
    nip.step.set(nb,0,sign);
    //free(nb);


    bool*titleAutoRepos = csAlloc<bool>(1,0);
    CSSECMAN::addAction(nip.idUp, incrementFunction, 2, &nip, titleAutoRepos);
    CSSECMAN::addAction(nip.idDown, decrementFunction, 2, &nip, titleAutoRepos);
    CSSECMAN::addAction(nip.idSection, reposIncBtn, 1, &nip.idUp);
    return nip;
}

void reposIncBtn(CSARGS Args)
{
    if(UINT(Args) == WM_SIZE)
    {
        RECT r = RECTCL[int(Args)];
        int c = 2*dimCoef;
        MoveWindow(SECTION[*(int*)Args[0]], r.right-r.bottom+c, c, r.bottom-c, r.bottom-c, 1);
    }
}

void CSUIOBJECTS::createToolTips(COLORREF color)
{
    if(SECTION.size())
    {
        TIPS_POPUP = CSSECMAN::createSection(0, {0,0,200/dimCoef,200/dimCoef}, color, {0}, 0, 0, 0);
        CSSECMAN::enableDarkEdge(TIPS_POPUP);
        SetTimer(SECTION[TIPS_POPUP], 0, 20, 0);
    }
}




void strDecrementFunction(CSARGS Args)
{
    UINT msg = UINT(Args);
    int timerid = 2;
    static int delay=5, count = 0;
    bool titleAutoRepos = *(bool*)Args[1];
    if(msg == WM_LBUTTONDOWN||msg == WM_LBUTTONDBLCLK)
    {
        CS_STRING_INCREMENTER_PARAMS sip = *(CS_STRING_INCREMENTER_PARAMS*)Args[0];

        if(!sip.item.size())
            return;

        long res = sip.currentItem - sip.step;
        if(res < 0)
        {
            if(sip.loopStyle)
                res = res + sip.item.size();
            else
                res = res + sip.step;
        }

        ((CS_STRING_INCREMENTER_PARAMS*)Args[0])->currentItem = res;

        wchar_t*& text = TITLE[sip.idSection].Text;
        int t;
        text = (wchar_t*)realloc(text, (t = wcslen(sip.item[res].c_str())+1)*sizeof(wchar_t));
        wcscpy_s(text, t,  sip.item[res].c_str());

        //TITLE[sip.idSection].Text = text;
        InvalidateRect(SECTION[sip.idSection],0,1);

        SetWindowTextW(richEdits[sip.idText], (LPCWSTR)text);

        if(richEditUpdatingFunction[sip.idText])
            richEditUpdatingFunction[sip.idText](*(richEditUpdatingFunctionArgs[sip.idText]));

        if(msg == WM_LBUTTONDOWN)
        {
            SetTimer((HWND)Args, timerid, 100, 0);
            SetCapture(HWND(Args));
            count = 0;
        }
    }

    else if(msg == WM_TIMER && WPARAM(Args) == timerid)
    {
        if(count < delay)
        {
            count++;
        }
        
        if(count == delay)
        SendMessage(HWND(Args), WM_LBUTTONDBLCLK, 0, 0);
    }

    else if(msg == WM_LBUTTONUP)
    {
        KillTimer(HWND(Args), timerid);
        ReleaseCapture();
        
        if(titleAutoRepos)
        {
            CS_STRING_INCREMENTER_PARAMS sip = *(CS_STRING_INCREMENTER_PARAMS*)Args[0];
            RECT rt = TITLE[sip.idSection].TextRect;

            if(rt.right-rt.left - TITLE[sip.idSection].Marging.left > RECTCL[sip.idSection].right)
            {
                TITLE[sip.idSection].Align = CS_TA_CENTER_RIGHT;
                TITLE[sip.idSection].Marging.left = -richEditMarging[sip.idText].right;
            }
            else
            {
                TITLE[sip.idSection].Align = CS_TA_CENTER;
                TITLE[sip.idSection].Marging.left = -richEditMarging[sip.idText].right/2;
            }
        }
        //InvalidateRect(SECTION[nip.idSection],0,1);
        
    }

};

void strIncrementFunction(CSARGS Args)
{
    UINT msg = UINT(Args);
    int timerid = 2;
    static int delay=5, count = 0;
    bool titleAutoRepos = *(bool*)Args[1];
    if(msg == WM_LBUTTONDOWN||msg == WM_LBUTTONDBLCLK)
    {
        CS_STRING_INCREMENTER_PARAMS sip = *(CS_STRING_INCREMENTER_PARAMS*)Args[0];
        
        if(!sip.item.size())
            return;

        long res = sip.currentItem + sip.step;
        if(res >= sip.item.size())
        {
            if(sip.loopStyle)
                res = res - sip.item.size();
            else
                res = res - sip.step;
        }

        ((CS_STRING_INCREMENTER_PARAMS*)Args[0])->currentItem = res;
        
        wchar_t*& text = TITLE[sip.idSection].Text;
        int t;
        text = (wchar_t*)realloc(text, (t = wcslen(sip.item[res].c_str())+1)*sizeof(wchar_t));
        wcscpy_s(text, t,  sip.item[res].c_str());
        //wcout<<45<<L" itemsizwe\n";
        //TITLE[sip.idSection].Text = text;
        InvalidateRect(SECTION[sip.idSection],0,1);

        SetWindowTextW(richEdits[sip.idText], (LPCWSTR)text);

        if(richEditUpdatingFunction[sip.idText])
            richEditUpdatingFunction[sip.idText](*(richEditUpdatingFunctionArgs[sip.idText]));

        if(msg == WM_LBUTTONDOWN)
        {
            SetTimer((HWND)Args, timerid, 100, 0);
            SetCapture(HWND(Args));
            count = 0;
        }
    }

    else if(msg == WM_TIMER && WPARAM(Args) == timerid)
    {
        if(count < delay)
        {
            count++;
        }
        
        if(count == delay)
        SendMessage(HWND(Args), WM_LBUTTONDBLCLK, 0, 0);
    }

    else if(msg == WM_LBUTTONUP)
    {
        KillTimer(HWND(Args), timerid);
        ReleaseCapture();
        
        if(titleAutoRepos)
        {
            CS_STRING_INCREMENTER_PARAMS sip = *(CS_STRING_INCREMENTER_PARAMS*)Args[0];
            RECT rt = TITLE[sip.idSection].TextRect;

            if(rt.right-rt.left - TITLE[sip.idSection].Marging.left > RECTCL[sip.idSection].right)
            {
                TITLE[sip.idSection].Align = CS_TA_CENTER_RIGHT;
                TITLE[sip.idSection].Marging.left = -richEditMarging[sip.idText].right;
            }
            else
            {
                TITLE[sip.idSection].Align = CS_TA_CENTER;
                TITLE[sip.idSection].Marging.left = -richEditMarging[sip.idText].right/2;
            }
        }
        //InvalidateRect(SECTION[nip.idSection],0,1);
        
    }

};

void CS_STRING_INCREMENTER_PARAMS::newItem(const wchar_t* title, bool _status)
{
    int sz = richEditNumericBounds[idText].maxBound ? strlen(richEditNumericBounds[idText].maxBound)+2 : 2;
    richEditNumericBounds[idText].maxBound = (char*)realloc(richEditNumericBounds[idText].maxBound, sz);
    sprintf(richEditNumericBounds[idText].maxBound, "%d", item.size());

    item.push_back(title);
    status.push_back(_status);
}

CS_STRING_INCREMENTER_PARAMS* CSUIOBJECTS::stringIncrementer(int idp, RECT r, wchar_t* value, long step, bool loopStyle, COEFS4 bindCoefs, FLAGS4 bindFlags)
{
    CS_STRING_INCREMENTER_PARAMS sip = {0};
    sip.idSection = CSSECMAN::createSection(idp, r,  RGB(30,30,30), {0,0,0,0});
    sip.newItem((const wchar_t*)value, 1);
    sip.step = step;
    sip.currentItem = 0;

    CSBIND_GEOM_PARAMS bd;
    bd = {sip.idSection, bindCoefs, bindFlags};
    CSSECMAN::bindGeometry(idp, bd);
    int l = (r.bottom-4);
    
    CSSECMAN::setBorderThick(sip.idSection, 2);
    CSSECMAN::setTitle(sip.idSection, CSTEXT{.Text=L"\0", .Font=L"Arial", .FontSize=12, .Italic=1, .Bold=FW_BOLD, .Color={100,100,100},
                                   .Marging={0,0}, .Align = CS_TA_CENTER, .Show=1, .ShowEntierText=1});

    sip.idUp = CSUIOBJECTS::iconButton02(sip.idSection, "resources/next2.bmp\0", "resources/next.bmp\0", {r.right-l-2,2,l,l});
    sip.idDown = CSUIOBJECTS::iconButton02(sip.idSection, "resources/back2.bmp\0", "resources/back.bmp\0", {2,2,l,l});

    
    int c = 2*dimCoef;
    sip.idText = csCreateRichEdit(sip.idSection, {(2+l+2)*dimCoef,c,(2+l+2)*dimCoef,c}, (const wchar_t*)value, 0, 0);
    csSetRichEditFormat(sip.idText, INPUT_FORMAT_POSITIVE_INTERGER);

    sip.loopStyle = loopStyle;
    
    bool*titleAutoRepos = csAlloc<bool>(1,0);
    CS_STRING_INCREMENTER_PARAMS* psip = new CS_STRING_INCREMENTER_PARAMS;
    *psip = sip;

    CSSECMAN::addAction(sip.idUp, strIncrementFunction, 2, psip, titleAutoRepos);
    CSSECMAN::addAction(sip.idDown, strDecrementFunction, 2, psip, titleAutoRepos);
    CSSECMAN::addAction(sip.idSection, reposIncBtn, 1, &psip->idUp);

    return psip;
}

/************************************************************************************************************************** */

void progressBarAction(CSARGS Args);
void __workAnimFunc(int id, int* iter, double* maxLevel, double* level, wstring workMessage);
void __resultAnimFunc(int id, int* iter, int* resultAnimMaxTime, bool* status, wstring successMessage, wstring errorMessage);

CSUIOBJECTS::CS_ABSTRACT_PROGRESSBAR::CS_ABSTRACT_PROGRESSBAR(int idp, RECT rect)
{
    id = CSSECMAN::createSection(idp, rect,  RGB(20,20,20), {0,0,1});
    CSUIFX::setTitleColorGradient(id, {180,180,180},{220,220,200},3,3);
    CSUIFX::setTitleColorClickEffect(id, {220,220,120});
    CSSECMAN::setTitle(id, CSTEXT{.Text=L"No Task", .Font=L"calibri", .FontSize = 12, .Italic=1,
        .Bold=0, .Color={180,180,180},
        .Marging={0,0}, .Align = CS_TA_CENTER, .Show=1});
    CSSECMAN::autoFitToTitle(id, 10);
    CSLANGMAN::translateTitle(id, 0);

    apArgs = new vector<CS_AP_ARGS>;
    workAnimDeltaTime = new int;
    *workAnimDeltaTime = 1000;
    workAnimFunc = csAlloc<void(*)(int, int*, double*, double*, wstring)>(1, __workAnimFunc);
    resultAnimFunc = csAlloc<void(*)(int, int*, int*,bool*, wstring, wstring)>(1, __resultAnimFunc);
    //workAnimFunc = new (void(*)(int, double*, double*, wstring));
    resultAnimDeltaTime = new int;
    *resultAnimDeltaTime = 1000;
    resultAnimMaxTime = new int;
    *resultAnimMaxTime = 900;

    CSSECMAN::addAction(id, progressBarAction, 6, apArgs, workAnimDeltaTime, workAnimFunc, resultAnimDeltaTime, resultAnimMaxTime, resultAnimFunc);
}

int CSUIOBJECTS::CS_ABSTRACT_PROGRESSBAR::regFunction(void(*func)(CSARGS), CSARGS args, bool* execute, wstring workMessage, wstring successMessage, wstring errorMessage)
{
    apArgs->push_back({func, args, execute, workMessage, successMessage, errorMessage});
    return apArgs->size()-1;
}

void CSUIOBJECTS::CS_ABSTRACT_PROGRESSBAR::setProgressbarAnimFunctions(void(*_workAnimFunc)(int, int*, double*, double*, wstring), void(*_resultAnimFunc)(int, int*, int*, bool*, wstring, wstring))
{
    *workAnimFunc = _workAnimFunc;
    *resultAnimFunc = _resultAnimFunc;
}

void progressBarAction(CSARGS Args)
{
    vector<CSUIOBJECTS::CS_ABSTRACT_PROGRESSBAR::CS_AP_ARGS>* apArgs = (vector<CSUIOBJECTS::CS_ABSTRACT_PROGRESSBAR::CS_AP_ARGS>*)Args[0];
    int id = (int)Args;
    UINT msg = (UINT)Args;
    static bool isworking = 0, showStatus = 0;
    static int statusIter = 0, k;

    if(msg == WM_TIMER)
    {
    int N = apArgs->size();
    for(k = 0; k<N; k++)
    if(*(*apArgs)[k].execute)
    {
        *(*apArgs)[k].execute = 0;
        //cout<<(*apArgs)[k].successMessage<<"init---\n";

        if(statusIter > 0)
            statusIter = 900; //terminer le processus precedent
            

        if(!isworking)
        {
            isworking = 1;

            thread t(
                [](CSARGS Args, int k)
                {
                    vector<CSUIOBJECTS::CS_ABSTRACT_PROGRESSBAR::CS_AP_ARGS>* apArgs = (vector<CSUIOBJECTS::CS_ABSTRACT_PROGRESSBAR::CS_AP_ARGS>*)Args[0];
                    (*apArgs)[k].func((*apArgs)[k].args);
                    isworking = 0;

                }, Args, k
            );
            t.detach();

            thread t2(
                [](CSARGS Args, int k, int id)
                {
                    vector<CSUIOBJECTS::CS_ABSTRACT_PROGRESSBAR::CS_AP_ARGS>* apArgs = (vector<CSUIOBJECTS::CS_ABSTRACT_PROGRESSBAR::CS_AP_ARGS>*)Args[0];
                    int *workAnimDeltaTime = (int*)Args[1];
                    void(*workAnimFunc)(int, int*, double*, double*, wstring) = *(void(**)(int, int*, double*, double*, wstring))Args[2];
                    int iter = 0;
                    //int id = (int)Args; //ne marche pas car (int)Args est ephemere
                    while(1)
                    {
                        workAnimFunc(id, &iter, (double*)(*apArgs)[k].args[1], (double*)(*apArgs)[k].args[2], (*apArgs)[k].workMessage);
                        iter++;
                        if(!isworking)
                        {
                            showStatus = 1;
                            break;
                        }
                        std::this_thread::sleep_for(std::chrono::microseconds(*workAnimDeltaTime));
                    }

                }, Args, k, id
            );
            t2.detach();

            thread t3(
                [](CSARGS Args, int k, int id)
                {
                    vector<CSUIOBJECTS::CS_ABSTRACT_PROGRESSBAR::CS_AP_ARGS>* apArgs = (vector<CSUIOBJECTS::CS_ABSTRACT_PROGRESSBAR::CS_AP_ARGS>*)Args[0];
                    //int id = (int)Args; doesnt work

                    int* resultAnimDeltaTime = (int*)Args[3];
                    int* resultAnimMaxTime = (int*)Args[4];
                    void(*resultAnimFunc)(int, int*, int*, bool*, wstring, wstring) = *(void(**)(int, int*, int*, bool*, wstring, wstring))Args[5];

                    while (1)
                    {

                        if(showStatus)
                        {
                            statusIter++;
                            resultAnimFunc(id, &statusIter, resultAnimMaxTime, (bool*)(*apArgs)[k].args[0], (*apArgs)[k].successMessage, (*apArgs)[k].errorMessage);

                            if(statusIter > *resultAnimMaxTime)
                            {
                                statusIter = 0;
                                showStatus = 0;
                                break;
                            }
                        }
                        std::this_thread::sleep_for(std::chrono::microseconds(*resultAnimDeltaTime));
                    }

                }, Args,k, id
            );
            t3.detach();
        }
        else
        {
            cout<<"Please wait until active translation ends up !\n";
        }

    }
    }
}

void __workAnimFunc(int id, int* iter, double* maxLevel, double* level, wstring workMessage)
{
    if(*iter == 50)
    {
        CSSECMAN::setTitle(id, CSTEXT{.Text= (wchar_t*)(workMessage + L".").c_str()}, 1);
        CSSECMAN::updateSection(id);
    }
    else if(*iter == 100)
    {
        CSSECMAN::setTitle(id, CSTEXT{.Text=(wchar_t*)(workMessage + L"..").c_str()}, 1);
        CSSECMAN::updateSection(id);
    }
    else if(*iter == 150)
    {
        CSSECMAN::setTitle(id, CSTEXT{.Text=(wchar_t*)(workMessage + L"...").c_str()}, 1);
        CSSECMAN::updateSection(id);
    }
    else if(*iter > 150)
    {
        *iter = 0;
    }
    
}

void __resultAnimFunc(int id, int *iter, int* resultAnimMaxTime, bool* status, wstring successMessage, wstring errorMessage)
{
    static bool b = 0;
   
    if(*status && !b)
    {
        CSSECMAN::setTitleColor(id, {70,200,70});
        CSSECMAN::setTitle(id, CSTEXT{.Text=(wchar_t*)(successMessage).c_str()}, 1);
        CSSECMAN::updateSection(id);
        b = 1;
    }
    else if(!b)
    {
        CSSECMAN::setTitleColor(id, {200,70,70});
        CSSECMAN::setTitle(id, CSTEXT{.Text=(wchar_t*)(errorMessage).c_str()}, 1);
        CSSECMAN::updateSection(id);
        b = 1;
    }

    if(*iter > *resultAnimMaxTime)
    {
        CSSECMAN::setTitleColor(id, {200,200,200});
        CSSECMAN::setTitle(id, CSTEXT{.Text=L"No task."}, 1);
        CSSECMAN::updateSection(id);
        b = 0;
        *status = 0;
    }
}
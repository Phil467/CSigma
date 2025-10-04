#include "csUIControls.h"
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

extern float dimFact;
void smoothScrolling(CSARGS Args);

CSSCROLLBAR CSUICONTROLS::addHScrollBar(int* idp, int* idClient, int* idMask, int thick, int autoHideIntensity)
{
    RECT r = RECTCL[*idp];
    int*idc = idClient;
    if(!idc) idc = idp;
    CSSCROLLBAR hscroll(*idp, {0,r.bottom-thick,r.right,thick});
    //cout<<" success \n";
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
    CSUIMAN::addAction(*idc,smoothScrolling,1,hscroll.getIdPtr());
    withHScroll[*idc] = hscroll.getId();
    return hscroll;
}

CSSCROLLBAR CSUICONTROLS::addVScrollBar(int* idp, int* idClient, int* idMask, int thick, int autoHideIntensity)
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
    CSUIMAN::addAction(*idc,smoothScrolling,1,vscroll.getIdPtr());
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


int CSUICONTROLS::iconButton01(int idp, char* path1, char* path2, RECT r)
{

    int BUTTON = CSUIMAN::createSection(idp, r,  RGB(30,30,30), {0,0,0,0});
    CSBIND_GEOM_PARAMS bd = {BUTTON, {0,0,1,0}, {0,0,BIND_DEST_LEFT_EDGE,0}};
    CSUIMAN::bindGeometry(idp, bd);
    CSUIFX::setImageGradient(BUTTON, path1, path2, {0}, {r.bottom*dimFact,r.bottom*dimFact}, 0.1, 4,1);

    return BUTTON;
}

int CSUICONTROLS::iconButton02(int idp, char* path1, char* path2, RECT r)
{

    int BUTTON = CSUIMAN::createSection(idp, r,  RGB(30,30,30), {0,0,0,0});
    /*CSBIND_GEOM_PARAMS bd = {BUTTON, {0,0,1,0}, {0,0,BIND_DEST_LEFT_EDGE,0}};
    CSUIMAN::bindGeometry(idp, bd);*/
    CSUIFX::setImageGradient(BUTTON, path1, path2, {0}, {r.bottom*dimFact,r.bottom*dimFact}, 0.1, 4,1);

    return BUTTON;
}

int CSUICONTROLS::darkTextButton01(int idp, wchar_t*title, RECT r, int fontSize)
{

    int BUTTON = CSUIMAN::createSection(idp, r,  RGB(30,30,30), {0,0,0,0});
    CSBIND_GEOM_PARAMS bd = {BUTTON, {0,0,1,0}, {0,0,BIND_DEST_RIGHT_EDGE,0}};
    CSUIMAN::bindGeometry(idp, bd);
    CSUIFX::setBackgroundColorGradient(BUTTON, {50,50,50}, {20,20,20}, 2, 2);
    CSUIFX::setBackgroundColorClickEffect(BUTTON, {60,60,60});
    CSUIFX::setTitleColorGradient(BUTTON, {220,200,100}, {200,200,200}, 2, 2);
    CSUIFX::setTitleColorClickEffect(BUTTON, {220,220,120});
    CSUIFX::setBorderColorGradient(BUTTON, {100,100,100}, {25,25,25}, 2, 2);
    CSUIFX::setBorderColorClickEffect(BUTTON, {120,120,120});
    CSUIMAN::setBorderThick(BUTTON, 2);
    CSUIMAN::setTitle(BUTTON, CSTEXT{.Text=title, .Font=L"Segoe UI", .FontSize=fontSize, .Color={150,150,150},
                            .Marging={0,0}, .Align = CS_TA_CENTER, .Show=1});

    return BUTTON;
}

int CSUICONTROLS::darkImgTextButton01(int idp, wchar_t*title, char* path1, char* path2, RECT r, int fontSize)
{

    int BUTTON = CSUIMAN::createSection(idp, r,  RGB(30,30,30), {0,0,0,0});
    CSBIND_GEOM_PARAMS bd = {BUTTON, {0,0,1,0}, {0,0,BIND_DEST_RIGHT_EDGE,0}};
    CSUIMAN::bindGeometry(idp, bd);
    CSUIFX::setImageGradient(BUTTON, path1, path2, {0}, {26,26}, 0.1, 2, 2);
    CSUIFX::setBackgroundColorGradient(BUTTON, {5,5,5}, {20,20,20}, 2, 2);
    CSUIFX::setBackgroundColorClickEffect(BUTTON, {10,10,10});
    CSUIFX::setTitleColorGradient(BUTTON, {220,200,100}, {200,200,200}, 2, 2);
    CSUIFX::setTitleColorClickEffect(BUTTON, {220,220,120});
    CSUIFX::setBorderColorGradient(BUTTON, {10,10,10}, {25,25,25}, 2, 2);
    CSUIFX::setBorderColorClickEffect(BUTTON, {20,20,20});
    CSUIMAN::setBorderThick(BUTTON, 2);
    CSUIMAN::setTitle(BUTTON, CSTEXT{.Text=title, .Font=L"Segoe UI", .FontSize=fontSize, .Color={150,150,140},
                            .Marging={32/dimFact,0}, .Align = CS_TA_CENTER_LEFT, .Show=1});

    return BUTTON;
}

int CSUICONTROLS::darkTextButton02(int idp, wchar_t*title, RECT r, int fontSize)
{

    int BUTTON = CSUIMAN::createSection(idp, r,  RGB(30,30,30), {0,0,0,0});
    CSBIND_GEOM_PARAMS bd = {BUTTON, {0,0,1,0}, {0,0,BIND_DEST_RIGHT_EDGE,0}};
    CSUIMAN::bindGeometry(idp, bd);
    CSUIFX::setBackgroundColorGradient(BUTTON, {50,50,50}, {20,20,20}, 2, 2);
    CSUIFX::setBackgroundColorClickEffect(BUTTON, {60,60,60});
    CSUIFX::setTitleColorGradient(BUTTON, {220,220,220}, {200,200,200}, 2, 2);
    CSUIFX::setTitleColorClickEffect(BUTTON, {190,190,190});
    CSUIFX::setBorderColorGradient(BUTTON, {25,25,25}, {25,25,25}, 2, 2);
    CSUIFX::setBorderColorClickEffect(BUTTON, {120,120,120});
    CSUIMAN::setBorderThick(BUTTON, 1);
    CSUIMAN::setTitle(BUTTON, CSTEXT{.Text=title, .Font=L"Segoe UI", .FontSize=fontSize, .Color={200,200,200},
                            .Marging={0,0}, .Align = CS_TA_CENTER, .Show=1});

    return BUTTON;
}

CSSYSCOMMAND_SECTION CSUICONTROLS::addSysCommand(int& id, POINT pos)
{
    CSSYSCOMMAND_SECTION sc;
    int width = (GetSystemMetrics(SM_CYCAPTION)-2);
    sc.SYSCOMMAND_SECTION = CSUIMAN::createSection(id, {RECTCL[id].right/dimFact-3*width-2,pos.y,3*width,width},  RGB(5,5,5), {0,0,0,0});
    CSBIND_GEOM_PARAMS bd = {sc.SYSCOMMAND_SECTION, {-1,0,1,0}, {BIND_DEST_LEFT_EDGE,0,BIND_DEST_LEFT_EDGE,0}};
    CSUIMAN::bindGeometry(id, bd);
    CSUIMAN::setBorderColorAndThick(sc.SYSCOMMAND_SECTION, RGB(20,20,20), 1);

    width -= 2;
    sc.SYS_MIN = CSUICONTROLS::iconButton01(sc.SYSCOMMAND_SECTION, "img/_min02.bmp\0", "img/_min01.bmp\0", {1,1,width,width});
    sc.SYS_MAX = CSUICONTROLS::iconButton01(sc.SYSCOMMAND_SECTION, "img/_max02.bmp\0", "img/_max01.bmp\0", {1+width,1,width,width});
    sc.SYS_CLOSE = CSUICONTROLS::iconButton01(sc.SYSCOMMAND_SECTION, "img/_close02.bmp\0", "img/_close01.bmp\0", {1+width*2,1,width,width});
    CSUIMAN::setAsMinButton(sc.SYS_MIN, id);
    CSUIMAN::setAsMaxButton(sc.SYS_MAX, id);
    CSUIMAN::setAsCloseButton(sc.SYS_CLOSE, id);

    return sc;
}

int CSUICONTROLS::addTitle(int& id, wchar_t*title, SIZE size, char*iconPath, int fontSize, wchar_t*fontName)
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
    
    int TITLE_SECTION = CSUIMAN::createSection(id, {2,0,size.cx,size.cy},  RGB(5,5,5), {0,0,0,0});
    
    CSUIMAN::setBorderColorAndThick(TITLE_SECTION, RGB(20,20,20), 1);
    if(iconPath)
        CSUIFX::setImageGradient(TITLE_SECTION, iconPath, iconPath, {2,2}, {26,26}, 0.05, 2,1);
    CSUIMAN::setTitle(TITLE_SECTION, CSTEXT{.Text=title, .Font=fontName, .FontSize = fontSize, .Italic=0,
                                   .Bold=0, .Color={150,150,150},
                                   .Marging={35/dimFact,0}, .Align = CS_TA_CENTER_LEFT, .Show=1});
    
    CSUIMAN::inert(TITLE_SECTION,190);
    SetWindowTextW(SECTION[id], title);

    CSUIMAN::autoFitToTitle(TITLE_SECTION, 10);

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
                SetWindowPos(SECTION[id], 0, 0,0,r.right, 20*dimFact, SWP_NOMOVE|SWP_NOZORDER);
            }
        }
    };

    CSUIMAN::addAction(TITLE_SECTION, f, 0);
    
    return TITLE_SECTION;
}


extern int csCreateRichEdit(int idpar, RECT marging, const wchar_t* defaultText, int style, int hMenuId);

CS_NUMERIC_INCREMENTER_PARAMS CSUICONTROLS::numericIncrementer(int idp, RECT r, double value, double incr, bool intNum)
{
    CS_NUMERIC_INCREMENTER_PARAMS nip;
    nip.idSection = CSUIMAN::createSection(idp, r,  RGB(15,15,15), {0,0,0,0});
    //bd = {idp, {-0.5,0,0.5,0}, {BIND_DEST_LEFT_EDGE,0,BIND_DEST_LEFT_EDGE,0}};
    //bindGeometry(ROOT, bd);
    int l = (r.bottom-4);
    //CSUIFX::setBorderColorGradient(nip.idSection, {100,100,100}, {40,40,40}, 2, 2);
    CSUIMAN::setBorderThick(nip.idSection, 2);
    CSUIMAN::setTitle(nip.idSection, CSTEXT{.Text=L"\0", .Font=L"Arial", .FontSize=14, .Italic=1, .Color={100,100,100},
                                   .Marging={-45,0}, .Align = CS_TA_CENTER, .Show=1});

    nip.idUp = CSUICONTROLS::iconButton02(nip.idSection, "img/increase.bmp\0", "img/increase2.bmp\0", {r.right-l*3-2,2,l,l});
    nip.idDown = CSUICONTROLS::iconButton02(nip.idSection, "img/decrease.bmp\0", "img/decrease2.bmp\0", {r.right-l*2-2,2,l,l});
    nip.idValidate = CSUICONTROLS::iconButton02(nip.idSection, "img/go.bmp\0", "img/go2.bmp\0", {r.right-l-2,2,l,l});


    wchar_t num[5000];
    if(intNum)
        wsprintf(num, L"%s\0", to_wstring((long)value).c_str());
    else
        wsprintf(num, L"%s\0", to_wstring(value).c_str());
    
    nip.idText = csCreateRichEdit(nip.idSection, {4,4,((l+1)*3)*dimFact,4}, (const wchar_t*)num, 0, 0);

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

CS_NUMERIC_INCREMENTER_PARAMS CSUICONTROLS::numericIncrementerExt(int idp, RECT r, wchar_t* value, wchar_t* step, short format)
{
    CS_NUMERIC_INCREMENTER_PARAMS nip = {0};
    nip.idSection = CSUIMAN::createSection(idp, r,  RGB(25,25,25), {0,0,0,0});
    //bd = {idp, {-0.5,0,0.5,0}, {BIND_DEST_LEFT_EDGE,0,BIND_DEST_LEFT_EDGE,0}};
    //bindGeometry(ROOT, bd);
    int l = (r.bottom-4);
    //CSUIFX::setBorderColorGradient(nip.idSection, {100,100,100}, {40,40,40}, 2, 2);
    CSUIMAN::setBorderThick(nip.idSection, 2);
    CSUIMAN::setTitle(nip.idSection, CSTEXT{.Text=L"\0", .Font=L"Arial", .FontSize=14, .Italic=1, .Bold=FW_BOLD, .Color={100,100,100},
                                   .Marging={-((l+1)*3)*dimFact/2,0}, .Align = CS_TA_CENTER, .Show=1, .ShowEntierText=1});

    nip.idUp = CSUICONTROLS::iconButton02(nip.idSection, "img/increase.bmp\0", "img/increase2.bmp\0", {r.right-l*3-2,2,l,l});
    nip.idDown = CSUICONTROLS::iconButton02(nip.idSection, "img/decrease.bmp\0", "img/decrease2.bmp\0", {r.right-l*2-2,2,l,l});
    nip.idValidate = CSUICONTROLS::iconButton02(nip.idSection, "img/go.bmp\0", "img/go2.bmp\0", {r.right-l-2,2,l,l});

    
    nip.idText = csCreateRichEdit(nip.idSection, {4,4,((l+1)*3)*dimFact,4}, (const wchar_t*)value, 0, 0);
    csSetRichEditFormat(nip.idText, format);
    char*nb = (char*)wcharPtrToCharPtr((const wchar_t*)step).c_str();
    bool sign = CSUTILS::signExtraction(nb); 
    nip.step.set(nb,0,sign);
    //free(nb);

    bool*titleAutoRepos = csAlloc<bool>(1,1);
    CSUIMAN::addAction(nip.idUp, incrementFunction, 2, &nip, titleAutoRepos);
    CSUIMAN::addAction(nip.idDown, decrementFunction, 2, &nip, titleAutoRepos);
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


CS_NUMERIC_INCREMENTER_PARAMS CSUICONTROLS::numericIncrementerExt1(int idp, RECT r, wchar_t* value, wchar_t* step, short format, COEFS4 bindCoefs, FLAGS4 bindFlags)
{
    CS_NUMERIC_INCREMENTER_PARAMS nip = {0};
    nip.idSection = CSUIMAN::createSection(idp, r,  RGB(25,25,25), {0,0,0,0});
    CSUIMAN::setBorderColorAndThick(nip.idSection, RGB(40,40,40), 1);
    CSBIND_GEOM_PARAMS bd;
    bd = {nip.idSection, bindCoefs, bindFlags};
    CSUIMAN::bindGeometry(idp, bd);
    int l = (r.bottom-4);
    //CSUIFX::setBorderColorGradient(nip.idSection, {100,100,100}, {40,40,40}, 2, 2);
    CSUIMAN::setBorderThick(nip.idSection, 2);
    CSUIMAN::setTitle(nip.idSection, CSTEXT{.Text=L"\0", .Font=L"Arial", .FontSize=14, .Italic=1, .Bold=FW_BOLD, .Color={100,100,100},
                                   .Marging={-5,0}, .Align = CS_TA_CENTER_RIGHT, .Show=1, .ShowEntierText=1});

    nip.idUp = CSUICONTROLS::iconButton02(nip.idSection, "img/u1.bmp\0", "img/u1.bmp\0", {2,2,l,l});
    nip.idDown = CSUICONTROLS::iconButton02(nip.idSection, "img/d1.bmp\0", "img/d1.bmp\0", {2+l,2,l,l});

    
    nip.idText = csCreateRichEdit(nip.idSection, {(2+l*2+2)*dimFact,4,4,4}, (const wchar_t*)value, 0, 0);
    csSetRichEditFormat(nip.idText, format);
    char*nb = (char*)wcharPtrToCharPtr((const wchar_t*)step).c_str();
    bool sign = CSUTILS::signExtraction(nb); 
    nip.step.set(nb,0,sign);
    //free(nb);


    bool*titleAutoRepos = csAlloc<bool>(1,0);
    CSUIMAN::addAction(nip.idUp, incrementFunction, 2, &nip, titleAutoRepos);
    CSUIMAN::addAction(nip.idDown, decrementFunction, 2, &nip, titleAutoRepos);
    return nip;
}

void reposIncBtn(CSARGS Args);
CS_NUMERIC_INCREMENTER_PARAMS CSUICONTROLS::numericIncrementerExt2(int idp, RECT r, wchar_t* value, wchar_t* step, short format)
{
    CS_NUMERIC_INCREMENTER_PARAMS nip = {0};
    nip.idSection = CSUIMAN::createSection(idp, r,  RGB(30,30,30), {0,0,0,0});
    CSBIND_GEOM_PARAMS bd;
    bd = {nip.idSection, {0,0,1,0}, {0,0,BIND_DEST_RIGHT_EDGE,0}};
    CSUIMAN::bindGeometry(idp, bd);
    int l = (r.bottom-4);
    //CSUIFX::setBorderColorGradient(nip.idSection, {100,100,100}, {40,40,40}, 2, 2);
    CSUIMAN::setBorderThick(nip.idSection, 2);
    CSUIMAN::setTitle(nip.idSection, CSTEXT{.Text=L"\0", .Font=L"Arial", .FontSize=14, .Italic=1, .Bold=FW_BOLD, .Color={100,100,100},
                                   .Marging={0,0}, .Align = CS_TA_CENTER, .Show=1, .ShowEntierText=1});

    nip.idUp = CSUICONTROLS::iconButton02(nip.idSection, "img/next2.bmp\0", "img/next.bmp\0", {r.right-l-2,2,l,l});
    nip.idDown = CSUICONTROLS::iconButton02(nip.idSection, "img/back2.bmp\0", "img/back.bmp\0", {2,2,l,l});

    
    int c = 2*dimFact;
    nip.idText = csCreateRichEdit(nip.idSection, {(2+l+2)*dimFact,c,(2+l+2)*dimFact,c}, (const wchar_t*)value, 0, 0);
    csSetRichEditFormat(nip.idText, format);
    char*nb = (char*)wcharPtrToCharPtr((const wchar_t*)step).c_str();
    bool sign = CSUTILS::signExtraction(nb); 
    nip.step.set(nb,0,sign);
    //free(nb);


    bool*titleAutoRepos = csAlloc<bool>(1,0);
    CSUIMAN::addAction(nip.idUp, incrementFunction, 2, &nip, titleAutoRepos);
    CSUIMAN::addAction(nip.idDown, decrementFunction, 2, &nip, titleAutoRepos);
    CSUIMAN::addAction(nip.idSection, reposIncBtn, 1, &nip.idUp);
    return nip;
}

void reposIncBtn(CSARGS Args)
{
    if(UINT(Args) == WM_SIZE)
    {
        RECT r = RECTCL[int(Args)];
        int c = 2*dimFact;
        MoveWindow(SECTION[*(int*)Args[0]], r.right-r.bottom+c, c, r.bottom-c, r.bottom-c, 1);
    }
}

void CSUICONTROLS::createToolTips(COLORREF color)
{
    if(SECTION.size())
    {
        TIPS_POPUP = CSUIMAN::createSection(0, {0,0,200/dimFact,200/dimFact}, color, {0}, 0, 0, 0);
        CSUIMAN::enableDarkEdge(TIPS_POPUP);
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
        text = (wchar_t*)realloc(text, (wcslen(sip.item[res].c_str())+1)*sizeof(wchar_t));
        wsprintf(text, L"%s\0",  sip.item[res].c_str());

        TITLE[sip.idSection].Text = text;
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
        text = (wchar_t*)realloc(text, (wcslen(sip.item[res].c_str())+1)*sizeof(wchar_t));
        wsprintf(text, L"%s\0",  sip.item[res].c_str());

        TITLE[sip.idSection].Text = text;
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

CS_STRING_INCREMENTER_PARAMS CSUICONTROLS::stringIncrementer(int idp, RECT r, wchar_t* value, long step, bool loopStyle, COEFS4 bindCoefs, FLAGS4 bindFlags)
{
    CS_STRING_INCREMENTER_PARAMS sip = {0};
    sip.idSection = CSUIMAN::createSection(idp, r,  RGB(30,30,30), {0,0,0,0});
    sip.newItem((const wchar_t*)value, 1);
    sip.step = step;
    sip.currentItem = 0;

    CSBIND_GEOM_PARAMS bd;
    bd = {sip.idSection, bindCoefs, bindFlags};
    CSUIMAN::bindGeometry(idp, bd);
    int l = (r.bottom-4);
    
    CSUIMAN::setBorderThick(sip.idSection, 2);
    CSUIMAN::setTitle(sip.idSection, CSTEXT{.Text=L"\0", .Font=L"Arial", .FontSize=14, .Italic=1, .Bold=FW_BOLD, .Color={100,100,100},
                                   .Marging={0,0}, .Align = CS_TA_CENTER, .Show=1, .ShowEntierText=1});

    sip.idUp = CSUICONTROLS::iconButton02(sip.idSection, "img/next2.bmp\0", "img/next.bmp\0", {r.right-l-2,2,l,l});
    sip.idDown = CSUICONTROLS::iconButton02(sip.idSection, "img/back2.bmp\0", "img/back.bmp\0", {2,2,l,l});

    
    int c = 2*dimFact;
    sip.idText = csCreateRichEdit(sip.idSection, {(2+l+2)*dimFact,c,(2+l+2)*dimFact,c}, (const wchar_t*)value, 0, 0);
    csSetRichEditFormat(sip.idText, INPUT_FORMAT_POSITIVE_INTERGER);

    sip.loopStyle = loopStyle;

    bool*titleAutoRepos = csAlloc<bool>(1,0);
    CSUIMAN::addAction(sip.idUp, strIncrementFunction, 2, &sip, titleAutoRepos);
    CSUIMAN::addAction(sip.idDown, strDecrementFunction, 2, &sip, titleAutoRepos);
    CSUIMAN::addAction(sip.idSection, reposIncBtn, 1, &sip.idUp);

    return sip;
}
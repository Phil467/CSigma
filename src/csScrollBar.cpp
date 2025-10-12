#include "csScrollBar.h"

extern vector<HWND> SECTION;
extern vector<int> PARID;
extern vector<int> backgroundColor;

extern vector<RECT> RECTWND;
extern vector<RECT> RECTWNDSAVED;
extern vector<RECT> RECTCL;
extern vector<HDC> hdcontext;

extern vector<HDC> hdStackContext;
extern vector<HBITMAP> hStackBmp;
extern vector<SIZE> hdcSize;
extern vector<RECT> RECTPARREFSAVED;

extern vector<HDC> hdcontextExt;
extern vector<POINT> hdcontextExtInPos;
extern vector<POINT> hdcontextExtOutPos;
extern vector<SIZE> hdcontextExtSize;
extern vector<HBITMAP> hdBmpExt;

extern vector<HDC> hdStackContext;
extern vector<HBITMAP> hStackBmp;
extern vector<SIZE> hdcSize;

extern vector<int> withHScroll;
extern vector<int> withVScroll;
extern vector<int> viewedAreaRightMarging;
extern vector<int> viewedAreaBottomMarging;


extern vector<RECT> bltRect;
extern vector<float> hZoom;
extern vector<float> vZoom;
extern vector<BYTE> mouseWheelPreference;

extern vector<bool> halftoneMode;

extern vector<HCURSOR> cursor;
extern POINT LBD_POINT;
extern float dimCoef;
extern HINSTANCE _hInstance;
int IDCAPTURE;

extern vector<CSZOOM_PARAMS> zoomParams;
extern void delay(int del);


int SCROLLBAR_AUTO_REPOS_TIMER_ID = 1;

void resize(CSARGS Args);
void lbtnDown(CSARGS Args);
void mouseMove(CSARGS Args);
void mouseLeave_(CSARGS Args);
void timer_(CSARGS Args);
void lbtnup_(CSARGS Args);
void autoRepos(CSARGS Args);
void autoRepos_parentContext(CSARGS Args);
void mouseMovesetCursor(CSARGS Args);
void clickUnder(CSARGS Args);
void clickOver(CSARGS Args);
void decrease(CSARGS Args);
void increase(CSARGS Args);
void longPress2(CSARGS Args);
void longPress1(CSARGS Args);
void mouseMoveSetColor(CSARGS Args);
void clientGesture(CSARGS Args);
void mouseWheel(CSARGS Args);

extern bool END_CREATE;

void groupMsg(CSARGS Args)
{
    UINT msg = (UINT)Args;

    if(msg == WM_SIZE)
    {
        resize(Args);
    }
    else if(msg == WM_LBUTTONDOWN)
    {
        lbtnDown(Args);
        clickUnder(Args);
        clickOver(Args);
        //decrease(Args);
        //increase(Args);
    }
    else if(msg == WM_LBUTTONUP)
    {
        mouseMovesetCursor(Args);
        lbtnup_(Args);
    }
    else if(msg == WM_MOUSEMOVE)
    {
        if(WPARAM(Args) != 1)
            mouseMovesetCursor(Args);
        mouseMove(Args);
    }
    else if(msg == WM_MOUSELEAVE)
    {
        mouseLeave_(Args);
    }
    else if(msg == WM_TIMER /*  && END_CREATE  */)
    {
        
       // if(WPARAM(Args) == SCROLLBAR_AUTO_REPOS_TIMER_ID)
        {
            //if(!GetAsyncKeyState(VK_LBUTTON))
                autoRepos(Args);
        }
        //else
        {
            timer_(Args);
            mouseMoveSetColor(Args);
        }
    }

    longPress2(Args);
    longPress1(Args);

}

void parentResize(CSARGS Args)
{
    if(UINT(Args) == WM_SIZE)
    {
        bool* bSize = (bool*)Args[6];
        
        if(*bSize == 0 && (GetAsyncKeyState(VK_LBUTTON)&&0x8000))
        {
            *bSize = 1;
            std::thread t(
                [](CSARGS Args)
                {
                    bool* bSize = (bool*)Args[6];
                    while(*bSize == 1)
                    {
                        autoRepos_parentContext(Args);

                        if(!(GetAsyncKeyState(VK_LBUTTON)&&0x8000))
                        {
                            *bSize = 0;
                        }
                        delay(10000);
                    }
                }, Args
            );
            t.detach();
            cout<<"done------------------------------------------------- !\n";
        }
    }
}

CSSCROLLBAR::CSSCROLLBAR(int idp, RECT geometry, int sctype, bool orientation)
{
    init(idp, geometry, sctype, orientation);
}

void CSSCROLLBAR::update()
{
    int id = idp;
    RECT r = RECTCL[id];
    SetWindowPos(SECTION[id], 0, 0,0,r.right-2,r.bottom-2, SWP_NOMOVE|SWP_NOZORDER);
    SetWindowPos(SECTION[id], 0, 0,0,r.right,r.bottom, SWP_NOMOVE|SWP_NOZORDER);
}

void CSSCROLLBAR::setScrollColors(CSRGBA c1, CSRGBA c2)
{
    (*colors)[2][0] = c1;
    (*colors)[2][0] = c2;

    /*if(Args.getArgNumber())
    {
        Args.setArg(1, colors);
    }*/
}

void CSSCROLLBAR::setBackgroundColor(COLORREF color)
{
    HDC dc = hdcontextExt[*id];
    HBRUSH hb = CreateSolidBrush(color);
    HPEN hp = CreatePen(0,0,color);
    SelectBrush(dc,hb);
    SelectBrush(dc,hp);
    if(*orient == CS_SBAR_HORIZONTAL)
        Rectangle(dc, 0, 0, GetSystemMetrics(SM_CXSCREEN), *width);
    else
        Rectangle(dc, 0, 0, *width, GetSystemMetrics(SM_CYSCREEN));
    DeleteBrush(hb);
    DeleteBrush(hp);
}

void CSSCROLLBAR::setViewFrameRightMarging(int marging)
{
    if(*idc)
        viewedAreaRightMarging[*idc] = marging*dimCoef;
}
void CSSCROLLBAR::setViewFrameBottomMarging(int marging)
{
    if(*idc)
        viewedAreaBottomMarging[*idc] = marging*dimCoef;
}

void CSSCROLLBAR::init(int _idp, RECT geometry, int _sctype, bool orientation)
{
    idp = _idp;
    id = csAlloc<int>(1,0);
    idc = csAlloc<int>(1,0);
    idMask = csAlloc<int>(1,0);
    if(orientation == CS_SBAR_HORIZONTAL)
        width = csAlloc<int>(1,geometry.bottom*dimCoef);
    else
        width = csAlloc<int>(1,geometry.right*dimCoef);
    orient = csAlloc<bool>(1,orientation);
    scType = csAlloc<int>(1,_sctype);
    tLength = csAlloc<int>(1,0);
    hmarg = csAlloc<int>(1,1);;
    vmarg = csAlloc<int>(1,1);
    rectSelect = csAlloc<int>(1,-1);
    mhId = csAlloc<int>(1,0);
    value = csAlloc<float>(1,0.5);
    value0 = csAlloc<float>(1,0.5);
    scval = csAlloc<float>(1,0.2);
    szzoom = csAlloc<SIZE>(1,{200,100});
    oldClientPos = csAlloc<POINT>(1,{0,0});
    oldClientSize = csAlloc<SIZE>(1,{0,0});
    CX = csAlloc(1,0); CY = csAlloc(1,0);
    threadBool = csAlloc<bool>(1,0);
    mhId0 = csAlloc(1,-1);
    vEdge = csAlloc(1,0);
    hEdge = csAlloc(1,0);
    bSize = 0;
    
    rcs = new vector<RECT>;
    colors = new vector<vector<CSRGBA>>;
    cloths = new vector<vector<HDC>>;
    

    //if(orientation == CS_SBAR_HORIZONTAL)
    {
        rcs->push_back({*hmarg,*vmarg,*width-*hmarg,*width-*vmarg});
        rcs->push_back({(*rcs)[0].right,*vmarg,(*rcs)[0].right,*width-*vmarg});
        rcs->push_back({(*rcs)[1].right,*vmarg,(*rcs)[1].right+szzoom->cx+(szzoom->cy)/2,*width-*vmarg});
        rcs->push_back({(*rcs)[2].right,*vmarg,geometry.right*dimCoef-*width-*hmarg,*width-*vmarg});
        rcs->push_back({(*rcs)[3].right,*vmarg,geometry.right*dimCoef-*hmarg,*width-*vmarg});

    }
    (*colors).push_back(newVector<CSRGBA>());
    (*colors).push_back(newVector<CSRGBA>());
    (*colors).push_back(newVector<CSRGBA>());
    (*colors).push_back(newVector<CSRGBA>());
    (*colors).push_back(newVector<CSRGBA>());

    goto dark;

    light:
    {
        (*colors)[0].push_back({150,150,15});
        (*colors)[0].push_back({150,150,150});
        (*colors)[0].push_back({240,240,240});
        (*colors)[0].push_back({150,150,150});
        (*colors)[0].push_back({150,150,15});

        (*colors)[1].push_back({150,150,15});
        (*colors)[1].push_back({150,150,150});
        (*colors)[1].push_back({240,240,240});
        (*colors)[1].push_back({150,150,150});
        (*colors)[1].push_back({150,150,15});

        (*colors)[2].push_back({200,200,30});
        (*colors)[2].push_back({180,180,180});
        (*colors)[2].push_back({255,255,255});
        (*colors)[2].push_back({180,180,180});
        (*colors)[2].push_back({200,200,30});

        (*colors)[3].push_back({200,200,30});
        (*colors)[3].push_back({180,180,180});
        (*colors)[3].push_back({30,30,30});
        (*colors)[3].push_back({180,180,180});
        (*colors)[3].push_back({200,200,30});
    }

    dark:
    {
        (*colors)[0].push_back({200,200,30});
        (*colors)[0].push_back({30,30,30});
        (*colors)[0].push_back({70,70,70});
        (*colors)[0].push_back({30,30,30});
        (*colors)[0].push_back({200,200,30});

        (*colors)[1].push_back({150,150,15});
        (*colors)[1].push_back({35,35,35});
        (*colors)[1].push_back({80,80,80});
        (*colors)[1].push_back({35,35,35});
        (*colors)[1].push_back({150,150,15});

        (*colors)[2].push_back({30,30,30});
        (*colors)[2].push_back({50,50,50});
        (*colors)[2].push_back({55,55,55});
        (*colors)[2].push_back({50,50,50});
        (*colors)[2].push_back({200,200,30});

        (*colors)[3].push_back({200,200,30});
        (*colors)[3].push_back({180,180,180});
        (*colors)[3].push_back({30,30,30});
        (*colors)[3].push_back({180,180,180});
        (*colors)[3].push_back({200,200,30});
    }

    
    (*cloths).push_back(newVector<HDC>());
    (*cloths).push_back(newVector<HDC>());
    (*cloths).push_back(newVector<HDC>());
    (*cloths).push_back(newVector<HDC>());
    (*cloths).push_back(newVector<HDC>());


    int cx = RECTCL[idp].right;
    int cy = RECTCL[idp].bottom;
    if(orientation == CS_SBAR_HORIZONTAL)
    {
        int edge = *hEdge;
        int thick = geometry.bottom*dimCoef;
        int t2 = thick;
        *id = CSUIMAN::createSection(_idp, {edge/dimCoef, (cy - edge - thick)/dimCoef, (cx - 2*edge - thick)/dimCoef, thick/dimCoef},  RGB(200,200,200), {0,0,0,0});
            
        cursor[*id] = LoadCursor(0,IDC_SIZEWE);
        
        hdStackContext[*id] = CreateCompatibleDC(hdcontext[_idp]);
        hStackBmp[*id] = CreateCompatibleBitmap(hdcontext[_idp],GetSystemMetrics(SM_CXSCREEN), thick);
        SelectBitmap(hdStackContext[*id], hStackBmp[*id]);
        
        hdcontextExt[*id] = CreateCompatibleDC(hdcontext[_idp]);
        hdBmpExt[*id] = CreateCompatibleBitmap(hdcontext[_idp], GetSystemMetrics(SM_CXSCREEN), thick);
        SelectBitmap(hdcontextExt[*id], hdBmpExt[*id]);

        CSGRAPHIC_CONTEXT gc01 = csGraphics::getImageGraphicContextResizedW(L"img/back.bmp", {t2-*hmarg,t2-*vmarg});
        DeleteBitmap(gc01.hbmp);
        (*cloths)[0].push_back(gc01.dc);
        CSGRAPHIC_CONTEXT gc02 = csGraphics::getImageGraphicContextResizedW(L"img/back2.bmp", {t2-*hmarg,t2-*vmarg});
        DeleteBitmap(gc02.hbmp);
        (*cloths)[0].push_back(gc02.dc);
        (*cloths)[0].push_back(gc02.dc);
        (*cloths)[0].push_back(gc01.dc);

        CSGRAPHIC_CONTEXT gc11 = csGraphics::getImageGraphicContextResizedW(L"img/next.bmp", {t2-*hmarg,t2-*vmarg});
        DeleteBitmap(gc11.hbmp);
        (*cloths)[4].push_back(gc11.dc);
        CSGRAPHIC_CONTEXT gc12 = csGraphics::getImageGraphicContextResizedW(L"img/next2.bmp", {t2-*hmarg,t2-*vmarg});
        DeleteBitmap(gc12.hbmp);
        (*cloths)[4].push_back(gc12.dc);
        (*cloths)[4].push_back(gc12.dc);
        (*cloths)[4].push_back(gc11.dc);

    }
    else
    {
        int edge = *vEdge;
        int thick = geometry.right*dimCoef;
        int t2 = thick;
        *id = CSUIMAN::createSection(_idp, {(cx - edge - thick)/dimCoef, edge/dimCoef, thick/dimCoef, (cy - 2*edge - thick)/dimCoef},  RGB(200,200,200), {0,0,0,0});

        hdStackContext[*id] = CreateCompatibleDC(hdcontext[_idp]);
        hStackBmp[*id] = CreateCompatibleBitmap(hdcontext[_idp], thick, GetSystemMetrics(SM_CYSCREEN));
        SelectBitmap(hdStackContext[*id], hStackBmp[*id]);

        cursor[*id] = LoadCursor(0,IDC_SIZENS);
        hdcontextExt[*id] = CreateCompatibleDC(hdcontext[_idp]);
        hdBmpExt[*id] = CreateCompatibleBitmap(hdcontext[_idp], thick, GetSystemMetrics(SM_CYSCREEN));
        SelectBitmap(hdcontextExt[*id], hdBmpExt[*id]);
        
        CSGRAPHIC_CONTEXT gc01 = csGraphics::getImageGraphicContextResizedW(L"img/up.bmp", {t2-*hmarg,t2-*vmarg});
        DeleteBitmap(gc01.hbmp);
        (*cloths)[0].push_back(gc01.dc);
        CSGRAPHIC_CONTEXT gc02 = csGraphics::getImageGraphicContextResizedW(L"img/up2.bmp", {t2-*hmarg,t2-*vmarg});
        DeleteBitmap(gc02.hbmp);
        (*cloths)[0].push_back(gc02.dc);
        (*cloths)[0].push_back(gc02.dc);
        (*cloths)[0].push_back(gc01.dc);

        CSGRAPHIC_CONTEXT gc11 = csGraphics::getImageGraphicContextResizedW(L"img/down.bmp", {t2-*hmarg,t2-*vmarg});
        DeleteBitmap(gc11.hbmp);
        (*cloths)[4].push_back(gc11.dc);
        CSGRAPHIC_CONTEXT gc12 = csGraphics::getImageGraphicContextResizedW(L"img/down2.bmp", {t2-*hmarg,t2-*vmarg});
        DeleteBitmap(gc12.hbmp);
        (*cloths)[4].push_back(gc12.dc);
        (*cloths)[4].push_back(gc12.dc);
        (*cloths)[4].push_back(gc11.dc);
    }

    setBackgroundColor(RGB(35,35,35));

    //SetTimer(SECTION[*id], SCROLLBAR_AUTO_REPOS_TIMER_ID, 10, 0); // timer pour redimensionnement automatique du scrollbar

    bool*block = csAlloc<bool>(1,0);

    Args.init(26);
    Args.regArg(rcs, colors, value, value0, hmarg, vmarg, 
                    width, szzoom, orient, tLength, rectSelect, mhId, scval, idc, 
                    scType, idMask, block, oldClientPos, oldClientSize, CX, CY, cloths, 
                    threadBool, mhId0, hEdge, vEdge);
    
    CSUIMAN::addAction(*id, groupMsg, Args);
    //CSUIMAN::addAction(idp, parentResize, 9, idc, id, &hEdge, &vEdge, &CX, &CY, &bSize, orient, &idp);
}

int CSSCROLLBAR::getId()
{
    return *id;
}
int* CSSCROLLBAR::getIdPtr()
{
    return id;
}

void CSSCROLLBAR::setPositionRatio(float posRatio)
{
    if(*idc && *idMask)
    {
        if(posRatio >= 0 && posRatio <= 1.0)
        {
            if(*orient == CS_SBAR_HORIZONTAL)
            {
                long visiblePart = bltRect[*idc].right-bltRect[*idc].left;
                long TotalLength = hdcontextExtSize[*idc].cx*hZoom[*idc];
                long hiddenPart = TotalLength - visiblePart;
                hdcontextExtInPos[*idc].x = hiddenPart*posRatio;
                
            }
            else
            {
                long visiblePart = bltRect[*idc].bottom-bltRect[*idc].top;
                long TotalLength = hdcontextExtSize[*idc].cy*vZoom[*idc];
                long hiddenPart = TotalLength - visiblePart;
                hdcontextExtInPos[*idc].y = hiddenPart*posRatio;
                
            }
        }
    }
    else
    {
        *value0 = *value = posRatio; 
        //SendMessage(SECTION[idp],WM_SIZE,0,0);
    }
}

void CSSCROLLBAR::setPosition(ULONG pos)
{
    if(*idc && *idMask)
    {
        {
            if(*orient == CS_SBAR_HORIZONTAL && pos < hdcontextExtSize[*idc].cx)
            {
                hdcontextExtInPos[*idc].x = pos;
                
            }
            else if(*orient == CS_SBAR_VERTICAL && pos < hdcontextExtSize[*idc].cy)
            {
                hdcontextExtInPos[*idc].y = pos;
                
            }
        }
    }
    else
    {
        //*value0 = *value = posRate; 
    }
}

void CSSCROLLBAR::mouseLeaveHide(int alphaMin)
{
    /*getParams()->AutoAlphaFade.Geometry = {0,0,0,0};
    getParams()->AutoAlphaFade.FadeTime = 8;
    getParams()->AutoAlphaFade.AlphaMin = alphaMin;
    getParams()->AutoAlphaFade.State = CS_AAF_HIDE_CNTR;
    getParams()->AutoAlphaFade.SetGeometry = 0;
    getParams()->MouseLeaveTransparency = 1;
    SendMessage(SECTION[*id],WM_MOUSELEAVE,0,0);*/
}
void CSSCROLLBAR::setClient(int _idc, int _idMask)
{
    *idc = _idc;
    *idMask = _idMask;
    if(*scType == CS_SBAR_SURFACE)
    {
        *oldClientPos = hdcontextExtInPos[*idc];
        if(!hdStackContext[*idc])
        {
            hdStackContext[*idc] = CreateCompatibleDC(hdcontext[idp]);
            hStackBmp[*idc] = CreateCompatibleBitmap(hdcontext[idp], hdcSize[*idc].cx, hdcSize[*idc].cy);
            SelectBitmap(hdStackContext[*idc], hStackBmp[*idc]);
        }
        if(!hdcontextExt[*idc])
        {
            hdcontextExt[*idc] = CreateCompatibleDC(hdcontext[idp]);
            hdBmpExt[*idc] = CreateCompatibleBitmap(hdcontext[idp], hdcSize[*idc].cx, hdcSize[*idc].cy);
            SelectBitmap(hdcontextExt[*idc], hdBmpExt[*idc]);
            hdcontextExtSize[*idc] = hdcSize[*idc];
        }
        
        //CSUIMAN::addAction(*idc, clientGesture, 0);
        CSUIMAN::addAction(*idc, mouseWheel, 0);
    }
}

void CSSCROLLBAR::updateViewArea()
{
    if(*orient == CS_SBAR_HORIZONTAL)
    {
        POINT p = hdcontextExtOutPos[*idc];
        int visiblePart = (RECTWND[*idMask].right-viewedAreaRightMarging[*idc])-(RECTWND[*idc].left+p.x);
        RECT _r = {p.x, p.y, visiblePart+p.x, (RECTWND[*idMask].bottom-viewedAreaBottomMarging[*idc])-RECTWND[*idc].top};
        if(!withVScroll[idp])
            bltRect[*idc] = _r; //-----------------------------attention conflit memoire. tester avec withVscroll et ne remplir que cx si nessecaire....
        else
        {
            bltRect[*idc].left = _r.left; 
            bltRect[*idc].right = _r.right; 
        }
    }
    else
    {
        POINT p = hdcontextExtOutPos[*idc];
        int visiblePart = (RECTWND[*idMask].bottom-viewedAreaBottomMarging[*idc])-(RECTWND[*idc].top+p.y);
        RECT _r = {p.x, p.y, (RECTWND[*idMask].right-viewedAreaRightMarging[*idc])-RECTWND[*idc].left, visiblePart+p.y};
        if(!withHScroll[idp])
            bltRect[*idc] = _r; //-----------------------------attention conflit memoire. tester avec withVscroll et ne remplir que cx si nessecaire....
        else
        {
            bltRect[*idc].top = _r.top; 
            bltRect[*idc].bottom = _r.bottom; 
        }
    }
}

void resize(CSARGS Args)
{
    int id = int(Args);
    vector<RECT> rcs = *(vector<RECT>*)Args[0];
    vector<vector<CSRGBA>> colors = *(vector<vector<CSRGBA>>*)Args[1];
    float* value = (float*)Args[2];
    float* value0 = (float*)Args[3];
    int hmarg = *(int*)Args[4];
    int vmarg = *(int*)Args[5];
    int width = *(int*)Args[6];
    SIZE* szzoom = (SIZE*)Args[7];
    bool orientation = *(bool*)Args[8];
    float *scval = (float*)Args[12];

    
    int idc = *(int*)Args[13];
    int scType = *(int*)Args[14];
    int idMask = *(int*)Args[15];
    bool *block = (bool*)Args[16]; // bloque une partie du code pour des receptions venant wm_mouseMouve
    vector<vector<HDC>> cloths = *(vector<vector<HDC>>*)Args[21];

    SIZE sz={0};
    RECT r = RECTCL[id];
    
    if(orientation == CS_SBAR_HORIZONTAL)
    {
        if(idMask && idc/* && !*block*/)
        {
            POINT p = hdcontextExtOutPos[idc];
            int visiblePart = (RECTWND[idMask].right-viewedAreaRightMarging[idc])-(RECTWND[idc].left+p.x);
            RECT _r = {p.x, p.y, visiblePart+p.x, (RECTWND[idMask].bottom-viewedAreaBottomMarging[idc])-RECTWND[idc].top};
            if(!withVScroll[id])
                bltRect[idc] = _r; //-----------------------------attention conflit memoire. tester avec withVscroll et ne remplir que cx si nessecaire....
            else
            {
                bltRect[idc].left = _r.left; 
                bltRect[idc].right = _r.right; 
            }
            
            /*zoomParams[idc].hmin = visiblePart/(hdcontextExtSize[idc].cx + zoomParams[idc].marging);
            if(zoomParams[idc].hmin < zoomParams[idc].vmin) zoomParams[idc].hmin = zoomParams[idc].vmin;*/
            
            if(scType == CS_SBAR_SURFACE && !*block)
            {
                int totalLengthScrollable = hdcontextExtSize[idc].cx*hZoom[idc];
                
                *scval = float(visiblePart)/totalLengthScrollable;
                
                int cx = (totalLengthScrollable-visiblePart);
                POINT pos = hdcontextExtInPos[idc];
                int hiddenPart = abs(pos.x*hZoom[idc]);
                int diff = totalLengthScrollable-hiddenPart - visiblePart;
                if(diff < 0)
                {
                    hiddenPart += diff; // puisque hiddenPart est positif
                    if(hiddenPart >= 0)
                        hdcontextExtInPos[idc].x = hiddenPart*1.0/hZoom[idc];
                    else hdcontextExtInPos[idc].x = 0.5*hiddenPart/hZoom[idc];
                    //InvalidateRect(SECTION[idc],&r,1);
                    cx = (totalLengthScrollable-visiblePart + diff);
                    //hdcontextExtInPos[idc] = {pos.x+diff, pos.y};
                }

                //if(totalLengthScrollable < visiblePart)
                if(cx <= 0)
                {
                    *value = *value0 = 0;
                    *scval = 1;
                }
                else
                {
                    *value = float(hiddenPart)/cx;
                    *value0 = *value;
                }

            }
            //InvalidateRect(SECTION[idc],&r,1);
        }

        int a = RECTCL[id].right - (hmarg + width)*2;
        szzoom->cx = ceil(a**scval);
        int tlen = a - szzoom->cx;
        *(int*)Args[9] = tlen;
        rcs[1] = {rcs[0].right,vmarg,rcs[0].right+long(tlen**value),width-vmarg};
        rcs[2] = {rcs[1].right,vmarg,rcs[1].right+szzoom->cx/*+(szzoom->cy)/2*/,width-vmarg};
        rcs[3] = {rcs[2].right,vmarg,r.right-width-hmarg,width-vmarg};
        rcs[4] = {rcs[3].right,vmarg,r.right-hmarg,width-vmarg};
    }
    else
    {
        if(idMask && idc/* && !*block*/)
        {
            POINT p = hdcontextExtOutPos[idc];
            int visiblePart = (RECTWND[idMask].bottom-viewedAreaBottomMarging[idc])-(RECTWND[idc].top+p.y);
            RECT _r = {p.x, p.y, (RECTWND[idMask].right-viewedAreaRightMarging[idc])-RECTWND[idc].left, visiblePart+p.y};
            if(!withHScroll[id])
                bltRect[idc] = _r; //-----------------------------attention conflit memoire. tester avec withVscroll et ne remplir que cx si nessecaire....
            else
            {
                bltRect[idc].top = _r.top; 
                bltRect[idc].bottom = _r.bottom; 
            }
            
            /*zoomParams[idc].vmin = visiblePart/(hdcontextExtSize[idc].cy + zoomParams[idc].marging);
            if(zoomParams[idc].vmin < zoomParams[idc].hmin) zoomParams[idc].vmin = zoomParams[idc].hmin;*/

            if(scType == CS_SBAR_SURFACE && !*block)
            {
                int totalLengthScrollable = hdcontextExtSize[idc].cy*vZoom[idc];
                *scval = float(visiblePart)/totalLengthScrollable;

                int cy = (totalLengthScrollable-visiblePart);
                POINT pos = hdcontextExtInPos[idc];
                int hiddenPart = abs(pos.y*vZoom[idc]);
                int diff = totalLengthScrollable-hiddenPart - visiblePart;
                if(diff < 0)
                {
                    hiddenPart += diff; // puisque hiddenPart est positif
                    cy = (totalLengthScrollable-visiblePart+diff);

                    if(hiddenPart >= 0)
                        hdcontextExtInPos[idc].y = hiddenPart*1.0/vZoom[idc];
                    else hdcontextExtInPos[idc].y = 0.5*hiddenPart/vZoom[idc];
                    //InvalidateRect(SECTION[idc],&r,1);
                }
                if(cy <= 0)
                {
                    *value = *value0 = 0;
                    *scval = 1;
                }
                else
                {
                    *value = float(hiddenPart)/cy;
                    *value0 = *value;
                }
                
            }
            
            //InvalidateRect(SECTION[idc],&r,1);
        }

        int a = RECTCL[id].bottom - (vmarg + width)*2;
        szzoom->cx = ceil(a**scval);
        int tlen = a - szzoom->cx;
        *(int*)Args[9] = tlen;
        rcs[1] = {hmarg,rcs[0].bottom,width-hmarg,rcs[0].bottom+long(tlen**value)};
        rcs[2] = {hmarg, rcs[1].bottom,width-hmarg,rcs[1].bottom+szzoom->cx/*+(szzoom->cy)/2*/};
        rcs[3] = {hmarg,rcs[2].bottom,width-hmarg,r.bottom-width-vmarg};
        rcs[4] = {hmarg,rcs[3].bottom,width-hmarg,r.bottom-vmarg};
    }

    //HDC dc = cv.CTX[id].redrawDwgSurface();
    HDC dc = hdcontextExt[id];

    for(int i=0; i<5; i++)
    {
        if(!cloths[i].size())
        {
            CSRGBA col = colors[0][i];
            HBRUSH hb = CreateSolidBrush(RGB(col.r, col.g, col.b));
            HPEN hp = CreatePen(0,0,RGB((col.r+5)%255, (col.g+5)%255, (col.b+5)%255));
            SelectBrush(dc,hb);
            SelectBrush(dc,hp);
            Rectangle(dc,rcs[i].left, rcs[i].top, rcs[i].right, rcs[i].bottom);
            DeleteBrush(hb);
            DeleteBrush(hp);
        }
    }
    for(int i=0; i<5; i++)
    {
        if(cloths[i].size())
        {
            BitBlt(dc,rcs[i].left,rcs[i].top,rcs[i].right-rcs[i].left,rcs[i].bottom-rcs[i].top, cloths[i][0],0,0, SRCCOPY);
        }
    }

    InvalidateRect(HWND(Args), 0,1);
    
    bool threadBool = *(bool*)Args[22];

    if(!threadBool) // evite les conflits de mises a jour entre idc et id, et ameliore la fluidite
    {
        InvalidateRect(SECTION[idc], &bltRect[idc], 1);
        //cout<<"ooooo\n";
    }

    *(vector<RECT>*)Args[0] = rcs;
    *block = 0; // vaut 1 pour une reception (sendmessage) venant de wm_mouseMouve
}

void autoRepos(CSARGS Args)
{
    //if(UINT(Args) == WM_TIMER)
    int idp = PARID[int(Args)];

    int cx = *(int*)Args[19];
    int cy = *(int*)Args[20];

    if(RECTCL[idp].right != cx || RECTCL[idp].bottom != cy)
    {
        bool orientation = *(bool*)Args[8];
        cx = RECTCL[idp].right;
        cy = RECTCL[idp].bottom;

        int idc = *(int*)Args[13];
        int id = (int)Args;

        if(orientation == CS_SBAR_HORIZONTAL)
        {
            int thick = RECTCL[id].bottom;
            RECT r = RECTPARREFSAVED[id];
            int edge = *(int*)Args[24];

            int a = withVScroll[idp] ? thick : 0;
            MoveWindow(SECTION[id], /*r.left + */edge, cy /*- r.top*/ - edge - thick, 
                                            cx /*- r.left*/ - 2*edge - a, thick, 0);
        }
        else if(orientation == CS_SBAR_VERTICAL)
        {
            int thick = RECTCL[id].right;
            RECT r = RECTPARREFSAVED[id];
            int edge = *(int*)Args[25];

            MoveWindow(SECTION[id], cx /*- r.left*/ - edge - thick, /*r.top +*/ edge,  
                                            thick, cy /*- r.top*/ - 2*edge, 1);
         }
         *(int*)Args[19] = cx;
         *(int*)Args[20] = cy;
    }
}
void autoRepos_parentContext(CSARGS Args)
{
    //int idp = int(Args);
    int idp = *(int*)Args[8];

    int cx = *(int*)Args[4];
    int cy = *(int*)Args[5];

    if(RECTCL[idp].right != cx || RECTCL[idp].bottom != cy)
    {
        bool orientation = *(bool*)Args[7];
        cx = RECTCL[idp].right;
        cy = RECTCL[idp].bottom;

        int idc = *(int*)Args[0];
        int idScroll = *(int*)Args[1];
        if(orientation == CS_SBAR_HORIZONTAL)
        {
            int thick = RECTCL[idScroll].bottom;
            RECT r = RECTPARREFSAVED[idScroll];
            int edge = *(int*)Args[2];
            
            int a = withVScroll[idp] ? thick : 0;
            MoveWindow(SECTION[idScroll], /*r.left + */edge, cy /*- r.top*/ - edge - thick, 
                                            cx /*- r.left*/ - 2*edge - a, thick, 1);
        }
        else if(orientation == CS_SBAR_VERTICAL)
        {
            int thick = RECTCL[idScroll].right;
            RECT r = RECTPARREFSAVED[idScroll];
            int edge = *(int*)Args[3];

            MoveWindow(SECTION[idScroll], cx /*- r.left*/ - edge - thick, /*r.top +*/ edge,  
                                            thick, cy /*- r.top*/ - 2*edge, 1);
        }
        
        *(int*)Args[4] = cx;
        *(int*)Args[5] = cy;
    }
}

extern void delay(int del);

void lbtnDown(CSARGS Args)
{
    int* rectSelect = (int*)Args[10];

    if(*rectSelect == 2)
    {
        float* value = (float*)Args[2];
        float* value0 = (float*)Args[3];
        *value0 = *value;
        
        SetCapture((HWND)Args);
        IDCAPTURE = (int)Args;

        bool *threadBool = (bool*)Args[22];
        *threadBool = 1;

        thread t(
            [](CSARGS Args)
            {
                bool *threadBool = (bool*)Args[22];
                int idc = *(int*)Args[13];
                while(*threadBool)
                {
                    halftoneMode[idc] = 0;
                    InvalidateRect(SECTION[idc], &bltRect[idc], 1);
                    delay(20000);
                }
            }, Args
        );
        t.detach();
    }
    //POINT p1 = LBD_POINT;
}


void _apply(CSARGS Args);

void mouseMovesetCursor(CSARGS Args)
{
    vector<RECT> rcs = *(vector<RECT>*)Args[0];
    int* rectSelect = (int*)Args[10];
    bool orientation = *(bool*)Args[8];
    int *mhId = (int*)Args[11];

    POINT p1;
    GetCursorPos(&p1);
    ScreenToClient(SECTION[int(Args)], &p1);

    if(PtInRect(&rcs[1], p1))
    {
        *rectSelect = 1; 
        cursor[int(Args)] = LoadCursor(0, IDC_HAND);

    }
    else if(PtInRect(&rcs[2], p1))
    {
        *rectSelect = 2;
        if(orientation == CS_SBAR_HORIZONTAL)
            cursor[int(Args)] = LoadCursor(0, IDC_SIZEWE);
        else
            cursor[int(Args)] = LoadCursor(0, IDC_SIZENS);
    }
    else if(PtInRect(&rcs[3], p1))
    {
        *rectSelect = 3;
        cursor[int(Args)] = LoadCursor(0, IDC_HAND);
    }
    else if(PtInRect(&rcs[0], p1))
    {
        *rectSelect = 0;
        cursor[int(Args)] = LoadCursor(0, IDC_ARROW);
    }
    else if(PtInRect(&rcs[4], p1))
    {
        *rectSelect = 4;
        cursor[int(Args)] = LoadCursor(0, IDC_ARROW);
    }
    else 
    {
        *rectSelect = -1;
        cursor[int(Args)] = LoadCursor(0, IDC_ARROW);
    }
    *mhId = *rectSelect;
}


void mouseMove(CSARGS Args)
{
    int tlen = *(int*)Args[9];
    float* value = (float*)Args[2];
    float* value0 = (float*)Args[3];
    bool orientation = *(bool*)Args[8];
    int rectSelect = *(int*)Args[10];
    

    bool *block = (bool*)Args[16];
    *block = 1;

    POINT p2;
    GetCursorPos(&p2);
    ScreenToClient(SECTION[int(Args)], &p2);
    POINT p1 = LBD_POINT;
    ScreenToClient(SECTION[int(Args)], &p1);

    if(WPARAM(Args) == 1 && IDCAPTURE == int(Args))
    //if((GetAsyncKeyState(VK_LBUTTON)&&0x8000) && IDCAPTURE == int(Args))
    {
        float *scval = (float*)Args[12];
        if(rectSelect == 2 && *scval < 1 /*si le scroller n'occupe pas tout l'espace*/)
        {
            if(orientation == CS_SBAR_HORIZONTAL)
                *value = *value0 + float(p2.x-p1.x)/tlen;
            else
                *value = *value0 + float(p2.y-p1.y)/tlen;
            if(*value<0.0) *value = 0;
            if(*value>1.0) *value = 1.0;

            _apply(Args);
        }
    }
    //SendMessage(HWND(Args),WM_SIZE,0,0);
    //InvalidateRect(HWND(Args), 0,1);
    
}

void clickUnder(CSARGS Args)
{
    vector<RECT> rcs = *(vector<RECT>*)Args[0];
    int tlen = *(int*)Args[9];
    float* value = (float*)Args[2];
    float* value0 = (float*)Args[3];
    bool orientation = *(bool*)Args[8];
    int rectSelect = *(int*)Args[10];


    float *scval = (float*)Args[12];
    if(rectSelect == 1 && *scval < 1 /*si le scroller n'occupe pas tout l'espace*/)
    {
        POINT p2;
        GetCursorPos(&p2);
        ScreenToClient(SECTION[int(Args)], &p2);
        
        if(orientation == CS_SBAR_HORIZONTAL)
        {
            POINT p1 = {(rcs[2].right-rcs[2].left)/2 + rcs[2].left, 0};
            float v = *value0 + float(p2.x-p1.x)/tlen;
            *value = v >= 0 ? v : 0;
        }
        else
        {
            POINT p1 = {0, (rcs[2].bottom-rcs[2].top)/2 + rcs[2].top};
            float v = *value0 + float(p2.y-p1.y)/tlen;
            *value = v >= 0 ? v : 0;
        }

        _apply(Args);
    }
    
}

void clickOver(CSARGS Args)
{
    vector<RECT> rcs = *(vector<RECT>*)Args[0];
    int tlen = *(int*)Args[9];
    float* value = (float*)Args[2];
    float* value0 = (float*)Args[3];
    bool orientation = *(bool*)Args[8];
    int rectSelect = *(int*)Args[10];


    float *scval = (float*)Args[12];
    if(rectSelect == 3 && *scval < 1 /*si le scroller n'occupe pas tout l'espace*/)
    {
        POINT p2;
        GetCursorPos(&p2);
        ScreenToClient(SECTION[int(Args)], &p2);
        
        if(orientation == CS_SBAR_HORIZONTAL)
        {
            POINT p1 = {(rcs[2].right-rcs[2].left)/2 + rcs[2].left, 0};
            float v = *value0 + float(p2.x-p1.x)/tlen;
            *value = v <= 1.0 ? v : 1.0;
        }
        else
        {
            POINT p1 = {0, (rcs[2].bottom-rcs[2].top)/2 + rcs[2].top};
            float v = *value0 + float(p2.y-p1.y)/tlen;
            *value = v <= 1.0 ? v : 1.0;
        }

        _apply(Args);
    }
    
}

void decrease(CSARGS Args)
{
    float* value = (float*)Args[2];
    float* value0 = (float*)Args[3];
    int rectSelect = *(int*)Args[10];


    float *scval = (float*)Args[12];
    if(rectSelect == 0 && *scval < 1 /*si le scroller n'occupe pas tout l'espace*/)
    {
        float v = *value0 - 0.02;
        *value = v >= 0.0 ? v : 0.0;
        
        _apply(Args);
    }
    
}

void increase(CSARGS Args)
{
    float* value = (float*)Args[2];
    float* value0 = (float*)Args[3];
    int rectSelect = *(int*)Args[10];


    float *scval = (float*)Args[12];
    if(rectSelect == 4 && *scval < 1 /*si le scroller n'occupe pas tout l'espace*/)
    {
        float v = *value0 + 0.02;
        *value = v <= 1.0 ? v : 1.0;

        _apply(Args);
    }
    
}

void _apply(CSARGS Args)
{
    vector<RECT> rcs = *(vector<RECT>*)Args[0];
    int tlen = *(int*)Args[9];
    float* value = (float*)Args[2];
    float* value0 = (float*)Args[3];
    bool orientation = *(bool*)Args[8];
    POINT*oldClientPos = (POINT*)Args[17];

    int idc = *(int*)Args[13];
    int scType = *(int*)Args[14];
    int idMask = *(int*)Args[15];

    bool *block = (bool*)Args[16];
    *block = 1;

    if(idc && idMask)
    {
        if(scType == CS_SBAR_SURFACE)
        {
            POINT p = hdcontextExtOutPos[idc];
            if(orientation == CS_SBAR_HORIZONTAL)
            {
                int d = (RECTWND[idMask].right-viewedAreaRightMarging[idc])-(RECTWND[idc].left + p.x); //RECTWND[idMask].right-RECTWND[idc].left;
                int cx = (hdcontextExtSize[idc].cx*hZoom[idc]-d);
                
                if (cx < 0) cx = 0;
                int len = ceil(*value*cx);
                hdcontextExtInPos[idc] = {len*1.0/hZoom[idc], hdcontextExtInPos[idc].y};
                *oldClientPos = hdcontextExtInPos[idc];

                //RECT r = {p.x, p.y, d+p.x, (RECTWND[idMask].bottom-viewedAreaBottomMarging[idc])-RECTWND[idc].top};
                //InvalidateRect(SECTION[idc],&r,1);
            }
            else
            {
                int d = (RECTWND[idMask].bottom-viewedAreaBottomMarging[idc])-(RECTWND[idc].top+p.y);//RECTWND[idMask].bottom-RECTWND[idc].top;
                int cy = (hdcontextExtSize[idc].cy*vZoom[idc]-d);
                if (cy < 0) cy = 0;
                int len = ceil(*value*cy);
                hdcontextExtInPos[idc] = {hdcontextExtInPos[idc].x, len*1.0/vZoom[idc]};
                *oldClientPos = hdcontextExtInPos[idc];

                //RECT r = {p.x, p.y, (RECTWND[idMask].right-viewedAreaRightMarging[idc])-RECTWND[idc].left, d+p.x};
                //InvalidateRect(SECTION[idc],&r,1);
            }
        }
    }

    SendMessage(HWND(Args),WM_SIZE,0,0);
}


void lbtnup_(CSARGS Args)
{
    ReleaseCapture();
    float* value = (float*)Args[2];
    float* value0 = (float*)Args[3];
    *value0 = *value;

    bool *threadBool = (bool*)Args[22];
    *threadBool = 0;

    int idc = *(int*)Args[13];
    halftoneMode[idc] = 1;
    InvalidateRect(SECTION[idc],&bltRect[idc],1);
}

void timer_(CSARGS Args)
{
    int idc = *(int*)Args[13];
    int scType = *(int*)Args[14];
    //float* value = (float*)Args[2];
    //float* value0 = (float*)Args[3];
    bool orientation = *(bool*)Args[8];
    POINT*oldClientPos = (POINT*)Args[17];
    SIZE*oldClientSize = (SIZE*)Args[18];
    //int idMask = *(int*)Args[15];
    //bool *block = (bool*)Args[16];

    if(scType == CS_SBAR_SURFACE)
    {
        if(orientation == CS_SBAR_HORIZONTAL)
        { 
            
            if(oldClientPos->x != hdcontextExtInPos[idc].x)
            {
                oldClientPos->x = hdcontextExtInPos[idc].x;
                SendMessage((HWND)Args,WM_SIZE,0,0);
            }
            else if(oldClientSize->cx != hdcontextExtSize[idc].cx)
            {
                int idMask = *(int*)Args[15];
                float* value = (float*)Args[2];
                float* value0 = (float*)Args[3];
                float *scval = (float*)Args[12];
                if(hdcontextExtSize[idc].cx < bltRect[idc].right-bltRect[idc].left)
                {
                    *value = *value0 = 0;
                    *scval = 1;
                }
                oldClientSize->cx = hdcontextExtSize[idc].cx;
            }
            if(hdcontextExtInPos[idc].x < 0 && bltRect[idc].right < hdcontextExtSize[idc].cx*hZoom[idc])
            {
                hdcontextExtInPos[idc].x = 0;
                InvalidateRect(SECTION[idc], &bltRect[idc], 1);
                SendMessage((HWND)Args, WM_SIZE, 0,0);
            }
        }
        else if(orientation == CS_SBAR_VERTICAL)
        {
            if(oldClientPos->y != hdcontextExtInPos[idc].y)
            {
                oldClientPos->y = hdcontextExtInPos[idc].y;
                SendMessage((HWND)Args,WM_SIZE,0,0);
            }
            else if(oldClientSize->cy != hdcontextExtSize[idc].cy) // superflux, modifer
            {   //std::cout<<"hey00000\n";
                int idMask = *(int*)Args[15];
                float* value = (float*)Args[2];
                float* value0 = (float*)Args[3];
                float *scval = (float*)Args[12];
                if(hdcontextExtSize[idc].cy < bltRect[idc].bottom-bltRect[idc].top)
                {
                    *value = *value0 = 0;
                    *scval = 1;
                    //InvalidateRect((HWND)Args,0,1); // renvoie une boucle infinie ... pourquoi ????
                }
                oldClientSize->cy = hdcontextExtSize[idc].cy;
            }

            if(hdcontextExtInPos[idc].y < 0 && bltRect[idc].bottom < hdcontextExtSize[idc].cy*vZoom[idc])
            {
                hdcontextExtInPos[idc].y = 0;
                InvalidateRect(SECTION[idc], &bltRect[idc], 1);
                SendMessage((HWND)Args, WM_SIZE, 0,0);
            }
        }

    }

}

CSSCROLLBAR::~CSSCROLLBAR()
{
    //dtor
}

void longPress2(CSARGS Args)
{
    UINT msg = (UINT)Args;
    //int timerid = 1;
    //static bool signal=0;

    if(msg == WM_LBUTTONDOWN)
    {
        increase(Args);
        //signal = 1;
        //SetTimer((HWND)Args, timerid, 150, 0);
    }
    if(msg == WM_LBUTTONDBLCLK)
    {
        increase(Args);
    }
    /*if(msg == WM_LBUTTONUP)
    {
        signal = 0;
        KillTimer(HWND(Args), timerid);
    }
    if(msg == WM_TIMER && (WPARAM)Args == timerid && signal)
    {
        cout<<"kjkj\n";
        increase(Args);
    }*/
}
void longPress1(CSARGS Args)
{
    UINT msg = (UINT)Args;
    //int timerid = 1;
    //static bool signal=0;

    if(msg == WM_LBUTTONDOWN)
    {
        decrease(Args);
        //signal = 1;
        //SetTimer((HWND)Args, timerid, 150, 0);
    }
    if(msg == WM_LBUTTONDBLCLK)
    {
        decrease(Args);
    }
    /*else if(msg == WM_LBUTTONUP)
    {
        signal = 0;
        KillTimer(HWND(Args), timerid);
    }
    else if(msg == WM_TIMER && (WPARAM)Args == timerid && signal)
    {
        decrease(Args);
    }*/
}

void mouseMoveSetColor(CSARGS Args)
{
    int *mhId = (int*)Args[11];
    int *mhId0 = (int*)Args[23];


    if(*mhId != *mhId0)
    {
        int id = int(Args);
        vector<RECT> rcs = *(vector<RECT>*)Args[0];
        vector<vector<CSRGBA>> colors = *(vector<vector<CSRGBA>>*)Args[1];
        vector<vector<HDC>> cloths = *(vector<vector<HDC>>*)Args[21];
        HDC dc = hdcontextExt[id];

        int i = *mhId;

        if(i >= 0)
        {
            if(!cloths[i].size())
            {
                CSRGBA col = colors[1][i];
                HBRUSH hb = CreateSolidBrush(RGB(col.r, col.g, col.b));
                HPEN hp = CreatePen(0,0,RGB((col.r+5)%255, (col.g+5)%255, (col.b+5)%255));
                SelectBrush(dc,hb);
                SelectBrush(dc,hp);
                Rectangle(dc,rcs[i].left, rcs[i].top, rcs[i].right, rcs[i].bottom);
                DeleteBrush(hb);
                DeleteBrush(hp);
            }
            else
            {
                BitBlt(dc,rcs[i].left,rcs[i].top,rcs[i].right-rcs[i].left,rcs[i].bottom-rcs[i].top, cloths[i][1],0,0, SRCCOPY);
            }
        }

        i = *mhId0;
        if(i >= 0)
        {
            if(!cloths[i].size())
            {
                CSRGBA col = colors[0][i];
                HBRUSH hb = CreateSolidBrush(RGB(col.r, col.g, col.b));
                HPEN hp = CreatePen(0,0,RGB((col.r+5)%255, (col.g+5)%255, (col.b+5)%255));
                SelectBrush(dc,hb);
                SelectBrush(dc,hp);
                Rectangle(dc,rcs[i].left, rcs[i].top, rcs[i].right, rcs[i].bottom);
                DeleteBrush(hb);
                DeleteBrush(hp);
            }
            else
            {
                BitBlt(dc,rcs[i].left,rcs[i].top,rcs[i].right-rcs[i].left,rcs[i].bottom-rcs[i].top, cloths[i][0],0,0, SRCCOPY);
            }
        }
        *mhId0 = *mhId;
        InvalidateRect(SECTION[id], 0, 1);
    }

}

void mouseLeave_(CSARGS Args)
{
    //vector<vector<CSRGBA>>* colors = (vector<vector<CSRGBA>>*)Args[1];
    int* mhId = (int*)Args[11];
    *mhId = -1;
}

extern POINT TIMER_POINT;

void mouseWheel(CSARGS Args)
{
    if(UINT(Args) == WM_MOUSEWHEEL)
    {
        int idc = int(Args);
        POINT p = hdcontextExtInPos[idc];
        RECT r = bltRect[idc];
        int delta = 10;
        if(GET_WHEEL_DELTA_WPARAM(WPARAM(Args))<0)
        {
            bool a = 0, b = 0;
            if(withHScroll[idc] && (GetAsyncKeyState(VK_CONTROL) || mouseWheelPreference[idc] == CS_MOUSEWHEEL_HSCROLL))
            {
                long px = p.x/hZoom[idc];
                long cx = (bltRect[idc].right/hZoom[idc]);
                long d = px+cx + delta - hdcontextExtSize[idc].cx;
                //cout<<"+++++++++++++++++++++++++++++++++++++++++++++\n";
                if(d <= 0)
                {
                    hdcontextExtInPos[idc].x += delta;
                }
                else
                    hdcontextExtInPos[idc].x = hdcontextExtSize[idc].cx - bltRect[idc].right;
                
                a = 1;
            }
            if(withVScroll[idc] && (GetAsyncKeyState(VK_SHIFT) || mouseWheelPreference[idc] == CS_MOUSEWHEEL_VSCROLL))
            {
                long py = p.y/vZoom[idc];
                long cy = (bltRect[idc].bottom/vZoom[idc]);
                long d = py+cy + delta - hdcontextExtSize[idc].cy;
                //cout<<"+++++++++++++++++++++++++++++++++++++++++++++\n";
                if(d <= 0)
                {
                    hdcontextExtInPos[idc].y += delta;
                }
                else
                    hdcontextExtInPos[idc].y = hdcontextExtSize[idc].cy - bltRect[idc].bottom;

                b = 1;
            }
            
            if((!a && !b) && mouseWheelPreference[idc] == CS_MOUSEWHEEL_ZOOM)
            {//halftoneMode[idc] = 0;
                POINT pt = TIMER_POINT;
                ScreenToClient(SECTION[idc], &pt);
                if(withHScroll[idc] && hZoom[idc] <= zoomParams[idc].hmax)
                {
                    hZoom[idc] += 0.05;

                    // zoom to cursor
                    /*pt.x -= hdcontextExtOutPos[idc].x;
                    long dx = 1.0*((bltRect[idc].right-bltRect[idc].left-hdcontextExtOutPos[idc].x)/2 - pt.x)*hZoom[idc];
                    hdcontextExtInPos[idc].x = -dx;*/
                    //cout<<" dx = "<<dx<<"\n";

                    SendMessage(SECTION[withHScroll[idc]], WM_SIZE, 0,0);
                }
                if(withVScroll[idc] && vZoom[idc] <= zoomParams[idc].vmax)
                {
                    vZoom[idc] += 0.05;

                    // zoom to cursor
                    /*pt.y -= hdcontextExtOutPos[idc].y;
                    long dy = 1.0*((bltRect[idc].bottom-bltRect[idc].top-hdcontextExtOutPos[idc].y)/2 - pt.y)*vZoom[idc];
                    hdcontextExtInPos[idc].y = -dy;*/

                    SendMessage(SECTION[withVScroll[idc]], WM_SIZE, 0,0);
                }
                InvalidateRect(SECTION[idc], &bltRect[idc], 1);
            }
        }
        else
        {
            bool a = 0 , b = 0;
            if(withHScroll[idc] && (GetAsyncKeyState(VK_CONTROL) || mouseWheelPreference[idc] == CS_MOUSEWHEEL_HSCROLL))
            {
                long px = p.x/hZoom[idc];
                long cx = (bltRect[idc].right/hZoom[idc]);
                long d = px+cx - delta;
                if(d >= 0)
                {
                    hdcontextExtInPos[idc].x -= delta;
                }
                if(hdcontextExtInPos[idc].x < 0)
                    hdcontextExtInPos[idc].x = 0;

                a = 1;
            }

            if(withVScroll[idc] && (GetAsyncKeyState(VK_SHIFT) || mouseWheelPreference[idc] == CS_MOUSEWHEEL_VSCROLL))
            {
                long py = p.y/vZoom[idc];
                long cy = (bltRect[idc].bottom/vZoom[idc]);
                long d = py+cy - delta;
                if(d >= 0)
                {
                    hdcontextExtInPos[idc].y -= delta;
                }

                if(hdcontextExtInPos[idc].y < 0)
                    hdcontextExtInPos[idc].y = 0;
                b = 1;
            }

            if((!a && !b) && mouseWheelPreference[idc] == CS_MOUSEWHEEL_ZOOM)
            {//halftoneMode[idc] = 0;
                POINT pt = TIMER_POINT;
                ScreenToClient(SECTION[idc], &pt);
                if(withHScroll[idc] && hZoom[idc] >= zoomParams[idc].hmin)
                {
                    hZoom[idc] -= 0.05;

                    // zoom to cursor

                    /*pt.x -= hdcontextExtOutPos[idc].x;
                    long dx = 1.0*((bltRect[idc].right-bltRect[idc].left-hdcontextExtOutPos[idc].y)/2 - pt.x)*hZoom[idc];
                    hdcontextExtInPos[idc].x = -dx;*/

                    SendMessage(SECTION[withHScroll[idc]], WM_SIZE, 0,0);
                }
                if(withVScroll[idc] && vZoom[idc] >= zoomParams[idc].vmin)
                {
                    vZoom[idc] -= 0.05;
                    
                    // zoom to cursor

                    /*pt.y -= hdcontextExtOutPos[idc].y;
                    long dy = 1.0*((bltRect[idc].bottom-bltRect[idc].top-hdcontextExtOutPos[idc].y)/2 - pt.y)*vZoom[idc];
                    hdcontextExtInPos[idc].y = -dy;*/
                    
                    SendMessage(SECTION[withVScroll[idc]], WM_SIZE, 0,0);
                }
                InvalidateRect(SECTION[idc], &bltRect[idc], 1);
            }
        }
    }
}

void clientGesture(CSARGS Args)
{
    if(UINT(Args) == WM_GESTURE)
    {
        int idc = (int)Args;
        POINT p = hdcontextExtInPos[idc];
        RECT r = bltRect[idc];
        HGESTUREINFO hGestureInfo = (HGESTUREINFO)(LPARAM)Args;
        GESTUREINFO gi = {0};
        gi.cbSize = sizeof(GESTUREINFO);
cout<<"gesture\n";
        if (GetGestureInfo(hGestureInfo, &gi))
        {
            if (gi.dwID == GID_PAN)
            {
                // Vérifier si c'est un mouvement horizontal ou vertical
                if (gi.ptsLocation.y == 0)
                {
                    // Mouvement horizontal
                    if (gi.ullArguments & GF_BEGIN)
                    {
                        // Début du mouvement horizontal
                    }
                    else if (gi.ullArguments == 0)
                    {
                        // Mouvement horizontal en cours
                        POINT ptDelta = { (LONG)(short)LOWORD(gi.ullArguments), (LONG)(short)HIWORD(gi.ullArguments) };
                        // Utiliser ptDelta.x pour le déplacement horizontal
                    }
                }
                else
                {
                    // Mouvement vertical
                    if (gi.ullArguments & GF_BEGIN)
                    {
                        // Début du mouvement vertical
                    }
                    else if (gi.ullArguments == 0)
                    {
                        // Mouvement vertical en cours
                        POINT ptDelta = { (LONG)(short)LOWORD(gi.ullArguments), (LONG)(short)HIWORD(gi.ullArguments) };
                        long px = p.x*hZoom[idc];
                        long cx = (bltRect[idc].right/hZoom[idc]);
                        long d = px+cx + ptDelta.x - hdcontextExtSize[idc].cx;
                        if(d < 0)
                        {
                            hdcontextExtInPos[idc].x += ptDelta.x;
                        }
                        // Utiliser ptDelta.y pour le déplacement vertical
                    }
                }
            }
            CloseGestureInfoHandle(hGestureInfo);
        }
    }
}
#include "csLISTBOX.h"

extern vector<HWND> SECTION;
extern vector<HDC> hdcontext;
extern vector<HDC> hdcontextExt;
extern vector<HBITMAP> hdBmpExt;
extern vector<POINT> hdcontextExtInPos;
extern vector<POINT> hdcontextExtOutPos;
extern vector<SIZE> hdcontextExtSize;

extern POINT TIMER_POINT;

csLISTBOX::csLISTBOX(int id, int nx, int ny, int CX, int CY, int hMarging, int vMarging, int nInRect)
{
    init(id, nx, ny, CX, CY, hMarging, vMarging, nInRect);
}

csLISTBOX::~csLISTBOX()
{
    //dtor
}

void csLISTBOX::init(int id, int nx, int ny, int CX, int CY, int hMarging, int vMarging, int _nInRect)
{
    lbs.nx = nx;
    lbs.ny = ny;
    lbs.CX = CX;
    lbs.CY = CY;
    lbs.cx = 2*hMarging;
    lbs.cy = 2*vMarging;

    lbs.dcSize.cx = nx*CX;
    lbs.dcSize.cy = ny*CY;

    if(!hdcontextExt[id])
    {
        hdcontextExt[id] = (CreateCompatibleDC(hdcontext[id]));
        hdBmpExt[id] = (CreateCompatibleBitmap(hdcontext[id],lbs.dcSize.cx, lbs.dcSize.cy));
        SelectBitmap(hdcontextExt[id], hdBmpExt[id]);
        lbs.parentDC = hdcontextExt[id];
        return;
    }
    lbs.parentDC = hdcontextExt[id];

    int idp = id;

    lbs.defTextColor.disabled = {190,190,190};
    lbs.defTextColor.normal = {195,195,195};
    lbs.defTextColor.mouseOver = {200,200,200};
    lbs.defTextColor.inMouseOver = {202,202,202};
    lbs.defTextColor.actived = {205,205,205};
    lbs.defTextColor.inActived = {210,210,210};

    lbs.defBorderColor.disabled = {20,20,20};
    lbs.defBorderColor.normal = {25,25,25};
    lbs.defBorderColor.mouseOver = {30,30,30};
    lbs.defBorderColor.inMouseOver = {32,32,32};
    lbs.defBorderColor.actived = {35,35,35};
    lbs.defBorderColor.inActived = {40,40,40};

    lbs.defBkgColor.disabled = {15,15,15};
    lbs.defBkgColor.normal = {20,20,20};
    lbs.defBkgColor.mouseOver = {25,25,25};
    lbs.defBkgColor.inMouseOver = {27,27,27};
    lbs.defBkgColor.actived = {30,30,30};
    lbs.defBkgColor.inActived = {35,35,35};

    nInRect = _nInRect;
    int timerIter = 0;

    _setRects();
}

void csLISTBOX::_setRects()
{
    int n = lbs.nx*lbs.ny;

    for(int i=0; i<n; i++)
    {
        for(int j = 0; j<nInRect; j++)
        {
            //lbs.lsis.push_back();
        }
    }
}

wchar_t* csLISTBOX::_newText(wchar_t*text)
{
    wchar_t* str = (wchar_t*)malloc(wcslen(text)*sizeof(wchar_t));
    wsprintf(str, L"%s\0", text);
    
    return str;
}

bool csLISTBOX::isIdActived(int id)
{
    int n = lbs.nActiveIds;
    for(int i=0; i<n; i++)
    {
        if(lbs.activeId[i] == id)
            return 1;
    }
    return 0;
}

bool csLISTBOX::isMouseHoverId(int id)
{
    id == lbs.mouseId;
}

void csLISTBOX::deleteActiveIds()
{
    int n = lbs.activeId.size();
    for(int i=0; i<n; i++)
    {
        int k = lbs.activeId[i];
        int m = lbs.lsis[k].size();

        for(int j=0; j<n; i++)
        {
            DeleteDC(lbs.lsis[k][j].img.actived);
            DeleteDC(lbs.lsis[k][j].img.disabled);
            DeleteDC(lbs.lsis[k][j].img.inActived);
            DeleteDC(lbs.lsis[k][j].img.mouseOver);
            DeleteDC(lbs.lsis[k][j].img.inMouseOver);
            DeleteDC(lbs.lsis[k][j].img.normal);
            free(lbs.lsis[k][j].text);
            free(lbs.lsis[k][j].fontPar.name);
        }

        lbs.lsis[k].clear();
        lbs.lsis.erase(lbs.lsis.begin() + k);
        lbs.state.erase(lbs.state.begin() + k);
        lbs.unbinded.erase(lbs.unbinded.begin() + k);
    }

    if(lbs.state.size())
    {
        int id = lbs.activeId[0];
        for(int i=0; i<lbs.activeId.size(); i++)
        {
            if(lbs.activeId[i] < id)
                id = lbs.activeId[i];
        }
        if(id-1>=0)
            id =  id-1;
        else
            id = id+1;
        
        lbs.activeId.clear();
        lbs.activeId.push_back(id);
    }

    create();
}

int csLISTBOX::getIdFromPosition_timer()
{
    POINT _p = TIMER_POINT;
    ScreenToClient(CSUIMAN::sHandle(idp), &_p);
    POINT p = {_p.x-hdcontextExtOutPos[idp].x, _p.y-hdcontextExtOutPos[idp].y};
    int i = p.x/lbs.CX;
    int j = p.y/lbs.CY;
    int id = lbs.nx*j + i;
    
    if(id < lbs.lsis.size())
    {
        RECT r0 = lbs.lsis[id][0].r;
        RECT r1 = lbs.lsis[id][ lbs.lsis[id].size() ].r;
        RECT r = {r0.left, r0.top, r1.right, r1.bottom};

        if((lbs.lastMouseId != id))
        {
            if(PtInRect(&r, p))
            {
                lbs.lastMouseId = lbs.mouseId;
                lbs.mouseId = id;
            }
            else id = -1;
        }

        if(id > -1)
        {
            if(timerIter >= lbs.lsis[id].size())
            {
                timerIter = 0;
            }

            if(lbs.lastMouseInId != timerIter)
            {
                if(PtInRect(&lbs.lsis[id][timerIter].r, p))
                {
                    lbs.lastMouseId = lbs.mouseId;
                    lbs.mouseInId = timerIter;
                }
                else
                {
                    lbs.lastMouseId = lbs.mouseId;
                    lbs.mouseInId = -1;
                }
                SendMessage(CSUIMAN::sHandle(idp), WM_PAINT, 0,0);
                InvalidateRect(CSUIMAN::sHandle(idp), 0,1);
            }

            timerIter++;
        }
    }
}

void csLISTBOX::select()
{
    int id = lbs.mouseId;
    if(!isIdActived(id))
    {
        if(GetAsyncKeyState(VK_CONTROL))
        {

        }
    }
}

void csLISTBOX::applyMouseOverVisual()
{
    int id = lbs.mouseId;
    int inId = lbs.mouseInId;
    int lid = lbs.lastMouseId;
    int linId = lbs.lastMouseInId;
    //if(id > -1)
    int n = lbs.lsis[id].size();

    HDC dc = lbs.parentDC;
    RECT r;
    CSRGBA c, pc, bc;
    csLISTBOX::FONT_PARAMS fontPar;
    POINT p;
    SIZE sz;
    for(int i=0; i<n; i++)
    {
        if(!lbs.lsis[id][i].selected)
        {
            pc = lbs.lsis[id][i].borderColor.mouseOver;
            bc = lbs.lsis[id][i].bkgColor.mouseOver;
            r = lbs.lsis[id][i].r;
            _drawRectangle(dc, r, pc, bc);
            wchar_t* text = lbs.lsis[id][i].text;
            if(text)
            {
                c = lbs.lsis[id][i].textColor.mouseOver;
                fontPar = lbs.lsis[id][i].fontPar;
                p = lbs.lsis[id][i].textPos;
                _drawText(dc, text, p, c, fontPar);
            }

            HDC hdc = lbs.lsis[id][i].img.mouseOver;
            if(hdc)
            {
                p = lbs.lsis[id][i].imgPos;
                sz = lbs.lsis[id][i].imgSize;
                BitBlt(dc, p.x, p.y, sz.cx, sz.cy, hdc, 0,0, SRCCOPY);
            }
        }
    }
    if(inId > -1 && !lbs.lsis[id][inId].selected)
    {
        int i = lid;
        pc = lbs.lsis[id][i].borderColor.inMouseOver;
        bc = lbs.lsis[id][i].bkgColor.inMouseOver;
        r = lbs.lsis[id][i].r;
        _drawRectangle(dc, r, pc, bc);
        wchar_t* text = lbs.lsis[id][i].text;
        if(text)
        {
            c = lbs.lsis[id][i].textColor.inMouseOver;
            fontPar = lbs.lsis[id][i].fontPar;
            p = lbs.lsis[id][i].textPos;
            _drawText(dc, text, p, c, fontPar);
        }

        HDC hdc = lbs.lsis[id][i].img.inMouseOver;
        if(hdc)
        {
            p = lbs.lsis[id][i].imgPos;
            sz = lbs.lsis[id][i].imgSize;
            BitBlt(dc, p.x, p.y, sz.cx, sz.cy, hdc, 0,0, SRCCOPY);
        }

    }

    if(lid != id && !isIdActived(lid))
    {
        for(int i=0; i<n; i++)
        {
            if(!lbs.lsis[id][i].selected)
            {
                pc = lbs.lsis[id][i].borderColor.normal;
                bc = lbs.lsis[id][i].bkgColor.normal;
                r = lbs.lsis[id][i].r;
                _drawRectangle(dc, r, pc, bc);
                wchar_t* text = lbs.lsis[id][i].text;
                if(text)
                {
                    c = lbs.lsis[id][i].textColor.normal;
                    fontPar = lbs.lsis[id][i].fontPar;
                    p = lbs.lsis[id][i].textPos;
                    _drawText(dc, text, p, c, fontPar);
                }

                HDC hdc = lbs.lsis[id][i].img.normal;
                if(hdc)
                {
                    p = lbs.lsis[id][i].imgPos;
                    sz = lbs.lsis[id][i].imgSize;
                    BitBlt(dc, p.x, p.y, sz.cx, sz.cy, hdc, 0,0, SRCCOPY);
                }
            }
        }
    }
}


void csLISTBOX::applySelectionVisual()
{
    
}

void csLISTBOX::create()
{

}
void csLISTBOX::_drawRectangle(HDC dc, RECT r, CSRGBA pc, CSRGBA bc)
{
    HPEN hp = CreatePen(0,1,RGB(pc.r, pc.g, pc.b));
    HBRUSH hb = CreateSolidBrush(RGB(bc.r, bc.g, bc.a));
    SelectBrush(dc, hb);
    SelectPen(dc, hp);
    Rectangle(dc, r.left, r.top, r.right, r.bottom);
    DeleteBrush(hb);
    DeletePen(hp);
}

void csLISTBOX::_drawText(HDC dc, wchar_t* text, POINT pos, CSRGBA color, csLISTBOX::FONT_PARAMS fontPar)
{
    HFONT font = CreateFontW(
    fontPar.size, 0, fontPar.angle, 0, fontPar.weight, fontPar.italic, fontPar.underlined, FALSE,
    ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, fontPar.name //Segoe UI Emoji
    );
    SelectFont(dc, font);
    SetBkMode(dc, TRANSPARENT);
    TextOutW(dc, pos.x, pos.y, text, wcslen(text));
    DeleteFont(font);
}

csLISTBOX::IMAGE_PARAMS csLISTBOX::loadImages(csLISTBOX::IMAGE_PATH ip, POINT imgPos)
{
    CSGRAPHIC_CONTEXT gc1 = csGraphics::getImageGraphicContextW(ip.normal);

    CSGRAPHIC_CONTEXT gc2;
    if(ip.mouseOver == 0)
        gc2 = gc1;
    else
        gc2 = csGraphics::getImageGraphicContextW(ip.mouseOver);

    CSGRAPHIC_CONTEXT gc3;
    if(ip.inMouseOver == 0)
        gc3 = gc1;
    else
        gc3 = csGraphics::getImageGraphicContextW(ip.inMouseOver);

    CSGRAPHIC_CONTEXT gc4;
    if(ip.actived == 0)
        gc4 = gc1;
    else
        gc4 = csGraphics::getImageGraphicContextW(ip.actived);

    CSGRAPHIC_CONTEXT gc5;
    if(ip.inActived == 0)
        gc5 = gc1;
    else
        gc5 = csGraphics::getImageGraphicContextW(ip.inActived);

    CSGRAPHIC_CONTEXT gc6;
    if(ip.disabled == 0)
        gc6 = gc1;
    else
        gc6 = csGraphics::getImageGraphicContextW(ip.disabled);

    IMAGE_PARAMS ipar;

    ipar.imgPos = imgPos;
    ipar.imgSize = gc1.sz;
    ipar.imgElm.normal = gc1.dc;
    ipar.imgElm.mouseOver = gc2.dc;
    ipar.imgElm.inMouseOver = gc3.dc;
    ipar.imgElm.actived = gc4.dc;
    ipar.imgElm.inActived = gc5.dc;
    ipar.imgElm.disabled = gc6.dc;
    
    return ipar;
}

csLISTBOX::IMAGE_PARAMS csLISTBOX::loadImagesExt(csLISTBOX::IMAGE_PATH ip, POINT imgPos, SIZE imgSize)
{
    CSGRAPHIC_CONTEXT gc1 = csGraphics::getImageGraphicContextResizedW(ip.normal, imgSize);

    CSGRAPHIC_CONTEXT gc2;
    if(ip.mouseOver == 0)
        gc2 = gc1;
    else
        gc2 = csGraphics::getImageGraphicContextResizedW(ip.mouseOver, imgSize);

    CSGRAPHIC_CONTEXT gc3;
    if(ip.inMouseOver == 0)
        gc3 = gc1;
    else
        gc3 = csGraphics::getImageGraphicContextResizedW(ip.inMouseOver, imgSize);

    CSGRAPHIC_CONTEXT gc4;
    if(ip.actived == 0)
        gc4 = gc1;
    else
        gc4 = csGraphics::getImageGraphicContextResizedW(ip.actived, imgSize);

    CSGRAPHIC_CONTEXT gc5;
    if(ip.inActived == 0)
        gc5 = gc1;
    else
        gc5 = csGraphics::getImageGraphicContextResizedW(ip.inActived, imgSize);

    CSGRAPHIC_CONTEXT gc6;
    if(ip.disabled == 0)
        gc6 = gc1;
    else
        gc6 = csGraphics::getImageGraphicContextResizedW(ip.disabled, imgSize);

    IMAGE_PARAMS ipar;

    ipar.imgPos = imgPos;
    ipar.imgSize = gc1.sz;
    ipar.imgElm.normal = gc1.dc;
    ipar.imgElm.mouseOver = gc2.dc;
    ipar.imgElm.inMouseOver = gc3.dc;
    ipar.imgElm.actived = gc4.dc;
    ipar.imgElm.inActived = gc5.dc;
    ipar.imgElm.disabled = gc6.dc;
    
    return ipar;
}
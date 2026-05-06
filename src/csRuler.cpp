#include "CSRULER.h"
#include "cSRULER.h"
#include "csTypes.h"
#include "minwindef.h"
#include "windows.h"
#include "csUtils.h"

extern vector<vector<CSGRAPHIC_ENTITY>> entity;
extern vector<vector<CSRULER_ENTITY>> ruler;

extern vector<POINT> hdcontextExtInPos;
extern vector<HDC> hdcontextExt;
extern vector<HDC> hdStackContext;

void updatePosition(CSARGS Args);

CSRULER* csNewRuler(int id, POINT _pos, SIZE _size, bool _style, double _first, double _outdx,
        unsigned _unit, unsigned _dx, unsigned _DX)
{
    CSRULER* ru = new CSRULER(id, _pos, _size, _style, _first, _outdx, _unit, _dx, _DX);
    CSSECMAN::addAction(id, updatePosition, 1, ru);

    CSRULER_ENTITY ruEntity = {0};
    ruEntity.show = ru->getShowPtr();
    ruEntity.position = ru->getPositionPtr();
    ruEntity.inPositionX = ru->getInPositionXPtr();
    ruEntity.inPositionY = ru->getInPositionYPtr();
    ruEntity.gContext = ru->getGContextPtr();
    ruler[id].push_back(ruEntity);

    entity[id].push_back(CSGRAPHIC_ENTITY{.type=CSG_ENTITY_RULER, .tid=ruler[id].size()-1, .group=0});
    return ru;
}

CSRULER::CSRULER(int _idp, POINT _pos, SIZE _size, bool _style, double _first, double _outdx,
    unsigned _unit, unsigned _dx, unsigned _DX)
{
    init(_idp, _pos, _size, _style, _first, _outdx, _unit, _dx, _DX);
}
void CSRULER::init(int _idp, POINT _pos, SIZE _size, bool _style, double _first, double _outdx,
        unsigned _unit, unsigned _dx, unsigned _DX)
{
    idp = _idp;
    pos = _pos;
    size = _size;
    show = 1;
    if(size.cx > size.cy)
    {
        orient = CS_RULER_HORIZONTAL;
        inPosX = &hdcontextExtInPos[idp].x;
        inPosY = csAlloc<LONG>(1,0);
    }
    else
    {
        orient = CS_RULER_VERTICAL;
        inPosX = csAlloc<LONG>(1,0);
        inPosY = &hdcontextExtInPos[idp].y;
    }
        
    style = _style;
    first = _first;
    outdx = _outdx;
    unit = _unit;
    dx = _dx;
    DX = _DX;
    l1 = 4;
    l2 = 8;
    fontSize = 12;

    goto dark;

    light :
    {
        bkgCol = {100,100,100};
        brdCol = {100,100,100};
        linesCol1 = {10,10,10};
        linesCol2 = {0,0,0};
        textCol = {0,0,0};
    }

    dark:
    {
        bkgCol = {30,30,30};
        brdCol = {25,25,25};
        linesCol1 = {150,150,150};
        linesCol2 = {200,200,200};
        textCol = {150,150,150};
    }

    //update();

    //ctor
}

SIZE* CSRULER::getSizePtr()
{
    return &size;
}
POINT* CSRULER::getPositionPtr()
{
    return &pos;
}
LONG* CSRULER::getInPositionXPtr()
{
    return inPosX;
}
LONG* CSRULER::getInPositionYPtr()
{
    return inPosY;
}
bool* CSRULER::getOrientationPtr()
{
    return &orient;
}

bool* CSRULER::getShowPtr()
{
    return &show;
}

CSGRAPHIC_CONTEXT* CSRULER::getGContextPtr()
{
    return &rulerDC;
}

void CSRULER::setPosition(POINT pos)
{
    this->pos = pos;
}
void CSRULER::setInPositionX(LONG x)
{
    *inPosX = x;
}
void CSRULER::setInPositionY(LONG y)
{
    *inPosY = y;
}
void CSRULER::setSize(SIZE size)
{
    this->size = size;
}

void CSRULER::setColors(CSRGBA bkgCol, CSRGBA brdCol, CSRGBA linesCol1, CSRGBA linesCol2, CSRGBA textCol)
{
    this->bkgCol = bkgCol;
    this->brdCol = brdCol;
    this->linesCol1 = linesCol1;
    this->linesCol2 = linesCol2;
    this->textCol = textCol;
}
void CSRULER::setFontSize(int fontSize)
{
    this->fontSize = fontSize;
}
void CSRULER::setFirst(double first)
{
    this->first = first;
}

void CSRULER::setOutdx(double outdx)
{
    this->outdx = outdx;
}
void CSRULER::setUnit(unsigned unit)
{
    this->unit = unit;
}
void CSRULER::setdx(unsigned dx)
{
    this->dx = dx;
}
void CSRULER::setDX(unsigned DX)
{
    this->DX = DX;
}

void CSRULER::update()
{
    rulerDC = csGraphics::createGraphicContext({size.cx, size.cy});

    HDC dc = rulerDC.dc;
    HBRUSH hbr = CreateSolidBrush(RGB(bkgCol.r, bkgCol.g, bkgCol.b));
    HPEN hpn = CreatePen(0,1,RGB(brdCol.r, brdCol.g, brdCol.b));
    SelectBrush(dc, hbr);
    SelectPen(dc, hpn);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc,RGB(textCol.r,textCol.g,textCol.b));
    
    Rectangle(dc,0,0,size.cx,size.cy);

    DeleteBrush(hbr);
    DeletePen(hpn);

    char str[20];

    int d = dx*DX, od = outdx*DX;
    int n = size.cx/dx, n2 = n*2;
    int N = size.cx/d, N2 = N*2;

    if(orient == CS_RULER_VERTICAL)
    {
        n = size.cy/dx; n2 = n*2;
        N = size.cy/d; N2 = N*2;
    }

    POINT pts[n2];
    POINT pts2[N2];
    unsigned long sz[n], sz2[N];
    POLYTEXTW ppt[N];
    if(orient == CS_RULER_VERTICAL)
    {
        
        hf = CreateFontA(CSUTILS::getAdjustedFontSizeX(fontSize),0,-900, 0,FW_THIN,FALSE,FALSE,FALSE,ANSI_CHARSET,
                          OUT_OUTLINE_PRECIS, CLIP_MASK, DEFAULT_QUALITY,
                          VARIABLE_PITCH, (LPCSTR)"Calibri");
        SelectFont(dc, hf);

        char str[10];
        if(style == CS_RULER_TEXT_UNDER)
        {
            for(int i=0; i<n2; i+=2)
            {
                pts[i] = {0, i*dx};
                pts[i+1] = {l1, i*dx};
                sz[i/2] = 2;
            }
            for(int i=0; i<N2; i+=2)
            {
                int j = i*d;
                pts2[i] = {0, j};
                pts2[i+1] = {l2, j};
                sz2[i/2] = 2;
                sprintf(str,"%.0lf",(double)first + i*od);
                LPSIZE lps = CSUTILS::textExtent(0,hf,str);
                TextOutA(dc,l2+lps->cy,j-lps->cx/2,LPCSTR(str),strlen(str));
                free(lps);
            }
        }
        else
        {
            for(int i=0; i<n2; i+=2)
            {
                pts[i] = {size.cx, i*dx};
                pts[i+1] = {size.cx-l1, i*dx};
                sz[i/2] = 2;
            }
            for(int i=0; i<N2; i+=2)
            {
                int j = i*d;
                pts2[i] = {size.cx, j};
                pts2[i+1] = {size.cx-l2, j};
                sz2[i/2] = 2;

                sprintf(str,"%.0lf",(double)first + i*od);
                LPSIZE lps = CSUTILS::textExtent(0,hf,str);
                TextOutA(dc,lps->cy,j-lps->cx/2,LPCSTR(str),strlen(str));
                free(lps);
            }
        }
        DeleteFont(hf);
        
    }
    else
    {
        hf = CreateFontA(CSUTILS::getAdjustedFontSizeX(fontSize),0,0, 0,FW_THIN,FALSE,FALSE,FALSE,ANSI_CHARSET,
                          OUT_OUTLINE_PRECIS, CLIP_MASK, DEFAULT_QUALITY,
                          VARIABLE_PITCH, (LPCSTR)"Calibri");
        SelectFont(dc, hf);

        if(style == CS_RULER_TEXT_UNDER)
        {
            for(int i=0; i<n2; i+=2)
            {
                pts[i] = {i*dx, 0};
                pts[i+1] = {i*dx, l1};
                sz[i/2] = 2;
            }
            for(int i=0; i<N2; i+=2)
            {
                int j = i*d;
                pts2[i] = {j, 0};
                pts2[i+1] = {j, l2};
                sz2[i/2] = 2;

                sprintf(str,"%.0lf",(double)first + i*od);
                LPSIZE lps = CSUTILS::textExtent(0,hf,str);
                TextOutA(dc,j-lps->cx/2, l2,LPCSTR(str),strlen(str));
                free(lps);
            }
        }
        else
        {
            for(int i=0; i<n2; i+=2)
            {
                pts[i] = {i*dx, size.cy};
                pts[i+1] = {i*dx, size.cy-l1};
                sz[i/2] = 2;
            }
            for(int i=0; i<N2; i+=2)
            {
                int j = i*d;
                pts2[i] = {j, size.cy};
                pts2[i+1] = {j, size.cy-l2};
                sz2[i/2] = 2;

                sprintf(str,"%.0lf",(double)first + i*od);
                LPSIZE lps = CSUTILS::textExtent(0,hf,str);
                TextOutA(dc,j-lps->cx/2, 0,LPCSTR(str),strlen(str));
                free(lps);
            }
        }
        DeleteFont(hf);
    }
    
    hpn = CreatePen(0,1,RGB(linesCol1.r, linesCol1.g, linesCol1.b));
    SelectPen(dc,hpn);
    PolyPolyline(dc,(const POINT*)pts, (const DWORD*)sz, n);
    DeletePen(hpn);

    hpn = CreatePen(0,1,RGB(linesCol2.r, linesCol2.g, linesCol2.b));
    SelectPen(dc,hpn);
    PolyPolyline(dc,(const POINT*)pts2, (const DWORD*)sz2, N);
    DeletePen(hpn);
    
}

extern vector<bool> bltUpdate;

void updatePosition(CSARGS Args)
{
    int id = int(Args);
    CSRULER* ru = (CSRULER*)Args[0];
    
    if(UINT(Args) == WM_TIMER)
    {
        if(ru->xdc != hdcontextExtInPos[id].x || ru->ydc != hdcontextExtInPos[id].y)
        {
            /*POINT* p = ru->getPositionPtr();
            SIZE s = *ru->getSizePtr();
            LONG* inPosX = ru->getInPositionXPtr();
            LONG* inPosY = ru->getInPositionYPtr();
            
            BitBlt(hdStackContext[id], p->x, p->y, s.cx, s.cy, ru->getGContextPtr()->dc, *inPosX, *inPosY, SRCCOPY);
            ru->xdc = hdcontextExtInPos[id].x;
            ru->ydc = hdcontextExtInPos[id].y;*/
            //bltUpdate[id] = 1;

            //cout<<"position: "<<p->x<<", "<<p->y<<" inPositionX: "<<*inPosX<<", inPositionY: "<<*inPosY<<"\n";
        }
    }
}


CSRULER::~CSRULER()
{
    //dtor
}



#include "CSUNIFORMLISTBOX.h"
#include "readwfile.h"

using namespace csGraphics;
using namespace CSUTILS;

extern int TIPS_POPUP;
csLIST<CSUNIFORMLISTBOX*> rbList;

extern vector<HWND> SECTION;
extern vector<RECT> RECTCL;
extern vector<RECT> RECTWND;
extern vector<COLORREF> backgroundColor;
extern vector<COLORREF> borderColor;

extern vector<COLORREF> backgroundColor;
extern vector<COLORREF> borderColor;
extern vector<HDC> hdcontext;
extern vector<HDC> hdBkgContext;
extern vector<HDC> hdcontextExt;
extern vector<POINT> hdcontextExtInPos;
extern vector<SIZE> hdcontextExtSize;
extern vector<HDC> hdStackContext;
extern float geomCoef;

extern vector<float> hZoom;
extern vector<float> vZoom;

extern CSAPP_STRINGS appStrings, appStringsLoaded;
extern bool loadStrings;

void autoReposition(CSARGS Args);
void _mouseMove(CSARGS Args);
void mouseClick(CSARGS Args);
void mouseLeave(CSARGS Args);
void vkeydownSelect(CSARGS Args);
void startCutPaste(CSARGS Args);
void endCutPaste(CSARGS Args);
void makeCutPaste(CSARGS Args);

using namespace CSSTRUTILS;

CSUNIFORMLISTBOX* newUniformlListBoxPtr(int* idp, int _gridWidth, bool gridOrientation, bool translate)
{
    CSUNIFORMLISTBOX rb(idp, _gridWidth, gridOrientation);
    rbList.placeBack(csAlloc<CSUNIFORMLISTBOX>(1, rb));
    return rbList[rbList.size()-1];
}

CSUNIFORMLISTBOX::CSUNIFORMLISTBOX(int* idp, int _gridWidth, bool gridOrientation, bool translate)
{
    init(idp, _gridWidth, gridOrientation, translate);
}

void CSUNIFORMLISTBOX::init(int* idp, int _gridWidth, bool _gridOrientation, bool _translate)
{
    title.init(1);
    pos.init(1);
    posImg.init(1);
    posTitle.init(1);
    color0.init(1);
    color1.init(1);
    color2.init(1);
    color3.init(1);
    color4.init(1);
    bkgcol0.init(1);
    bkgcol1.init(1);
    bkgcol2.init(1);
    bkgcol3.init(1);
    bkgcol4.init(1);
    font.init(1);
    dcs0.init(1);
    dcs1.init(1);
    dcs2.init(1);
    dcs3.init(1);
    dcs4.init(1);
    dfltTitle = 0;
    pdfltSz = 0;
    pmarging = 0;
    poffset = 0;
    pimgSize = 0;
    phdc = 0;
    pgridWidth = 0;
    lastMouseOverid = 0;
    lastMouseClickid = 0;
    n = 0;
    cntActivate = 0;
    vkCtrlCount = 0;
    vkCtrlCountControl = 0;
    vkCtrlPoint = 0;
    vkCtrlGdcPos = 0;
    smoothRepos = 0;
    smoothReposCount = 0;
    cutPasteViewer = 0;
    cutPasteStart = 0;
    cutPasteDone = 0;
    cutPasteDone0 = 0;
    copyPasteKeyDownState = 0;
    click_message = 0;
    extFunc = 0;
    extFuncArgs = 0;

    maxTextWidth = 40;

    translate = _translate;

    fontSize = {15,0};
    dfltFont = CreateFontW(getAdjustedFontSizeX(fontSize.cx),getAdjustedFontSizeY(fontSize.cy),
    0, 0, 0,0,0, 0,0,0,0,0,0, (LPCWSTR)"Calibri");
    dfltColor1 = RGB(250,250,250);
    dfltColor2 = RGB(255,255,255);
    //dfltColor3 = RGB(255,255,100);
    dfltColor3 = RGB(10,10,10);
    CSRGBA col = toRGBA(backgroundColor[*idp]);
    dfltColor4 = RGB(col.r+70,col.g+70,col.b+70);

    dfltBkgCol1 = RGB(col.r,col.g,col.b);
    defcol = col;
    dfltBkgCol2 = RGB(col.r+20,col.g+20,col.b+20);
    //dfltBkgCol3 = RGB(col.r-20,col.g-20,col.b-20);
    dfltBkgCol3 = RGB(200,200,200);
    dfltBkgCol4 = RGB(col.r,col.g,col.b);
    dfltSz = {100,20};
    marging = {1,1};
    offset = {5,5};
    imgSize = {15,15};
    imgdc1 = createGraphicContextResizedFromFileW(L"resources\\img\\combo_12.bmp", {15,15});
    imgdc2 = createGraphicContextResizedFromFileW(L"resources\\img\\combo_12.bmp", {15,15});
    imgdc3 = createGraphicContextResizedFromFileW(L"resources\\img\\combo_1.bmp", {15,15});
    imgdc4 = createGraphicContextResizedFromFileW(L"resources\\img\\combo_1.bmp", {15,15});
    icons.placeBack({imgdc1.dc, imgdc2.dc, imgdc3.dc, imgdc4.dc});
    gridOrientation = _gridOrientation;
    textOrientation = CS_RDBTN_TEXT_H;
    TextPos = CS_RDBTN_TEXT_AFTER;
    gridWidth = _gridWidth;
    parent = idp;

    lastMouseOverid = (int*)malloc(sizeof(int));
    lastMouseClickid = (int*)malloc(sizeof(int));
    *lastMouseOverid = 0;
    *lastMouseClickid = 0;

    dfltTitle = (wchar_t*)malloc(100*sizeof(wchar_t));
    wsprintf(dfltTitle,L"Item");

    pimgSize = (SIZE*)malloc(sizeof(SIZE)); *pimgSize = imgSize;
    n = (int*)malloc(sizeof(int));
    activeChoosed = 0;
    cntActivate = (bool*)malloc(sizeof(bool));
    *cntActivate = true;
    iter = 0;
    animated = 0;

    hdc = hdcontext[*parent];
    phdc = (HDC*)malloc(sizeof(HDC));
    *phdc = hdc;

    smoothRepos = csAlloc<int>(1,1);
    smoothReposCount = csAlloc<int>(1,0);

    vkCtrlCount = (int*)malloc(sizeof(int)); *vkCtrlCount = 0;
    vkCtrlCountControl = (int*)malloc(sizeof(int)); *vkCtrlCountControl = 0;
    vkCtrlPoint = (POINT*)malloc(sizeof(POINT)); *vkCtrlPoint = {0,0};
    vkCtrlGdcPos = (POINT*)malloc(sizeof(POINT)); *vkCtrlGdcPos = {0,0};
    pmarging = (SIZE*)malloc(sizeof(SIZE)); *pmarging = {0,0};
    pdfltSz = (SIZE*)malloc(sizeof(SIZE)); *pdfltSz = {0,0};
    poffset = (SIZE*)malloc(sizeof(SIZE)); *poffset = {0,0};
    pgridWidth = (int*)malloc(sizeof(int)); *pgridWidth = _gridWidth;
    pf = (void*(*)())malloc(sizeof(void (*)()));
    click_message = (bool*)malloc(sizeof(bool)); *click_message = 0;

    zebColor = {255,255,255};
    zebFact = 0.0;

    cutPasteDone0 = 0;
    cutPasteDone = 0;
    cutPasteStart = 0;
    cutPasteViewer = 0;

    rbItem.title=0;

    extFunc = 0;
    extFuncArgs = 0;

    args.init(1);
}

CSUNIFORMLISTBOX::~CSUNIFORMLISTBOX()
{
    //dtor
}

void CSUNIFORMLISTBOX::autoRepos()
{
    *smoothReposCount = 0;
}

bool* CSUNIFORMLISTBOX::getCntState()
{
    return cntActivate;
}
void CSUNIFORMLISTBOX::setCntState(bool activate)
{
    *cntActivate = activate;
}

void CSUNIFORMLISTBOX::setMaxTextWidth(int _maxTextWidth)
{
    maxTextWidth = _maxTextWidth;
}

void CSUNIFORMLISTBOX::setIcon(int idi, wchar_t*path1, wchar_t*path2,wchar_t*path3,wchar_t*path4)
{
    DeleteDC(icons[idi].ic1);
    DeleteDC(icons[idi].ic2);
    DeleteDC(icons[idi].ic3);
    DeleteDC(icons[idi].ic4);
    wchar_t* p2 = path2 != 0 ? path2 : path1;
    wchar_t* p3 = path3 != 0 ? path3 : path1;
    wchar_t* p4 = path4 != 0 ? path4 : path1;
    SIZE size = imgSize;
    imgdc1 = createGraphicContextResizedFromFileW(path1, {size.cx,size.cy});
    imgdc2 = createGraphicContextResizedFromFileW(p2, {size.cx,size.cy});
    imgdc3 = createGraphicContextResizedFromFileW(p3, {size.cx,size.cy});
    imgdc4 = createGraphicContextResizedFromFileW(p4, {size.cx,size.cy});

    DeleteBitmap(imgdc1.hbmp);
    DeleteBitmap(imgdc2.hbmp);
    DeleteBitmap(imgdc3.hbmp);
    DeleteBitmap(imgdc4.hbmp);

    icons[idi].ic1 = imgdc1.dc;
    icons[idi].ic2 = imgdc2.dc;
    icons[idi].ic3 = imgdc3.dc;
    icons[idi].ic4 = imgdc4.dc;

    
    icons[idi].path1 = path1;
    icons[idi].path2 = p2;
    icons[idi].path3 = p3;
    icons[idi].path4 = p4;
}

void CSUNIFORMLISTBOX::newIcon(wchar_t*path1, wchar_t*path2,wchar_t*path3,wchar_t*path4)
{
    icons.placeBack({0});
    setIcon(icons.size()-1, path1, path2, path3, path4);
}

void CSUNIFORMLISTBOX::setIconSize(int idi, SIZE size)
{
    imgSize = size;
    for(int i=0; i<icons.size(); i++)
    {
        setIcon(i, icons[i].path1, icons[i].path2, icons[i].path3, icons[i].path4);
    }
}

void CSUNIFORMLISTBOX::setParent(int* idp)
{
    parent = idp;

}
void CSUNIFORMLISTBOX::setGridWidth(int grdWidth)
{
    gridWidth = grdWidth;
}
void CSUNIFORMLISTBOX::setRect(int id, RECT r)
{
    pos[id] = r;
}
void CSUNIFORMLISTBOX::setTitle(int id, wchar_t* _title)
{
    titleReal[id] = _title;

    free(title[id]); //attention
    LPSIZE sz;
    title[id] = truncateHorizontalTextW(_title,font[id], maxTextWidth, sz);
    free(sz);
}
void CSUNIFORMLISTBOX::setTitles(wchar_t* ids[], wchar_t* _title[], int n)
{
    LPSIZE sz;
    for(int i=0; i<n; i++)
    {
        
        int j = (int)strtod(utf16_to_utf8(ids[i]).c_str(),0);
        titleReal[j] = (wchar_t*)_title[i];

        free(title[j]);
        title[j] = truncateHorizontalTextW(_title[i], font[j], maxTextWidth, sz);
        free(sz);
    }
}
void CSUNIFORMLISTBOX::setFont(int id, wchar_t* fnt, SIZE size)
{
    if(textOrientation == CS_RDBTN_TEXT_H)
        font[id] = CreateFontW(getAdjustedFontSizeX(size.cx),getAdjustedFontSizeY(size.cy), 0, 0, FW_THIN,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
                            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                          DEFAULT_PITCH|FF_DONTCARE, (LPCWSTR)fnt);
    else if(textOrientation == CS_RDBTN_TEXT_V1)
        font[id] = CreateFontW(getAdjustedFontSizeX(size.cx),getAdjustedFontSizeY(size.cy), 900, 0, FW_THIN,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
                            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                          DEFAULT_PITCH|FF_DONTCARE, (LPCWSTR)fnt);
    else if(textOrientation == CS_RDBTN_TEXT_V2)
        font[id] = CreateFontW(getAdjustedFontSizeX(size.cx),getAdjustedFontSizeY(size.cy), -900, 0, FW_THIN,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
                            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                          DEFAULT_PITCH|FF_DONTCARE, (LPCWSTR)fnt);
    fontSize = size;
}

void CSUNIFORMLISTBOX::setDefaultFont(wchar_t* fnt, SIZE size)
{
    DeleteFont(dfltFont);
    if(textOrientation == CS_RDBTN_TEXT_H)
        dfltFont = CreateFontW(getAdjustedFontSizeX(size.cx),getAdjustedFontSizeY(size.cy), 0, 0, FW_THIN,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
                            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                          DEFAULT_PITCH|FF_DONTCARE, (LPCWSTR)fnt);
    else if(textOrientation == CS_RDBTN_TEXT_V1)
        dfltFont = CreateFontW(getAdjustedFontSizeX(size.cx),getAdjustedFontSizeY(size.cy), 900, 0, FW_THIN,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
                            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                          DEFAULT_PITCH|FF_DONTCARE, (LPCWSTR)fnt);
    else if(textOrientation == CS_RDBTN_TEXT_V2)
        dfltFont = CreateFontW(getAdjustedFontSizeX(size.cx),getAdjustedFontSizeY(size.cy), -900, 0, FW_THIN,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
                            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                          DEFAULT_PITCH|FF_DONTCARE, (LPCWSTR)fnt);
    fontSize = size;
}

void CSUNIFORMLISTBOX::zebraStyle(CSRGBA zcol, float zFactor)
{
    zebFact = zFactor;
    zebColor = zcol;
}

void CSUNIFORMLISTBOX::copyPasteStyle()
{
    cutPasteViewer = &TIPS_POPUP;
    //csDetachCnt(*cutPasteViewer);
    cutPasteDone0 = csAlloc<bool>(1,0);
    cutPasteDone = csAlloc<bool>(1,0);
    cutPasteStart = csAlloc<bool>(1,0);
    copyPasteKeyDownState = csAlloc<int>(1,0);
}

void CSUNIFORMLISTBOX::setTitleColors(int id, COLORREF color, COLORREF highlightColor, COLORREF selectionColor)
{
    color1[id] = color;
    color2[id] = highlightColor;
    color3[id] = selectionColor;
}
void CSUNIFORMLISTBOX::setAllTitleColors(COLORREF color, COLORREF highlightColor, COLORREF selectionColor)
{
    for(int i=0; i<color0.size(); i++)
    {
        if(i != *lastMouseClickid)
            color0[i] = color;
        color1[i] = color;
        color2[i] = highlightColor;
        color3[i] = selectionColor;
        updateItem(i);
    }
}


void CSUNIFORMLISTBOX::getItemBackground(int id, COLORREF* color, COLORREF* highlightColor, COLORREF* selectionColor)
{
    *color = bkgcol1[id];
    *highlightColor = bkgcol2[id];
    *selectionColor = bkgcol3[id];
}

void CSUNIFORMLISTBOX::setItemBackground(int id, COLORREF color, COLORREF highlightColor, COLORREF selectionColor)
{
    bkgcol0[id] = color;
    bkgcol1[id] = color;
    bkgcol2[id] = highlightColor;
    bkgcol3[id] = selectionColor;
    updateItem(id);
}

void CSUNIFORMLISTBOX::setMultipleItemsBackground(int*ids, int idsSize, COLORREF color, COLORREF highlightColor, COLORREF selectionColor)
{
    for(int i=0; i<idsSize; i++)
    {
        setItemBackground(ids[i], color, highlightColor, selectionColor);
    }
}

void CSUNIFORMLISTBOX::setMultipleItemsBackground(std::vector<int> ids, COLORREF color, COLORREF highlightColor, COLORREF selectionColor)
{
    int n = ids.size();
    for(int i=0; i<n; i++)
    {
        setItemBackground(ids[i], color, highlightColor, selectionColor);
    }
}

void CSUNIFORMLISTBOX::setItemBackground(wchar_t*_title, COLORREF color, COLORREF highlightColor, COLORREF selectionColor)
{
    int n = title.size();
    for(int i=0; i<n; i++)
    {
        if(wcscmp(titleReal[i],_title) == 0)
        {
            setItemBackground(i, color, highlightColor, selectionColor);
            break;
        }
    }
}

void CSUNIFORMLISTBOX::setMultipleItemsBackground(wchar_t**_titles, int _titlesSize, COLORREF color, COLORREF highlightColor, COLORREF selectionColor)
{
    for(int i=0; i<_titlesSize; i++)
    {
        setItemBackground(_titles[i], color, highlightColor, selectionColor);
    }
}

void CSUNIFORMLISTBOX::setMultipleItemsBackground(std::vector<std::wstring> titles, COLORREF color, COLORREF highlightColor, COLORREF selectionColor)
{
    int n = titles.size();
    for(int i=0; i<n; i++)
    {
        setItemBackground((wchar_t*)titles[i].c_str(), color, highlightColor, selectionColor);
    }
}


void CSUNIFORMLISTBOX::setAllBackgrounds(COLORREF color, COLORREF highlightColor, COLORREF selectionColor)
{
    for(int i=0; i<color0.size(); i++)
    {
        if(i != *lastMouseClickid)
            bkgcol0[i] = color;
        bkgcol1[i] = color;
        bkgcol2[i] = highlightColor;
        bkgcol3[i] = selectionColor;
    }
}

void CSUNIFORMLISTBOX::setDefaultTitleColors(COLORREF color, COLORREF highlightColor, COLORREF selectionColor, COLORREF disableColor)
{
    dfltColor1 = color;
    dfltColor2 = highlightColor;
    dfltColor3 = selectionColor;
    dfltColor4 = disableColor;
    defcol = {color&0xff, color>>8&0xff, color>>16&0xff};
}

void CSUNIFORMLISTBOX::setDefaultBackgroundColors(COLORREF color, COLORREF highlightColor, COLORREF selectionColor, COLORREF disableColor)
{
    dfltBkgCol1 = color;
    dfltBkgCol2 = highlightColor;
    dfltBkgCol3 = selectionColor;
    dfltBkgCol4 = disableColor;
}

void CSUNIFORMLISTBOX::setDefaultSize(SIZE size)
{
    dfltSz = size;
    if(size.cx>=size.cy)
        textOrientation = CS_RDBTN_TEXT_H;
    else
        textOrientation = CS_RDBTN_TEXT_V1;
}

wchar_t* CSUNIFORMLISTBOX::defaultItemTitle()
{
    wchar_t*str = (wchar_t*)malloc(100*sizeof(wchar_t));
    wsprintf(str,L"%s %d", dfltTitle, iter++);
    return str;
}

void CSUNIFORMLISTBOX::setOrientation(int orient)
{
    textOrientation = orient;
}
void CSUNIFORMLISTBOX::setMarging(SIZE _marging)
{
    marging = _marging;
}
void CSUNIFORMLISTBOX::setOffset(SIZE _offset)
{
    offset = _offset;
}

void CSUNIFORMLISTBOX::setGridOrientation(bool orientation)
{
    gridOrientation = orientation;
}

int* CSUNIFORMLISTBOX::getActiveItem()
{
    return lastMouseClickid;
}
int* CSUNIFORMLISTBOX::getMouseOverItem()
{
    return lastMouseOverid;
}

int CSUNIFORMLISTBOX::getId()
{
    return *parent;
}

int CSUNIFORMLISTBOX::getSize()
{
    return *n;
}


void CSUNIFORMLISTBOX::setActiveItem(int id)
{
    if(id < title.size())
    {
        *lastMouseClickid = id;
        //*lastMouseOverid = id; // attention annule le 28/06/2024
        color0[id]=dfltColor3;
        bkgcol0[id]=dfltBkgCol3;
        dcs0[id]=imgdc3.dc;
        activeChoosed = 1;

        /*args.setArg(18, (void*)lastMouseOverid);
        args.setArg(19, (void*)lastMouseClickid);
        args.setHwnd(cv.CNTLIST[*parent]);
        int *par = csAlloc<int>(1,*parent);
        args.setId((void*)par);
        mouseClick(args);*/
    }
}

void CSUNIFORMLISTBOX::setMouseOverItem(int id)
{
    *lastMouseOverid = id;
}


void CSUNIFORMLISTBOX::updateActiveItem(int id)
{
    if(id < title.size())
    {
        *lastMouseOverid = id;
        args.setId(parent);
        args.setHwnd(&SECTION[*parent]);
        mouseClick(args);
    }
}

void CSUNIFORMLISTBOX::updateItem(int id)
{
    if(id>=0 && id<pos.size())
    {
        HDC gdc = hdcontextExt[*parent];
        int i=id;
        SIZE sz;
        CSGRAPHIC_CONTEXT idc;

        sz = {pos[i].right-pos[i].left,pos[i].bottom-pos[i].top};
        idc = createCompatibleGraphicContext(hdc,sz, bkgcol0[i], bkgcol0[i]);
        SetBkMode(idc.dc, TRANSPARENT);
        SelectFont(idc.dc, font[i]);
        SetTextColor(idc.dc, color0[i]);
        TextOutW(idc.dc,posTitle[i].x-pos[i].left,posTitle[i].y-pos[i].top,(LPCWSTR)title[i],wcslen(title[i]));
        BitBlt(idc.dc,posImg[i].x-pos[i].left,posImg[i].y-pos[i].top,imgSize.cx,imgSize.cy, dcs0[i],0,0,SRCCOPY);
        BitBlt(gdc,pos[i].left,pos[i].top,sz.cx,sz.cy, idc.dc,0,0,SRCCOPY);
        releaseGraphicContext(idc);

        RECT r = {pos[i].left*hZoom[*parent], pos[i].top*vZoom[*parent], sz.cx*hZoom[*parent],sz.cy*vZoom[*parent]};
        InvalidateRect(SECTION[*parent],&r,1);
    }
}

void CSUNIFORMLISTBOX::unActivateItem(int id)
{
    if(id < title.size())
    {
        color0[id]=color1[id];
        bkgcol0[id]=bkgcol1[id];
        dcs0[id]=imgdc1.dc;
        activeChoosed = 1;

    }
}

void CSUNIFORMLISTBOX::create()
{
    if(activeChoosed==0)
    {
        setActiveItem(0);
    }
    organize();
    update();
    animate();
}

void CSUNIFORMLISTBOX::copyItem(int id)
{
    rbItem.id = id;
    rbItem.bkgcol0 = bkgcol0[id];
    rbItem.bkgcol1 = bkgcol1[id];
    rbItem.bkgcol2 = bkgcol2[id];
    rbItem.bkgcol3 = bkgcol3[id];
    rbItem.bkgcol4 = bkgcol4[id];

    rbItem.color0 = color0[id];
    rbItem.color1 = color1[id];
    rbItem.color2 = color2[id];
    rbItem.color3 = color3[id];
    rbItem.color4 = color4[id];

    rbItem.dcs0 = dcs0[id];
    rbItem.dcs1 = dcs1[id];
    rbItem.dcs2 = dcs2[id];
    rbItem.dcs3 = dcs3[id];
    rbItem.dcs4 = dcs4[id];

    rbItem.font = font[id];
    /*if(rbItem.title)
    {
        free(rbItem.title);
        rbItem.title = 0;
    }*/
   int l;
    rbItem.titleReal = csAlloc<wchar_t>(l = wcslen(titleReal[id])+1);
    //wsprintf(rbItem.titleReal,L"%s",title[id]);
    wcscpy_s(rbItem.titleReal, l, title[id]);

    if(rbItem.title)
    free(rbItem.title); //attention
    LPSIZE sz;
    rbItem.title = truncateHorizontalTextW(titleReal[id], font[id], maxTextWidth, sz);
    free(sz);

    rbItem.posTitle = posTitle[id];
    rbItem.pos = pos[id];
    rbItem.posImg = posImg[id];

}

bool* CSUNIFORMLISTBOX::getCopyPasteState()
{
    return cutPasteDone;
}

int* CSUNIFORMLISTBOX::getCopyPasteKeyDownState()
{
    return copyPasteKeyDownState;
}

void CSUNIFORMLISTBOX::cutItem(int id)
{
    copyItem(id);
    removeItem(id);
}

void CSUNIFORMLISTBOX::pasteItem(int id)
{

    unActivateItem(*lastMouseClickid);

    bkgcol0.placeAt(rbItem.bkgcol0, id);
    bkgcol1.placeAt(rbItem.bkgcol1, id);
    bkgcol2.placeAt(rbItem.bkgcol2, id);
    bkgcol3.placeAt(rbItem.bkgcol3, id);
    bkgcol4.placeAt(rbItem.bkgcol4, id);

    color0.placeAt(rbItem.color0, id);
    color1.placeAt(rbItem.color1, id);
    color2.placeAt(rbItem.color2, id);
    color3.placeAt(rbItem.color3, id);
    color4.placeAt(rbItem.color4, id);

    dcs0.placeAt(rbItem.dcs0, id);
    dcs1.placeAt(rbItem.dcs1, id);
    dcs2.placeAt(rbItem.dcs2, id);
    dcs3.placeAt(rbItem.dcs3, id);
    dcs4.placeAt(rbItem.dcs4, id);

    font.placeAt(rbItem.font, id);
    titleReal.placeAt(rbItem.titleReal, id);
    title.placeAt(rbItem.title, id);

    posTitle.placeAt(rbItem.posTitle, id);
    pos.placeAt(rbItem.pos, id);
    posImg.placeAt(rbItem.posImg, id);

    setActiveItem(id);

    _pasteFilePath(rbItem.id);
}

void CSUNIFORMLISTBOX::duplicate(int src, int _pos)
{
    color0[_pos] = color1[_pos];
    dcs0[_pos] = dcs1[_pos];
    bkgcol0[_pos] = bkgcol1[_pos];

    unActivateItem(*lastMouseClickid);

    bkgcol0.placeAt(bkgcol0[src], _pos);
    bkgcol1.placeAt(bkgcol1[src], _pos);
    bkgcol2.placeAt(bkgcol2[src], _pos);
    bkgcol3.placeAt(bkgcol3[src], _pos);
    bkgcol4.placeAt(bkgcol4[src], _pos);

    color0.placeAt(color0[src], _pos);
    color1.placeAt(color1[src], _pos);
    color2.placeAt(color2[src], _pos);
    color3.placeAt(color3[src], _pos);
    color4.placeAt(color4[src], _pos);

    dcs0.placeAt(dcs0[src], _pos);
    dcs1.placeAt(dcs1[src], _pos);
    dcs2.placeAt(dcs2[src], _pos);
    dcs3.placeAt(dcs3[src], _pos);
    dcs4.placeAt(dcs4[src], _pos);

    font.placeAt(font[src], _pos);
    titleReal.placeAt(titleReal[src], _pos);
    title.placeAt(title[src], _pos);

    posTitle.placeAt(posTitle[src], _pos);
    pos.placeAt(pos[src], _pos);
    posImg.placeAt(posImg[src], _pos);


    if(zebFact > 0.0)
    {
        float zf = zebFact/2;
        COLORREF col_inact = RGB((defcol.r*(1-zf) + zebColor.r*zf),
                                    (defcol.g*(1-zf) + zebColor.g*zf),
                                    (defcol.b*(1-zf) + zebColor.b*zf));
        COLORREF col = RGB((defcol.r*(1-zebFact) + zebColor.r*zebFact),
                                (defcol.g*(1-zebFact) + zebColor.g*zebFact),
                                (defcol.b*(1-zebFact) + zebColor.b*zebFact));
        *n= color0.size();
        for(int i=_pos; i<*n; i++)
        {
            if(i%2 == 0)
            {
                bkgcol0[i] = col;
                bkgcol1[i] = col;
                bkgcol4[i] = col_inact;
            }
            else
            {
                bkgcol0[i] = dfltBkgCol1;
                bkgcol1[i] = dfltBkgCol1;
                bkgcol4[i] = dfltBkgCol4;
            }
        }
    }

    setActiveItem(_pos);

    _pasteFilePath(src);
}

void CSUNIFORMLISTBOX::moveItem(int from, int to)
{
    //if(from >=0 && to >= 0 && from < *n && to < *n)
    {
        color0[to] = color1[to];
        dcs0[to] = dcs1[to];
        bkgcol0[to] = bkgcol1[to];

        unActivateItem(from);

        color0.move(from, to);
        color1.move(from, to);
        color2.move(from, to);
        color3.move(from, to);
        color4.move(from, to);

        dcs0.move(from, to);
        dcs1.move(from, to);
        dcs2.move(from, to);
        dcs3.move(from, to);
        dcs4.move(from, to);

        font.move(from, to);
        titleReal.move(from, to);
        title.move(from, to);

        posTitle.move(from, to);
        pos.move(from, to);
        posImg.move(from, to);

        if(zebFact == 0)
        {
            bkgcol0.move(from, to);
            bkgcol1.move(from, to);
            bkgcol2.move(from, to);
            bkgcol3.move(from, to);
            bkgcol4.move(from, to);
        }
        setActiveItem(to);

        _moveFilePath(from, to);
    }
}

void CSUNIFORMLISTBOX::removeItem(int id)
{
    if(title.size()>1)
    {
        int n = title.size()-1;
        if(zebFact>0 && id<n)
        {
            for(int i=n; i>id; i--)
            {
                int id1 = i-1;
                bkgcol0[i] = bkgcol0[id1];
                bkgcol1[i] = bkgcol1[id1];
                bkgcol2[i] = bkgcol2[id1];
                bkgcol3[i] = bkgcol3[id1];
            }

        }
        titleReal.deleteAt(id);
        free(title[id]); // attention
        title.deleteAt(id);
        pos.deleteAt(id);
        posImg.deleteAt(id);
        posTitle.deleteAt(id);
        color0.deleteAt(id);
        color1.deleteAt(id);
        color2.deleteAt(id);
        color3.deleteAt(id);
        color4.deleteAt(id);
        bkgcol0.deleteAt(id);
        bkgcol1.deleteAt(id);
        bkgcol2.deleteAt(id);
        bkgcol3.deleteAt(id);
        bkgcol4.deleteAt(id);
        font.deleteAt(id);
        dcs0.deleteAt(id);
        dcs1.deleteAt(id);
        dcs2.deleteAt(id);
        dcs3.deleteAt(id);
        dcs4.deleteAt(id);

        if(title.size()>id)
            setActiveItem(id);
        else
            setActiveItem(id-1);
    }
    else
    {
        titleReal.clear();
        title.clear();
        pos.clear();
        posImg.clear();
        posTitle.clear();
        color0.clear();
        color1.clear();
        color2.clear();
        color3.clear();
        color4.clear();
        bkgcol0.clear();
        bkgcol1.clear();
        bkgcol2.clear();
        bkgcol3.clear();
        bkgcol4.clear();
        font.clear();
        dcs0.clear();
        dcs1.clear();
        dcs2.clear();
        dcs3.clear();
        dcs4.clear();
    }
    _removeFilePath(id);
}

void CSUNIFORMLISTBOX::_removeFilePath(int id)
{
    int n = filePath.size();

    for(int i=0; i<n; i++)
    {
        if(id == filePathID[i])
        {
            free(filePath[i]);
            filePath.deleteAt(i);
            filePathID.deleteAt(i);
        }
    }
}

void CSUNIFORMLISTBOX::_pasteFilePath(int id)
{
    int n = filePath.size();

    for(int i=0; i<n; i++)
    {
        if(id == filePathID[i])
        {
            int l;
            wchar_t* str = csAlloc<wchar_t>(l=wcslen(filePath[i])+1);
            wcscpy_s(str, l, filePath[i]);
            filePath.placeBack(str);
            filePathID.placeBack(id);
        }
    }
}

void CSUNIFORMLISTBOX::_moveFilePath(int from, int to)
{
    int n = filePath.size();

    for(int i=0; i<n; i++)
    {
        if(from == filePathID[i])
        {
            filePathID[i] = to;
        }
    }
}

void CSUNIFORMLISTBOX::setDefaultTitle(wchar_t* _dfltTitle)
{
    int l = wcslen(_dfltTitle);
    if(l < 80)
        wsprintf(dfltTitle,L"%s",_dfltTitle);
    else
        wcscpy_s(dfltTitle,80,_dfltTitle);
}

void CSUNIFORMLISTBOX::newFilePath(wchar_t* filter, unsigned iconId)
{
    OPENFILENAMEW ofn = {};
    wchar_t szFile[1024] = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = SECTION[*parent];
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    if (GetOpenFileNameW(&ofn))
    {
        int n = wcslen(szFile);
        wchar_t* p = szFile + n + 1;
        if (*p == '\0')
        {
            filePath.placeBack(makeWString(szFile));
            csLIST<wchar_t> chlist = filePath.toListW(filePath.size()-1);
            int i = chlist.findLast('\\');
            if(i > -1)
            {
                wchar_t* str = chlist.toStringW(i+1,chlist.size()-1);
                newItem(str,1,iconId);
                free(str);
            }
            chlist.clear();
        }
        else
        {
            std::wstring folder = szFile;
            while (*p)
            {
                std::wstring fullPath = folder + L"\\" + p;
                filePath.placeBack(makeWString(fullPath.c_str()));
                filePathID.placeBack(title.size());
                newItem(p,1,iconId);
                p += wcslen(p) + 1;
            }
        }
    }
}

void CSUNIFORMLISTBOX::newItem(wchar_t*_title,int n, unsigned iconId)
{
    wchar_t*str = _title;
    for(int i=0; i<n; i++)
    {
        if(!_title)
        {
            str = defaultItemTitle();
        }

        if(loadStrings) str = *appStrings.asd[appStrings.count++].viewedText;
        
        titleReal.placeBack(str);
        if(translate) appStrings.newString(&titleReal[titleReal.size()-1]);

        LPSIZE sz;
        title.placeBack(truncateHorizontalTextW(str, dfltFont, maxTextWidth, sz));
        free(sz);

        pos.placeBack({0});
        posImg.placeBack({0});
        posTitle.placeBack({0});
        color0.placeBack(dfltColor1);
        color1.placeBack(dfltColor1);
        color2.placeBack(dfltColor2);
        color3.placeBack(dfltColor3);
        color4.placeBack(dfltColor4);
        if(zebFact == 0.0)
        {
            bkgcol0.placeBack(dfltBkgCol1);
            bkgcol1.placeBack(dfltBkgCol1);
            bkgcol4.placeBack(dfltBkgCol4);
        }
        else
        {
            if(color0.size()%2 == 1)
            {
                COLORREF col = RGB((defcol.r*(1-zebFact) + zebColor.r*zebFact),
                                   (defcol.g*(1-zebFact) + zebColor.g*zebFact),
                                   (defcol.b*(1-zebFact) + zebColor.b*zebFact));
                bkgcol0.placeBack(col);
                bkgcol1.placeBack(col);
                float zf = zebFact/2;
                bkgcol4.placeBack(RGB((defcol.r*(1-zf) + zebColor.r*zf),
                                   (defcol.g*(1-zf) + zebColor.g*zf),
                                   (defcol.b*(1-zf) + zebColor.b*zf)));
            }
            else
            {
                bkgcol0.placeBack(dfltBkgCol1);
                bkgcol1.placeBack(dfltBkgCol1);
                bkgcol4.placeBack(dfltBkgCol4);
            }
        }
        bkgcol2.placeBack(dfltBkgCol2);
        bkgcol3.placeBack(dfltBkgCol3);
        font.placeBack(dfltFont);
        
        dcs0.placeBack(icons[iconId].ic1);
        dcs1.placeBack(icons[iconId].ic1);
        dcs2.placeBack(icons[iconId].ic2);
        dcs3.placeBack(icons[iconId].ic3);
        dcs4.placeBack(icons[iconId].ic4);

        unActivateItem(*lastMouseClickid);
        setActiveItem(title.size()-1);
        //activeChoosed = 0;

    }
}

void CSUNIFORMLISTBOX::organize()
{

    //HDC dc = hdcontextExt[*parent];
    //SetBkMode(dc, TRANSPARENT);
    if(TextPos == CS_RDBTN_TEXT_AFTER)
    {
        int ximg, yimg, xstr, ystr, xof = offset.cx, yof = offset.cy, j, k, N=gridWidth;
        cxmax = 0; cymax = 0;
        bool b = 0;
        for(int i=0; i<title.size(); i++)
        {
            k = i/N;
            j = i%N;
            /*if(k==0)
                xof = 0;
            else
                xof = offset.cx;
            if(j==0)
                yof = 0;
            else
                yof = offset.cy;*/
            int a, b;
            if(!gridOrientation)
            {   a = (dfltSz.cx + xof)*j; b = (dfltSz.cy + yof)*k; }
            else
            {   a = (dfltSz.cx + xof)*k; b = (dfltSz.cy + yof)*j;   }
            
            ximg = marging.cx + a;
            yimg = marging.cy + b + (dfltSz.cy - imgdc1.sz.cy)/2;

            xstr = ximg + imgdc1.sz.cx + 5;
            
            if(textOrientation == CS_RDBTN_TEXT_H)
            {
                if(!fontSize.cy)
                    ystr = marging.cy + b + (dfltSz.cy-getAdjustedFontSizeX(fontSize.cx))/2;
                else
                    ystr = marging.cy + b + (dfltSz.cy-getAdjustedFontSizeY(fontSize.cy))/2;
            }
            else if(textOrientation == CS_RDBTN_TEXT_V1)
                ystr = marging.cy + b ;//+ dfltSz.cy/2;
            else if(textOrientation == CS_RDBTN_TEXT_V2)
            {
                //LPSIZE lps = csTextExtent(*parent, font[i], title[i]);
                ystr = marging.cy - b - dfltSz.cy/2;
            }

            pos[i] = {ximg,marging.cy+b, ximg+dfltSz.cx, marging.cy+b+dfltSz.cy};
            posImg[i] = {ximg,yimg};
            posTitle[i] = {xstr, ystr};
            
            cxmax = cxmax < ximg ? ximg : cxmax;
            cymax = cymax < yimg ? yimg : cymax;

            /*drawGDIRectangle(dc,bkgcol0[i],bkgcol0[i],1,pos[i]);
            SelectFont(dc, font[i]);
            SetTextColor(dc, color0[i]);
            TextOutW(dc,xstr,ystr,(LPCWSTR)title[i],wcslen(title[i]));
            BitBlt(dc,ximg,yimg,imgdc1.sz.cx,imgdc1.sz.cy, dcs0[i],0,0,SRCCOPY);*/
        }
        cxmax += dfltSz.cx;
        cymax += dfltSz.cy;
        if(hdcontextExtSize[*parent].cy < cymax || hdcontextExtSize[*parent].cx < cxmax)
        {
            if(hdcontextExtSize[*parent].cx < cxmax)
                setGraphicAreaXSize(*parent, cxmax);
            if(hdcontextExtSize[*parent].cy < cymax)
                setGraphicAreaYSize(*parent, cymax);

            updateGraphicArea(*parent, 1);
            *phdc = hdcontextExt[*parent];
        }
    }
    SendMessage(SECTION[*parent], WM_ERASEBKGND,0,0);

}

extern vector<int> withHScroll;
extern vector<int> withVScroll;

void organize2(int parent, int n, SIZE imgSize,
                RECT*pos,POINT*posImg,POINT*posTitle, HDC* dcs0, HFONT* font, COLORREF* color0, COLORREF* bkgcol0, wchar_t**title, HDC hdc,
                int cxmax, int cymax)
{

    //updateGraphicArea(parent, 1);
    HDC dc = hdcontextExt[parent];
    POINT p = {0,0};
//hdc = dc;
    SetBkMode(dc, TRANSPARENT);
    for(int i=0; i<n; i++)
    {
        SelectFont(dc, font[i]);
        SetTextColor(dc, color0[i]);
        drawGDIRectangle(dc,bkgcol0[i],bkgcol0[i],1,pos[i]);
        TextOutW(dc,posTitle[i].x+p.x,posTitle[i].y+p.y,(LPCWSTR)title[i],wcslen(title[i]));
        BitBlt(dc,posImg[i].x+p.x,posImg[i].y+p.y,imgSize.cx,imgSize.cy, dcs0[i],0,0,SRCCOPY);
    }

    int cx = cxmax*hZoom[parent], cy = cymax*vZoom[parent];
    cx = cx < RECTCL[parent].right ? cx : RECTCL[parent].right - 10*geomCoef*(withHScroll[parent] > 0 ? 1 : 0)  /* scrollBar default width*/;
    cy = cy < RECTCL[parent].bottom ? cy : RECTCL[parent].bottom - 10*geomCoef*(withVScroll[parent] > 0 ? 1 : 0) /* scrollBar default width*/;
    RECT r = {pos[0].left*hZoom[parent], pos[0].top*vZoom[parent], cx, cy};
    InvalidateRect(SECTION[parent],&r,0);
}


void colorTracking(int parent, int lastItem, int currentItem, int n, SIZE imgSize,
                RECT*pos,POINT*posImg,POINT*posTitle, HDC* dcs0, HFONT* font,
                 COLORREF* color0, COLORREF* bkgcol0, wchar_t**title, HDC hdc, int cxmax, int cymax)
{

    HDC gdc = hdcontextExt[parent];
    int i=lastItem;
    SIZE sz;
    CSGRAPHIC_CONTEXT idc;

    if(i>=0 && i<n) // new modif
    {
        sz = {pos[i].right-pos[i].left,pos[i].bottom-pos[i].top};
        idc = createCompatibleGraphicContext(hdc,sz, bkgcol0[i], bkgcol0[i]);
        SetBkMode(idc.dc, TRANSPARENT);
        SelectFont(idc.dc, font[i]);
        SetTextColor(idc.dc, color0[i]);
        TextOutW(idc.dc,posTitle[i].x-pos[i].left,posTitle[i].y-pos[i].top,(LPCWSTR)title[i],wcslen(title[i]));
        BitBlt(idc.dc,posImg[i].x-pos[i].left,posImg[i].y-pos[i].top,imgSize.cx,imgSize.cy, dcs0[i],0,0,SRCCOPY);
        BitBlt(gdc,pos[i].left,pos[i].top,sz.cx,sz.cy, idc.dc,0,0,SRCCOPY);
        releaseGraphicContext(idc);
    }

    i = currentItem;
    sz = {pos[i].right-pos[i].left,pos[i].bottom-pos[i].top};
    idc = createCompatibleGraphicContext(hdc,sz, bkgcol0[i], bkgcol0[i]);
    SetBkMode(idc.dc, TRANSPARENT);
    SelectFont(idc.dc, font[i]);
    SetTextColor(idc.dc, color0[i]);
    TextOutW(idc.dc,posTitle[i].x-pos[i].left,posTitle[i].y-pos[i].top,(LPCWSTR)title[i],wcslen(title[i]));
    BitBlt(idc.dc,posImg[i].x-pos[i].left,posImg[i].y-pos[i].top,imgSize.cx,imgSize.cy, dcs0[i],0,0,SRCCOPY);
    BitBlt(gdc,pos[i].left,pos[i].top,sz.cx,sz.cy, idc.dc,0,0,SRCCOPY);
    
    int cx = cxmax*hZoom[parent], cy = cymax*vZoom[parent];
    cx = cx < RECTCL[parent].right ? cx : RECTCL[parent].right - 10*geomCoef*(withHScroll[parent] > 0 ? 1 : 0) /* scrollBar default width*/;
    cy = cy < RECTCL[parent].bottom ? cy : RECTCL[parent].bottom - 10*geomCoef*(withVScroll[parent] > 0 ? 1 : 0) /* scrollBar default width*/;
    RECT r = {pos[0].left*hZoom[parent], pos[0].top*vZoom[parent], cx, cy};
    InvalidateRect(SECTION[parent],&r,0);
    releaseGraphicContext(idc);
}


void catchEvents(CSARGS Args);

extern vector<vector<void(*)(CSARGS)>> GROUPED_EVENTS_FUNC;
extern vector<vector<CSARGS>> GROUPED_EVENTS_ARGS;

void CSUNIFORMLISTBOX::animate()
{
    n = csAlloc(1,pos.size());
    pimgSize = csAlloc(1,imgSize);
    pmarging = csAlloc(1,marging);
    pdfltSz = csAlloc(1,dfltSz);
    poffset = csAlloc(1,offset);
    pgridWidth = csAlloc(1,gridWidth);
    int* pcxmax = csAlloc(1, cxmax);
    int* pcymax = csAlloc(1, cymax);

    args.clear();
    args.setArgNumber(45);
    args.regArg(dcs0.getData(), dcs1.getData(),dcs2.getData(),dcs3.getData(),
                 color0.getData(), color1.getData(),color2.getData(),color3.getData(),
                 bkgcol0.getData(), bkgcol1.getData(),bkgcol2.getData(),bkgcol3.getData(),
                 pos.getData(), posImg.getData(), posTitle.getData(),
                 font.getData(), title.getData(), pimgSize,
        lastMouseOverid, lastMouseClickid, n, &colorTracking, cntActivate, phdc,
        vkCtrlCount, vkCtrlCountControl, vkCtrlPoint, vkCtrlGdcPos, pmarging, pdfltSz,
        poffset, pgridWidth, smoothRepos, smoothReposCount,
        cutPasteViewer, cutPasteStart, cutPasteDone, cutPasteDone0,copyPasteKeyDownState,click_message, pcxmax, pcymax,
        &gridOrientation, extFunc, extFuncArgs);
    

    if(!animated)
    {
        groupMsgPos = CSSECMAN::addAction(*parent, catchEvents, 45, dcs0.getData(), dcs1.getData(),dcs2.getData(),dcs3.getData(),
                 color0.getData(), color1.getData(),color2.getData(),color3.getData(),
                 bkgcol0.getData(), bkgcol1.getData(),bkgcol2.getData(),bkgcol3.getData(),
                 pos.getData(), posImg.getData(), posTitle.getData(),
                 font.getData(), title.getData(), pimgSize,
        lastMouseOverid, lastMouseClickid, n, &colorTracking, cntActivate, phdc,
        vkCtrlCount, vkCtrlCountControl, vkCtrlPoint, vkCtrlGdcPos, pmarging, pdfltSz,
        poffset, pgridWidth, smoothRepos, smoothReposCount,
        cutPasteViewer, cutPasteStart, cutPasteDone, cutPasteDone0,copyPasteKeyDownState,click_message, pcxmax, pcymax,
        &gridOrientation, extFunc, extFuncArgs);
        
        animated = 1;
    }
    else
    {
        GROUPED_EVENTS_ARGS[*parent][groupMsgPos] = args;
    }

}

int CSUNIFORMLISTBOX::getEventsGroupId()
{
    return groupMsgPos;
}

void CSUNIFORMLISTBOX::hide()
{
    CSSECMAN::catchEventsGroup(*parent, groupMsgPos, 0);
    CSSECMAN::updateSection(*parent);
}
void CSUNIFORMLISTBOX::show()
{
    CSSECMAN::catchEventsGroup(*parent, groupMsgPos, 1);
    CSSECMAN::updateSection(*parent);
}

RECT CSUNIFORMLISTBOX::getItemRect(int id)
{
    return pos[id];
}
RECT* CSUNIFORMLISTBOX::getItemRectPtr(int id)
{
    return &pos[id];
}
void CSUNIFORMLISTBOX::addExternalFunction(void(*_extFunc)(CSPARAARGS), CSPARAARGS* _extFuncArgs)
{
    extFunc = _extFunc;
    extFuncArgs = _extFuncArgs;
}

void CSUNIFORMLISTBOX::update()
{

    if(*cntActivate)
    {
        organize2(*parent, pos.size(), imgSize,
            pos.getData(),posImg.getData(),posTitle.getData(),
            dcs1.getData(), font.getData(), color1.getData(),bkgcol1.getData(), title.getData(), hdc, cxmax, cymax);

        if(font.size())
        {
            POINT p = {0,0};
            int i = *lastMouseClickid;
            HDC dc = hdcontextExt[*parent];
            SelectFont(dc, font[i]);
            SetTextColor(dc, color3[i]);
            drawGDIRectangle(dc,bkgcol3[i],bkgcol3[i],1,pos[i]);
            TextOutW(dc,posTitle[i].x+p.x,posTitle[i].y+p.y,(LPCWSTR)title[i],wcslen(title[i]));
            BitBlt(dc,posImg[i].x+p.x,posImg[i].y+p.y,imgSize.cx,imgSize.cy, dcs3[i],0,0,SRCCOPY);
        }
    }
    else
    {
        organize2(*parent, pos.size(), imgSize,
            pos.getData(),posImg.getData(),posTitle.getData(),
            dcs4.getData(), font.getData(), color4.getData(),bkgcol4.getData(), title.getData(), hdc, cxmax, cymax);
    }
    //InvalidateRect(SECTION[*parent], 0,1);
    //SendMessage(SECTION[*parent],WM_ERASEBKGND,0,0);
}


void catchEvents(CSARGS Args)
{
    UINT msg = (UINT)Args;

    if(msg == WM_MOUSEMOVE) _mouseMove(Args); // contient makeCutPaste()
    else if(msg == WM_LBUTTONDOWN) mouseClick(Args); // contient startCutPaste()
    else if(msg == WM_LBUTTONUP) endCutPaste(Args);
    else if(msg == WM_MOUSELEAVE) mouseLeave(Args);
    else if(msg == WM_KEYDOWN) vkeydownSelect(Args);
    //else if(msg == WM_TIMER) autoReposition(Args);

    void(*extFunc)(CSPARAARGS) = (void(*)(CSPARAARGS))Args[43];
    if(extFunc)
    {  
        CSPARAARGS* pArgs = (CSPARAARGS*)Args[44];
        pArgs->setProcParams((HWND)Args, (UINT)Args, (WPARAM)Args, (LPARAM)Args, (int)Args);
        extFunc(*pArgs);
    }
}

extern POINT TIMER_POINT;

void _mouseMove(CSARGS Args)
{
    bool cntState = *(bool*)Args[22];
    int id = int(Args);
    short g = GetAsyncKeyState(VK_LBUTTON); // evite un bug
    POINT dcpos = {-hdcontextExtInPos[id].x,-hdcontextExtInPos[id].y};
    POINT ptm = TIMER_POINT;
    ScreenToClient(HWND(Args), &ptm);
    ptm.x -= csGraphics::getGraphicAreaXPos(id);
    ptm.y -= csGraphics::getGraphicAreaYPos(id);
    int cxmax = *(int*)Args[40];
    int cymax = *(int*)Args[41];
    RECT rr = {dcpos.x, dcpos.y, 
                cxmax*hZoom[id], cymax*vZoom[id]};
    //if(cntState && (!g || (g && PtInRect(&rr,ptm)))) 
    
    if(cntState && PtInRect(&rr,ptm)) 
    //if(cntState && (!g || (g && PtInRect(&RECTWND[id],TIMER_POINT))))
    {
        HDC* dcs0=(HDC*)Args[0];
        HDC* dcs1=(HDC*)Args[1];
        HDC* dcs2=(HDC*)Args[2];
        HDC* dcs3=(HDC*)Args[3];
        COLORREF* color0=(COLORREF*)Args[4];
        COLORREF* color1=(COLORREF*)Args[5];
        COLORREF* color2=(COLORREF*)Args[6];
        COLORREF* color3=(COLORREF*)Args[7];
        COLORREF* bkgcol0=(COLORREF*)Args[8];
        COLORREF* bkgcol1=(COLORREF*)Args[9];
        COLORREF* bkgcol2=(COLORREF*)Args[10];
        COLORREF* bkgcol3=(COLORREF*)Args[11];
        RECT* pos=(RECT*)Args[12];
        POINT* posImg=(POINT*)Args[13];
        POINT* posTitle=(POINT*)Args[14];
        HFONT* font=(HFONT*)Args[15];
        wchar_t** title=(wchar_t**)Args[16];
        SIZE imgSize = *(SIZE*)Args[17];
        int* lastMouseOverid = (int*)Args[18];
        int* lastMouseClickid = (int*)Args[19];
        int n = *(int*)Args[20];
        HDC parentdc = *(HDC*)Args[23];

        int *vkCtrlCount = (int*)Args[24];
        int *vkCtrlCountControl = (int*)Args[25];
        POINT vkCtrlPoint = *(POINT*)Args[26];
        POINT vkCtrlGdcPos = *(POINT*)Args[27];
        SIZE marging = *(SIZE*)Args[28];
        SIZE dfltSz = *(SIZE*)Args[29];
        SIZE offset = *(SIZE*)Args[30];
        int width = *(int*)Args[31];
        bool gridOrientation = *(bool*)Args[42];

        POINT p={0,0}, pvk;
        GetCursorPos(&p);
        pvk = p;

        ScreenToClient(HWND(Args), &p);
        p.x -= csGraphics::getGraphicAreaXPos(id);
        p.y -= csGraphics::getGraphicAreaYPos(id);

        p.x /= hZoom[id];
        p.y /= vZoom[id];

        if(!PtInRect(&RECTWND[id], pvk))
        {
            SendMessage(HWND(Args), WM_LBUTTONUP,0,0); // resolution du bug cause par wm_keydown
        }
        else
        {if(*vkCtrlCount == 0)
        {
            

            if(n>0)
            {
                int lastId = *lastMouseOverid;
                float jf = (p.x - dcpos.x - marging.cx)/float(dfltSz.cx + offset.cx);
                float kf = (p.y - dcpos.y - marging.cy)/float(dfltSz.cy + offset.cy);

                int j = ((jf - int(jf))*(dfltSz.cx + offset.cx) <= dfltSz.cx)?int(jf):-1;
                int k = ((kf - int(kf))*(dfltSz.cy + offset.cy) <= dfltSz.cy)?int(kf):-1;

                if(k == -1 || j == -1)
                    return;
                    
                int i = k*width + j;
                if(gridOrientation) 
                {
                    i = j*width + k;
                    if(k >= width) return;
                }
                else
                {
                    i = k*width + j;
                    if(j >= width) return;
                } 
                //cout << "listbox -> item i: " << i << " width: " << width << endl;

                if(i>=0 && i<n) // new modif
                {
                    if(*lastMouseClickid != lastId && lastId >= 0 && lastId < n)
                    {
                        color0[lastId] = color1[lastId];
                        bkgcol0[lastId] = bkgcol1[lastId];
                        dcs0[lastId] = dcs1[lastId];
                    }

                    if(i != *lastMouseClickid)
                    {
                        color0[i] = color2[i];
                        bkgcol0[i] = bkgcol2[i];
                        dcs0[i] = dcs2[i];
                    }
                    if(*lastMouseOverid != i)
                    {
                        //std::cout<<"\n lastMouseOverid = "<<lastId<<" , i = "<<i<<"   ----------\n";
                        ((void(*)(int,int,int,int,SIZE, RECT*,POINT*,POINT*, HDC*, HFONT*, COLORREF*, COLORREF*, wchar_t**, HDC, int, int))((void*)Args[21]))(
                            id,lastId, i,n, imgSize, pos, posImg, posTitle, dcs0, font, color0, bkgcol0, title, parentdc, cxmax, cymax);
                        //*lastMouseOverid = i;
                    }
                }
                else
                    SendMessage(SECTION[id],WM_MOUSELEAVE,0,0);
                *lastMouseOverid = i;
            }
        }
        /*else if(*vkCtrlCount == 2)
        {
            int dx = pvk.x - vkCtrlPoint.x;
            int dy = pvk.y - vkCtrlPoint.y;

            int DX = RECTWND[id].right - vkCtrlPoint.x;
            int DY = RECTWND[id].bottom - vkCtrlPoint.y;

            int CX = cv.cp[id]->DCSizes.width - cv.CNTCLWRECT[id].right;
            int CY = cv.cp[id]->DCSizes.height - cv.CNTCLWRECT[id].bottom;

            int cx = -dx*CX*2/DX;
            int cy = -dy*CY*2/DY;
            POINT pt = {vkCtrlGdcPos.x+cx, vkCtrlGdcPos.y+cy};

            if(pt.x>0) pt.x = 0;
            if(pt.y>0) pt.y=0;
            if(pt.x<-CX) pt.x = cx;
            if(pt.y<-CY) pt.y = cy;
            //std::cout<<pt.x<<" , "<<pt.y<<"\n";
            cv.CTX[id].setDwgSurfPos(pt);
            InvalidateRect((HWND)Args,0,1);

        }*/
        }



        makeCutPaste(Args);
    }
}


void mouseClick(CSARGS Args)
{
    bool cntState = *(bool*)Args[22];
    
    if(cntState)
    {
        int id = (int)Args;
        HDC* dcs0=(HDC*)Args[0];
        HDC* dcs1=(HDC*)Args[1];
        HDC* dcs2=(HDC*)Args[2];
        HDC* dcs3=(HDC*)Args[3];
        COLORREF* color0=(COLORREF*)Args[4];
        COLORREF* color1=(COLORREF*)Args[5];
        COLORREF* color2=(COLORREF*)Args[6];
        COLORREF* color3=(COLORREF*)Args[7];
        COLORREF* bkgcol0=(COLORREF*)Args[8];
        COLORREF* bkgcol1=(COLORREF*)Args[9];
        COLORREF* bkgcol2=(COLORREF*)Args[10];
        COLORREF* bkgcol3=(COLORREF*)Args[11];
        RECT* pos=(RECT*)Args[12];
        POINT* posImg=(POINT*)Args[13];
        POINT* posTitle=(POINT*)Args[14];
        HFONT* font=(HFONT*)Args[15];
        wchar_t** title=(wchar_t**)Args[16];
        SIZE imgSize = *(SIZE*)Args[17];
        int* lastMouseOverid = (int*)Args[18];
        int* lastMouseClickid = (int*)Args[19];
        int n = *(int*)Args[20];
        HDC parentdc = *(HDC*)Args[23];
        bool*click_message = (bool*)Args[39];


        int* smoothReposCount = (int*)Args[33];
        int cxmax = *(int*)Args[40];
        int cymax = *(int*)Args[41];

        if(n>0 && *lastMouseOverid >=0 &&  *lastMouseOverid < n && *lastMouseClickid >= 0 && *lastMouseClickid < n)
        {
            int i = *lastMouseOverid, j = *lastMouseClickid;
            if(i!=j)
            {
                color0[i] = color3[i];
                bkgcol0[i] = bkgcol3[i];
                dcs0[i] = dcs3[i];
                //std::cout<<"\n j = "<<j<<" , i = "<<i<<"   ----------\n";
                //std::cout<<"\n idparent = "<<id<<"   ----------\n";
                dcs0[j] = dcs1[j];
                color0[j] = color1[j];
                bkgcol0[j] = bkgcol1[j];
                *lastMouseClickid = i;
                ((void(*)(int,int,int,int,SIZE, RECT*,POINT*,POINT*, HDC*, HFONT*, COLORREF*, COLORREF*, wchar_t**, HDC, int, int))((void*)Args[21]))(
                                id,j, i,n, imgSize, pos, posImg, posTitle, dcs0, font, color0, bkgcol0, title, parentdc, cxmax, cymax);

                *click_message = 1;
            }
        }

        //void (*f)() = (void(*)())((void*)Args[11]);
        //f();
        int *vkCtrlCount = (int*)Args[24];
        int *vkCtrlCountControl = (int*)Args[25];
        *vkCtrlCount = 0;
        *vkCtrlCountControl = 0;

        *smoothReposCount = 0;

        startCutPaste(Args);

    }
}

void mouseLeave(CSARGS Args)
{
    bool cntState = *(bool*)Args[22];
    if(cntState)
    {
        int id = (int)Args;
        HDC* dcs0=(HDC*)Args[0];
        HDC* dcs1=(HDC*)Args[1];
        HDC* dcs2=(HDC*)Args[2];
        HDC* dcs3=(HDC*)Args[3];
        COLORREF* color0=(COLORREF*)Args[4];
        COLORREF* color1=(COLORREF*)Args[5];
        COLORREF* color2=(COLORREF*)Args[6];
        COLORREF* color3=(COLORREF*)Args[7];
        COLORREF* bkgcol0=(COLORREF*)Args[8];
        COLORREF* bkgcol1=(COLORREF*)Args[9];
        COLORREF* bkgcol2=(COLORREF*)Args[10];
        COLORREF* bkgcol3=(COLORREF*)Args[11];
        RECT* pos=(RECT*)Args[12];
        POINT* posImg=(POINT*)Args[13];
        POINT* posTitle=(POINT*)Args[14];
        HFONT* font=(HFONT*)Args[15];
        wchar_t** title=(wchar_t**)Args[16];
        SIZE imgSize = *(SIZE*)Args[17];
        int* lastMouseOverid = (int*)Args[18];
        int* lastMouseClickid = (int*)Args[19];
        int n = *(int*)Args[20];
        HDC parentdc = *(HDC*)Args[23];
        int cxmax = *(int*)Args[40];
        int cymax = *(int*)Args[41];

        if(*lastMouseOverid != *lastMouseClickid && n>0 && *lastMouseOverid < n && *lastMouseClickid < n
            && *lastMouseOverid >= 0 && *lastMouseClickid >= 0)
        {
            int i = *lastMouseOverid;
            color0[i] = color1[i];
            bkgcol0[i] = bkgcol1[i];
            dcs0[i] = dcs1[i];
            ((void(*)(int,int,int,int,SIZE, RECT*,POINT*,POINT*, HDC*, HFONT*, COLORREF*, COLORREF*, wchar_t**, HDC, int, int))((void*)Args[21]))(
                            id,*lastMouseClickid, i,n, imgSize, pos, posImg, posTitle, dcs0, font, color0, bkgcol0, title, parentdc, cxmax, cymax);
            *lastMouseOverid=*lastMouseClickid;
        }

        //void (*f)() = (void(*)())((void*)Args[11]);
        //f();
        int *vkCtrlCount = (int*)Args[24];
        int *vkCtrlCountControl = (int*)Args[25];
        *vkCtrlCount = 0;
        *vkCtrlCountControl = 0;

        int *cutPasteViewer = (int*)Args[34];
        if(cutPasteViewer)
        {
            if(IsWindowVisible(SECTION[*cutPasteViewer]))
            {
                bool*cutPasteStart = (bool*)Args[35];
                bool*cutPasteDone = (bool*)Args[36];
                bool*cutPasteDone0 = (bool*)Args[37];

                ShowWindow(SECTION[*cutPasteViewer], 0);
                *cutPasteStart = 0;

                if(*cutPasteDone0)
                {
                    *cutPasteDone0 = 0;
                    *cutPasteDone = 1;
                }
           }
        }

    }
}

void autoReposition(CSARGS Args)
{
    /*bool cntState = *(bool*)Args[22];
    if(cntState)
    {

        int id = (int)Args;
        RECT* pos=(RECT*)Args[12];
        int lastMouseOverid = *(int*)Args[18];
        int lastMouseClickid = *(int*)Args[19];
        int n = *(int*)Args[20];
        int* smoothRepos = (int*)Args[32];
        int* smoothReposCount = (int*)Args[33];

        if(*smoothReposCount < *smoothRepos)
        {
            if(lastMouseClickid >= 0 && lastMouseClickid < n && n>0)
            {
                POINT p = cv.CTX[id].getDwgSurfPos();
                if(pos[lastMouseClickid].right+p.x > cv.CNTCLWRECT[id].right)
                {
                    int dx = (pos[lastMouseClickid].right+p.x - cv.CNTCLWRECT[id].right);
                    if(pos[lastMouseClickid].left+p.x - dx > 0)
                    {
                        int a = dx/(*smoothRepos);
                        if(a == 0) a = dx;
                        p.x -= a;
                        cv.CTX[id].setDwgSurfPos(p);
                        InvalidateRect(HWND(Args),0,1);
                    }
                }
                else if(pos[lastMouseClickid].left+p.x < 0)
                {
                    int dx = pos[lastMouseClickid].left+p.x;
                    int a = dx/(*smoothRepos);
                    if(a == 0) a = dx;
                    p.x -= a;
                    cv.CTX[id].setDwgSurfPos(p);
                    InvalidateRect(HWND(Args),0,1);
                }
                else if(pos[n-1].right+p.x < cv.CNTCLWRECT[id].right)
                {
                    int dx = -(p.x+pos[n-1].right - cv.CNTCLWRECT[id].right);
                    if(dx + p.x <= 0)
                    {
                        int a = dx/(*smoothRepos);
                        if(a == 0) a = dx;
                        p.x += a;
                        cv.CTX[id].setDwgSurfPos(p);
                        InvalidateRect(HWND(Args),0,1);
                    }
                    else if(p.x < 0)
                    {
                        cv.CTX[id].setDwgSurfPos({0,p.y});
                        InvalidateRect(HWND(Args),0,1);
                    }
                }

                if(pos[lastMouseClickid].bottom+p.y > cv.CNTCLWRECT[id].bottom)
                {
                    int dy = (pos[lastMouseClickid].bottom+p.y - cv.CNTCLWRECT[id].bottom);
                    if(pos[lastMouseClickid].top+p.y - dy > 0)
                    {
                        int a = dy/(*smoothRepos);
                        if(a == 0) a = dy;
                        p.y -= a;
                        cv.CTX[id].setDwgSurfPos(p);
                        InvalidateRect(HWND(Args),0,1);
                    }
                }
                else if(pos[lastMouseClickid].top+p.y < 0)
                {
                    int dy = pos[lastMouseClickid].top+p.y;
                    int a = dy/(*smoothRepos);
                    if(a == 0) a = dy;
                    p.y -= a;
                    cv.CTX[id].setDwgSurfPos(p);
                    InvalidateRect(HWND(Args),0,1);
                }

                else if(pos[n-1].bottom+p.y < cv.CNTCLWRECT[id].bottom)
                {
                    int dy = -(p.y+pos[n-1].bottom - cv.CNTCLWRECT[id].bottom);
                    if(dy + p.y <= 0)
                    {
                        int a = dy/(*smoothRepos);
                        if(a == 0) a = dy;
                        p.y += a;
                        cv.CTX[id].setDwgSurfPos(p);
                        InvalidateRect(HWND(Args),0,1);
                    }
                    else if(p.y < 0)
                    {
                        cv.CTX[id].setDwgSurfPos({p.x,0});
                        InvalidateRect(HWND(Args),0,1);
                    }
                }

            }
            *smoothReposCount += 1;
        }
    }*/
}


void vkeydownSelect(CSARGS Args)
{
    bool cntState = *(bool*)Args[22];
    if(cntState)
    {
        HDC* dcs0=(HDC*)Args[0];
        HDC* dcs3=(HDC*)Args[3];
        COLORREF* color0=(COLORREF*)Args[4];
        COLORREF* color3=(COLORREF*)Args[7];
        COLORREF* bkgcol0=(COLORREF*)Args[8];
        COLORREF* bkgcol3=(COLORREF*)Args[11];

        int id = (int)Args;
        int *count = (int*)Args[24];
        int *countControl = (int*)Args[25];

        int* lastMouseOverid = (int*)Args[18];
        int* lastMouseClickid = (int*)Args[19];
        int n = *(int*)Args[20];
        int* smoothRepos = (int*)Args[32];
        int width = *(int*)Args[31];
        int* smoothReposCount = (int*)Args[33];

        WPARAM wp = WPARAM(Args);
        if(n > 0)
        {
            if(wp == VK_UP && *lastMouseClickid > 0 && *lastMouseClickid < n)
            {
                int id  = *lastMouseClickid-1;
                color0[id] = color3[id];
                bkgcol0[id] = bkgcol3[id];
                dcs0[id] = dcs3[id];
                *lastMouseOverid = id;
                *smoothReposCount = 1;
                SendMessage(HWND(Args), WM_LBUTTONDOWN, 0,0);
                SendMessage(HWND(Args), WM_LBUTTONUP, 0,0);
            }
            else if(wp == VK_DOWN && *lastMouseClickid+1 < n)
            {
                int id  = *lastMouseClickid+1;
                color0[id] = color3[id];
                bkgcol0[id] = bkgcol3[id];
                dcs0[id] = dcs3[id];
                *lastMouseOverid = id;
                *smoothReposCount = 1;
                SendMessage(HWND(Args), WM_LBUTTONDOWN, 0,0);
                SendMessage(HWND(Args), WM_LBUTTONUP, 0,0);
            }
            else if(wp == VK_LEFT && *lastMouseClickid-n/width >= 0)
            {
                int id  = *lastMouseClickid-n/width;
                color0[id] = color3[id];
                bkgcol0[id] = bkgcol3[id];
                dcs0[id] = dcs3[id];
                *lastMouseOverid = id;
                *smoothReposCount = 1;
                SendMessage(HWND(Args), WM_LBUTTONDOWN, 0,0);
            }
            else if(wp == VK_RIGHT && *lastMouseClickid+n/width < n)
            {
                int id  = *lastMouseClickid+n/width;
                color0[id] = color3[id];
                bkgcol0[id] = bkgcol3[id];
                dcs0[id] = dcs3[id];
                *lastMouseOverid = id;
                *smoothReposCount = 1;
                SendMessage(HWND(Args), WM_LBUTTONDOWN, 0,0);
            }
        }
    }
}

void startCutPaste(CSARGS Args)
{
    bool cntState = *(bool*)Args[22];
    if(cntState)
    {

        if((int*)Args[34])
        {

            bool*cutPasteStart = (bool*)Args[35];
            //int cutPasteViewer = *(int*)Args[34];
            bool*cutPasteDone0 = (bool*)Args[37];

            *cutPasteStart = 1;
            *cutPasteDone0 = 0;
        }
    }
}

void endCutPaste(CSARGS Args)
{
    bool cntState = *(bool*)Args[22];
    if(cntState)
    {

        if((int*)Args[34])
        {
            bool*cutPasteStart = (bool*)Args[35];
            bool*cutPasteDone = (bool*)Args[36];
            bool*cutPasteDone0 = (bool*)Args[37];
            int cutPasteViewer = *(int*)Args[34];
    //std::cout<<"6656456\n";
            ShowWindow(SECTION[cutPasteViewer], 0);
            *cutPasteStart = 0;

            if(*cutPasteDone0)
            {
                *cutPasteDone0 = 0;
                *cutPasteDone = 1;
            }
            /*int* lastMouseOverid = (int*)Args[18];
            std::cout<<"lastMouseOverid : "<<*lastMouseOverid<<" --\n";*/

        }
    }
}

void makeCutPaste(CSARGS Args)
{
    bool cntState = *(bool*)Args[22];
    if(cntState)
    {
        static bool a = 0;
        int n = *(int*)Args[20];
        if((int*)Args[34] && n)
        {
            a = 0;
            int* copyPasteKeyDownState = (int*)Args[38];
            if(GetAsyncKeyState(VK_LSHIFT))
            {
                *copyPasteKeyDownState = VK_LSHIFT;
                a = 1;
            }
            else if(GetAsyncKeyState(VK_LBUTTON))
            {
                *copyPasteKeyDownState = VK_LBUTTON;
                a = 1;
            }

            if(a)
            {
                bool*cutPasteStart = (bool*)Args[35];
                int cutPasteViewer = *(int*)Args[34];

                if(*cutPasteStart)
                {
                    bool*cutPasteDone0 = (bool*)Args[37];
                    ShowWindow(SECTION[cutPasteViewer], 0);
                    int* lastMouseClickid = (int*)Args[19];
                    RECT pos=((RECT*)Args[12])[*lastMouseClickid];
                    POINT p = TIMER_POINT;

                    MoveWindow(SECTION[cutPasteViewer],p.x+20,p.y+20, pos.right-pos.left, pos.bottom-pos.top,1);
                    GetClientRect(SECTION[cutPasteViewer], &RECTCL[cutPasteViewer]);

                    HDC* dcs3=(HDC*)Args[3];
                    COLORREF* color3=(COLORREF*)Args[7];
                    COLORREF* bkgcol3=(COLORREF*)Args[11];

                    POINT* posImg=(POINT*)Args[13];
                    POINT* posTitle=(POINT*)Args[14];
                    HFONT* font=(HFONT*)Args[15];
                    wchar_t** title=(wchar_t**)Args[16];
                    SIZE imgSize = *(SIZE*)Args[17];

                    //RECT r = cv.CNTCLWRECT[int(Args)];
                    RECT r = RECTCL[int(Args)];

                    int i = *lastMouseClickid;
                    HDC dc = hdcontextExt[cutPasteViewer];
                    SetBkMode(dc,TRANSPARENT);
                    SelectFont(dc, font[i]);
                    SetTextColor(dc, color3[i]);
                    drawGDIRectangle(dc,bkgcol3[i],bkgcol3[i],1,{0,0,pos.right-pos.left, pos.bottom-pos.top});
                    TextOutW(dc,posTitle[i].x-posImg[i].x,(r.bottom-imgSize.cy)/2,(LPCWSTR)title[i],wcslen(title[i]));
                    BitBlt(dc,1,(r.bottom-imgSize.cy)/2,imgSize.cx,imgSize.cy, dcs3[i],0,0,SRCCOPY);
                    PostMessage(SECTION[cutPasteViewer],WM_ERASEBKGND,0,0);
                    //InvalidateRect(SECTION[cutPasteViewer],0,1);
                    *cutPasteStart = 0;
                    *cutPasteDone0 = 1;
                }
                //std::cout<<"lastMouseOverid : "<<*lastMouseOverid<<"\n";
                POINT p;
                GetCursorPos(&p);
                SetWindowPos(SECTION[cutPasteViewer],0,p.x+20,p.y+20, 0,0,SWP_NOSIZE);
            }
        }
    }
}


//------------------------------------------------------------------

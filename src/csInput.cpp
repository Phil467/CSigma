#include "csInput.h"
#include "csUtils.h"
#include "csStrUtils.h"
#include "csUIObjects.h"

using namespace csGraphics;
using namespace CSSTRUTILS;
using namespace CSUTILS;
using namespace CSSECMAN;

extern HINSTANCE Hinst;

extern vector<vector<bool>> HIDEGROUPMSG;
extern vector<HCURSOR> cursor;
extern int CURSOR_NCHITTEST_POS;
extern POINT TIMER_POINT;
extern vector<SIZE> hdcSize;
extern vector<COLORREF> backgroundColor;
extern vector<COLORREF> borderColor;
extern vector<vector<CSARGS>> GROUPED_EVENTS_ARGS;

typedef CSLINEAR CSL;

RECT csRectContextualized(RECT r, RECT rparent);
CSINPUT::CSINPUT()
{
    // Constructeur par défaut - les membres seront initialisés par init()
}

CSINPUT::CSINPUT(int*idp)
{
    init(idp);

}

void CSINPUT::updateStackDC()
{
    csGraphics::releaseGraphicContext(stackDC);
    parentDC = csGraphics::getGraphicAreaContext(*parent);
    SIZE sz = csGraphics::getGraphicAreaSize(*parent);
    stackDC = csGraphics::createCompatibleGraphicContext(parentDC,
                            sz, backgroundColor[*parent], borderColor[*parent]);
}

void CSINPUT::init(int*idp)
{
    parent = idp;
    groupId = HIDEGROUPMSG[*idp].size();
    //HIDEGROUPMSG[*idp].push_back(0);
    updateStackDC();

    xmarging = 3;
    ymarging = 3;
    titleFont = CreateFontW(14, 0, 0, 1,FW_BOLD,1, 0,0,0,0,0,0,0, (LPCWSTR)"Book Antiqua");
    textFont = CreateFontW(14, 0, 0, 1,0,0, 0,0,0,0,0,0,0, (LPCWSTR)"Arial");
    checkCount = 0;
    startChecking = 0;
    ctxRect = {INT_MAX, INT_MAX, 0, 0};
}

void CSINPUT::__getEventsGroup()
{
    groupMsgPos = GROUPED_EVENTS_ARGS[*parent].size()-1;
}

void CSINPUT::hide()
{
    CSSECMAN::catchEventsGroup(*parent, groupMsgPos, 0);
    CSSECMAN::updateSection(*parent);
}
void CSINPUT::show()
{
    CSSECMAN::catchEventsGroup(*parent, groupMsgPos, 1);
    CSSECMAN::updateSection(*parent);
}

void CSINPUT::newInput(wchar_t*title, wchar_t*defaultText, RECT geometry, int style, int titleRectWidth, int titleRectHeight)
{
    CSINPUT::CSINPUT_PARAMS ipp;

    ipp.textChar.init(2,15);
    ipp.textCharGeometry.init(2,15);
    ipp.buttonMarging = 2;
    ipp.textRectEnd = geometry.right + geometry.left - xmarging;
    ipp.title = makeWString(title);
    ipp.defaultText = makeWString(defaultText);
    ipp.style = style;
    ipp.rect = {geometry.left, geometry.top, ipp.textRectEnd + xmarging, geometry.bottom+geometry.top};
    ipp.rsize = {geometry.right, geometry.bottom};
    ipp.titleRectWidth = titleRectWidth;
    ipp.titleRectHeight = titleRectHeight;
    ipp.charHeight = (geometry.bottom-2*ymarging)-3;
    
    ipp.titleFont = CreateFontW(ipp.charHeight, 0, 0, 10,FW_MEDIUM,0, 0,0,0,0,0,0,0, (LPCWSTR)"Book Antiqua");
    ipp.textFont = CreateFontW(ipp.charHeight, 0, 0, 10,FW_MEDIUM,0, 0,0,0,0,0,0,0, (LPCWSTR)"Arial");

    //cout<<"how \n";

    wchar_t* str = L"A\0";
    LPSIZE lps = textExtentW(*parent, ipp.textFont, str);
    ipp.charWidth = lps->cx;
    ipp.charHeight = lps->cy;
    delete lps;

    ipp.titleFont = titleFont;
    ipp.textDc = CreateCompatibleDC(parentDC);

    RECT r = ipp.rect;
    int bmarg = ipp.buttonMarging;
    switch(ipp.style)
    {
        case 0:
            ipp.titleRect = {r.left + xmarging, r.top + ymarging, r.left+xmarging+titleRectWidth, r.bottom-ymarging};
            ipp.textRect = {ipp.titleRect.right + bmarg, r.top + ymarging, ipp.textRectEnd, r.bottom-ymarging};
            break;
        case 1:
            ipp.titleRect = {r.left + xmarging, r.top + ymarging, r.left+xmarging+titleRectWidth, r.top+ymarging+titleRectHeight};
            ipp.textRect = {r.left + xmarging, ipp.titleRect.bottom + bmarg, ipp.textRectEnd, r.bottom-ymarging};
            break;
    }

    ipp.buttonWidth = ipp.textRect.bottom-ipp.textRect.top;
    ipp.textBmp = CreateCompatibleBitmap(parentDC, ipp.charHeight-2, ipp.charHeight-2);
    ip.push_back(new CSINPUT_PARAMS(ipp));
    createContext(ip.size()-1);

    ctxRect = {std::min(ctxRect.left, r.left), std::min(ctxRect.top, r.top), 
               std::max(ctxRect.right, r.right), std::max(ctxRect.bottom, r.bottom)};
}

void CSINPUT::textCharGeometryCorrection(int from)
{
    csLIST<RECT>& r = ip[idActive]->textCharGeometry;

    int n = r.size();
    int d;
    for(int i=from; i<n; i++)
    {
        d = r[i].right - r[i].left;
        r[i].left = r[i-1].right;
        r[i].right = r[i].left + d;
    }
}

void CSINPUT::addChar(wchar_t chr)
{

    CSINPUT::CSINPUT_PARAMS* inp = ip[idActive];
    HDC& hdc = ip[idActive]->cntDc;

    int pos = ip[idActive]->caretPos;

    wchar_t _chr[2];
    wsprintf(_chr,L"%c\0", chr);
    LPSIZE lps = textExtentW(*parent, inp->textFont, _chr);
    lps->cx += inp->charSpacing;
    inp->charWidth = lps->cx;
    
    RECT r = pos > 0 ? inp->textCharGeometry[pos-1] : (RECT){2,0,2,0};
    inp->textCharGeometry.insertAt({r.right, r.top, r.right+lps->cx, lps->cy}, pos);
    textCharGeometryCorrection(pos+1);
    inp->textChar.insertAt(chr, pos);
    ip[idActive]->caretPos++;
    
    delete lps;

    DeleteDC(inp->textDc);
    DeleteBitmap(inp->textBmp);
    inp->textDcWidth += inp->charWidth;
    inp->textDc = CreateCompatibleDC(parentDC);
    inp->textBmp = CreateCompatibleBitmap(parentDC, inp->textDcWidth,  inp->charHeight);

    SelectBitmap(inp->textDc, inp->textBmp);
    SelectFont(inp->textDc, inp->textFont);
    //RECT rt = inp->textRect;
    //drawGdiRectangle(inp->textDc, inp->bkgColor1, inp->borderColor1, {0,0,inp->textDcWidth,inp->charHeight});

    SetTextColor(inp->textDc, RGB(inp->textColor1.r,inp->textColor1.g,inp->textColor1.b));
    SetBkColor(inp->textDc,RGB(inp->bkgColor1.r,inp->bkgColor1.g,inp->bkgColor1.b));
    //SetBkMode(inp->textDc, TRANSPARENT);

    wchar_t*str = inp->textChar.toStringW(0,inp->textChar.size()-1);

    SetTextCharacterExtra(inp->textDc, inp->charSpacing);
    TextOutW(inp->textDc, 0, 0, str, wcslen(str));
    //wcout<<str<<" \n";
    free(str);

    //section to view in textRect
    updateText(idActive);

}

void CSINPUT::deleteChar(bool frontCaret)
{

    CSINPUT::CSINPUT_PARAMS* inp = ip[idActive];
    HDC& hdc = ip[idActive]->cntDc;
    wchar_t _chr[2];
    RECT rchar;
    if(frontCaret)
    {
        int pos = ip[idActive]->caretPos-1;

        RECT r = inp->textCharGeometry[pos];
        rchar = r;
        inp->charWidth = r.right-r.left;

        inp->textChar.deleteAt(pos);
        inp->textCharGeometry.deleteAt(pos);
        if(pos>0)
        {
            inp->activeCharRect = getFrontEndCharRect(idActive, pos-1);
        }
        textCharGeometryCorrection(pos);
        ip[idActive]->caretPos--;
        inp->textDcWidth -= inp->charWidth;// must be here !!

    }
    else
    {
        int pos = ip[idActive]->caretPos;
        RECT r = inp->textCharGeometry[pos];
        rchar = r;
        inp->charWidth = r.right-r.left;

        inp->textChar.deleteAt(pos);
        inp->textCharGeometry.deleteAt(pos);
        inp->activeCharRect = getFrontEndCharRect(idActive, pos-1);
        textCharGeometryCorrection(pos);
        
        //ip[idActive]->caretPos--;

        inp->textDcWidth -= inp->charWidth;// must be here !!
    }
    if(inp->textChar.size())
    {
        DeleteDC(inp->textDc);
        DeleteBitmap(inp->textBmp);
        inp->textDc = CreateCompatibleDC(parentDC);
        inp->textBmp = CreateCompatibleBitmap(parentDC, inp->textDcWidth,  inp->charHeight);

        SelectBitmap(inp->textDc, inp->textBmp);
        SelectFont(inp->textDc, inp->textFont);
        //RECT rt = inp->textRect;
        //drawGdiRectangle(inp->textDc, inp->bkgColor1, inp->borderColor1, {0,0,inp->textDcWidth,inp->charHeight});
    
        SetTextColor(inp->textDc, RGB(inp->textColor1.r,inp->textColor1.g,inp->textColor1.b));
        SetBkColor(inp->textDc,RGB(inp->bkgColor1.r,inp->bkgColor1.g,inp->bkgColor1.b));
        //SetBkMode(inp->textDc, TRANSPARENT);

        wchar_t*str = inp->textChar.toStringW(0,inp->textChar.size()-1);
        
        //updateActiveTextBkg();
        SetTextCharacterExtra(inp->textDc, inp->charSpacing);
        TextOutW(inp->textDc, 0, 0, str, wcslen(str));


        free(str);
        //inp->textChar.printAll(L"text : ");
        

        RECT r = inp->rect;
        SIZE s = inp->rsize;
        BitBlt(parentDC, r.left, r.top, s.cx, s.cy, hdc, 0,0,SRCCOPY);

        r = inp->textRect;
        BitBlt(parentDC, r.left+2, r.top, r.right-r.left-4, r.bottom-r.top, inp->textDc, 0,0,SRCCOPY);
        //InvalidateRect(sHandle(*parent), &inp->textRect, 1);

        updateText(idActive);
    }
    else
    {
        updateActiveTextBkg(); // erase last wchar_t on the textDc
    }
}

void CSINPUT::updateText(int idInput)
{
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];

    int cx = inp->textRect.right - inp->textRect.left - 4;
    
    RECT r = inp->textRect;
    //POINT p = {csGraphics::getGraphicAreaInXPos(*parent), csGraphics::getGraphicAreaInYPos(*parent)};
    POINT p = {0,0};
    int delta = inp->textDcWidth - cx;
    if(delta < 0) 
    {
        BitBlt(parentDC, r.left+2+p.x, r.top+p.y, inp->textDcWidth, inp->charHeight, inp->textDc, 0,0, SRCCOPY);
        inp->showedSectionRect = {0,0,inp->textDcWidth, inp->charHeight};
    }
    else
    {
        BitBlt(parentDC, r.left+2+p.x, r.top+p.y, inp->textDcWidth, inp->charHeight, inp->textDc, delta,0, SRCCOPY);
        inp->showedSectionRect = {delta,0, inp->textDcWidth, inp->charHeight};
    }
}

void CSINPUT::highlightActiveChar(RECT r, bool state)
{
    
    CSINPUT::CSINPUT_PARAMS* inp = ip[idActive];
    //if(r.right < inp->textRect.right && r.left > inp->textRect.left)
    {
        //r = getFrontEndCharRect(idActive, i);
        int left = r.left;
        if(left > inp->textRect.right || left < inp->textRect.left)
            return;

        wchar_t _str[2];
        updateText(idActive);
        updateText(idLastActive);
        SelectFont(parentDC, inp->textFont);
        inp->activeCharRect = r;
        
        if(!state)
        {
            SetTextColor(parentDC, RGB(inp->activeCharColor.r,inp->activeCharColor.g,inp->activeCharColor.b));
            SetBkColor(parentDC, RGB(inp->activeCharBkgColor.r,inp->activeCharBkgColor.g,inp->activeCharBkgColor.b));
        }
        else
        {
            SetTextColor(parentDC, RGB(inp->textColor1.r,inp->textColor1.g,inp->textColor1.b));
            SetBkColor(parentDC, RGB(inp->bkgColor1.r,inp->bkgColor1.g,inp->bkgColor1.b));
        }

        //RECT r1;
        if(inp->caretPos > 1)
        {
            int i = inp->caretPos-1;
            wsprintf(_str, L"%c\0", inp->textChar[i]);
            TextOutW(parentDC, left-2, inp->textRect.top, _str, 1);
            //r1 = inp->activeCharRect;
        }

        RECT r2;
        if(inp->caretPos < inp->textChar.size())
        {
            int i = inp->caretPos;
            wsprintf(_str, L"%c\0", inp->textChar[i]);
            TextOutW(parentDC, r.right-2, inp->textRect.top, _str, 1);
            r2 = inp->activeCharRect;
        }
        RECT rh = {left-3, inp->textRect.top, r2.right+r2.left, inp->textRect.bottom};
        InvalidateRect(sHandle(*parent), &rh, 1);
    }
    
}

void CSINPUT::addIncrementButtons(int _idInput, wchar_t*path11, wchar_t*path12, wchar_t*path13, wchar_t*path14,
                                  wchar_t*path21, wchar_t*path22, wchar_t*path23, wchar_t*path24)
{
    int idInput = (_idInput < 0) ? ip.size()+_idInput : _idInput;
    
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];

    RECT r = inp->rect;
    int titleRectWidth = inp->titleRectWidth;
    int titleRectHeight = inp->titleRectHeight;
    int textRectEnd = inp->textRectEnd;
    int buttonWidth = inp->buttonWidth;
    int bmarg = inp->buttonMarging;
    int a = buttonWidth/2 - 1;
    switch(inp->style)
    {
        case 0:
            inp->textRectEnd -= (bmarg + a);
            inp->titleRect = {r.left + xmarging, r.top + ymarging, r.left+xmarging+titleRectWidth, r.bottom-ymarging};
            inp->textRect = {inp->titleRect.right + bmarg, r.top + ymarging, inp->textRectEnd, r.bottom-ymarging};
            inp->upRect = {inp->textRectEnd + bmarg, r.top + ymarging, inp->textRectEnd + bmarg + a, r.top + ymarging + a};
            inp->downRect = inp->upRect;
            inp->downRect.top += a+2;
            inp->downRect.bottom += a+2;
            break;
        case 1:
            inp->textRectEnd -= (bmarg + a);
            inp->titleRect = {r.left + xmarging, r.top + ymarging, r.left+xmarging+titleRectWidth, r.top+ymarging+titleRectHeight};
            inp->textRect = {r.left + xmarging, inp->titleRect.bottom + bmarg, inp->textRectEnd, r.bottom-ymarging};
            inp->upRect = {inp->textRectEnd + bmarg, inp->titleRect.bottom + bmarg, inp->textRectEnd + bmarg + a, r.top + ymarging + a};
            inp->downRect = inp->upRect;
            inp->downRect.top += a+2;
            inp->downRect.bottom += a+2;
            break;
    }

    wchar_t* p12 = (path12 == 0) ? path11 : path12;
    wchar_t* p13 = (path13 == 0) ? path11 : path13;
    wchar_t* p14 = (path14 == 0) ? path11 : path14;

    wchar_t* p22 = (path22 == 0) ? path21 : path22;
    wchar_t* p23 = (path23 == 0) ? path21 : path23;
    wchar_t* p24 = (path24 == 0) ? path21 : path24;
    
    SIZE sz = {inp->upRect.right-inp->upRect.left,inp->upRect.bottom-inp->upRect.top};
    
    inp->incUpImage1 = createGraphicContextResizedFromFileW(path11, sz);
    inp->incUpImage2 = createGraphicContextResizedFromFileW(p12, sz);
    inp->incUpImage3 = createGraphicContextResizedFromFileW(p13, sz);
    inp->incUpImage4 = createGraphicContextResizedFromFileW(p14, sz);
    
    inp->incDownImage1 = createGraphicContextResizedFromFileW(path21, sz);
    inp->incDownImage2 = createGraphicContextResizedFromFileW(p22, sz);
    inp->incDownImage3 = createGraphicContextResizedFromFileW(p23, sz);
    inp->incDownImage4 = createGraphicContextResizedFromFileW(p24, sz);

}

void CSINPUT::addUndoRedoButtons(int _idInput, wchar_t*path11, wchar_t*path12, wchar_t*path13, wchar_t*path14,
                                  wchar_t*path21, wchar_t*path22, wchar_t*path23, wchar_t*path24)
{
    int idInput = (_idInput < 0) ? ip.size()+_idInput : _idInput;

    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];

    RECT r = inp->rect;
    int titleRectWidth = inp->titleRectWidth;
    int titleRectHeight = inp->titleRectHeight;
    int textRectEnd = inp->textRectEnd;
    int buttonWidth = inp->buttonWidth;
    int bmarg = inp->buttonMarging;
    int a = buttonWidth;
    switch(inp->style)
    {
        case 0:
            inp->textRectEnd -= 2*(bmarg + a);
            inp->titleRect = {r.left + xmarging, r.top + ymarging, r.left+xmarging+titleRectWidth, r.bottom-ymarging};
            inp->textRect = {inp->titleRect.right + bmarg, r.top + ymarging, inp->textRectEnd, r.bottom-ymarging};
            inp->undoRect = {inp->textRectEnd + bmarg, r.top + ymarging, inp->textRectEnd + bmarg + a, r.top + ymarging + a};
            inp->redoRect = inp->undoRect;
            inp->redoRect.left += a+bmarg;
            inp->redoRect.right += a+bmarg;
            break;
        case 1:
            inp->textRectEnd -= (bmarg + a);
            inp->titleRect = {r.left + xmarging, r.top + ymarging, r.left+xmarging+titleRectWidth, r.top+ymarging+titleRectHeight};
            inp->textRect = {r.left + xmarging, inp->titleRect.bottom + bmarg, inp->textRectEnd, r.bottom-ymarging};
            inp->undoRect = {inp->textRectEnd + bmarg, inp->titleRect.bottom + bmarg, inp->textRectEnd + bmarg + a, r.top + ymarging + a};
            inp->redoRect = inp->undoRect;
            inp->redoRect.left += a+bmarg;
            inp->redoRect.right += a+bmarg;
            break;
    }

    wchar_t* p12 = (path12 == 0) ? path11 : path12;
    wchar_t* p13 = (path13 == 0) ? path11 : path13;
    wchar_t* p14 = (path14 == 0) ? path11 : path14;

    wchar_t* p22 = (path22 == 0) ? path21 : path22;
    wchar_t* p23 = (path23 == 0) ? path21 : path23;
    wchar_t* p24 = (path24 == 0) ? path21 : path24;
    
    SIZE sz = {inp->undoRect.right-inp->undoRect.left,inp->undoRect.bottom-inp->undoRect.top};
    
    inp->undoImage1 = createGraphicContextResizedFromFileW(path11, sz);
    inp->undoImage2 = createGraphicContextResizedFromFileW(p12, sz);
    inp->undoImage3 = createGraphicContextResizedFromFileW(p13, sz);
    inp->undoImage4 = createGraphicContextResizedFromFileW(p14, sz);
    
    inp->redoImage1 = createGraphicContextResizedFromFileW(path21, sz);
    inp->redoImage2 = createGraphicContextResizedFromFileW(p22, sz);
    inp->redoImage3 = createGraphicContextResizedFromFileW(p23, sz);
    inp->redoImage4 = createGraphicContextResizedFromFileW(p24, sz);
}

void CSINPUT::addUnrollButton(int _idInput, wchar_t*path11, wchar_t*path12, wchar_t*path13, wchar_t*path14)
{
    int idInput = (_idInput < 0) ? ip.size()+_idInput : _idInput;

    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];

    RECT r = inp->rect;
    int titleRectWidth = inp->titleRectWidth;
    int titleRectHeight = inp->titleRectHeight;
    int textRectEnd = inp->textRectEnd;
    int buttonWidth = inp->buttonWidth;
    int bmarg = inp->buttonMarging;

    switch(inp->style)
    {
        case 0:
            inp->textRectEnd -= (bmarg + buttonWidth);
            inp->titleRect = {r.left + xmarging, r.top + ymarging, r.left+xmarging+titleRectWidth, r.bottom-ymarging};
            inp->textRect = {inp->titleRect.right + bmarg, r.top + ymarging, inp->textRectEnd, r.bottom-ymarging};
            inp->unrollRect = {inp->textRectEnd + bmarg, r.top + ymarging, inp->textRectEnd + bmarg + buttonWidth, r.top + ymarging + buttonWidth};
            break;
        case 1:
            inp->textRectEnd -= (bmarg + buttonWidth);
            inp->titleRect = {r.left + xmarging, r.top + ymarging, r.left+xmarging+titleRectWidth, r.top+ymarging+titleRectHeight};
            inp->textRect = {r.left + xmarging, inp->titleRect.bottom + bmarg, inp->textRectEnd, r.bottom-ymarging};
            inp->unrollRect = {inp->textRectEnd + bmarg, inp->titleRect.bottom + bmarg, inp->textRectEnd + bmarg + buttonWidth, r.top + ymarging + buttonWidth};
            break;
    }

    wchar_t* p12 = (path12 == 0) ? path11 : path12;
    wchar_t* p13 = (path13 == 0) ? path11 : path13;
    wchar_t* p14 = (path14 == 0) ? path11 : path14;
    
    SIZE sz = {inp->unrollRect.right-inp->unrollRect.left,inp->unrollRect.bottom-inp->unrollRect.top};
    
    inp->unrollImage1 = createGraphicContextResizedFromFileW(path11, sz);
    inp->unrollImage2 = createGraphicContextResizedFromFileW(p12, sz);
    inp->unrollImage3 = createGraphicContextResizedFromFileW(p13, sz);
    inp->unrollImage4 = createGraphicContextResizedFromFileW(p14, sz);
    
}

void CSINPUT::addOpenFolderButton(int _idInput, wchar_t*path11, wchar_t*path12, wchar_t*path13, wchar_t*path14)
{
    int idInput = (_idInput < 0) ? ip.size()+_idInput : _idInput;

    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];

    RECT r = inp->rect;
    int titleRectWidth = inp->titleRectWidth;
    int titleRectHeight = inp->titleRectHeight;
    int textRectEnd = inp->textRectEnd;
    int buttonWidth = inp->buttonWidth;
    int bmarg = inp->buttonMarging;

    switch(inp->style)
    {
        case 0:
            inp->textRectEnd -= (bmarg + buttonWidth);
            inp->titleRect = {r.left + xmarging, r.top + ymarging, r.left+xmarging+titleRectWidth, r.bottom-ymarging};
            inp->textRect = {inp->titleRect.right + bmarg, r.top + ymarging, inp->textRectEnd, r.bottom-ymarging};
            inp->openFolderRect = {inp->textRectEnd + bmarg, r.top + ymarging, inp->textRectEnd + bmarg + buttonWidth, r.top + ymarging + buttonWidth};
            break;
        case 1:
            inp->textRectEnd -= (bmarg + buttonWidth);
            inp->titleRect = {r.left + xmarging, r.top + ymarging, r.left+xmarging+titleRectWidth, r.top+ymarging+titleRectHeight};
            inp->textRect = {r.left + xmarging, inp->titleRect.bottom + bmarg, inp->textRectEnd, r.bottom-ymarging};
            inp->openFolderRect = {inp->textRectEnd + bmarg, inp->titleRect.bottom + bmarg, inp->textRectEnd + bmarg + buttonWidth, r.top + ymarging + buttonWidth};
            break;
    }

    wchar_t* p12 = (path12 == 0) ? path11 : path12;
    wchar_t* p13 = (path13 == 0) ? path11 : path13;
    wchar_t* p14 = (path14 == 0) ? path11 : path14;
    
    SIZE sz = {inp->openFolderRect.right-inp->openFolderRect.left,inp->openFolderRect.bottom-inp->openFolderRect.top};
    
    inp->openFolderImage1 = createGraphicContextResizedFromFileW(path11, sz);
    inp->openFolderImage2 = createGraphicContextResizedFromFileW(p12, sz);
    inp->openFolderImage3 = createGraphicContextResizedFromFileW(p13, sz);
    inp->openFolderImage4 = createGraphicContextResizedFromFileW(p14, sz);

    
}

CSINPUT::CSLINEAR_BIND* CSINPUT::CSLINEAR_BIND_PTR(CSLINEAR_BIND lb)
{
    return new CSLINEAR_BIND(lb);
}

void CSINPUT::setGBP(int _idInput, int bindLeft, int bindTop, int bindRight, int bindBottom)
{
    int idInput = (_idInput < 0) ? ip.size()+_idInput : _idInput;

    RECT r = sRectClient(*parent);

    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];

    CSLINEAR left = (bindLeft == 1) ? (CSL){0,inp->rect.left/float(r.right),0} : 
                                  ((bindLeft == 2) ? (CSL){float(r.right)-inp->rect.left,1,0} : (CSL){0,0,inp->rect.left});
    CSLINEAR top = (bindTop == 1) ? (CSL){0,inp->rect.top/float(r.bottom),0} : 
                                  ((bindTop == 2) ? (CSL){float(r.bottom)-inp->rect.top,1,0} : (CSL){0,0,inp->rect.top});
    CSLINEAR right = (bindRight == 1) ? (CSL){0,inp->rect.right/float(r.right),0} : 
                                  ((bindRight == 2) ? (CSL){float(r.right)-inp->rect.right,1,0} : (CSL){0,0,inp->rect.right});
    CSLINEAR bottom = (bindBottom == 1) ? (CSL){0,inp->rect.bottom/float(r.bottom),0} : 
                                  ((bindBottom == 2) ? (CSL){float(r.bottom)-inp->rect.bottom,1,0} : (CSL){0,0,inp->rect.bottom});

    
    inp->gbpCnt = CSLINEAR_BIND_PTR({left,top,right,bottom});

    left = (bindLeft == 1) ? (CSL){0,inp->textRect.left/float(r.right),0} : 
                                  ((bindLeft == 2) ? (CSL){float(r.right)-inp->textRect.left,1,0} : (CSL){0,0,inp->textRect.left});
    top = (bindTop == 1) ? (CSL){0,inp->textRect.top/float(r.bottom),0} : 
                                  ((bindTop == 2) ? (CSL){float(r.bottom)-inp->textRect.top,1,0} : (CSL){0,0,inp->textRect.top});
    right = (bindLeft == 1 || bindRight == 1) ? (CSL){0,inp->textRect.right/float(r.right),0} : 
                                  ((bindLeft == 2 || bindRight == 2) ? (CSL){float(r.right)-inp->textRect.right,1,0} : (CSL){0,0,inp->textRect.right});
    bottom = (bindBottom == 1) ? (CSL){0,inp->textRect.bottom/float(r.bottom),0} : 
                                  ((bindBottom == 2) ? (CSL){float(r.bottom)-inp->textRect.bottom,1,0} : (CSL){0,0,inp->textRect.bottom});

    inp->gbpBkg = CSLINEAR_BIND_PTR({left,top,right,bottom});

    left = (bindLeft == 1) ? (CSL){0,inp->titleRect.left/float(r.right),0} : 
                                  ((bindLeft == 2) ? (CSL){float(r.right)-inp->titleRect.left,1,0} : (CSL){0,0,inp->titleRect.left});
    top = (bindTop == 1) ? (CSL){0,inp->titleRect.top/float(r.bottom),0} : 
                                  ((bindTop == 2) ? (CSL){float(r.bottom)-inp->titleRect.top,1,0} : (CSL){0,0,inp->titleRect.top});
    right = (bindLeft == 1) ? (CSL){0,inp->titleRect.right/float(r.right),0} : 
                                  ((bindLeft == 2) ? (CSL){float(r.right)-inp->titleRect.right,1,0} : (CSL){0,0,inp->titleRect.right});
    bottom = (bindBottom == 1) ? (CSL){0,inp->titleRect.bottom/float(r.bottom),0} : 
                                  ((bindBottom == 2) ? (CSL){float(r.bottom)-inp->titleRect.bottom,1,0} : (CSL){0,0,inp->titleRect.bottom});

    inp->gbpTitle = CSLINEAR_BIND_PTR({left,top,right,bottom});

    left = (bindLeft == 1) ? (CSL){0,ctxRect.left/float(r.right),0} : 
                                  ((bindLeft == 2) ? (CSL){float(r.right)-ctxRect.left,1,0} : (CSL){0,0,ctxRect.left});
    top = (bindTop == 1) ? (CSL){0,ctxRect.top/float(r.bottom),0} : 
                                  ((bindTop == 2) ? (CSL){float(r.bottom)-ctxRect.top,1,0} : (CSL){0,0,ctxRect.top});
    right = (bindLeft == 1 || bindRight == 1) ? (CSL){0,ctxRect.right/float(r.right),0} : 
                                  ((bindLeft == 2 || bindRight == 2) ? (CSL){float(r.right)-ctxRect.right,1,0} : (CSL){0,0,ctxRect.right});
    bottom = (bindBottom == 1) ? (CSL){0,ctxRect.bottom/float(r.bottom),0} : 
                                  ((bindBottom == 2) ? (CSL){float(r.bottom)-ctxRect.bottom,1,0} : (CSL){0,0,ctxRect.bottom});

//std::cout<<ctxRect.left<<" "<<ctxRect.top<<" "<<ctxRect.right<<" "<<ctxRect.bottom<<" "<<bindRight<<"\n";
    if(gbpCtx) delete gbpCtx;
    gbpCtx = CSLINEAR_BIND_PTR({left,top,right,bottom});
    

    if(inp->upRect.right)
    {
        //left = (bindLeft == 1) ? (CSL){0,inp->upRect.left/float(r.right),0} : 
        //                          ((bindLeft == 2) ? (CSL){float(r.right)-inp->upRect.left,1,0} : (CSL){0,0,inp->upRect.left});
        left = (bindRight == 1) ? (CSL){0,inp->upRect.left/float(r.right),0} : 
                                  ((bindRight == 2) ? (CSL){float(r.right)-inp->upRect.left,1,0} : (CSL){0,0,inp->upRect.left});
        top = (bindTop == 1) ? (CSL){0,inp->upRect.top/float(r.bottom),0} : 
                                    ((bindTop == 2) ? (CSL){float(r.bottom)-inp->upRect.top,1,0} : (CSL){0,0,inp->upRect.top});
        right = (bindRight == 1) ? (CSL){0,inp->upRect.right/float(r.right),0} : 
                                    ((bindRight == 2) ? (CSL){float(r.right)-inp->upRect.right,1,0} : (CSL){0,0,inp->upRect.right});
        bottom = (bindBottom == 1) ? (CSL){0,inp->upRect.bottom/float(r.bottom),0} : 
                                    ((bindBottom == 2) ? (CSL){float(r.bottom)-inp->upRect.bottom,1,0} : (CSL){0,0,inp->upRect.bottom});

        inp->gbpIncUp = CSLINEAR_BIND_PTR({left,top,right,bottom});

        left = (bindRight == 1) ? (CSL){0,inp->downRect.left/float(r.right),0} : 
                                    ((bindRight == 2) ? (CSL){float(r.right)-inp->downRect.left,1,0} : (CSL){0,0,inp->downRect.left});
        top = (bindTop == 1) ? (CSL){0,inp->downRect.top/float(r.bottom),0} : 
                                    ((bindTop == 2) ? (CSL){float(r.bottom)-inp->downRect.top,1,0} : (CSL){0,0,inp->downRect.top});
        right = (bindRight == 1) ? (CSL){0,inp->downRect.right/float(r.right),0} : 
                                    ((bindRight == 2) ? (CSL){float(r.right)-inp->downRect.right,1,0} : (CSL){0,0,inp->downRect.right});
        bottom = (bindBottom == 1) ? (CSL){0,inp->downRect.bottom/float(r.bottom),0} : 
                                    ((bindBottom == 2) ? (CSL){float(r.bottom)-inp->downRect.bottom,1,0} : (CSL){0,0,inp->downRect.bottom});

        inp->gbpIncDown = CSLINEAR_BIND_PTR({left,top,right,bottom});
    }

    if(inp->undoRect.right)
    {
        left = (bindRight == 1) ? (CSL){0,inp->undoRect.left/float(r.right),0} : 
                                  ((bindRight == 2) ? (CSL){float(r.right)-inp->undoRect.left,1,0} : (CSL){0,0,inp->undoRect.left});
        top = (bindTop == 1) ? (CSL){0,inp->undoRect.top/float(r.bottom),0} : 
                                    ((bindTop == 2) ? (CSL){float(r.bottom)-inp->undoRect.top,1,0} : (CSL){0,0,inp->undoRect.top});
        right = (bindRight == 1) ? (CSL){0,inp->undoRect.right/float(r.right),0} : 
                                    ((bindRight == 2) ? (CSL){float(r.right)-inp->undoRect.right,1,0} : (CSL){0,0,inp->undoRect.right});
        bottom = (bindBottom == 1) ? (CSL){0,inp->undoRect.bottom/float(r.bottom),0} : 
                                    ((bindBottom == 2) ? (CSL){float(r.bottom)-inp->undoRect.bottom,1,0} : (CSL){0,0,inp->undoRect.bottom});

        inp->gbpUndo = CSLINEAR_BIND_PTR({left,top,right,bottom});

        left = (bindRight == 1) ? (CSL){0,inp->redoRect.left/float(r.right),0} : 
                                    ((bindRight == 2) ? (CSL){float(r.right)-inp->redoRect.left,1,0} : (CSL){0,0,inp->redoRect.left});
        top = (bindTop == 1) ? (CSL){0,inp->redoRect.top/float(r.bottom),0} : 
                                    ((bindTop == 2) ? (CSL){float(r.bottom)-inp->redoRect.top,1,0} : (CSL){0,0,inp->redoRect.top});
        right = (bindRight == 1) ? (CSL){0,inp->redoRect.right/float(r.right),0} : 
                                    ((bindRight == 2) ? (CSL){float(r.right)-inp->redoRect.right,1,0} : (CSL){0,0,inp->redoRect.right});
        bottom = (bindBottom == 1) ? (CSL){0,inp->redoRect.bottom/float(r.bottom),0} : 
                                    ((bindBottom == 2) ? (CSL){float(r.bottom)-inp->redoRect.bottom,1,0} : (CSL){0,0,inp->redoRect.bottom});

        inp->gbpRedo = CSLINEAR_BIND_PTR({left,top,right,bottom});
    }

    if(inp->unrollRect.right)
    {
        left = (bindRight == 1) ? (CSL){0,inp->unrollRect.left/float(r.right),0} : 
                                  ((bindRight == 2) ? (CSL){float(r.right)-inp->unrollRect.left,1,0} : (CSL){0,0,inp->unrollRect.left});
        top = (bindTop == 1) ? (CSL){0,inp->unrollRect.top/float(r.bottom),0} : 
                                    ((bindTop == 2) ? (CSL){float(r.bottom)-inp->unrollRect.top,1,0} : (CSL){0,0,inp->unrollRect.top});
        right = (bindRight == 1) ? (CSL){0,inp->unrollRect.right/float(r.right),0} : 
                                    ((bindRight == 2) ? (CSL){float(r.right)-inp->unrollRect.right,1,0} : (CSL){0,0,inp->unrollRect.right});
        bottom = (bindBottom == 1) ? (CSL){0,inp->unrollRect.bottom/float(r.bottom),0} : 
                                    ((bindBottom == 2) ? (CSL){float(r.bottom)-inp->unrollRect.bottom,1,0} : (CSL){0,0,inp->unrollRect.bottom});

        inp->gbpUnroll = CSLINEAR_BIND_PTR({left,top,right,bottom});
    }

    if(inp->openFolderRect.right)
    {
        left = (bindRight == 1) ? (CSL){0,inp->openFolderRect.left/float(r.right),0} : 
                                  ((bindRight == 2) ? (CSL){float(r.right)-inp->openFolderRect.left,1,0} : (CSL){0,0,inp->openFolderRect.left});
        top = (bindTop == 1) ? (CSL){0,inp->openFolderRect.top/float(r.bottom),0} : 
                                    ((bindTop == 2) ? (CSL){float(r.bottom)-inp->openFolderRect.top,1,0} : (CSL){0,0,inp->openFolderRect.top});
        right = (bindRight == 1) ? (CSL){0,inp->openFolderRect.right/float(r.right),0} : 
                                    ((bindRight == 2) ? (CSL){float(r.right)-inp->openFolderRect.right,1,0} : (CSL){0,0,inp->openFolderRect.right});
        bottom = (bindBottom == 1) ? (CSL){0,inp->openFolderRect.bottom/float(r.bottom),0} : 
                                    ((bindBottom == 2) ? (CSL){float(r.bottom)-inp->openFolderRect.bottom,1,0} : (CSL){0,0,inp->openFolderRect.bottom});

        inp->gbpOpenFolder = CSLINEAR_BIND_PTR({left,top,right,bottom});
    }

    
}

CSINPUT::~CSINPUT()
{
    // Nettoyer toutes les ressources des inputs
    // Les ressources individuelles sont maintenant gérées par le destructeur de CSINPUT_PARAMS
    for(auto& inp : ip)
    {
        // Libérer la mémoire allouée pour cet objet
        delete inp;
    }

    // Nettoyer les ressources de classe
    if(gbpCtx) delete gbpCtx;
}

RECT CSINPUT::getFrontEndCharRect(int idInput, int idChar)
{
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];

    RECT r = inp->textCharGeometry[idChar];
    r.left -= inp->showedSectionRect.left;
    r.right -= inp->showedSectionRect.left;
    r.left += inp->textRect.left + 2;
    r.right += inp->textRect.left + 2;

    //r.top += inp->textRect.top;
    //r.bottom += inp->textRect.top;

    return r;
}

RECT CSINPUT::placeCaret(POINT p)
{
    CSINPUT::CSINPUT_PARAMS* inp = ip[idActive];
    int n = inp->textCharGeometry.size();
    RECT r={-1000,-1000};
    if(n)
    {
        for(int i=0; i<n; i++)
        {
            r = inp->textCharGeometry[i];
            r.left -= inp->showedSectionRect.left;
            r.right -= inp->showedSectionRect.left;
            r.left += inp->textRect.left+2;
            r.right += inp->textRect.left +2;

            r.top += inp->textRect.top;
            r.bottom += inp->textRect.top;

            //cout<<r.left<<" "<<r.right<<"  ;  "<<p.x, 
            if(PtInRect(&r, p))
            {
                inp->caretPos = i+1;
                return r;
            }
        }

        if(p.x > r.right)
        {
        inp->caretPos = inp->textCharGeometry.size(); 
        }
        else if(p.x < inp->textCharGeometry[0].left)
        {
            inp->caretPos = 0; 
            r = inp->textCharGeometry[0];
            r.left -= inp->showedSectionRect.left;
            r.right -= inp->showedSectionRect.left;
            r.left += inp->textRect.left+2;
            r.right += inp->textRect.left +2;

            r.top += inp->textRect.top;
            r.bottom += inp->textRect.top;
        }
        else
        {
            r={-1000,-1000};
        }
    }
    return r;
}


RECT CSINPUT::updateRect(int idInput, CSLINEAR_BIND*lb)
{
    if(lb)
    {
        CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];

        RECT r;
        r.left = inp->rect.left + (inp->rsize.cx-lb->l.x)*lb->l.a + lb->l.b;
        r.top = inp->rect.top + (inp->rsize.cy-lb->t.x)*lb->t.a + lb->t.b;
        //r.right = inp->rect.left + (inp->rsize.cx-lb->r.x)*lb->r.a + lb->r.b;
        //r.bottom = inp->rect.top + (inp->rsize.cy-lb->b.x)*lb->b.a + lb->b.b;
        r.right = r.left + (inp->rsize.cx-lb->r.x)*lb->r.a + lb->r.b;
        r.bottom = r.top + (inp->rsize.cy-lb->b.x)*lb->b.a + lb->b.b;
        return r;
    }

}

void CSINPUT::_updateGeometry(int idInput)
{
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];

    if(inp->gbpCnt)
    {
        CSLINEAR_BIND*lb = inp->gbpCnt;
        RECT* r = &inp->rect;
        RECT rp = sRectClient(*parent);
        r->left = (rp.right-lb->l.x)*lb->l.a + lb->l.b;
        r->top = (rp.bottom-lb->t.x)*lb->t.a + lb->t.b;
        //r.right = inp->rect.left + (inp->rsize.cx-lb->r.x)*lb->r.a + lb->r.b;
        //r.bottom = inp->rect.top + (inp->rsize.cy-lb->b.x)*lb->b.a + lb->b.b;
        r->right = (rp.right-lb->r.x)*lb->r.a + lb->r.b;
        r->bottom = (rp.bottom-lb->b.x)*lb->b.a + lb->b.b;

        inp->rsize = {r->right-r->left, r->bottom-r->top};

        lb = inp->gbpBkg;
        r = &inp->textRect;
        r->left = (rp.right-lb->l.x)*lb->l.a + lb->l.b;
        r->top = (rp.right-lb->t.x)*lb->t.a + lb->t.b;
        r->right = (rp.right-lb->r.x)*lb->r.a + lb->r.b;
        r->bottom = (rp.bottom-lb->b.x)*lb->b.a + lb->b.b;

        lb = inp->gbpIncUp;
        if(lb)
        {
            r = &inp->upRect;
            r->left = (rp.right-lb->l.x)*lb->l.a + lb->l.b;
            r->top = (rp.right-lb->t.x)*lb->t.a + lb->t.b;
            r->right = (rp.right-lb->r.x)*lb->r.a + lb->r.b;
            r->bottom = (rp.bottom-lb->b.x)*lb->b.a + lb->b.b;
        }

        lb = inp->gbpIncDown;
        if(lb)
        {
            r = &inp->downRect;
            r->left = (rp.right-lb->l.x)*lb->l.a + lb->l.b;
            r->top = (rp.right-lb->t.x)*lb->t.a + lb->t.b;
            r->right = (rp.right-lb->r.x)*lb->r.a + lb->r.b;
            r->bottom = (rp.bottom-lb->b.x)*lb->b.a + lb->b.b;
        }

        lb = inp->gbpUndo;
        if(lb)
        {
            r = &inp->undoRect;
            r->left = (rp.right-lb->l.x)*lb->l.a + lb->l.b;
            r->top = (rp.right-lb->t.x)*lb->t.a + lb->t.b;
            r->right = (rp.right-lb->r.x)*lb->r.a + lb->r.b;
            r->bottom = (rp.bottom-lb->b.x)*lb->b.a + lb->b.b;
        }

        lb = inp->gbpRedo;
        if(lb)
        {
            r = &inp->redoRect;
            r->left = (rp.right-lb->l.x)*lb->l.a + lb->l.b;
            r->top = (rp.right-lb->t.x)*lb->t.a + lb->t.b;
            r->right = (rp.right-lb->r.x)*lb->r.a + lb->r.b;
            r->bottom = (rp.bottom-lb->b.x)*lb->b.a + lb->b.b;
        }

        lb = inp->gbpTitle;
        if(lb)
        {
            r = &inp->titleRect;
            r->left = (rp.right-lb->l.x)*lb->l.a + lb->l.b;
            r->top = (rp.right-lb->t.x)*lb->t.a + lb->t.b;
            r->right = (rp.right-lb->r.x)*lb->r.a + lb->r.b;
            r->bottom = (rp.bottom-lb->b.x)*lb->b.a + lb->b.b;
        }

        lb = inp->gbpUnroll;
        if(lb)
        {
            r = &inp->unrollRect;
            r->left = (rp.right-lb->l.x)*lb->l.a + lb->l.b;
            r->top = (rp.right-lb->t.x)*lb->t.a + lb->t.b;
            r->right = (rp.right-lb->r.x)*lb->r.a + lb->r.b;
            r->bottom = (rp.bottom-lb->b.x)*lb->b.a + lb->b.b;
        }

        lb = inp->gbpOpenFolder;
        if(lb)
        {
            r = &inp->openFolderRect;
            r->left = (rp.right-lb->l.x)*lb->l.a + lb->l.b;
            r->top = (rp.right-lb->t.x)*lb->t.a + lb->t.b;
            r->right = (rp.right-lb->r.x)*lb->r.a + lb->r.b;
            r->bottom = (rp.bottom-lb->b.x)*lb->b.a + lb->b.b;
        }
        

        /*r = &inp->activeCharRect;
        r->left += 
        r->top = (rp.right-lb->t.x)*lb->t.a + lb->t.b;
        r->right = (rp.right-lb->r.x)*lb->r.a + lb->r.b;
        r->bottom = (rp.bottom-lb->b.x)*lb->b.a + lb->b.b;*/
    }

}

void CSINPUT::updateGeometry(int idInput)
{
    _updateGeometry(idInput);
    createContext(idInput);
    updateVisual_state1(idInput, 1);
}

/*void CSINPUT::updateVisibleBackground()
{
    //parentDC = csCTX(*parent).redrawDwgSurface();
}*/

int CSINPUT::getId()
{
    return *parent;
}
void CSINPUT::updateVisibleBackground()
{
    parentDC = csGraphics::getGraphicAreaContext(*parent);
    RECT rv = csGraphics::getGraphicAreaContextVisiblePart(*parent);
    BitBlt(parentDC, rv.left, rv.top, rv.right-rv.left, rv.bottom-rv.top, stackDC.dc, 0,0,SRCCOPY);
}
void CSINPUT::updateBackground()
{
    parentDC = csGraphics::getGraphicAreaContext(*parent);
    BitBlt(parentDC, 0, 0, stackDC.sz.cx, stackDC.sz.cy, stackDC.dc, 0,0,SRCCOPY);
    RECT rv = csGraphics::getGraphicAreaContextVisiblePart(*parent);
}

void CSINPUT::updateActiveTextBkg()
{
    RECT r = ip[idActive]->textRect;
    CSRGBA bcol = ip[idActive]->bkgColor2;
    CSRGBA brdcol = ip[idActive]->borderColor2;
    //drawGdiRectangle(ip[idActive]->cntDc, brdcol, bcol, r);
    drawGdiRectangle(parentDC, brdcol, bcol, r);
}
void CSINPUT::updateActiveTextBkg(RECT rchar)
{
    CSRGBA bcol = ip[idActive]->bkgColor2;
    CSRGBA brdcol = ip[idActive]->borderColor2;
    RECT r = ip[idActive]->textRect;
    drawGdiRectangle(ip[idActive]->cntDc, brdcol, bcol, {rchar.left+r.left, rchar.top+1, rchar.right+r.left, rchar.bottom-1});
    //drawGdiRectangle(parentDC, brdcol, bcol, rchar);
}

void CSINPUT::createContext(int idInput)
{
    CSINPUT::CSINPUT::CSINPUT_PARAMS* ipp = ip[idInput];
    if(ipp->cntDc)
    {
        DeleteDC(ipp->cntDc);
        DeleteBitmap(ipp->cntBmp);
    }
    parentDC = csGraphics::getGraphicAreaContext(*parent);
    ipp->cntDc = CreateCompatibleDC(parentDC);
    //std::cout<<ipp->rsize.cx<<" "<<ipp->rsize.cy<<"\n";
    ipp->cntBmp = CreateCompatibleBitmap(parentDC, ipp->rsize.cx, ipp->rsize.cy);
    SelectBitmap(ipp->cntDc, ipp->cntBmp);
    
    /*RECT r = ipp->upRect;
    CSGRAPHIC_CONTEXT dc = ipp->incUpImage2;
    if(r.bottom)
    {
        int cx = r.right-r.left;
        int cy = r.bottom-r.top;
        //cout<<dc.sz.width<<"  "<<dc.sz.height<<" -- \n";
        StretchBlt(ipp->cntDc, r.left-ctxRect.left, r.top-ctxRect.top, cx, cy, dc.dc, 0,0,dc.sz.width,dc.sz.height, SRCCOPY);
        //StretchBlt(parentDC, ctxRect.left, ctxRect.top, cx, cy, ipp->cntDc, 0,0,dc.sz.width,dc.sz.height, SRCCOPY);
        //InvalidateRect(sHandle(*parent), &r, 1);
    }
    drawImage(ipp->cntDc, ipp->incUpImage2, ipp->upRect);
    drawImage(ipp->cntDc, ipp->incDownImage2, ipp->downRect);
    drawImage(ipp->cntDc, ipp->undoImage2, ipp->undoRect);
    drawImage(ipp->cntDc, ipp->redoImage2, ipp->redoRect);
    drawImage(ipp->cntDc, ipp->unrollImage2, ipp->unrollRect);
    drawImage(ipp->cntDc, ipp->openFolderImage2, ipp->openFolderRect);*/


}

RECT csRectContextualized(RECT r, RECT rparent)
{
    return {r.left-rparent.left, r.top-rparent.top, r.right-rparent.left, r.bottom-rparent.top};
}

void CSINPUT::_updateVisual_state1(int idInput, bool noRedrawActiveChar)
{
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];
    RECT r = inp->rect;
    CSRGBA bcol = inp->cntColor1;
    CSRGBA brdcol = inp->cntBorderColor1;
    drawGdiRectangle(inp->cntDc, brdcol, bcol, csRectContextualized(r,r));
    
    bcol = inp->bkgColor1;
    brdcol = inp->borderColor1;
    drawGdiRectangle(inp->cntDc, brdcol, bcol, csRectContextualized(inp->titleRect,r));
    drawGdiText(inp->cntDc, inp->title, inp->titleFont, inp->titleColor1, csRectContextualized(inp->titleRect,r));

    drawImage(inp->cntDc, inp->incUpImage2, csRectContextualized(inp->upRect, r));
    drawImage(inp->cntDc, inp->incDownImage2, csRectContextualized(inp->downRect, r));
    drawImage(inp->cntDc, inp->undoImage2, csRectContextualized(inp->undoRect, r));
    drawImage(inp->cntDc, inp->redoImage2, csRectContextualized(inp->redoRect, r));
    drawImage(inp->cntDc, inp->unrollImage2, csRectContextualized(inp->unrollRect, r));
    drawImage(inp->cntDc, inp->openFolderImage2, csRectContextualized(inp->openFolderRect, r));
//cout<<inp->cntDc<<" "<<r.top<<"\n";
}


void CSINPUT::updateVisual_state1(int idInput, bool noRedrawActiveChar)
{
    _updateVisual_state1(idInput, noRedrawActiveChar);
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];
    RECT r = inp->rect;
    SIZE s = inp->rsize;
    BitBlt(parentDC, r.left, r.top, s.cx, s.cy, inp->cntDc, 0,0,SRCCOPY);
    updateText(idInput);
    if(!noRedrawActiveChar)
        highlightActiveChar(ip[idActive]->activeCharRect);
    //if(r.left == s.cx || r.top == s.cy) cout<<" less than zero !\n";
    //InvalidateRect(sHandle(*parent), &r, 1);
}


void CSINPUT::_updateVisual_state2(int idInput, bool noRedrawActiveChar)
{
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];

    RECT r = inp->rect;
    CSRGBA bcol = inp->cntColor2;
    CSRGBA brdcol = inp->cntBorderColor2;
    drawGdiRectangle(inp->cntDc, brdcol, bcol, csRectContextualized(r,r));
    
    bcol = inp->bkgColor2;
    brdcol = inp->borderColor2;
    drawGdiRectangle(inp->cntDc, brdcol, bcol, csRectContextualized(inp->titleRect,r));
    drawGdiText(inp->cntDc, inp->title, inp->titleFont, inp->titleColor2, csRectContextualized(inp->titleRect,r));


    drawImage(inp->cntDc, inp->incUpImage2, csRectContextualized(inp->upRect, r));
    drawImage(inp->cntDc, inp->incDownImage2, csRectContextualized(inp->downRect, r));
    drawImage(inp->cntDc, inp->undoImage2, csRectContextualized(inp->undoRect, r));
    drawImage(inp->cntDc, inp->redoImage2, csRectContextualized(inp->redoRect, r));
    drawImage(inp->cntDc, inp->unrollImage2, csRectContextualized(inp->unrollRect, r));
    drawImage(inp->cntDc, inp->openFolderImage2, csRectContextualized(inp->openFolderRect, r));
}

void CSINPUT::updateVisual_state2(int idInput, bool noRedrawActiveChar)
{
    _updateVisual_state2(idInput, noRedrawActiveChar);
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];
    RECT r = inp->rect;
    SIZE s = inp->rsize;
    //BitBlt(parentDC, rv.left, rv.top, stackDC.sz.cx, stackDC.sz.cy, stackDC.dc, 0,0,SRCCOPY);
    BitBlt(parentDC, r.left, r.top, s.cx, s.cy, inp->cntDc, 0,0,SRCCOPY);
    updateText(idInput);
    if(!noRedrawActiveChar)
        highlightActiveChar(ip[idActive]->activeCharRect);
    //InvalidateRect(sHandle(*parent), &r, 1);
}

void CSINPUT::updateAll()
{
    int n = ip.size();
    CSINPUT::CSLINEAR_BIND* lb = gbpCtx;
    RECT *r = &ctxRect;
    RECT rp = sRectClient(*parent);
    r->left = (rp.right-lb->l.x)*lb->l.a + lb->l.b;
    r->top = (rp.right-lb->t.x)*lb->t.a + lb->t.b;
    r->right = (rp.right-lb->r.x)*lb->r.a + lb->r.b;
    r->bottom = (rp.bottom-lb->b.x)*lb->b.a + lb->b.b;
    ///parentDC=csCTX(*parent).redrawDwgSurface();
    BITMAPINFO* bmi=0  , *bmi0;
    SIZE sectionSize = {ctxRect.right-ctxRect.left, ctxRect.bottom-ctxRect.top};
    //CSGRAPHIC_CONTEXT* visualCtrl = csCreateGraphicContextPtr(sectionSize, bmi);
    //BitBlt(visualCtrl->dc, 0, 0, sectionSize.cx, sectionSize.cy, parentDC, ctxRect.left,ctxRect.top,SRCCOPY);
    for(int i=0; i<n; i++)
    {
        createContext(i);
        _updateVisual_state1(i);
        RECT r1 = ip[i]->rect;
        SIZE s = ip[i]->rsize;
    
        //BitBlt(stackDC.dc, r1.left, r1.top, s.cx, s.cy, ip[i]->cntDc, 0,0,SRCCOPY);
        BitBlt(parentDC, r1.left, r1.top, s.cx, s.cy, ip[i]->cntDc, 0,0,SRCCOPY);
   
    }
    
    //BitBlt(parentDC, r->left,r->top, sectionSize.cx, sectionSize.cy, visualCtrl->dc, 0,0,SRCCOPY);
    update();
    //csFreeGraphicContextExt(*visualCtrl);
    //free(visualCtrl);
}

void CSINPUT::mouseMoveEvent(int idInput, POINT p)
{
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];

    if(PtInRect(&inp->rect, p))
    {
        if(!inp->cntOn)
        {
            inp->cntOn = 1;
            updateVisual_state2(idInput);

            idMouseHover = idInput;

            //update();

            inp->btnUpOn = 0;
            inp->btnDownOn = 0;
            inp->btnUndoOn = 0;
            inp->btnRedoOn = 0;
            inp->btnUnrollOn = 0;
            inp->btnOpenFolderOn = 0;
            //return;
        }
        
    }
    else
    {
        if(inp->cntOn)
        {
            inp->cntOn = 0;
            updateVisual_state1(idInput);

            //update();

            //redraw buttons
            inp->btnUpOn = 1;
            inp->btnDownOn = 1;
            inp->btnUndoOn = 1;
            inp->btnRedoOn = 1;
            inp->btnUnrollOn = 1;
            inp->btnOpenFolderOn = 1;
            //return;
        }
    }

    if(PtInRect(&inp->textRect, p))
    {
        if(!inp->bkgOn)
        {
            inp->bkgOn = 1;
            cursor[*parent] = LoadCursor(0, IDC_IBEAM);
            idMouseHover = idInput;
            
            //return;
        }
        
    }
    else
    {
        if(inp->bkgOn)
        {
            inp->bkgOn = 0;
            cursor[*parent] = LoadCursor(0, IDC_ARROW);
            //return;
        }
    }

    if(PtInRect(&inp->upRect, p))
    {
        if(!inp->btnUpOn)
        {
            inp->btnUpOn = 1;
            drawImage(parentDC, inp->incUpImage1, inp->upRect);
            idMouseHover = idInput;
            //return;
        }
        
    }
    else
    {
        if(inp->btnUpOn)
        {
            inp->btnUpOn = 0;
            drawImage(parentDC, inp->incUpImage2, inp->upRect);
            //return;
        }
    }

    if(PtInRect(&inp->downRect, p))
    {
        if(!inp->btnDownOn)
        {
            inp->btnDownOn = 1;
            drawImage(parentDC, inp->incDownImage1, inp->downRect);
            idMouseHover = idInput;
            //return;
        }
        
    }
    else
    {
        if(inp->btnDownOn)
        {
            inp->btnDownOn = 0;
            drawImage(parentDC, inp->incDownImage2, inp->downRect);
            //return;
        }
    }

    if(PtInRect(&inp->undoRect, p))
    {
        if(!inp->btnUndoOn)
        {
            inp->btnUndoOn = 1;
            drawImage(parentDC, inp->undoImage1, inp->undoRect);
            idMouseHover = idInput;
            //return;
        }
        
    }
    else
    {
        if(inp->btnUndoOn)
        {
            inp->btnUndoOn = 0;
            drawImage(parentDC, inp->undoImage2, inp->undoRect);
            //return;
        }
    }

    if(PtInRect(&inp->redoRect, p))
    {
        if(!inp->btnRedoOn)
        {
            inp->btnRedoOn = 1;
            drawImage(parentDC, inp->redoImage1, inp->redoRect);
            idMouseHover = idInput;
            //return;
        }
        
    }
    else
    {
        if(inp->btnRedoOn)
        {
            inp->btnRedoOn = 0;
            drawImage(parentDC, inp->redoImage2, inp->redoRect);
            //return;
        }
    }

    if(PtInRect(&inp->unrollRect, p))
    {
        if(!inp->btnUnrollOn)
        {
            inp->btnUnrollOn = 1;
            drawImage(parentDC, inp->unrollImage1, inp->unrollRect);
            idMouseHover = idInput;
            //return;
        }
        
    }
    else
    {
        if(inp->btnUnrollOn)
        {
            inp->btnUnrollOn = 0;
            drawImage(parentDC, inp->unrollImage2, inp->unrollRect);
            //return;
        }
    }

    if(PtInRect(&inp->openFolderRect, p))
    {
        if(!inp->btnOpenFolderOn)
        {
            inp->btnOpenFolderOn = 1;
            drawImage(parentDC, inp->openFolderImage1, inp->openFolderRect);
            idMouseHover = idInput;
            //return;
        }
        
    }
    else
    {
        if(inp->btnOpenFolderOn)
        {
            inp->btnOpenFolderOn = 0;
            drawImage(parentDC, inp->openFolderImage2, inp->openFolderRect);
            //return;
        }
    }

    update();
}

void CSINPUT::update()
{
    RECT r = csGraphics::getViewAreaRect(*parent);
    //r.left -= 10; r.top -= 10; r.right += 10; r.bottom += 10;
    /*RECT r = ip[idMouseHover]->rect;
    r.left -= 20; r.top -= 20; r.right += 20; r.bottom += 20;
    InvalidateRect(sHandle(*parent), &r, 1);
    r = ip[idLastMouseHover]->rect;
    r.left -= 20; r.top -= 20; r.right += 20; r.bottom += 20;*/
    InvalidateRect(sHandle(*parent), &r, 1);
}

void CSINPUT::drawGdiRectangle(HDC& dc, CSRGBA col, RECT rect)
{
    COLORREF cref = RGB(col.r, col.g, col.b);
    HPEN hp = CreatePen(0,1,cref);
    HBRUSH hb = CreateSolidBrush(cref);
    SelectPen(dc, hp);
    SelectBrush(dc, hb);
    //Rectangle(dc, rect.left-ctxRect.left, rect.top-ctxRect.top, rect.right-ctxRect.left, rect.bottom-ctxRect.top);
    Rectangle(dc, rect.left, rect.top, rect.right, rect.bottom);
    //InvalidateRect(sHandle(*parent), &rect, 1);
    DeletePen(hp);
    DeleteBrush(hb);

}
void CSINPUT::drawGdiRectangle(HDC& dc, CSRGBA brd, CSRGBA bkg, RECT rect)
{
    COLORREF cpen = RGB(brd.r, brd.g, brd.b);
    COLORREF cbrush = RGB(bkg.r, bkg.g, bkg.b);
    HPEN hp = CreatePen(0,1,cpen);
    HBRUSH hb = CreateSolidBrush(cbrush);
    SelectPen(dc, hp);
    SelectBrush(dc, hb);
    //Rectangle(dc, rect.left-ctxRect.left, rect.top-ctxRect.top, rect.right-ctxRect.left, rect.bottom-ctxRect.top);
    Rectangle(dc, rect.left, rect.top, rect.right, rect.bottom);
    //InvalidateRect(sHandle(*parent), &rect, 1);
    DeletePen(hp);
    DeleteBrush(hb);
}
void CSINPUT::drawGdiText(HDC& dc, wchar_t*text, HFONT hf, CSRGBA col, RECT rect)
{
    COLORREF c = RGB(col.r, col.g, col.b);
    SetTextColor(dc, c);
    SetBkMode(dc, TRANSPARENT); 
    SelectFont(dc, hf);
    wchar_t*str = truncateHorizontalTextW(text, hf, rect.right-rect.left);
    //TextOutW(dc, rect.left-ctxRect.left, rect.top-ctxRect.top, str, wcslen(str));
    TextOutW(dc, rect.left, rect.top, str, wcslen(str));
    free(str);
    //InvalidateRect(sHandle(*parent), &rect, 1);
}

void CSINPUT::drawImage(HDC& hdc, CSGRAPHIC_CONTEXT dc, RECT r)
{
    if(r.bottom)
    {
        int cx = r.right-r.left;
        int cy = r.bottom-r.top;
        //cout<<dc.sz.width<<"  "<<dc.sz.height<<" -- \n";
        StretchBlt(hdc, r.left, r.top, cx, cy, dc.dc, 0,0,dc.sz.cx,dc.sz.cy, SRCCOPY);
        //StretchBlt(hdc, 0, 0, cx, cy, dc.dc, 0,0,dc.sz.width,dc.sz.height, SRCCOPY);
        //InvalidateRect(sHandle(*parent), &r, 1);
    }
}

CSINPUT::CSINPUT::CSINPUT_PARAMS* CSINPUT::getInputParams(int idInput)
{
    return ip[idInput];
}
vector<CSINPUT::CSINPUT::CSINPUT_PARAMS*> CSINPUT::getInputParamsList()
{
    return ip;
}
int CSINPUT::getInputsNumber()
{
    return ip.size();
}
int CSINPUT::getActiveInputCharNumber()
{
    return ip[idActive]->textChar.size();
}

void CSINPUT::setActive(int idInput)
{
    idActive = idInput;
}
void CSINPUT::setLastActive(int idInput)
{
    idLastActive = idInput;
}

int CSINPUT::getActive()
{
    return idActive;
}
int CSINPUT::getLastActive()
{
    return idLastActive;
}
int CSINPUT::getMouseHoverId()
{
    return idMouseHover;
}

int CSINPUT::updateActiveInput()
{
    InvalidateRect(sHandle(*parent),&ip[idActive]->rect,1);
}
int CSINPUT::updateFormerInput()
{
    InvalidateRect(sHandle(*parent), &ip[idLastActive]->rect,1);
}
void CSINPUT::blit()
{
    BitBlt(parentDC, 0, 0, hdcSize[*parent].cx, hdcSize[*parent].cy, stackDC.dc, 0,0,SRCCOPY);
    update();
}
HDC CSINPUT::getStackDC()
{
    return stackDC.dc;
}

extern bool APP_CREATED;
//extern vector<CSTIMER_PARAMS> TIMER_PARAMS;
void updateGeometryThread(CSARGS Args);

void typeString(CSARGS Args)
{
    CSINPUT* ipp = (CSINPUT*)Args[0];

    UINT msg = (UINT)Args;
    WPARAM wp = (WPARAM)Args;

    static int count = 0;
    static POINT pmouse, psave;

    int idi = ipp->getActive();
    CSINPUT::CSINPUT::CSINPUT_PARAMS* ip = ipp->getInputParams(idi);

    static int timerId = 1;
    static RECT rhlight = {0};
    
    if(msg == WM_MOUSEHOVER)
    {
        SetTimer(HWND(Args), timerId, 20, 0);
        ipp->startChecking = 1;
        return;
    }
    if(msg == WM_MOUSELEAVE)
    {
        
        ipp->updateVisual_state1(ipp->getMouseHoverId());
        InvalidateRect(HWND(Args),0,1);
        //ipp->updateActiveInput();
        ipp->startChecking = 0;
        KillTimer(HWND(Args), timerId);
        return;
    }

    if(msg == WM_MOUSEMOVE)
    {
        count = 15;
    }

    
    if(msg == WM_TIMER && WPARAM(Args) == timerId)
    {
        if(count == 0)
        {
            GetCursorPos(&pmouse);
        }
        else count--;

        if(ipp->startChecking == 1 && !(TIMER_POINT.x == pmouse.x && TIMER_POINT.y == pmouse.y))
        {
            POINT p = TIMER_POINT;
            ScreenToClient(sHandle(int(Args)), &p);

            int i = ipp->getId();
            p.x += csGraphics::getGraphicAreaInXPos(i) - csGraphics::getGraphicAreaXPos(i);
            p.y += csGraphics::getGraphicAreaInYPos(i) - csGraphics::getGraphicAreaYPos(i);

            psave = p;
            for(int i = 0; i<ipp->getInputsNumber(); i++)
                ipp->mouseMoveEvent(i, p);
            
        
        }

    }

    
    static int hcount = 0;
    if(msg == WM_TIMER && WPARAM(Args) == 0)
    {
        if(hcount == 40)
        {
            ipp->highlightActiveChar(rhlight, 0);
            //InvalidateRect((HWND)Args, &rhlight, 1);
        }
        if(hcount == 80)
        {
            ipp->highlightActiveChar(rhlight, 1);
            //InvalidateRect((HWND)Args, &rhlight, 1);
            hcount = 0;
        }
        hcount++;
    }

    if(msg == WM_LBUTTONDOWN)
    {
        POINT p;
        GetCursorPos(&p);
        ScreenToClient(HWND(Args),&p);

        int i = ipp->getId();
        p.x += csGraphics::getGraphicAreaInXPos(i) - csGraphics::getGraphicAreaXPos(i);
        p.y += csGraphics::getGraphicAreaInYPos(i) - csGraphics::getGraphicAreaYPos(i);

        int n = ipp->getInputsNumber();
        for(int i=0; i<n; i++)
        {
            if(PtInRect(&ipp->getInputParams(i)->rect, p))
            {
                ipp->setLastActive(ipp->getActive());
                ipp->setActive(i);
                break;
            }
        }

        rhlight = ipp->placeCaret(p);  
        //ipp->highlightActiveChar(rhlight);
        
    }
    if(msg == WM_KEYDOWN)
    {
        if(wp == VK_LEFT)
        {
            if(ip->caretPos > 0)
                ip->caretPos--;
            return;
        }
        if(wp == VK_RIGHT)
        {
            if(ip->caretPos < ipp->getActiveInputCharNumber())
            {
                ip->caretPos++;
            }
            return;
        }
        if(wp == VK_BACK)
        {
            if(ip->caretPos > 0)
            {
                ipp->deleteChar(1);
            }
            return;
        }
        if(wp == VK_DELETE)
        {
            if(ip->caretPos < ipp->getActiveInputCharNumber()-1)
            {
                ipp->deleteChar(0);
            }
            return;
        }
        
        
    }
    if(msg == WM_CHAR )
    {
        if(wp != 8 && wp != 13)
        {
            ipp->addChar(wp);
        }

        if((TIMER_POINT.x == pmouse.x && TIMER_POINT.y == pmouse.y))
            ipp->mouseMoveEvent(ipp->getActive(), psave);
       
    }

    static int count_ = 20;
    if(msg == WM_SIZE)
    {
        int n = ipp->getInputsNumber();
        ipp->updateVisibleBackground();
        RECT r = csGraphics::getGraphicAreaContextVisiblePart(ipp->getId());
        for(int i = 0; i<n; i++)
        {
            RECT r2 = ipp->getInputParams(i)->rect;
            if(!(r2.left > r.right || r2.top > r.bottom || r2.right < r.left || r2.bottom < r.top))
            ipp->updateGeometry(i);
        }
        ipp->update();

        count_ = 20;
        return;
    }

    if(msg == WM_TIMER && WPARAM(Args) == 0)
    {
        
        if(count_ != 0)
        count_--;

        if(count_ == 2)
        {
            int n = ipp->getInputsNumber();
            ipp->updateBackground();
            for(int i = 0; i<n; i++)
            {
                RECT r2 = ipp->getInputParams(i)->rect;
                ipp->updateGeometry(i);
            }
            ipp->update();
            count_ = 0;
        }
    }

}


void updateGeometryThread(CSARGS Args)
{
    static CSINPUT* _ip = (CSINPUT*)Args[0];
     _ip = (CSINPUT*)Args[0];
    //CSINPUT**p = csAlloc(1,_ip);
    static bool b;
    b=CURSOR_NCHITTEST_POS;

    std::thread t(
        
        [](CSINPUT* ip, bool b){

                
            ip->startGBP = 1;
            int a = 3;
            
            RECT rc = sRectClient(ip->getId()), rc2 = {0};
            
            while(a > 0)
            {
                //InvalidateRect(HWND(Args),0,1);
                rc = sRectClient(ip->getId());

                if(rc.right != rc2.right || rc.bottom != rc2.bottom)
                {
                

                    //ip->updateStackDC();
                    int n = ip->getInputsNumber();
                    ip->updateVisibleBackground();
                    for(int i = 0; i<n; i++)
                    {
                        ip->_updateGeometry(i++);
                        //ip->updateGeometry(i);
                    }
                    ip->updateAll();
                        
                    //ip->blit();

                    rc2 = rc;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(22));
                if(!(GetAsyncKeyState(VK_LBUTTON)&&0x8000))
                {
                    a--;
                    if(a == 0)
                    {
            //std::cout<<ip->startGBP <<" auoiiiiiii 564645\n";
                        ip->startGBP = 0;

                        //if(b == 0) // ends if the cursor is in the client area
                        {
                            //ip->updateVisibleBackground();
                            int n = ip->getInputsNumber();
                            for(int i = 0; i<n; i++)
                            {
                                ip->updateGeometry(i);
                            }
                            //ip->blit();
                            InvalidateRect(sHandle(ip->getId()), 0,1);
                        } 
                        //free(ip);
                    }
                }
            }
        }, _ip,b
    );
    t.detach();
}

CSINPUT* csNewInputContext(int*id)
{
    CSINPUT* inp = new CSINPUT();
    inp->init(id);

    addAction(*id, typeString, 1, inp);
    inp->__getEventsGroup();
    
    return inp;
}

void templateInput(CSINPUT*& inp, RECT geometry)
{
    
    inp->newInput((wchar_t*)(wstring(L"input ")+to_wstring(inp->getInputsNumber()+1)).c_str(),L"0",geometry, 0, 50, 10);
    inp->addOpenFolderButton(-1,L"resources/img/Folder.bmp",L"resources/img/Folder2.bmp",0,0);
    inp->addUnrollButton(-1,L"resources/img/Hide.bmp",L"resources/img/Hide2.bmp",0,0);
    //inp->addUndoRedoButtons(-1,L"resources/img/ArrPrev.bmp",L"resources/img/ArrPrev2.bmp",0,0, L"resources/img/ArrNext.bmp",L"resources/img/ArrNext2.bmp",0,0);
    //inp->addIncrementButtons(-1,L"resources/img/ArrUp.bmp",L"resources/img/ArrUp2.bmp",0,0, L"resources/img/ArrDown.bmp",L"resources/img/ArrDown2.bmp",0,0);
    inp->setGBP(-1,0,0,2,0);
}

extern float geomCoef;

CSINPUT* inputContextExample(int idp)
{
    RECT r = sRectClient(idp);
    /*int id = createSection(idp, {5/geomCoef,30/geomCoef,(r.right-10)/geomCoef, (r.bottom-30)/geomCoef},  RGB(20,20,20), {0});
    CSBIND_GEOM_PARAMS bd = {id, {-1,-1,1,1}, {BIND_DEST_RIGHT_EDGE, BIND_DEST_BOTTOM_EDGE,
        BIND_DEST_RIGHT_EDGE, BIND_DEST_BOTTOM_EDGE}};
    bindGeometry(idp, bd);*/

    /*CSSCROLLBAR hscrollAbout = CSUIOBJECTS::addHScrollBar(&id, &id, 0, 10);
    CSSCROLLBAR vscrollAbout = CSUIOBJECTS::addVScrollBar(&id, &id, 0, 10);
    hscrollAbout.setViewFrameBottomMarging(20);
    vscrollAbout.setViewFrameRightMarging(20);*/

    csGraphics::setGraphicAreaPosition(idp, {2,5});
    csGraphics::setGraphicAreaColor(idp, {25,25,25}, {30,30,30});
    csGraphics::setGraphicAreaSize(idp, {GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)*2.5});
    csGraphics::updateGraphicArea(idp, 1);
    csGraphics::setMouseWheelPreference(idp, CS_MOUSEWHEEL_VSCROLL);

    SetTimer(sHandle(idp), 0, 12,0);

    int* idptr = new int(idp);
    CSINPUT* inp = csNewInputContext(idptr);

//inp->hide();

    int leftpos = 5, height = 24, topMarg = 10;
    for(int i=0; i<100; i++)
        templateInput(inp, {leftpos,topMarg+i*(height+2), (r.right-15),height});

//inp->show();

    return inp;
}
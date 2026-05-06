#include "csInput.h"
#include "csUtils.h"
#include "csStrUtils.h"
#include "csUIObjects.h"
#include "string.h"
#include "uxtheme.h"
#include "windef.h"
#include "windows.h"
#include <vector>

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
extern CSAPP_STRINGS appStrings;
extern bool loadStrings;

extern vector<RECT> bltRect;

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
    actionParams.count = 0;
    actionParams.hcount = 0;
    actionParams.count_ = 0;
    actionParams.count_sizeMove = 0;
    actionParams.pmouse = {0,0};
    actionParams.psave = {0,0};
    actionParams.r_sizeMove = {0,0,0,0};

    xmarging = 3;
    ymarging = 3;
    titleFont = CreateFontW(14, 0, 0, 1,FW_BOLD,1, 0,0,0,0,0,CLEARTYPE_QUALITY,
        DEFAULT_PITCH|FF_DONTCARE, (LPCWSTR)"Book Antiqua");
    textFont = CreateFontW(14, 0, 0, 1,0,0, 0,0,0,0,0,CLEARTYPE_QUALITY,
        DEFAULT_PITCH|FF_DONTCARE, (LPCWSTR)"Arial");
    checkCount = 0;
    startChecking = 0;
    ctxRect = {INT_MAX, INT_MAX, 0, 0};
    logSection = nullptr;
    globalModifiable = true;
}

bool CSINPUT::isMouseHoveringNoNameButton(int idInput)
{
    if(idInput<0) idInput = ip.size() + idInput;
    return ip[idInput]->btnNoNameOn;
}

void CSINPUT::setSwitchable(int idInput, bool state)
{
    if(idInput<0) idInput = ip.size() + idInput;
    ip[idInput]->switchableState = state;
}

bool CSINPUT::getSwitchable(int idInput)
{
    if(idInput<0) idInput = ip.size() + idInput;
    return ip[idInput]->switchableState;
}

void CSINPUT::setNoNameButtonVariableState(int idInput, bool state)
{
    if(idInput<0) idInput = ip.size() + idInput;
    ip[idInput]->noNameButtonVariableState = state;
}

void CSINPUT::setNoNameButtonState(bool state)
{
    globalNoNameButtonState = state;
}
void CSINPUT::setNoNameButtonVariableState(bool state)
{
    globalNoNameButtonVariableState = state;
}

void CSINPUT::setTitle(int idInput, wchar_t* title)
{
    if(idInput<0) idInput = ip.size() + idInput;
    free(ip[idInput]->title);
    ip[idInput]->title = makeWString(title);
}

void CSINPUT::setTitleColors(int idInput, CSRGBA colorNormal, CSRGBA colorHover, CSRGBA colorActive, CSRGBA colorDisabled)
{
    if(idInput<0) idInput = ip.size() + idInput;
    ip[idInput]->colorNormal_title = colorNormal;
    ip[idInput]->colorHover_title = colorHover;
    ip[idInput]->colorActive_title = colorActive;
    ip[idInput]->colorDisabled_title = colorDisabled;

    if(idInput == idActive)
    {
        switchInputState();
    }
}

void CSINPUT::setTitleFrameColors(int idInput, CSRGBA colorNormal, CSRGBA colorHover, CSRGBA colorActive, CSRGBA colorDisabled)
{
    if(idInput<0) idInput = ip.size() + idInput;
    ip[idInput]->colorNormal_titleFrame = colorNormal;
    ip[idInput]->colorHover_titleFrame = colorHover;
    ip[idInput]->colorActive_titleFrame = colorActive;
    ip[idInput]->colorDisabled_titleFrame = colorDisabled;

    if(idInput == idActive)
    {
        switchInputState();
    }
}
void CSINPUT::setTitleFrameBorderColors(int idInput, CSRGBA colorNormal, CSRGBA colorHover, CSRGBA colorActive, CSRGBA colorDisabled)
{
    if(idInput<0) idInput = ip.size() + idInput;
    ip[idInput]->colorNormal_titleFrameBorder = colorNormal;
    ip[idInput]->colorHover_titleFrameBorder = colorHover;
    ip[idInput]->colorActive_titleFrameBorder = colorActive;
    ip[idInput]->colorDisabled_titleFrameBorder = colorDisabled;

    if(idInput == idActive)
    {
        switchInputState();
    }
}

void CSINPUT::setTextColors(int idInput, CSRGBA colorNormal, CSRGBA colorHover, CSRGBA colorActive, CSRGBA colorDisabled)
{
    if(idInput<0) idInput = ip.size() + idInput;
    ip[idInput]->colorNormal_text = colorNormal;
    ip[idInput]->colorHover_text = colorHover;
    ip[idInput]->colorActive_text = colorActive;
    ip[idInput]->colorDisabled_text = colorDisabled;

    if(idInput == idActive)
    {
        switchInputState();
    }
}

void CSINPUT::setFrameColors(int idInput, CSRGBA colorNormal, CSRGBA colorHover, CSRGBA colorActive, CSRGBA colorDisabled)
{
    if(idInput<0) idInput = ip.size() + idInput;
    ip[idInput]->colorNormal_frame = colorNormal;
    ip[idInput]->colorHover_frame = colorHover;
    ip[idInput]->colorActive_frame = colorActive;
    ip[idInput]->colorDisabled_frame = colorDisabled;

    if(idInput == idActive)
    {
        switchInputState();
    }
}

void CSINPUT::setFrameBorderColors(int idInput, CSRGBA colorNormal, CSRGBA colorHover, CSRGBA colorActive, CSRGBA colorDisabled)
{
    if(idInput<0) idInput = ip.size() + idInput;
    ip[idInput]->colorNormal_frameBorder = colorNormal;
    ip[idInput]->colorHover_frameBorder = colorHover;
    ip[idInput]->colorActive_frameBorder = colorActive;
    ip[idInput]->colorDisabled_frameBorder = colorDisabled;

    if(idInput == idActive)
    {
        switchInputState();
    }
}

CSINPUT::CSINPUT_ENTITY_COLORS CSINPUT::getTitleColors(int idInput)
{
    if(idInput<0) idInput = ip.size() + idInput;
    return {ip[idInput]->colorNormal_title, ip[idInput]->colorHover_title, ip[idInput]->colorActive_title, ip[idInput]->colorDisabled_title};
}

bool CSINPUT::getNoNameButtonState(int idInput)
{
    if(idInput<0) idInput = ip.size() + idInput;
    return ip[idInput]->noNameButtonState;
}

void CSINPUT::setTextRectLeftMargin(int idInput, int margin)
{
    if(idInput<0) idInput = ip.size() + idInput;
    ip[idInput]->textRectLeftMargin = margin;
    ip[idInput]->textRectLeftMargin2 = margin;
}

void CSINPUT::setTextAlign(int idInput, int align)
{
    if(idInput<0) idInput = ip.size() + idInput;
    ip[idInput]->textAlign = align;
}

void CSINPUT::setTextFontSizeCoef(int idInput, float coef)
{
    if(idInput<0) idInput = ip.size() + idInput;
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];
    inp->textFontSizeCoef = coef;
    inp->charHeight = (inp->rsize.cy-2*ymarging)*inp->textFontSizeCoef-3;
    inp->textFont = CreateFontW(inp->charHeight, 0, 0, 10,FW_MEDIUM,0, 0,0,0,0,0,CLEARTYPE_QUALITY,
        DEFAULT_PITCH|FF_DONTCARE, (LPCWSTR)"Arial");
    inp->textNoteFont = CreateFontW(inp->charHeight-1, 0, 0, 10,FW_MEDIUM,1, 0,0,0,0,0,CLEARTYPE_QUALITY,
        DEFAULT_PITCH|FF_DONTCARE, (LPCWSTR)"Arial");
    
}
void CSINPUT::setTitleFontSizeCoef(int idInput, float coef)
{
    if(idInput<0) idInput = ip.size() + idInput;
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];
    inp->titleFontSizeCoef = coef;
    inp->titleHeight = (inp->rsize.cy-2*ymarging)*inp->titleFontSizeCoef-3;
    inp->titleFont = CreateFontW(inp->titleHeight, 0, 0, 10,FW_BOLD,1, 0,0,0,0,0,CLEARTYPE_QUALITY,
        DEFAULT_PITCH|FF_DONTCARE, (LPCWSTR)"Century Gothic");

    LPSIZE lps = textExtentW(*parent, inp->titleFont, inp->title);
    inp->titleWidth = lps->cx;
    delete lps;
    
}

void CSINPUT::setTitleAlign(int idInput, int align)
{
    if(idInput<0) idInput = ip.size() + idInput;
    ip[idInput]->titleAlign = align;
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


void CSINPUT::newInput(wchar_t*title, wchar_t*textNote, RECT geometry, int style, int titleRectWidth, int titleRectHeight)
{
    CSINPUT::CSINPUT_PARAMS ipp;

    idLastActive = idActive;
    idActive = ip.size();
    ipp.buttonMarging = 2;
    ipp.textRectEnd = geometry.right + geometry.left - xmarging;
    ipp.title = makeWString(title);
    ipp.textNote = makeWString(textNote);
    
    ipp.style = style;
    ipp.rect = {geometry.left, geometry.top, ipp.textRectEnd + xmarging, geometry.bottom+geometry.top};
    ipp.rsize = {geometry.right, geometry.bottom};
    ipp.titleRectWidth = titleRectWidth;
    ipp.titleRectHeight = titleRectHeight;
    ipp.textFontSizeCoef = 1.0f;
    ipp.charHeight = (geometry.bottom-2*ymarging)*ipp.textFontSizeCoef-3;
    ipp.editable = globalModifiable;
    ipp.titleFont = CreateFontW(ipp.charHeight, 0, 0, 10,FW_BOLD,0, 0,0,0,0,0,CLEARTYPE_QUALITY,
        DEFAULT_PITCH|FF_DONTCARE, (LPCWSTR)"Book Antiqua");
    ipp.textFont = CreateFontW(ipp.charHeight, 0, 0, 10,FW_MEDIUM,0, 0,0,0,0,0,CLEARTYPE_QUALITY,
        DEFAULT_PITCH|FF_DONTCARE, (LPCWSTR)"Arial");
    ipp.textNoteFont = CreateFontW(ipp.charHeight-1, 0, 0, 10,FW_MEDIUM,1, 0,0,0,0,0,CLEARTYPE_QUALITY,
        DEFAULT_PITCH|FF_DONTCARE, (LPCWSTR)"Arial");
    ipp.noNameButtonState = globalNoNameButtonState;
    ipp.noNameButtonVariableState = globalNoNameButtonVariableState;
    //cout<<"how \n";

    wchar_t* str = L"A\0";
    LPSIZE lps = textExtentW(*parent, ipp.textFont, str);
    ipp.charWidth = lps->cx;
    ipp.charHeight = lps->cy;
    delete lps;

    if(textNote)
    {
        LPSIZE lps = textExtentW(*parent, ipp.textFont, textNote);
        ipp.textNoteExtend = lps->cx;
        delete lps;
    }

    ipp.textDc = CreateCompatibleDC(parentDC);

    lps = textExtentW(*parent, ipp.titleFont, ipp.title);
    ipp.titleHeight = lps->cy;
    ipp.titleWidth = lps->cx;
    delete lps;

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

    switchInputState();
}

void CSINPUT::textCharGeometryCorrection(int from)
{
    vector<RECT>& r = ip[idActive]->textCharGeometry;

    int n = r.size();
    int d;
    for(int i=from; i<n; i++)
    {
        d = r[i].right - r[i].left;
        r[i].left = r[i-1].right;
        r[i].right = r[i].left + d;
    }
}

int CSINPUT::addChar(wchar_t chr)
{
    CSINPUT::CSINPUT_PARAMS* inp = ip[idActive];

    if(!inp->editable)
    {
        if(logSection)
        {
            CSSECMAN::setTitle(*logSection, CSTEXT{.Text=L"Ce champ n'est pas editable"}, 1);
            InvalidateRect(sHandle(*logSection), 0, 1);
        }
        return 0;
    }

    if(inp->forbiddenChars.size() > 0)
    {
        if(inp->forbiddenChars.find(chr) != wstring::npos)
        {
            return 0;
        }
    }
    else if(inp->allowedChars.size() > 0)
    {
        if(inp->allowedChars.find(chr) == wstring::npos)
        {
            return 0;
        }
    }
    else if(globalForbiddenChars.size() > 0)
    {
        if(globalForbiddenChars.find(chr) != wstring::npos)
        {
            return 0;
        }
    }
    else if(globalAllowedChars.size() > 0)
    {
        if(globalAllowedChars.find(chr) == wstring::npos)
        {
            return 0;
        }
    }

    HDC& hdc = ip[idActive]->frameDc;

    int pos = ip[idActive]->caretPos;

    wchar_t _chr[2];
    wsprintf(_chr,L"%c\0", chr);
    LPSIZE lps = textExtentW(*parent, inp->textFont, _chr);
    lps->cx += inp->charSpacing;
    inp->charWidth = lps->cx;
    
    RECT r = pos > 0 ? inp->textCharGeometry[pos-1] : (RECT){2,0,2,0};
    inp->textCharGeometry.insert(inp->textCharGeometry.begin() + pos, {r.right, r.top, r.right+lps->cx, lps->cy});
    textCharGeometryCorrection(pos+1);
    inp->textChar.insert(inp->textChar.begin() + pos, chr);
    ip[idActive]->caretPos++;
    
    delete lps;

    DeleteDC(inp->textDc);
    DeleteBitmap(inp->textBmp);
    inp->textDcWidth += inp->charWidth;
    inp->textDc = CreateCompatibleDC(parentDC);
    int prefixLength = inp->prefix ? wcslen(inp->prefix) : 0;
    int suffixLength = inp->suffix ? wcslen(inp->suffix) : 0;
    inp->textBmp = CreateCompatibleBitmap(parentDC, inp->textDcWidth+(prefixLength+suffixLength)*inp->charWidth,  inp->charHeight);

    SelectBitmap(inp->textDc, inp->textBmp);
    SelectFont(inp->textDc, inp->textFont);
    //RECT rt = inp->textRect;
    //drawGdiRectangle(inp->textDc, inp->colorNormal_titleFrame, inp->colorNormal_titleFrameBorder, {0,0,inp->textDcWidth,inp->charHeight});
    
    SetTextColor(inp->textDc, RGB(inp->colorNormal_text.r,inp->colorNormal_text.g,inp->colorNormal_text.b));
    SetBkColor(inp->textDc,RGB(inp->colorNormal_frame.r,inp->colorNormal_frame.g,inp->colorNormal_frame.b));
    //SetBkMode(inp->textDc, TRANSPARENT);

    //wchar_t*_str = inp->textChar.toStringW(0,inp->textChar.size()-1);
    const wchar_t*_str = inp->textChar.c_str();

    if(inp->prefix || inp->suffix)
    {
        wstring _strCopy;
        
        if(inp->prefix)
        {
            _strCopy.insert(0, inp->prefix);
        }
        _strCopy.insert(inp->textChar.size(), _str);
        if(inp->suffix)
        {
            _strCopy.insert(inp->textChar.size(), inp->suffix);
        }
        //wchar_t*str = strList.toStringW(0, strList.size()-1);
        const wchar_t*str = _strCopy.c_str();
        SetTextCharacterExtra(inp->textDc, inp->charSpacing);
        TextOutW(inp->textDc, 0, 0, str, _strCopy.size());
    }
    else
    {   
        SetTextCharacterExtra(inp->textDc, inp->charSpacing);
        TextOutW(inp->textDc, 0, 0, _str, inp->textChar.size());
    }

    //section to view in textRect
    updateText(idActive);

    return inp->charWidth;
}

int CSINPUT::deleteChar(bool frontCaret)
{

    CSINPUT::CSINPUT_PARAMS* inp = ip[idActive];
    if(!inp->editable)
    {
        if(logSection)
        {
            CSSECMAN::setTitle(*logSection, CSTEXT{.Text=L"Ce champ n'est pas editable."}, 1);
            InvalidateRect(sHandle(*logSection), 0, 1);
        }
        return 0;
    }
    HDC& hdc = ip[idActive]->frameDc;
    wchar_t _chr[2];
    RECT rchar;
    if(frontCaret)
    {
        int pos = ip[idActive]->caretPos-1;

        RECT r = inp->textCharGeometry[pos];
        rchar = r;
        inp->charWidth = r.right-r.left;

        inp->textChar.erase(inp->textChar.begin() + pos);
        inp->textCharGeometry.erase(inp->textCharGeometry.begin() + pos);
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

        inp->textChar.erase(inp->textChar.begin() + pos);
        inp->textCharGeometry.erase(inp->textCharGeometry.begin() + pos);
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
        int prefixLength = inp->prefix ? wcslen(inp->prefix) : 0;
        int suffixLength = inp->suffix ? wcslen(inp->suffix) : 0;
        inp->textBmp = CreateCompatibleBitmap(parentDC, inp->textDcWidth+(prefixLength+suffixLength)*inp->charWidth,  inp->charHeight);

        SelectBitmap(inp->textDc, inp->textBmp);
        SelectFont(inp->textDc, inp->textFont);
        //RECT rt = inp->textRect;
        //drawGdiRectangle(inp->textDc, inp->colorNormal_titleFrame, inp->colorNormal_titleFrameBorder, {0,0,inp->textDcWidth,inp->charHeight});
    
        SetTextColor(inp->textDc, RGB(inp->colorNormal_text.r,inp->colorNormal_text.g,inp->colorNormal_text.b));
        SetBkColor(inp->textDc,RGB(inp->colorNormal_frame.r,inp->colorNormal_frame.g,inp->colorNormal_frame.b));
        //SetBkMode(inp->textDc, TRANSPARENT);

        const wchar_t*_str = inp->textChar.c_str();
        
        if(inp->prefix || inp->suffix)
        {
            wstring _strCopy;
            
            if(inp->prefix)
            {
                _strCopy.insert(0, inp->prefix);
            }
            _strCopy.insert(inp->textChar.size(), _str);
            if(inp->suffix)
            {
                _strCopy.insert(inp->textChar.size(), inp->suffix);
            }
            const wchar_t*str = _strCopy.c_str();
            SetTextCharacterExtra(inp->textDc, inp->charSpacing);
            TextOutW(inp->textDc, 0, 0, str, wcslen(str));
        }
        else
        {
            SetTextCharacterExtra(inp->textDc, inp->charSpacing);
            TextOutW(inp->textDc, 0, 0, _str, wcslen(_str));
        }
        

        RECT r = inp->rect;
        SIZE s = inp->rsize;
        BitBlt(parentDC, r.left, r.top, s.cx, s.cy, hdc, 0,0,SRCCOPY);

       /* r = inp->textRect;
        BitBlt(parentDC, r.left+2, r.top, r.right-r.left-4, r.bottom-r.top, inp->textDc, 0,0,SRCCOPY);
        //InvalidateRect(sHandle(*parent), &inp->textRect, 1);*/

        updateText(idActive);
    }
    else
    {
        updateActiveTextBkg(); // erase last wchar_t on the textDc
        viewTextNote(idActive);
    }

    return inp->charWidth;
}

void CSINPUT::updateText(int idInput)
{
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];
    RECT r = inp->textRect;

    r.top += (r.bottom - r.top - inp->charHeight) / 2;
    if(inp->textAlign == CS_INPUT_TA_LEFT)
    {
        inp->textRectLeftMargin = inp->textRectLeftMargin2;
    }
    else if(inp->textAlign == CS_INPUT_TA_CENTER)
    {
        inp->textRectLeftMargin = (r.right - r.left - 4 - inp->textDcWidth) / 2;
        if(inp->textRectLeftMargin < 0) inp->textRectLeftMargin = inp->textRectLeftMargin2;
    }
    else if(inp->textAlign == CS_INPUT_TA_RIGHT)
    {
        inp->textRectLeftMargin = (r.right - r.left - 4 - (inp->textDcWidth));
        if(inp->textRectLeftMargin < 0) inp->textRectLeftMargin = inp->textRectLeftMargin2;
    }

    int cx = r.right - (r.left + inp->textRectLeftMargin) - 4;
    
    int delta = inp->textDcWidth - cx;
    if(delta < 0) 
    {
        BitBlt(parentDC, r.left+2+inp->textRectLeftMargin, r.top, inp->textDcWidth, inp->charHeight, inp->textDc, 0,0, SRCCOPY);
        inp->showedSectionRect = {0,0,inp->textDcWidth, inp->charHeight};
    }
    else
    {//cout<<"delta < 0\n";
        BitBlt(parentDC, r.left+2+inp->textRectLeftMargin, r.top, inp->textDcWidth, inp->charHeight, inp->textDc, delta,0, SRCCOPY);
        inp->showedSectionRect = {delta,0, inp->textDcWidth, inp->charHeight};
    }
}

void CSINPUT::highlightActiveChar(bool state)
{
    
    CSINPUT::CSINPUT_PARAMS* inp = ip[idActive];
    if(!inp->editable) return;
    if(inp->textChar.size() == 0) return;
    {

        wchar_t _str[2];
        SelectFont(parentDC, inp->textFont);
        
        if(state)
        {
            SetTextColor(parentDC, RGB(inp->activeCharColor.r,inp->activeCharColor.g,inp->activeCharColor.b));
            SetBkColor(parentDC, RGB(inp->activeCharBkgColor.r,inp->activeCharBkgColor.g,inp->activeCharBkgColor.b));
        }
        else
        {
            SetTextColor(parentDC, RGB(inp->colorNormal_text.r,inp->colorNormal_text.g,inp->colorNormal_text.b));
            SetBkColor(parentDC, RGB(inp->colorNormal_titleFrame.r,inp->colorNormal_titleFrame.g,inp->colorNormal_titleFrame.b));
        }

        int pos = inp->caretPos-1;

        RECT textRect = inp->textRect;
        textRect.left += inp->textRectLeftMargin;
        RECT r = inp->textCharGeometry[pos];

        int cx = (textRect.right-textRect.left);
        int dx = inp->textCharGeometry[inp->textCharGeometry.size()-1].right - cx;
        if(dx > 0)
        {
            r.left = textRect.right - (inp->textCharGeometry[inp->textCharGeometry.size()-1].right - r.left);
            r.right = textRect.right - (inp->textCharGeometry[inp->textCharGeometry.size()-1].right - r.right);
        }
        else
        {
            r.left += textRect.left;
            r.right += textRect.left;
        }
        r.top += textRect.top;
        r.bottom += textRect.top;

        r.top += (textRect.bottom - textRect.top - inp->charHeight) / 2;

        if(pos>=0)
        {
            wsprintf(_str, L"%c\0", inp->textChar[pos]);
            TextOutW(parentDC, r.left, r.top, _str, 1);
        }

        pos = inp->caretPos;
        if(pos < inp->textChar.size())
        {
            wsprintf(_str, L"%c\0", inp->textChar[pos]);
            TextOutW(parentDC, r.right, r.top, _str, 1);
        }
        InvalidateRect(sHandle(*parent), &textRect, 1);
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

void CSINPUT::addNoNameButton(int _idInput, wchar_t*path11, wchar_t*path12, wchar_t*path13, wchar_t*path14)
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
            inp->noNameRect = {inp->textRectEnd + bmarg, r.top + ymarging, inp->textRectEnd + bmarg + buttonWidth, r.top + ymarging + buttonWidth};
            break;
        case 1:
            inp->textRectEnd -= (bmarg + buttonWidth);
            inp->titleRect = {r.left + xmarging, r.top + ymarging, r.left+xmarging+titleRectWidth, r.top+ymarging+titleRectHeight};
            inp->textRect = {r.left + xmarging, inp->titleRect.bottom + bmarg, inp->textRectEnd, r.bottom-ymarging};
            inp->noNameRect = {inp->textRectEnd + bmarg, inp->titleRect.bottom + bmarg, inp->textRectEnd + bmarg + buttonWidth, r.top + ymarging + buttonWidth};
            break;
    }

    wchar_t* p12 = (path12 == 0) ? path11 : path12;
    wchar_t* p13 = (path13 == 0) ? path11 : path13;
    wchar_t* p14 = (path14 == 0) ? path11 : path14;
    
    SIZE sz = {inp->noNameRect.right-inp->noNameRect.left,inp->noNameRect.bottom-inp->noNameRect.top};
    
    inp->noNameImage1 = createGraphicContextResizedFromFileW(path11, sz);
    inp->noNameImage2 = createGraphicContextResizedFromFileW(p12, sz);
    inp->noNameImage3 = createGraphicContextResizedFromFileW(p13, sz);
    inp->noNameImage4 = createGraphicContextResizedFromFileW(p14, sz);

    
}

CSLINEAR_BIND* CSINPUT::CSLINEAR_BIND_PTR(CSLINEAR_BIND lb)
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

    
    inp->gbpFrame = CSLINEAR_BIND_PTR({left,top,right,bottom});

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

    if(inp->noNameRect.right)
    {
        left = (bindRight == 1) ? (CSL){0,inp->noNameRect.left/float(r.right),0} : 
                                  ((bindRight == 2) ? (CSL){float(r.right)-inp->noNameRect.left,1,0} : (CSL){0,0,inp->noNameRect.left});
        top = (bindTop == 1) ? (CSL){0,inp->noNameRect.top/float(r.bottom),0} : 
                                    ((bindTop == 2) ? (CSL){float(r.bottom)-inp->noNameRect.top,1,0} : (CSL){0,0,inp->noNameRect.top});
        right = (bindRight == 1) ? (CSL){0,inp->noNameRect.right/float(r.right),0} : 
                                    ((bindRight == 2) ? (CSL){float(r.right)-inp->noNameRect.right,1,0} : (CSL){0,0,inp->noNameRect.right});
        bottom = (bindBottom == 1) ? (CSL){0,inp->noNameRect.bottom/float(r.bottom),0} : 
                                    ((bindBottom == 2) ? (CSL){float(r.bottom)-inp->noNameRect.bottom,1,0} : (CSL){0,0,inp->noNameRect.bottom});

        inp->gbpNoName = CSLINEAR_BIND_PTR({left,top,right,bottom});
    }




    r = inp->textRect;

    r.top += (r.bottom - r.top - inp->charHeight) / 2;
    if(inp->textAlign == CS_INPUT_TA_LEFT)
    {
        inp->textRectLeftMargin = inp->textRectLeftMargin2;
    }
    else if(inp->textAlign == CS_INPUT_TA_CENTER)
    {
        inp->textRectLeftMargin = (r.right - r.left - 4 - inp->textDcWidth) / 2;
        if(inp->textRectLeftMargin < 0) inp->textRectLeftMargin = inp->textRectLeftMargin2;
    }
    else if(inp->textAlign == CS_INPUT_TA_RIGHT)
    {
        inp->textRectLeftMargin = (r.right - r.left - 4 - (inp->textDcWidth));
        if(inp->textRectLeftMargin < 0) inp->textRectLeftMargin = inp->textRectLeftMargin2;
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
    if(!inp->editable)
        return {0};
    int n = inp->textCharGeometry.size();
    RECT r={-1000,-1000};
    if(n)
    {
        for(int i=0; i<n; i++)
        {
            r = inp->textCharGeometry[i];
            r.left -= inp->showedSectionRect.left;
            r.right -= inp->showedSectionRect.left;
            r.left += inp->textRect.left+2+inp->textRectLeftMargin;
            r.right += inp->textRect.left +2+inp->textRectLeftMargin;

            r.top += inp->textRect.top;
            r.bottom += inp->textRect.top;

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
            /*r = inp->textCharGeometry[0];
            r.left -= inp->showedSectionRect.left;
            r.right -= inp->showedSectionRect.left;
            r.left += inp->textRect.left+2;
            r.right += inp->textRect.left +2;

            r.top += inp->textRect.top;
            r.bottom += inp->textRect.top;*/
        }
        /*else
        {
            r={-1000,-1000};
        }*/
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
    return {0, 0, 0, 0};
}

void CSINPUT::_updateGeometry(int idInput)
{
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];

    if(inp->gbpFrame)
    {
        CSLINEAR_BIND*lb = inp->gbpFrame;
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

        lb = inp->gbpNoName;
        if(lb)
        {
            r = &inp->noNameRect;
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
    r.left += ip[idActive]->textRectLeftMargin+2;
    CSRGBA bcol = ip[idActive]->colorNormal_frame;
    CSRGBA brdcol = ip[idActive]->colorNormal_frameBorder;
    drawGdiRectangle(parentDC, brdcol, bcol, r);

}
void CSINPUT::viewTextNote(int id)
{
    CSINPUT::CSINPUT::CSINPUT_PARAMS* inp = ip[id];
    RECT r = inp->textRect;
    r.left += inp->textRectLeftMargin+2;

    if(inp->textAlign == CS_INPUT_TA_CENTER)
    {
        r.left -= (inp->textNoteExtend) / 2 + 2;
    }
    else if(inp->textAlign == CS_INPUT_TA_RIGHT)
    {
        r.left -= inp->textNoteExtend + 2;
    }

    r.top += (r.bottom - r.top - inp->charHeight) / 2;

    CSRGBA c = inp->textNoteColor;
    SetBkMode(inp->frameDc, TRANSPARENT);
    SetTextColor(inp->frameDc, RGB(c.r,c.g,c.b));
    SelectFont(inp->frameDc, inp->textNoteFont);
    
    RECT rt = csRectContextualized(r, inp->rect);
    DrawTextW(inp->frameDc, ip[id]->textNote, wcslen(ip[id]->textNote), &rt, DT_WORD_ELLIPSIS);
}

void CSINPUT::updateActiveTextBkg(RECT rchar)
{
    CSRGBA bcol = ip[idActive]->colorHover_titleFrame;
    CSRGBA brdcol = ip[idActive]->colorHover_titleFrameBorder;
    RECT r = ip[idActive]->textRect;
    drawGdiRectangle(ip[idActive]->frameDc, brdcol, bcol, {rchar.left+r.left, rchar.top+1, rchar.right+r.left, rchar.bottom-1});
    //drawGdiRectangle(parentDC, brdcol, bcol, rchar);
}

void CSINPUT::createContext(int idInput)
{
    CSINPUT::CSINPUT::CSINPUT_PARAMS* ipp = ip[idInput];
    if(ipp->frameDc)
    {
        DeleteDC(ipp->frameDc);
        DeleteBitmap(ipp->frameBmp);
    }
    parentDC = csGraphics::getGraphicAreaContext(*parent);
    ipp->frameDc = CreateCompatibleDC(parentDC);
    //std::cout<<ipp->rsize.cx<<" "<<ipp->rsize.cy<<"\n";
    ipp->frameBmp = CreateCompatibleBitmap(parentDC, ipp->rsize.cx, ipp->rsize.cy);
    SelectBitmap(ipp->frameDc, ipp->frameBmp);
    
    /*RECT r = ipp->upRect;
    CSGRAPHIC_CONTEXT dc = ipp->incUpImage2;
    if(r.bottom)
    {
        int cx = r.right-r.left;
        int cy = r.bottom-r.top;
        //cout<<dc.sz.width<<"  "<<dc.sz.height<<" -- \n";
        StretchBlt(ipp->frameDc, r.left-ctxRect.left, r.top-ctxRect.top, cx, cy, dc.dc, 0,0,dc.sz.width,dc.sz.height, SRCCOPY);
        //StretchBlt(parentDC, ctxRect.left, ctxRect.top, cx, cy, ipp->frameDc, 0,0,dc.sz.width,dc.sz.height, SRCCOPY);
        //InvalidateRect(sHandle(*parent), &r, 1);
    }
    drawImage(ipp->frameDc, ipp->incUpImage2, ipp->upRect);
    drawImage(ipp->frameDc, ipp->incDownImage2, ipp->downRect);
    drawImage(ipp->frameDc, ipp->undoImage2, ipp->undoRect);
    drawImage(ipp->frameDc, ipp->redoImage2, ipp->redoRect);
    drawImage(ipp->frameDc, ipp->unrollImage2, ipp->unrollRect);
    drawImage(ipp->frameDc, ipp->noNameImage2, ipp->noNameRect);*/


}

RECT csRectContextualized(RECT r, RECT rparent)
{
    return {r.left-rparent.left, r.top-rparent.top, r.right-rparent.left, r.bottom-rparent.top};
}

void CSINPUT::_updateVisual_state1(int idInput, bool noRedrawActiveChar)
{
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];
    RECT r = inp->rect;
    CSRGBA bcol = inp->colorNormal_frame;
    CSRGBA brdcol = inp->colorNormal_frameBorder;
    drawGdiRectangle(inp->frameDc, brdcol, bcol, csRectContextualized(r,r));
    if(inp->textChar.size() == 0)
    {
        viewTextNote(idInput);
    }
    //updateActiveTextBkg(csRectContextualized(inp->textRect, r));
    
    bcol = inp->colorNormal_titleFrame;
    brdcol = inp->colorNormal_titleFrameBorder;
    drawGdiRectangle(inp->frameDc, brdcol, bcol, csRectContextualized(inp->titleRect,r));

    RECT trect = csRectContextualized(inp->titleRect,r);

    if(inp->titleAlign == CS_INPUT_TA_LEFT)
    {
        trect.left += 2;
    }
    if(inp->titleAlign == CS_INPUT_TA_CENTER)
    {
        trect.left = (trect.right - inp->titleWidth) / 2;
    }
    else if(inp->titleAlign == CS_INPUT_TA_RIGHT)
    {
        trect.left = trect.right - inp->titleWidth;
    }

    RECT rtext = inp->textRect;
    trect.top += (rtext.bottom - rtext.top - inp->titleHeight) / 2;

    drawGdiText(inp->frameDc, inp->title, inp->titleFont, inp->colorNormal_title, trect);

    drawImage(inp->frameDc, inp->incUpImage2, csRectContextualized(inp->upRect, r));
    drawImage(inp->frameDc, inp->incDownImage2, csRectContextualized(inp->downRect, r));
    drawImage(inp->frameDc, inp->undoImage2, csRectContextualized(inp->undoRect, r));
    drawImage(inp->frameDc, inp->redoImage2, csRectContextualized(inp->redoRect, r));
    drawImage(inp->frameDc, inp->unrollImage2, csRectContextualized(inp->unrollRect, r));
    drawImage(inp->frameDc, inp->noNameImage2, csRectContextualized(inp->noNameRect, r));
//cout<<inp->frameDc<<" "<<r.top<<"\n";
}


void CSINPUT::updateVisual_state1(int idInput, bool noRedrawActiveChar)
{
    _updateVisual_state1(idInput, noRedrawActiveChar);
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];
    RECT r = inp->rect;
    SIZE s = inp->rsize;
    BitBlt(parentDC, r.left, r.top, s.cx, s.cy, inp->frameDc, 0,0,SRCCOPY);
    updateText(idInput);
    /*if(!noRedrawActiveChar)
        highlightActiveChar();*/
    //if(r.left == s.cx || r.top == s.cy) cout<<" less than zero !\n";
    //InvalidateRect(sHandle(*parent), &r, 1);
}


void CSINPUT::_updateVisual_state2(int idInput, bool noRedrawActiveChar)
{
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];

    RECT r = inp->rect;
    CSRGBA bcol = inp->colorHover_frame;
    CSRGBA brdcol = inp->colorHover_frameBorder;
    drawGdiRectangle(inp->frameDc, brdcol, bcol, csRectContextualized(r,r));
    
    bcol = inp->colorHover_titleFrame;
    brdcol = inp->colorHover_titleFrameBorder;
    drawGdiRectangle(inp->frameDc, brdcol, bcol, csRectContextualized(inp->titleRect,r));

    RECT trect = csRectContextualized(inp->titleRect,r);

    if(inp->titleAlign == CS_INPUT_TA_LEFT)
    {
        trect.left += 2;
    }
    if(inp->titleAlign == CS_INPUT_TA_CENTER)
    {
        trect.left = (trect.right - inp->titleWidth) / 2;
    }
    else if(inp->titleAlign == CS_INPUT_TA_RIGHT)
    {
        trect.left = trect.right - inp->titleWidth;
    }
    
    RECT rtext = inp->textRect;
    trect.top += (rtext.bottom - rtext.top - inp->titleHeight) / 2  - 2; // -2 ajoute un effet de mouvement sur le titre

    drawGdiText(inp->frameDc, inp->title, inp->titleFont, inp->colorHover_title, trect);


    drawImage(inp->frameDc, inp->incUpImage2, csRectContextualized(inp->upRect, r));
    drawImage(inp->frameDc, inp->incDownImage2, csRectContextualized(inp->downRect, r));
    drawImage(inp->frameDc, inp->undoImage2, csRectContextualized(inp->undoRect, r));
    drawImage(inp->frameDc, inp->redoImage2, csRectContextualized(inp->redoRect, r));
    drawImage(inp->frameDc, inp->unrollImage2, csRectContextualized(inp->unrollRect, r));
    drawImage(inp->frameDc, inp->noNameImage2, csRectContextualized(inp->noNameRect, r));
}

void CSINPUT::updateVisual_state2(int idInput, bool noRedrawActiveChar)
{
    _updateVisual_state2(idInput, noRedrawActiveChar);
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];
    RECT r = inp->rect;
    SIZE s = inp->rsize;
    //BitBlt(parentDC, rv.left, rv.top, stackDC.sz.cx, stackDC.sz.cy, stackDC.dc, 0,0,SRCCOPY);
    BitBlt(parentDC, r.left, r.top, s.cx, s.cy, inp->frameDc, 0,0,SRCCOPY);
    updateText(idInput);
    /*if(!noRedrawActiveChar)
        highlightActiveChar();*/
        
    /*if(inp->textChar.size() == 0)
    {
        viewTextNote(idInput);
    }*/
    //InvalidateRect(sHandle(*parent), &r, 1);
}

void CSINPUT::updateAll()
{
    int n = ip.size();
    CSLINEAR_BIND* lb = gbpCtx;
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
    
        //BitBlt(stackDC.dc, r1.left, r1.top, s.cx, s.cy, ip[i]->frameDc, 0,0,SRCCOPY);
        BitBlt(parentDC, r1.left, r1.top, s.cx, s.cy, ip[i]->frameDc, 0,0,SRCCOPY);
   
    }
    
    //BitBlt(parentDC, r->left,r->top, sectionSize.cx, sectionSize.cy, visualCtrl->dc, 0,0,SRCCOPY);
    update();
    //csFreeGraphicContextExt(*visualCtrl);
    //free(visualCtrl);
}

void CSINPUT::resetActiveInput()
{
    CSINPUT::CSINPUT_PARAMS* inp = ip[idMouseHover];
    inp->frameOn = 0;
    updateVisual_state1(idMouseHover);

    //redraw buttons
    inp->btnUpOn = 1;
    inp->btnDownOn = 1;
    inp->btnUndoOn = 1;
    inp->btnRedoOn = 1;
    inp->btnUnrollOn = 1;
    inp->btnNoNameOn = 1;
}

void CSINPUT::switchNoNameButtonState(int idInput)
{
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];
    if(inp->noNameButtonVariableState && inp->btnNoNameOn)
    {
        if(inp->noNameButtonState)
        {
            inp->noNameButtonState = 0;
        }
        else
        {
            inp->noNameButtonState = 1;
        }
        CSGRAPHIC_CONTEXT noNameImage = inp->noNameImage2;
        inp->noNameImage2 = inp->noNameImage3;
        inp->noNameImage3 = noNameImage;
        RECT r = inp->rect;
        drawImage(inp->frameDc, inp->noNameImage2, csRectContextualized(inp->noNameRect, r));
        updateVisual_state1(idInput);
        updateVisual_state2(idInput);
        InvalidateRect(sHandle(*parent), &inp->noNameRect, 1);
        
    }
}

void CSINPUT::switchInputState()
{
    CSINPUT::CSINPUT_PARAMS* inp = ip[idActive];
    if(inp->switchableState && inp->frameOn)
    {
        CSRGBA col = inp->colorActive_frame;
        inp->colorActive_frame = inp->colorNormal_frame;
        inp->colorNormal_frame = col;

        col = inp->colorActive_frameBorder;
        inp->colorActive_frameBorder = inp->colorNormal_frameBorder;
        inp->colorNormal_frameBorder = col;
        
        col = inp->colorActive_titleFrame;
        inp->colorActive_titleFrame = inp->colorNormal_titleFrame;
        inp->colorNormal_titleFrame = col;
        
        col = inp->colorActive_titleFrameBorder;
        inp->colorActive_titleFrameBorder = inp->colorNormal_titleFrameBorder;
        inp->colorNormal_titleFrameBorder = col;
        
        
        col = inp->colorActive_text;
        inp->colorActive_text = inp->colorNormal_text;
        inp->colorNormal_text = col;

        col = inp->colorActive_title;
        inp->colorActive_title = inp->colorNormal_title;
        inp->colorNormal_title = col;

        updateVisual_state1(idActive);

        inp = ip[idLastActive];
        if(ip.size() > 1 && inp->switchableState)
        {
            col = inp->colorActive_frame;
            inp->colorActive_frame = inp->colorNormal_frame;
            inp->colorNormal_frame = col;

            col = inp->colorActive_frameBorder;
            inp->colorActive_frameBorder = inp->colorNormal_frameBorder;
            inp->colorNormal_frameBorder = col;
            
            col = inp->colorActive_titleFrame;
            inp->colorActive_titleFrame = inp->colorNormal_titleFrame;
            inp->colorNormal_titleFrame = col;
            
            col = inp->colorActive_titleFrameBorder;
            inp->colorActive_titleFrameBorder = inp->colorNormal_titleFrameBorder;
            inp->colorNormal_titleFrameBorder = col;
            
            
            col = inp->colorActive_text;
            inp->colorActive_text = inp->colorNormal_text;
            inp->colorNormal_text = col;

            col = inp->colorActive_title;
            inp->colorActive_title = inp->colorNormal_title;
            inp->colorNormal_title = col;

            updateVisual_state1(idLastActive);
        }

        InvalidateRect(sHandle(*parent), 0, 1);
    }
}

void CSINPUT::mouseMoveEvent(int idInput, POINT p)
{
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];

    if(PtInRect(&inp->rect, p))
    {
        if(!inp->frameOn)
        {
            inp->frameOn = 1;
            updateVisual_state2(idInput);

            idMouseHover = idInput;

            //update();

            inp->btnUpOn = 0;
            inp->btnDownOn = 0;
            inp->btnUndoOn = 0;
            inp->btnRedoOn = 0;
            inp->btnUnrollOn = 0;
            inp->btnNoNameOn = 0;
            //return;
        }
        
    }
    else
    {
        if(inp->frameOn)
        {
            inp->frameOn = 0;
            updateVisual_state1(idInput);

            //update();

            //redraw buttons
            inp->btnUpOn = 1;
            inp->btnDownOn = 1;
            inp->btnUndoOn = 1;
            inp->btnRedoOn = 1;
            inp->btnUnrollOn = 1;
            inp->btnNoNameOn = 1;
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

    if(PtInRect(&inp->noNameRect, p))
    {
        if(!inp->btnNoNameOn)
        {
            inp->btnNoNameOn = 1;
            if(!inp->noNameButtonState)
                drawImage(parentDC, inp->noNameImage1, inp->noNameRect);// bug
            idMouseHover = idInput;
            //return;
        }
        
    }
    else
    {
        if(inp->btnNoNameOn)
        {
            inp->btnNoNameOn = 0;
            if(!inp->noNameButtonState)
                drawImage(parentDC, inp->noNameImage2, inp->noNameRect);
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
    InvalidateRect(sHandle(*parent), &r, 0);
}

void CSINPUT::update(int idInput)
{
    if(idInput<0) idInput = ip.size() + idInput;
    updateVisual_state1(idInput);
    //RECT r = {ip[idInput]->rect.left, ip[idInput]->rect.top, ip[idInput]->rsize.cx, ip[idInput]->rsize.cy};
    //InvalidateRect(sHandle(*parent), &r, 1);
    InvalidateRect(sHandle(*parent), 0, 1);
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

void CSINPUT::setText(int idInput, wchar_t*value, bool update)
{
    if(idInput<0) idInput = ip.size() + idInput;
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];
    size_t size = wcslen(value);
    inp->textChar.clear();
    inp->textCharGeometry.clear();
    inp->textDcWidth = 0;
    inp->caretPos = 0;
    setActive(idInput);
    for(size_t i = 0; i < size; i++)
    {
        addChar(value[i]);
    }
    if(update)
    {
        updateVisual_state1(idInput, 1);
        InvalidateRect(sHandle(*parent), 0, 1);
    }
}

wstring CSINPUT::getText(int idInput)
{
    if(idInput<0) idInput = ip.size() + idInput;
    return ip[idInput]->textChar;
}

HFONT CSINPUT::getFont(int idInput)
{
    if(idInput<0) idInput = ip.size() + idInput;
    return ip[idInput]->textFont;
}

int CSINPUT::getActiveInputId()
{
    return idActive;
}

void CSINPUT::deleteValue(int idInput)
{
    if(idInput<0) idInput = ip.size() + idInput;
    size_t size = ip[idInput]->textChar.size();
    setActive(idInput);
    for(size_t i = size-1; i >= 0; i--)
    {
        ip[idInput]->caretPos = i;
        deleteChar(i);
    }
}

void CSINPUT::setAllowedChars(int idInput, wstring chars)
{
    if(idInput<0) idInput = ip.size() + idInput;
    ip[idInput]->allowedChars = chars;
}
void CSINPUT::replaceAllowedChar(int idInput, wchar_t oldChar, wchar_t newChar)
{
    if(idInput<0) idInput = ip.size() + idInput;
    int index = ip[idInput]->allowedChars.find(oldChar);
    if(index != wstring::npos)
    {
        ip[idInput]->allowedChars[index] = newChar;
    }
}
void CSINPUT::setForbiddenChars(int idInput, wstring chars)
{   
    if(idInput<0) idInput = ip.size() + idInput;
    ip[idInput]->forbiddenChars = chars;
}

void CSINPUT::setGlobalAllowedChars(wstring chars)
{
    globalAllowedChars = chars;
}
void CSINPUT::setGlobalForbiddenChars(wstring chars)
{
    globalForbiddenChars = chars;
}

void CSINPUT::setLogSection(int* logSection)
{
    this->logSection = logSection;
}
int* CSINPUT::getLogSection()
{
    return logSection;
}

void CSINPUT::setEditable(int idInput, bool editable)
{
    if(idInput<0) idInput = ip.size() + idInput;
    ip[idInput]->editable = editable;
}
void CSINPUT::setEditable(bool editable)
{
    globalModifiable = editable;
}

void CSINPUT::setPrefix(int idInput, wchar_t* prefix)
{
    if(idInput<0) idInput = ip.size() + idInput;
    ip[idInput]->prefix = _wcsdup(prefix);
}
void CSINPUT::setSuffix(int idInput, wchar_t* suffix)
{
    if(idInput<0) idInput = ip.size() + idInput;
    ip[idInput]->suffix = _wcsdup(suffix);
}
void CSINPUT::setTitleRectWidth(int idInput, int width)
{
    ip[idInput]->titleRectWidth = width;
    ip[idInput]->titleRect.right = ip[idInput]->titleRect.left + width;
    ip[idInput]->textRect.left = ip[idInput]->titleRect.right + ip[idInput]->buttonMarging;
}


inline void moveRect(RECT& r, int x, int y)
{
    r.left += x;
    r.top += y;
    r.right += x;
    r.bottom += y;
}

void CSINPUT::moveInput(int idInput, int x, int y, int gbpl, int gbpt, int gbpr, int gbpb)
{
    CSINPUT::CSINPUT_PARAMS* inp = ip[idInput];

    if(inp->gbpFrame)
    {
        RECT& r = inp->rect;
        moveRect(r, x, y);

        if(inp->gbpBkg)
        {
            RECT& r = inp->textRect;
            moveRect(r, x, y);
        }

        if(inp->gbpTitle)
        {
            RECT& r = inp->titleRect;
            moveRect(r, x, y);
        }

        if(inp->gbpIncUp)
        {
            RECT& r = inp->upRect;
            moveRect(r, x, y);
        }

        if(inp->gbpIncDown)
        {
            RECT& r = inp->downRect;
            moveRect(r, x, y);
        }

        if(inp->gbpUndo)
        {
            RECT& r = inp->undoRect;
            moveRect(r, x, y);
        }

        if(inp->gbpRedo)
        {
            RECT& r = inp->redoRect;
            moveRect(r, x, y);
        }

        if(inp->gbpNoName)
        {
            RECT& r = inp->noNameRect;
            moveRect(r, x, y);
        }

        if(inp->gbpUnroll)
        {
            RECT& r = inp->unrollRect;
            moveRect(r, x, y);
        }

        updateVisual_state1(idInput, 1);
        InvalidateRect(sHandle(*parent), &r, 1);
        setGBP(idInput, gbpl, gbpt, gbpr, gbpb);
    }
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
    return 0;
}
int CSINPUT::updateLastActiveInput()
{
    InvalidateRect(sHandle(*parent),&ip[idLastActive]->rect,1);
    return 0;
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

    int idi = ipp->getActive();
    CSINPUT::CSINPUT::CSINPUT_PARAMS* ip = ipp->getInputParams(idi);
    CSINPUT::ACTION_PARAMS* actionParams = &ipp->actionParams;

    static int timerId = 1;
    
    if(msg == WM_MOUSEHOVER)
    {
        SetTimer(HWND(Args), timerId, 20, 0);
        ipp->startChecking = 1;
        return;
    }
    if(msg == WM_MOUSELEAVE)
    {
        ipp->resetActiveInput();
        InvalidateRect(HWND(Args),&bltRect[ipp->getId()],1);
        ipp->startChecking = 0;
        KillTimer(HWND(Args), timerId);
        return;
    }

    if(msg == WM_MOUSEMOVE)
    {
        actionParams->count = 15;
    }

    
    if(msg == WM_TIMER && WPARAM(Args) == timerId)
    {
        if(actionParams->count == 0)
        {
            GetCursorPos(&actionParams->pmouse);
        }
        else actionParams->count--;

        if(ipp->startChecking == 1 && !(TIMER_POINT.x == actionParams->pmouse.x && TIMER_POINT.y == actionParams->pmouse.y))
        {
            
            POINT p = TIMER_POINT;
            ScreenToClient(sHandle(int(Args)), &p);

            int i = ipp->getId();
            p.x += csGraphics::getGraphicAreaInXPos(i) - csGraphics::getGraphicAreaXPos(i);
            p.y += csGraphics::getGraphicAreaInYPos(i) - csGraphics::getGraphicAreaYPos(i);

            actionParams->psave = p;
            for(int i = 0; i<ipp->getInputsNumber(); i++)
            {
                ipp->mouseMoveEvent(i, p);
            }
            
        
        }

    }

    
    if(msg == WM_TIMER && WPARAM(Args) == 0 && GetFocus() == HWND(Args))
    {
        if(actionParams->hcount == 40)
        {
            ipp->highlightActiveChar(0);
        }
        if(actionParams->hcount == 80)
        {
            ipp->highlightActiveChar(1);
            actionParams->hcount = 0;
        }
        actionParams->hcount++;
    }

    if(msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK)
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

        ipp->switchInputState();

        ipp->switchNoNameButtonState(ipp->getActive());

        ipp->placeCaret(p);
        ipp->updateText(ipp->getActive());
        ipp->updateText(ipp->getLastActive());
        ipp->updateActiveInput();
        ipp->updateLastActiveInput();

        //ipp->highlightActiveChar();

        
    }
    if(msg == WM_KEYDOWN)
    {
        if(wp == VK_LEFT)
        {
            if(ip->caretPos > 0)
            {
                ip->caretPos--;
                ipp->updateText(ipp->getActive());
                ipp->highlightActiveChar(1);
                ipp->updateActiveInput();
            }
            return;
        }
        if(wp == VK_RIGHT)
        {
            if(ip->caretPos < ipp->getActiveInputCharNumber())
            {
                ip->caretPos++;
                ipp->updateText(ipp->getActive());
                ipp->highlightActiveChar(1);
                ipp->updateActiveInput();
            }
            return;
        }
        if(wp == VK_BACK)
        {
            if(ip->caretPos > 0)
            {
                int w = ipp->deleteChar(1);
                ipp->highlightActiveChar(1);
            }
            return;
        }
        if(wp == VK_DELETE)
        {
            if(ip->caretPos < ipp->getActiveInputCharNumber()-1)
            {
                ipp->deleteChar(0);
                ipp->highlightActiveChar(1);
            }
            return;
        }
        
        
    }
    if(msg == WM_CHAR )
    {
        if(wp != 8 && wp != 13)
        {
            int w = ipp->addChar(wp);
            ipp->highlightActiveChar(1);
        }

        if((TIMER_POINT.x == actionParams->pmouse.x && TIMER_POINT.y == actionParams->pmouse.y))
            ipp->mouseMoveEvent(ipp->getActive(), actionParams->psave);
       
    }

    if(msg == WM_SIZE)
    {
        /*int n = ipp->getInputsNumber();
        ipp->updateVisibleBackground();
        RECT r = csGraphics::getGraphicAreaContextVisiblePart(ipp->getId());
        for(int i = 0; i<n; i++)
        {
            RECT r2 = ipp->getInputParams(i)->rect;
            if(!(r2.left > r.right || r2.top > r.bottom || r2.right < r.left || r2.bottom < r.top))
            ipp->updateGeometry(i);
        }
        ipp->update();*/

        if(actionParams->count_ == 0)
            actionParams->count_ = 20;
        PostMessage(HWND(Args), WM_TIMER, 0, 0);
        return;
    }

    if(msg == WM_CATCH_SIZEMOVE_INIT)
    {
        actionParams->r_sizeMove = CSSECMAN::sRectClient(ipp->getId());
        return;
    }

    if(msg == WM_CATCH_SIZEMOVE_EXIT || msg == WM_CATCH_ROOT_RESTORED || msg == WM_CATCH_ROOT_MAXIMIZED)
    {
        actionParams->count_sizeMove = 20;
        SendMessage(HWND(Args), WM_TIMER, 0, 0);
        return;
    }

    if(msg == WM_TIMER && WPARAM(Args) == 0)
    {
        if(actionParams->count_ % 4 == 0 && actionParams->count_ != 0)
        {
            int n = ipp->getInputsNumber();
            ipp->updateVisibleBackground();
            //csGraphics::updateGraphicArea(ipp->getId(), 1);
            RECT r = csGraphics::getGraphicAreaContextVisiblePart(ipp->getId());
            int margin = 200; // marge de securité pour eviter les erreurs de positionnement lors des changements brusques de taille de la fenetre
            for(int i = 0; i<n; i++)
            {
                RECT r2 = ipp->getInputParams(i)->rect;
                if(!(r2.left-margin > r.right || r2.top-margin > r.bottom || r2.right+margin < r.left || r2.bottom+margin < r.top))
                    ipp->updateGeometry(i);
            }
            ipp->update();
        }

        if(actionParams->count_ > 0)
        actionParams->count_--;

        if(actionParams->count_sizeMove % 4 == 0 && actionParams->count_sizeMove != 0)
        {
            RECT r = CSSECMAN::sRectClient(ipp->getId());
            //if(r.right != actionParams->r_sizeMove.right || r.bottom != actionParams->r_sizeMove.bottom)
            {
                int n = ipp->getInputsNumber();
                ipp->updateBackground();
                for(int i = 0; i<n; i++)
                {
                    RECT r2 = ipp->getInputParams(i)->rect;
                    ipp->updateGeometry(i);
                }
                ipp->update();
                actionParams->r_sizeMove = r; // important lorsque wm_catch_size_move_init n'est pas envoyé
            }
            //actionParams->count_ = 0;
            //cout<<"actionParams->count_sizeMove = "<<actionParams->count_sizeMove<<"\n";
        }

        if(actionParams->count_sizeMove > 0)
            actionParams->count_sizeMove--;

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
                            InvalidateRect(sHandle(ip->getId()), &bltRect[ip->getId()],1);
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
    
    inp->newInput((wchar_t*)(wstring(L"input ")+to_wstring(inp->getInputsNumber()+1)).c_str(),L"Type a text line here...",geometry, 0, 50, 10);
    inp->addNoNameButton(-1,L"resources/img/Folder.bmp",L"resources/img/Folder2.bmp",0,0);
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
    csGraphics::setGraphicAreaColor(idp, {30,30,30}, {35,35,35});
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
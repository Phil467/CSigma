#ifndef CSUTILS_H
#define CSUTILS_H

#include "csArgs.h"
#include "csUIFx.h"
#include "csTypes.h"
#include <wchar.h>
#include "csStrUtils.h"

#define CURSOR_NCHITTEST_POS_CLIENT 0
#define CURSOR_NCHITTEST_POS_LEFT 1
#define CURSOR_NCHITTEST_POS_TOP 2
#define CURSOR_NCHITTEST_POS_RIGHT 3
#define CURSOR_NCHITTEST_POS_BOTTOM 4
#define CURSOR_NCHITTEST_POS_TOPLEFT 5
#define CURSOR_NCHITTEST_POS_TOPRIGHT 6
#define CURSOR_NCHITTEST_POS_BOTTOMLEFT 7
#define CURSOR_NCHITTEST_POS_BOTTOMRIGHT 8
#define CURSOR_NCHITTEST_POS_CAPTION 9

#define BIND_DEST_LEFT_EDGE 10
#define BIND_DEST_TOP_EDGE 11
#define BIND_DEST_RIGHT_EDGE 12
#define BIND_DEST_BOTTOM_EDGE 13

#define BIND_DEST_LEFT_EDGE_ONE 14
#define BIND_DEST_TOP_EDGE_ONE 15
#define BIND_DEST_RIGHT_EDGE_ONE 16
#define BIND_DEST_BOTTOM_EDGE_ONE 17

#define BIND_DEST_LEFT_EDGE_OPP 180
#define BIND_DEST_TOP_EDGE_OPP 190
#define BIND_DEST_RIGHT_EDGE_OPP 200
#define BIND_DEST_BOTTOM_EDGE_OPP 210


#define CS_TA_TOP_LEFT 0
#define CS_TA_CENTER_LEFT 1
#define CS_TA_BOTTOM_LEFT 2

#define CS_TA_TOP_CENTER 3
#define CS_TA_CENTER 4
#define CS_TA_BOTTOM_CENTER 5

#define CS_TA_TOP_RIGHT 6
#define CS_TA_CENTER_RIGHT 7
#define CS_TA_BOTTOM_RIGHT 8

using namespace  std;


namespace CSUIMAN
{

void _CSIGMA_INIT_(HINSTANCE hInstance, void(*forceEventFunc)(CSARGS)=0, CSARGS* forceEventArgs=0);
int _CSIGMA_SOFTWARE_();
void __setAllRects();
int createSection(int id, RECT geom,  COLORREF color, BOOL_RECT edgeResize, bool show=1, bool isRoot=0, bool attach=1);
bool addAction(int id, void(*f)(CSARGS), CSARGS& args);
bool addAction(int id, void(*f)(CSARGS), int nbArgs, ...);
void setIcon(HWND hwnd, wchar_t* pathSmallIcon, wchar_t* pathBigIcon);
HWND sHandle(int id);
CSTEXT& sTitle(int id);
CSDYNAMIC_SIMPLE_TEXT& sDynSimpleText(int id);
RECT sRectParentContext(int id);
RECT sRectParentContextStatic(int id);
RECT sRectClient(int id);
POINT sDeltaPos(int id);
SIZE sDeltaSize(int id);
void setTitle(int id, CSTEXT title);
void setBorderThick(int id, int thick);
void setBorderColorAndThick(int id, COLORREF color, int thick);
void inert(int id, BYTE alphaLevel=255);
void setTransparent(int id);
void setTransparency(int id, char level);
void printRect(RECT r, char* title="rect");
void setSizeFactor(float factor);

void bindGeometry(int id, int n, ...);
void bindGeometry_(int id, BIND_GEOM_PARAMS*& Args, int& n);
template<typename ..._Args> void bindGeometry_(int id, BIND_GEOM_PARAMS*& Args, int& n, BIND_GEOM_PARAMS arg, _Args... rest)
{
    n += 1;
    Args = (BIND_GEOM_PARAMS*)realloc(Args, n*sizeof(BIND_GEOM_PARAMS));
    Args[n-1] = arg;
    bindGeometry_(id, Args, n,  rest...);
};

void bindGeometry(int id, BIND_GEOM_PARAMS* bgps, int n);
template<typename ..._Args> void bindGeometry(int id, BIND_GEOM_PARAMS bgp, _Args... bgps)
{
    BIND_GEOM_PARAMS* Args = 0;
    int n = 0;
    bindGeometry_(id, Args, n, bgp, bgps...);
    bindGeometry(id, Args, n);
    free(Args);
};

void _drawTitle(int id, HDC dc);

void setAsCloseButton(int id, int& id_close);
void setAsMinButton(int id, int& id_minimize);
void setAsMaxButton(int& id, int& id_maximize);

void manageTimers(int id);
void killAllTimers();
void setAllTimers();
RECT getTaskbarRect();
TASKBAR_INFO getTaskbarInfo();

void setLockable(int id, CSLOCKED_MODE lm);
void updateAfterReceivingResizeMessage(int id);
void addTips(int id, RECT rTips, POS_BOOL pb, int delay, bool locked, CSDYNAMIC_SIMPLE_TEXT message);
void enableDarkEdge(int id);
void __saveAppSizes(wchar_t* filePath);
void __getAppSizes(wchar_t* filePath);
void __setAppSizes();
void _updateApp(int id);

void setSaveAppSizes(bool b);
}

namespace CSUTILS
{
RECT rectInParentRef(int id);
RECT rectInParentRef_saved(int id);

LPSIZE textExtent(int id, HFONT font, char* text);
LPSIZE textExtent(HWND hwnd, HFONT font, char* text);
LPSIZE textExtentW(int id, HFONT font, wchar_t* text);
LPSIZE textExtentW(HWND hwnd, HFONT font, wchar_t*text);
LPSIZE numExtent(HWND hwnd, int num, int maxNumDigits);
LPSIZE numExtent(HWND hwnd, HFONT font, int num, int maxNumDigits);


int numDigits(unsigned long number);
int numDigits(long number);

int  getAdjustedFontSizeXY(int size);
int  getAdjustedFontSizeX(int xSize);
int  getAdjustedFontSizeY(int ySize);
char* truncateHorizontalText(char* str, HFONT hf, int maxLength, LPSIZE& lps);
char* truncateHorizontalText(char* str, HFONT hf, int maxLength);
wchar_t* truncateHorizontalTextW(wchar_t* str, HFONT hf, int maxLength, LPSIZE& lps);
wchar_t* truncateHorizontalTextW(wchar_t* str, HFONT hf, int maxLength);
int unilineTextToPolylineText(wchar_t*_str, HFONT hf, int maxLength, vector<wchar_t*>& lines);
bool signExtraction(char*& strNumeric);
bool signExtraction(const char* strNumeric, char*& ret);

wchar_t* makeWcharString(const wchar_t* str);

void drawGDIRectangle(HDC dc, COLORREF brush, COLORREF pen, int border, RECT r);
CSRGBA toRGBA(long color);
bool fileExists(const wchar_t* filename);
bool directoryExists(const wchar_t* dirname) ;
bool pathExists(const wchar_t* path) ;

RECT  r(int x, int y, int cx, int cy, int id=0);
SIZE  s(int cx, int cy);
int   l(int i);
}
#endif // CSUTILS_H

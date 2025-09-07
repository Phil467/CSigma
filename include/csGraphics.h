#ifndef CSGRAPHICS_H
#define CSGRAPHICS_H


using namespace std;

#include "csTypes.h"
#include "csUtils.h"

namespace csGraphics
{
vector<CSGRAPHIC_CONTEXT> getImageGradient(CSIMAGE_GRADIENT mhg, CSRGBA bkgColor);
CSGRAPHIC_CONTEXT getImageGraphicContext(char*path);
CSGRAPHIC_CONTEXT getImageGraphicContextW(wchar_t*path);
CSGRAPHIC_CONTEXT getImageGraphicContextResized(char*path, SIZE size);
CSGRAPHIC_CONTEXT getImageGraphicContextResizedW(wchar_t*path, SIZE size);
CSGRAPHIC_CONTEXT_EXT getImageGraphicContextExt(char*path, BITMAPINFO*bmi);
CSGRAPHIC_CONTEXT_EXT getImageGraphicContextExtW(wchar_t*path, BITMAPINFO*bmi);
CSGRAPHIC_CONTEXT_EXT createGraphicContextExt(SIZE sz, BITMAPINFO*bmi);
CSGRAPHIC_CONTEXT createGraphicContext(SIZE sz);
CSGRAPHIC_CONTEXT createCompatibleGraphicContext(HDC hdc, SIZE sz, CSRGBA bkgColor, CSRGBA brdColor);
CSGRAPHIC_CONTEXT createCompatibleGraphicContext(HDC hdc, SIZE sz, COLORREF bkgColor, COLORREF brdColor);

void releaseGraphicContext(CSGRAPHIC_CONTEXT dc);
void releaseGraphicContextExt(CSGRAPHIC_CONTEXT_EXT bmp);
BITMAPINFO setBMI(SIZE sz);

void setHzoom(int id, float zoom);
void setVzoom(int id, float zoom);
void setGraphicAreaPosition(int id, POINT position);
void setGraphicAreaSize(int id, SIZE size);
void setGraphicAreaXSize(int id, long cx);
void setGraphicAreaYSize(int id, long cy);
void setGraphicAreaColor(int id, CSRGBA background, CSRGBA border);
void updateGraphicArea(int id, bool reCreate);

void updateBackground(int id);

CSGRAPHIC_CONTEXT viewImage(int id, wchar_t*path, POINT position, SIZE size);
int loadImage(int id, wchar_t*path, POINT position, SIZE size);
void showImage(int id, int idEnt, POINT posCrop={0}, SIZE sizeCrop={0});
POINT getImagePos(int id, int idEnt);
SIZE getImageSize(int id, int idEnt);
SIZE getImageOriginalSize(int id, int idEnt);
void setImagePos(int id, int idEnt, POINT pos);
void setImageSize(int id, int idEnt, SIZE size);
void setMap(int id, SIZE size);
void mapImageEntity(int id, int idEnt);

void viewText(int id, wchar_t* text, CSRGBA color, POINT pos, wchar_t* fontName, int fontSize, bool italic, DWORD weight, int width);
void setDynamicSimpleText(int id, CSDYNAMIC_SIMPLE_TEXT dst);
}

#endif // CSGRAPHICS_H

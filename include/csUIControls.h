#ifndef CSUICONTROLS_H_INCLUDED
#define CSUICONTROLS_H_INCLUDED

#include "csSection.h"
#include "csScrollbar.h"

namespace CSUICONTROLS
{
    CSSCROLLBAR addHScrollBar(int* idp, int* idClient=0, int* idMask=0, int thick=10, int autoHideIntensity=5);
    CSSCROLLBAR addVScrollBar(int* idp, int* idClient=0, int* idMask=0, int thick=10, int autoHideIntensity=5);
    CSSYSCOMMAND_SECTION addSysCommand(int& id, POINT pos);
    int addTitle(int& id, wchar_t*title, SIZE size={0}, char*iconPath=0, int fontSize = 16, wchar_t*fontName = L"Bauhaus 93");

    int iconButton01(int idp, char* path1, char* path2, RECT r);
    int iconButton02(int idp, char* path1, char* path2, RECT r);
    int darkTextButton01(int idp, wchar_t*title, RECT r, int fontSize=12);
    int darkImgTextButton01(int idp, wchar_t*title, char* path1, char* path2, RECT r, int fontSize=12);
    int darkTextButton02(int idp, wchar_t*title, RECT r, int fontSize=12);

CS_NUMERIC_INCREMENTER_PARAMS numericIncrementer(int idp, RECT r, double value, double incr, bool intNum=0);
CS_NUMERIC_INCREMENTER_PARAMS numericIncrementerExt(int idp, RECT r, wchar_t* value, wchar_t* step, short format);
CS_NUMERIC_INCREMENTER_PARAMS numericIncrementerExt1(int idp, RECT r, wchar_t* value, wchar_t* step, short format
                                                    , COEFS4 bindCoefs={0,0,1,0}, FLAGS4 bindFlags={0,0,BIND_DEST_RIGHT_EDGE,0});
CS_NUMERIC_INCREMENTER_PARAMS numericIncrementerExt2(int idp, RECT r, wchar_t* value, wchar_t* step, short format);

CS_STRING_INCREMENTER_PARAMS stringIncrementer(int idp, RECT r, wchar_t* value, long step, bool loopStyle, COEFS4 bindCoefs, FLAGS4 bindFlags);

void createToolTips(COLORREF color);

}

#endif // CSUICONTROLS_H_INCLUDED

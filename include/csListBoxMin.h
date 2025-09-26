#ifndef CSLISTBOXMIN_H
#define CSLISTBOXMIN_H
#include "csSection.h"
#include "csUtils.h"
#include "csLIST.h"
#include "csGraphics.h"

#define CS_RDBTN_GRID_HV  100
#define CS_RDBTN_GRID_VH  200
#define CS_RDBTN_GRID_NONE  300

#define CS_RDBTN_TEXT_H  10
#define CS_RDBTN_TEXT_V1  20
#define CS_RDBTN_TEXT_V2  21

#define CS_RDBTN_TEXT_BEFORE  1
#define CS_RDBTN_TEXT_AFTER  2
#define CS_RDBTN_TEXT_OVER  3
#define CS_RDBTN_TEXT_UNDER  4

#define CS_ALIGN_HORIZONTAL  0
#define CS_ALIGN_VERTICAL  1


typedef struct CSLBM_ITEM
{
    wchar_t* title;
    wchar_t* titleReal;
    int id;
    RECT pos;
    POINT posImg;
    POINT posTitle;
    HFONT font;
    COLORREF color0, color1, color2, color3, color4;
    COLORREF bkgcol0, bkgcol1, bkgcol2, bkgcol3, bkgcol4;
    HDC dcs0, dcs1, dcs2, dcs3, dcs4;
};

class CSLISTBOXMIN
{
    public:
        typedef struct
        {
            HDC ic1, ic2, ic3, ic4;
            wchar_t* path1, *path2, *path3, *path4;
        }ICON;
        CSLISTBOXMIN(int* idp = 0, int gridStyle=CS_RDBTN_GRID_HV, int gridWidth = 1);
        void init(int* idp = 0, int gridStyle=CS_RDBTN_GRID_HV, int gridWidth = 1);
        void setIconSize(int idi, SIZE size);
        void newIcon(wchar_t*path1, wchar_t*path2=0,wchar_t*path3=0,wchar_t*path4=0);
        void setIcon(int idi, wchar_t*path1, wchar_t*path2=0,wchar_t*path3=0,wchar_t*path4=0);
        void setParent(int *idp);
        void setGridWidth(int gridWidth);
        void setRect(int id, RECT r);
        void setOrientation(int orient);
        void setOffset(SIZE _offset);
        void setMarging(SIZE _marging);
        void setTitle(int id, wchar_t* title);
        void setDefaultTitle(wchar_t* dfltTitle);
        void setTitles(wchar_t* ids[], wchar_t* _title[], int n);
        void setDefaultFont(wchar_t* fnt, SIZE size);
        void setFont(int id, wchar_t* fnt, SIZE size);
        void setTitleColors(int id,COLORREF color, COLORREF highlightColor, COLORREF selectionColor);
        void setAllTitleColors(COLORREF color, COLORREF highlightColor, COLORREF selectionColor);
        void getItemBackground(int id, COLORREF* color, COLORREF* highlightColor, COLORREF* selectionColor);
        void setItemBackground(int id, COLORREF color, COLORREF highlightColor, COLORREF selectionColor);
        void setMultipleItemsBackground(int*ids, int idsSize, COLORREF color, COLORREF highlightColor, COLORREF selectionColor);
        void setMultipleItemsBackground(std::vector<int> ids, COLORREF color, COLORREF highlightColor, COLORREF selectionColor);
        void setItemBackground(wchar_t*_title, COLORREF color, COLORREF highlightColor, COLORREF selectionColor);
        void setMultipleItemsBackground(wchar_t**_titles, int _titlesSize, COLORREF color, COLORREF highlightColor, COLORREF selectionColor);
        void setMultipleItemsBackground(std::vector<std::wstring> titles, COLORREF color, COLORREF highlightColor, COLORREF selectionColor);
        void setAllBackgrounds(COLORREF color, COLORREF highlightColor, COLORREF selectionColor);
        void setDefaultColors(COLORREF color, COLORREF highlightColor, COLORREF selectionColor);
        void setDefaultSize(SIZE size);
        void setMaxTextWidth(int _maxTextWidth);
        void setItemAlign(bool align);
        void zebraStyle(CSRGBA zcol, float zFactor);
        void newItem(wchar_t*title=0, int n=1, unsigned iconId=0);
        void newFilePath(wchar_t*filter=L"All Files\0*.*\0", unsigned iconId=0);
        int* getActiveItem();
        int* getMouseOverItem();
        void setActiveItem(int id);
        void moveItem(int from, int to);
        void setMouseOverItem(int id);
        void updateActiveItem(int id);
        void unActivateItem(int id);
        void refresh();
        void organize();
        void create();
        bool* getCntState();
        void setCntState(bool activate);
        void autoRepos();
        int getEventsGroupId();
        
        void removeItem(int id);
        void copyItem(int id);
        void cutItem(int id);
        void pasteItem(int id);
        void copyPasteStyle();
        bool* getCopyPasteState();
        int* getCopyPasteKeyDownState();
        void duplicate(int src, int pos);
        int getId();
        void updateItem(int id);
        
        int getSize();
        bool *click_message = 0;
        void* (*pf)();
        virtual ~CSLISTBOXMIN();

    protected:
        wchar_t*defaultItemTitle();
        void animate();
        void _removeFilePath(int id);
        void _pasteFilePath(int id);
        void _moveFilePath(int from, int to);

    private:
    int* parent;
    csLIST<wchar_t*> title;
    csLIST<wchar_t*> titleReal;
    wchar_t*dfltTitle;
    csLIST<RECT> pos;
    csLIST<POINT> posImg;
    csLIST<POINT> posTitle;
    csLIST<COLORREF> color0;
    csLIST<COLORREF> color1;
    csLIST<COLORREF> color2;
    csLIST<COLORREF> color3;
    csLIST<COLORREF> color4;
    csLIST<COLORREF> bkgcol0;
    csLIST<COLORREF> bkgcol1;
    csLIST<COLORREF> bkgcol2;
    csLIST<COLORREF> bkgcol3;
    csLIST<COLORREF> bkgcol4;
    csLIST<HFONT> font;
    csLIST<wchar_t*> filePath;
    csLIST<int> filePathID;
    HFONT dfltFont;
    CSRGBA defcol;
    COLORREF dfltColor1, dfltColor2, dfltColor3, dfltColor4;
    COLORREF dfltBkgCol1, dfltBkgCol2, dfltBkgCol3, dfltBkgCol4;
    SIZE dfltSz, *pdfltSz;
    int gridStyle;
    int textOrientation;
    int TextPos;
    SIZE marging, *pmarging;
    SIZE offset, *poffset;
    SIZE fontSize, imgSize, *pimgSize;
    CSGRAPHIC_CONTEXT imgdc1, imgdc2, imgdc3, imgdc4;
    HDC hdc, *phdc;
    int gridWidth, *pgridWidth, cxmax, cymax;
    csLIST<HDC> dcs0;
    csLIST<HDC> dcs1;
    csLIST<HDC> dcs2, dcs3, dcs4;
    csLIST<ICON> icons;
    int* lastMouseOverid;
    int* lastMouseClickid;
    int*n;
    CSARGS args;
    bool activeChoosed, *cntActivate, animated;
    int *vkCtrlCount, *vkCtrlCountControl;
    POINT *vkCtrlPoint, *vkCtrlGdcPos;
    int *smoothRepos;
    int *smoothReposCount;
    float zebFact;
    CSRGBA zebColor;
    bool itemAlign;

    CSLBM_ITEM rbItem;

    int iter, maxTextWidth;

    int *cutPasteViewer;
    bool *cutPasteStart, *cutPasteDone, *cutPasteDone0;
    int *copyPasteKeyDownState;

    int groupMsgPos;
};

CSLISTBOXMIN* csNewMinimalListBoxPtr(int* idp=0, int gridStyle=CS_RDBTN_GRID_HV, int gridWidth=1);
void organize2(int parent, int n, SIZE imgSize,
                RECT*pos,POINT*posImg,POINT*posTitle, HDC* dcs0, HFONT* font, COLORREF* color0, COLORREF* bkgcol0, wchar_t**title, HDC hdc,
                int cxmax, int cymax);
void colorTracking(int parent, int lastItem, int currentItem, int n, SIZE imgSize,
                RECT*pos,POINT*posImg,POINT*posTitle, HDC* dcs0, HFONT* font,
                 COLORREF* color0, COLORREF* bkgcol0, wchar_t**title, HDC hdc, int cxmax, int cymax);
#endif // CSLISTBOXMIN_H

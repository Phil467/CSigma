#ifndef CSLISTBOX_H
#define CSLISTBOX_H
#include "csSection.h"
#include "csUtils.h"
#include "csTypes.h"



class csLISTBOX
{
    public:
        typedef struct IMAGE_PATH
        {
            wchar_t* normal;
            wchar_t* mouseOver;
            wchar_t* inMouseOver;
            wchar_t* actived;
            wchar_t* inActived;
            wchar_t* disabled;
        };

        typedef struct ELEMENT_COLORS
        {
            CSRGBA normal;
            CSRGBA mouseOver;
            CSRGBA inMouseOver;
            CSRGBA actived;
            CSRGBA inActived;
            CSRGBA disabled;
        };
        
        typedef struct IMAGE_ELEMENT
        {
            HDC normal;
            HDC mouseOver;
            HDC inMouseOver;
            HDC actived;
            HDC inActived;
            HDC disabled;
        };

        typedef struct IMAGE_PARAMS
        {
            POINT imgPos;
            SIZE imgSize;
            IMAGE_ELEMENT imgElm;
        };

        typedef struct FONT_PARAMS
        {
            wchar_t* name;
            DWORD underlined;
            DWORD italic;
            int weight;
            int size;
            int angle;
        };

        typedef struct csLISTBOX_SUB_ITEM_STRUCT
        {
            RECT r;
            POINT textPos;
            POINT imgPos;
            SIZE imgSize;
            IMAGE_ELEMENT img;
            wchar_t* text, textTrunc;
            FONT_PARAMS fontPar;
            ELEMENT_COLORS textColor;
            ELEMENT_COLORS borderColor;
            ELEMENT_COLORS bkgColor;
            bool biState;
            bool selected;
        };
        
        struct csLISTBOX_STRUCT
        {
            int nx, ny, CX, CY, cx, cy;
            vector<int> activeId, formerActiveId;
            int mouseId, lastMouseId, mouseInId, lastMouseInId;
            int nActiveIds;
            HDC parentDC;
            SIZE dcSize;

            ELEMENT_COLORS defTextColor;
            ELEMENT_COLORS defBorderColor;
            ELEMENT_COLORS defBkgColor;

            vector<vector<csLISTBOX_SUB_ITEM_STRUCT>> lsis;
            vector<int> state;
            vector<bool> unbinded;

        };
        
        csLISTBOX(int id, int nx, int ny, int CX, int CY, int hMarging, int vMarging, int nInRect);
        bool isIdActived(int id);
        bool isMouseHoverId(int id);
        void init(int id, int nx, int ny, int CX, int CY, int hMarging, int vMarging, int nInRect);
        void deleteActiveIds();
        void create();
        void select();
        void applySelectionVisual();
        void applyInSelectionVisual();
        void applyNormalVisual();
        void applyMouseOverVisual();
        void applyDesactivateVisual();
        int getIdFromPosition_timer();

        csLISTBOX::IMAGE_PARAMS loadImages(csLISTBOX::IMAGE_PATH ip, POINT imgPos);
        csLISTBOX::IMAGE_PARAMS loadImagesExt(csLISTBOX::IMAGE_PATH ip, POINT imgPos, SIZE imgSize);
        virtual ~csLISTBOX();

    protected:
        void _setRects();
        void _drawRectangle(HDC dc, RECT r, CSRGBA pc, CSRGBA bc);
        void _drawText(HDC dc, wchar_t* title, POINT pos, CSRGBA color, csLISTBOX::FONT_PARAMS fontPar);
        wchar_t* _newText(wchar_t*text);

    private:
        csLISTBOX_STRUCT lbs;
        int idp, nInRect;
        int timerIter;
};

#endif // CSLISTBOX_H

#ifndef CSRULER_H
#define CSRULER_H
#include "csTypes.h"
#include "csGraphics.h"

#define CS_RULER_HORIZONTAL 0
#define CS_RULER_VERTICAL 1

#define CS_RULER_TEXT_UNDER 0
#define CS_RULER_TEXT_OVER 1

#define CS_RULER_TEXT_ALIGN_LEFT 0
#define CS_RULER_TEXT_ALIGN_CENTER 1
#define CS_RULER_TEXT_ALIGN_RIGHT 2

#define MILLIMETER 1
#define CENTIMETER 10
#define DECIMETER 100
#define METER 1000

class CSRULER
{
    public:
        CSRULER(int idp, POINT pos = {0,0}, SIZE size = {1000,20}, bool style = CS_RULER_TEXT_UNDER, double first = 0, double outdx=5, 
        unsigned unit=METER, unsigned dx=5, unsigned DX=5);
        void init(int idp, POINT pos, SIZE size, bool style, double first, double outdx,
            unsigned unit, unsigned dx, unsigned DX);
        
        SIZE* getSizePtr();
        POINT* getPositionPtr();
        LONG* getInPositionXPtr();
        LONG* getInPositionYPtr();
        bool* getOrientationPtr();
        CSGRAPHIC_CONTEXT* getGContextPtr();
        bool* getShowPtr();
        virtual ~CSRULER();
        void update();
        void setPosition(POINT pos);
        void setInPositionX(LONG x);
        void setInPositionY(LONG y);
        void setSize(SIZE size);
        void setColors(CSRGBA bkgCol, CSRGBA brdCol={25,25,25}, CSRGBA linesCol1={150,150,150}, CSRGBA linesCol2={200,200,200}, CSRGBA textCol={150,150,150});
        void setFontSize(int fontSize);
        void setFirst(double first);
        void setOutdx(double outdx);
        void setUnit(unsigned unit);
        void setdx(unsigned dx);
        void setDX(unsigned DX);

        RECT idMaskRect = {0};
        int xdc = 0, ydc = 0;

    protected:

    private:
        SIZE size;
        POINT pos;
        LONG* inPosX, *inPosY;
        bool orient, style, show;
        double first, outdx;
        unsigned unit, dx, DX;
        CSGRAPHIC_CONTEXT rulerDC;

        CSRGBA bkgCol, brdCol, linesCol1, linesCol2, textCol;
        unsigned l1, l2, fontSize;
        int idp;
        HFONT hf;

        bool xStatic, yStatic;

};

CSRULER* csNewRuler(int id, POINT pos, SIZE size, bool style, double first, double outdx,
    unsigned unit, unsigned dx, unsigned DX);
#endif // CSRULER_H

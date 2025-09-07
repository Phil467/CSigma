#ifndef CSSCROLLBAR_H
#define CSSCROLLBAR_H

#include "csUtils.h"
#include "csSection.h"

#define CS_SBAR_SURFACE 1
#define CS_SBAR_CONTAINER 2
#define CS_SBAR_HORIZONTAL 0
#define CS_SBAR_VERTICAL 1

class CSSCROLLBAR
{
    public:
        typedef struct
        {
            float before, after;
        }csSB_PROPORTION;
        
        CSSCROLLBAR(int idp=0, RECT geometry={0}, int sctype = CS_SBAR_SURFACE, bool orientation=CS_SBAR_HORIZONTAL);
        void init(int idp=0, RECT geometry={0}, int sctype = CS_SBAR_SURFACE, bool orientation=CS_SBAR_HORIZONTAL);
        virtual ~CSSCROLLBAR();
        int getId();
        int* getIdPtr();
        void setClient(int idc, int idMask);
        
        void mouseLeaveHide(int alphaMin=10);
        void setPositionRatio(float posRatio);
        void setPosition(ULONG pos);
        void setScrollColors(CSRGBA c1, CSRGBA c2);
        void setBackgroundColor(COLORREF color);

        void setViewFrameRightMarging(int marging);
        void setViewFrameBottomMarging(int marging);

    protected:

    private:
    int* scType;
    int idp, *idc;
    int* id, *idMask, idscroll;
    bool* orient;
    csSB_PROPORTION* prp;
    int*tLength;
    vector<RECT> rcs;
    vector<vector<HDC>> cloths;
    vector<vector<CSRGBA>> colors;
    float *value, *value0, *scval;
    int *hmarg, *vmarg, *width, *rectSelect, *mhId, mhId0;
    SIZE* szzoom, *oldClientSize;
    POINT *oldClientPos;
    CSARGS Args;
    int CX, CY;
    bool threadBool, bSize;
    int vEdge, hEdge;
};



#endif // CSSCROLLBAR_H

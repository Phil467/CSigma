#ifndef CSINPUT_H
#define CSINPUT_H

#include "csLIST.h"
#include "csTypes.h"
using namespace std;

class CSINPUT
{
    public:
        CSINPUT();
        CSINPUT(int*idp);
        virtual ~CSINPUT();
        typedef struct CSLINEAR_BIND
        {
            CSLINEAR l, t, r, b;

            CSLINEAR_BIND()
                : l{0, 0.0f, 0}, t{0, 0.0f, 0}, r{0, 0.0f, 0}, b{0, 0.0f, 0}
            {}

            CSLINEAR_BIND(CSLINEAR left, CSLINEAR top, CSLINEAR right, CSLINEAR bottom)
                : l(left), t(top), r(right), b(bottom)
            {}

            CSLINEAR_BIND(int lx, float la, int lb,
                         int tx, float ta, int tb,
                         int rx, float ra, int rb,
                         int bx, float ba, int bb)
                : l{lx, la, lb}, t{tx, ta, tb}, r{rx, ra, rb}, b{bx, ba, bb}
            {}
        };

        typedef struct CSINPUT_CHAR_VISUAL
        {
            HDC dc;
            HDC dcSel;

            CSINPUT_CHAR_VISUAL()
                : dc(nullptr), dcSel(nullptr)
            {}

            CSINPUT_CHAR_VISUAL(HDC hdc, HDC hdcSel)
                : dc(hdc), dcSel(hdcSel)
            {}
        };

        typedef struct CSINPUT_PARAMS
        {
            CSINPUT_PARAMS()
                : rect{0}, textRect{0}, titleRect{0}, upRect{0}, downRect{0}, undoRect{0}, redoRect{0}, unrollRect{0}, openFolderRect{0}
                , rsize{0}
                , showedSectionRect{0}, activeCharRect{-10000,-10000}
                , cntDc(0)
                , textDc(0)
                , textDcWidth(0), charSpacing(1)
                , textBmp(0), cntBmp(0)
                , style(0), textRectEnd(0), buttonMarging(0), titleRectWidth(0), titleRectHeight(0), buttonWidth(0)
                , caretPos(0)
                , title(0), defaultText(0)
                , titleFont(0)
                , textFont(0)
                , charHeight(0), charWidth(0)
                , multilineStyle(false)
                , withItemRollup(false)
                , withHistoryRollup(false)
                , incPos(0)
                , activeCharColor{255,255,255}
                , cntColor1{10,10,10}, cntBorderColor1{40,40,40}
                , bkgColor1{20,20,20}, borderColor1{80,80,80}
                , textColor1{150,150,150}, titleColor1{150,150,150}
                , cntColor2{30,30,30}, cntBorderColor2{150,150,150}
                , bkgColor2{40,40,40}, borderColor2{100,100,100}
                , textColor2{200,200,200}, titleColor2{220,220,220}
                , cntColor3{0,0,0}, cntBorderColor3{0,0,0}
                , bkgColor3{0,0,0}, borderColor3{0,0,0}
                , textColor3{0,0,0}, titleColor3{0,0,0}
                , cntColor4{0,0,0}, cntBorderColor4{0,0,0}
                , bkgColor4{0,0,0}, borderColor4{0,0,0}
                , textColor4{0,0,0}, titleColor4{0,0,0}
                , selectionColor{0,0,0}
                , gbpCnt(0)
                , gbpBkg(0)
                , gbpTitle(0)
                , gbpIncUp(0)
                , gbpIncDown(0)
                , gbpUndo(0)
                , gbpRedo(0)
                , gbpOpenFolder(0)
                , gbpUnroll(0)
                , cntOn(false)
                , bkgOn(false)
                , btnUpOn(false)
                , btnDownOn(false)
                , btnUndoOn(false)
                , btnRedoOn(false)
                , btnUnrollOn(false)
                , btnOpenFolderOn(false)
            {}

            // Constructeur de copie pour dupliquer les ressources
            CSINPUT_PARAMS(const CSINPUT_PARAMS& other)
                : rect(other.rect), textRect(other.textRect), titleRect(other.titleRect)
                , upRect(other.upRect), downRect(other.downRect), undoRect(other.undoRect)
                , redoRect(other.redoRect), unrollRect(other.unrollRect), openFolderRect(other.openFolderRect)
                , rsize(other.rsize)
                , showedSectionRect(other.showedSectionRect), activeCharRect(other.activeCharRect)
                , cntDc(other.cntDc)
                , textDc(other.textDc)
                , textDcWidth(other.textDcWidth), charSpacing(other.charSpacing)
                , textBmp(other.textBmp), cntBmp(other.cntBmp)
                , style(other.style), textRectEnd(other.textRectEnd), buttonMarging(other.buttonMarging)
                , titleRectWidth(other.titleRectWidth), titleRectHeight(other.titleRectHeight), buttonWidth(other.buttonWidth)
                , caretPos(other.caretPos)
                , title(other.title ? _wcsdup(other.title) : 0)  // Dupliquer la chaîne
                , defaultText(other.defaultText ? _wcsdup(other.defaultText) : 0)  // Dupliquer la chaîne
                , titleFont(other.titleFont)
                , textFont(other.textFont)
                , textChar(other.textChar)  // Copie de csLIST
                , textCharGeometry(other.textCharGeometry)  // Copie de csLIST
                , textCharVisual(other.textCharVisual)  // Copie de csLIST
                , charHeight(other.charHeight), charWidth(other.charWidth)
                , multilineStyle(other.multilineStyle)
                , withItemRollup(other.withItemRollup)
                , withHistoryRollup(other.withHistoryRollup)
                , incPos(other.incPos)
                , activeCharColor(other.activeCharColor)
                , cntColor1(other.cntColor1), cntBorderColor1(other.cntBorderColor1)
                , bkgColor1(other.bkgColor1), borderColor1(other.borderColor1)
                , textColor1(other.textColor1), titleColor1(other.titleColor1)
                , cntColor2(other.cntColor2), cntBorderColor2(other.cntBorderColor2)
                , bkgColor2(other.bkgColor2), borderColor2(other.borderColor2)
                , textColor2(other.textColor2), titleColor2(other.titleColor2)
                , cntColor3(other.cntColor3), cntBorderColor3(other.cntBorderColor3)
                , bkgColor3(other.bkgColor3), borderColor3(other.borderColor3)
                , textColor3(other.textColor3), titleColor3(other.titleColor3)
                , cntColor4(other.cntColor4), cntBorderColor4(other.cntBorderColor4)
                , bkgColor4(other.bkgColor4), borderColor4(other.borderColor4)
                , textColor4(other.textColor4), titleColor4(other.titleColor4)
                , selectionColor(other.selectionColor)
                , gbpCnt(other.gbpCnt ? new CSLINEAR_BIND(*other.gbpCnt) : 0)
                , gbpBkg(other.gbpBkg ? new CSLINEAR_BIND(*other.gbpBkg) : 0)
                , gbpTitle(other.gbpTitle ? new CSLINEAR_BIND(*other.gbpTitle) : 0)
                , gbpIncUp(other.gbpIncUp ? new CSLINEAR_BIND(*other.gbpIncUp) : 0)
                , gbpIncDown(other.gbpIncDown ? new CSLINEAR_BIND(*other.gbpIncDown) : 0)
                , gbpUndo(other.gbpUndo ? new CSLINEAR_BIND(*other.gbpUndo) : 0)
                , gbpRedo(other.gbpRedo ? new CSLINEAR_BIND(*other.gbpRedo) : 0)
                , gbpOpenFolder(other.gbpOpenFolder ? new CSLINEAR_BIND(*other.gbpOpenFolder) : 0)
                , gbpUnroll(other.gbpUnroll ? new CSLINEAR_BIND(*other.gbpUnroll) : 0)
                , cntOn(other.cntOn)
                , bkgOn(other.bkgOn)
                , btnUpOn(other.btnUpOn)
                , btnDownOn(other.btnDownOn)
                , btnUndoOn(other.btnUndoOn)
                , btnRedoOn(other.btnRedoOn)
                , btnUnrollOn(other.btnUnrollOn)
                , btnOpenFolderOn(other.btnOpenFolderOn)
            {
                // Copier les contextes graphiques
                incUpImage1 = other.incUpImage1;
                incDownImage1 = other.incDownImage1;
                undoImage1 = other.undoImage1;
                redoImage1 = other.redoImage1;
                unrollImage1 = other.unrollImage1;
                openFolderImage1 = other.openFolderImage1;

                incUpImage2 = other.incUpImage2;
                incDownImage2 = other.incDownImage2;
                undoImage2 = other.undoImage2;
                redoImage2 = other.redoImage2;
                unrollImage2 = other.unrollImage2;
                openFolderImage2 = other.openFolderImage2;

                incUpImage3 = other.incUpImage3;
                incDownImage3 = other.incDownImage3;
                undoImage3 = other.undoImage3;
                redoImage3 = other.redoImage3;
                unrollImage3 = other.unrollImage3;
                openFolderImage3 = other.openFolderImage3;

                incUpImage4 = other.incUpImage4;
                incDownImage4 = other.incDownImage4;
                undoImage4 = other.undoImage4;
                redoImage4 = other.redoImage4;
                unrollImage4 = other.unrollImage4;
                openFolderImage4 = other.openFolderImage4;
            }

            // Destructeur pour libérer les ressources dupliquées
            ~CSINPUT_PARAMS()
            {
                if(title) free(title);
                if(defaultText) free(defaultText);
                if(gbpCnt) delete gbpCnt;
                if(gbpBkg) delete gbpBkg;
                if(gbpTitle) delete gbpTitle;
                if(gbpIncUp) delete gbpIncUp;
                if(gbpIncDown) delete gbpIncDown;
                if(gbpUndo) delete gbpUndo;
                if(gbpRedo) delete gbpRedo;
                if(gbpOpenFolder) delete gbpOpenFolder;
                if(gbpUnroll) delete gbpUnroll;
            }

            RECT rect, textRect, titleRect, upRect, downRect, undoRect, redoRect, unrollRect, openFolderRect;
            SIZE rsize;
            RECT showedSectionRect, activeCharRect;
            HDC cntDc;
            HDC textDc;
            int textDcWidth, charSpacing;
            HBITMAP textBmp, cntBmp;

            int style, textRectEnd, buttonMarging, titleRectWidth, titleRectHeight, buttonWidth;
            int caretPos;

            CSGRAPHIC_CONTEXT incUpImage1, incDownImage1, undoImage1, redoImage1, unrollImage1, openFolderImage1;
            CSGRAPHIC_CONTEXT incUpImage2, incDownImage2, undoImage2, redoImage2, unrollImage2, openFolderImage2;
            CSGRAPHIC_CONTEXT incUpImage3, incDownImage3, undoImage3, redoImage3, unrollImage3, openFolderImage3;
            CSGRAPHIC_CONTEXT incUpImage4, incDownImage4, undoImage4, redoImage4, unrollImage4, openFolderImage4;

            wchar_t* title, *defaultText;
            HFONT titleFont;
            HFONT textFont;
            csLIST<wchar_t> textChar;
            csLIST<RECT> textCharGeometry;
            csLIST<CSINPUT_CHAR_VISUAL> textCharVisual;
            int charHeight, charWidth;

            bool multilineStyle;
            bool withItemRollup;
            bool withHistoryRollup;

            /*wchar_t* incUpImage1, incDownImage1, undoImage1, redoImage1;
            wchar_t* incUpImage2, incDownImage2, undoImage2, redoImage2;
            wchar_t* incUpImage3, incDownImage3, undoImage3, redoImage3;
            wchar_t* incUpImage4, incDownImage4, undoImage4, redoImage4;*/
            int incPos;

            CSRGBA activeCharColor;

            CSRGBA cntColor1, cntBorderColor1;// normal
            CSRGBA bkgColor1, borderColor1;
            CSRGBA textColor1, titleColor1;

            CSRGBA cntColor2, cntBorderColor2;// hover
            CSRGBA bkgColor2, borderColor2;
            CSRGBA textColor2, titleColor2;

            CSRGBA cntColor3, cntBorderColor3;// active
            CSRGBA bkgColor3, borderColor3;
            CSRGBA textColor3, titleColor3;

            CSRGBA cntColor4, cntBorderColor4;// disabled
            CSRGBA bkgColor4, borderColor4;
            CSRGBA textColor4, titleColor4;

            CSRGBA selectionColor;

            CSLINEAR_BIND* gbpCnt;
            CSLINEAR_BIND* gbpBkg;
            CSLINEAR_BIND* gbpTitle;
            CSLINEAR_BIND* gbpIncUp;
            CSLINEAR_BIND* gbpIncDown;
            CSLINEAR_BIND* gbpUndo;
            CSLINEAR_BIND* gbpRedo;
            CSLINEAR_BIND* gbpOpenFolder;
            CSLINEAR_BIND* gbpUnroll;

            bool cntOn;
            bool bkgOn;
            bool btnUpOn;
            bool btnDownOn;
            bool btnUndoOn;
            bool btnRedoOn;
            bool btnUnrollOn;
            bool btnOpenFolderOn;
        };
    
    bool startChecking = 0, startGBP=0;
    int checkCount = 0;
    void init(int*idp);
    void createContext(int idInput);
    
    void newInput(wchar_t*title, wchar_t*defaultText, RECT geometry, int style, int titleRectWidth, int titleRectHeight);
    void addIncrementButtons(int idInput, wchar_t*path11, wchar_t*path12, wchar_t*path13, wchar_t*path14,
                                  wchar_t*path21, wchar_t*path22, wchar_t*path23, wchar_t*path24);
    void addUndoRedoButtons(int idInput, wchar_t*path11, wchar_t*path12, wchar_t*path13, wchar_t*path14,
                                  wchar_t*path21, wchar_t*path22, wchar_t*path23, wchar_t*path24);
    void addUnrollButton(int idInput, wchar_t*path11, wchar_t*path12, wchar_t*path13, wchar_t*path14);
    void addOpenFolderButton(int idInput, wchar_t*path11, wchar_t*path12, wchar_t*path13, wchar_t*path14);

    void setGBP(int idInput, int bindLeft, int bindTop, int bindRight, int bindBottom);
    CSINPUT::CSINPUT_PARAMS* getInputParams(int idInput);
    void setActive(int idInput);
    void setLastActive(int idInput);
    int getActive();
    int getLastActive();
    int getMouseHoverId();
    vector<CSINPUT_PARAMS*> getInputParamsList();
    int getInputsNumber();
    void mouseMoveEvent(int idInput, POINT p);
    RECT updateRect(int idInput, CSLINEAR_BIND*lb);
    void updateGeometry(int idInput);
    void _updateGeometry(int idInput);
    void updateVisual_state1(int idInput, bool noRedrawActiveChar=0);
    void updateVisual_state2(int idInput, bool noRedrawActiveChar=0);
    void _updateVisual_state1(int idInput, bool noRedrawActiveChar=0);
    void _updateVisual_state2(int idInput, bool noRedrawActiveChar=0);
    void updateAll();
    void updateBackground();
    void addChar(wchar_t chr);
    void deleteChar(bool frontCaret);
    void updateText(int idInput);
    void updateActiveTextBkg();
    void updateActiveTextBkg(RECT rchar);
    int updateActiveInput();
    int updateFormerInput();
    RECT placeCaret(POINT p);
    void textCharGeometryCorrection(int from);
    RECT getFrontEndCharRect(int idInput, int idChar);
    void highlightActiveChar(RECT r);
    int getActiveInputCharNumber();
    int getId();
    void blit();
    void update();

    void hide();
    void show();
    HDC getStackDC();
    void updateStackDC();

    protected:
    CSLINEAR_BIND* CSLINEAR_BIND_PTR(CSLINEAR_BIND lb);
    void drawGdiRectangle(HDC& hdc, CSRGBA col, RECT rect);
    void drawGdiRectangle(HDC& hdc, CSRGBA brd, CSRGBA bkg, RECT rect);
    void drawGdiText(HDC& hdc, wchar_t*text, HFONT hf, CSRGBA col, RECT rect);
    void drawImage(HDC& hdc, CSGRAPHIC_CONTEXT dc, RECT r);

    

    private:
    int* parent, groupId;
    int* itemCnt;
    int idActive=0,idLastActive=0, idMouseHover=0, idLastMouseHover=0;
    HDC parentDC;
    CSGRAPHIC_CONTEXT stackDC;
    int xmarging, ymarging;
    HFONT titleFont, textFont;
    RECT ctxRect;
    CSLINEAR_BIND* gbpCtx = 0;

    vector<CSINPUT_PARAMS*> ip;
};

CSINPUT* csNewInputContext(int*id);
int* inputContextExample(int idp);

#endif // CSINPUT_H

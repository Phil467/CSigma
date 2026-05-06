#ifndef CSINPUT_H
#define CSINPUT_H

#include "csLIST.h"
#include "csTypes.h"
#include "windef.h"
using namespace std;

#define CS_INPUT_TA_LEFT 0
#define CS_INPUT_TA_CENTER 1
#define CS_INPUT_TA_RIGHT 2


class CSINPUT
{
    public:
        CSINPUT();
        CSINPUT(int*idp);
        virtual ~CSINPUT();

        typedef struct ACTION_PARAMS
        {
            int count;
            int hcount;
            int count_;
            int count_sizeMove;
            POINT pmouse;
            POINT psave;
            RECT r_sizeMove;
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

        typedef struct CSINPUT_ENTITY_COLORS
        {
            CSRGBA normal;
            CSRGBA hover;
            CSRGBA active;
            CSRGBA disabled;
        };

        typedef struct CSINPUT_PARAMS
        {
            CSINPUT_PARAMS()
                : rect{0}, textRect{0}, titleRect{0}, upRect{0}, downRect{0}, undoRect{0}, redoRect{0}, unrollRect{0}, noNameRect{0}
                , rsize{0}
                , showedSectionRect{0}, activeCharRect{-10000,-10000}
                , frameDc(0)
                , textDc(0)
                , textDcWidth(0), charSpacing(1)
                , textBmp(0), frameBmp(0)
                , style(0), textRectEnd(0), buttonMarging(0), titleRectWidth(0), titleRectHeight(0), buttonWidth(0)
                , caretPos(0)
                , title(0), textNote(0)
                , titleFont(0)
                , textFont(0)
                , textNoteFont(0)
                , textRectLeftMargin(0), textRectLeftMargin2(0), textNoteExtend(0)
                , textAlign(0), titleAlign(0)
                , charHeight(0), charWidth(0), titleHeight(0), titleWidth(0)
                , textFontSizeCoef(1.0f), titleFontSizeCoef(1.0f)
                , multilineStyle(false)
                , withItemRollup(false)
                , withHistoryRollup(false)
                , incPos(0)
                , activeCharColor{0,0,0}
                , activeCharBkgColor{220,220,220}
                , colorNormal_frame{15,15,15}, colorNormal_frameBorder{10,10,10}
                , colorNormal_titleFrame{20,20,20}, colorNormal_titleFrameBorder{30,30,30}
                , colorNormal_text{150,150,150}, colorNormal_title{150,150,150}
                , colorHover_frame{30,30,30}, colorHover_frameBorder{150,150,150}
                , colorHover_titleFrame{40,40,40}, colorHover_titleFrameBorder{100,100,100}
                , colorHover_text{200,200,200}, colorHover_title{220,220,220}
                , colorActive_frame{0,0,0}, colorActive_frameBorder{0,0,0}
                , colorActive_titleFrame{0,0,0}, colorActive_titleFrameBorder{0,0,0}
                , colorActive_text{0,0,0}, colorActive_title{0,0,0}
                , colorDisabled_frame{0,0,0}, colorDisabled_frameBorder{0,0,0}
                , colorDisabled_titleFrame{0,0,0}, colorDisabled_titleFrameBorder{0,0,0}
                , colorDisabled_text{0,0,0}, colorDisabled_title{0,0,0}
                , selectionColor{0,0,0}
                , textNoteColor{60,60,60}
                , gbpFrame(0)
                , gbpBkg(0)
                , gbpTitle(0)
                , gbpIncUp(0)
                , gbpIncDown(0)
                , gbpUndo(0)
                , gbpRedo(0)
                , gbpNoName(0)
                , gbpUnroll(0)
                , frameOn(false)
                , bkgOn(false)
                , btnUpOn(false)
                , btnDownOn(false)
                , btnUndoOn(false)
                , btnRedoOn(false)
                , btnUnrollOn(false)
                , btnNoNameOn(false)
                , editable(true)
                , noNameButtonState(false)
                , noNameButtonVariableState(false)
                , switchableState(false)
                , prefix(0), suffix(0)
            {}

            // Constructeur de copie pour dupliquer les ressources
            CSINPUT_PARAMS(const CSINPUT_PARAMS& other)
                : rect(other.rect), textRect(other.textRect), titleRect(other.titleRect)
                , upRect(other.upRect), downRect(other.downRect), undoRect(other.undoRect)
                , redoRect(other.redoRect), unrollRect(other.unrollRect), noNameRect(other.noNameRect)
                , rsize(other.rsize)
                , showedSectionRect(other.showedSectionRect), activeCharRect(other.activeCharRect)
                , frameDc(other.frameDc)
                , textDc(other.textDc)
                , textDcWidth(other.textDcWidth), charSpacing(other.charSpacing)
                , textBmp(other.textBmp), frameBmp(other.frameBmp)
                , style(other.style), textRectEnd(other.textRectEnd), buttonMarging(other.buttonMarging)
                , titleRectWidth(other.titleRectWidth), titleRectHeight(other.titleRectHeight), buttonWidth(other.buttonWidth)
                , caretPos(other.caretPos)
                , title(other.title ? _wcsdup(other.title) : 0)  // Dupliquer la chaîne
                , textNote(other.textNote ? _wcsdup(other.textNote) : 0)  // Dupliquer la chaîne
                , titleFont(other.titleFont)
                , textFont(other.textFont)
                , textNoteFont(other.textFont)
                , textRectLeftMargin(other.textRectLeftMargin), textRectLeftMargin2(other.textRectLeftMargin2), textNoteExtend(other.textNoteExtend)
                , textAlign(other.textAlign), titleAlign(other.titleAlign)
                , textChar(other.textChar)  // Copie de csLIST
                , textCharGeometry(other.textCharGeometry)  // Copie de csLIST
                , textCharVisual(other.textCharVisual)  // Copie de csLIST
                , charHeight(other.charHeight), charWidth(other.charWidth), titleHeight(other.titleHeight), titleWidth(other.titleWidth)
                , textFontSizeCoef(other.textFontSizeCoef), titleFontSizeCoef(other.titleFontSizeCoef)
                , multilineStyle(other.multilineStyle)
                , withItemRollup(other.withItemRollup)
                , withHistoryRollup(other.withHistoryRollup)
                , incPos(other.incPos)
                , activeCharColor(other.activeCharColor)
                , activeCharBkgColor{other.activeCharBkgColor}
                , colorNormal_frame(other.colorNormal_frame), colorNormal_frameBorder(other.colorNormal_frameBorder)
                , colorNormal_titleFrame(other.colorNormal_titleFrame), colorNormal_titleFrameBorder(other.colorNormal_titleFrameBorder)
                , colorNormal_text(other.colorNormal_text), colorNormal_title(other.colorNormal_title)
                , colorHover_frame(other.colorHover_frame), colorHover_frameBorder(other.colorHover_frameBorder)
                , colorHover_titleFrame(other.colorHover_titleFrame), colorHover_titleFrameBorder(other.colorHover_titleFrameBorder)
                , colorHover_text(other.colorHover_text), colorHover_title(other.colorHover_title)
                , colorActive_frame(other.colorActive_frame), colorActive_frameBorder(other.colorActive_frameBorder)
                , colorActive_titleFrame(other.colorActive_titleFrame), colorActive_titleFrameBorder(other.colorActive_titleFrameBorder)
                , colorActive_text(other.colorActive_text), colorActive_title(other.colorActive_title)
                , colorDisabled_frame(other.colorDisabled_frame), colorDisabled_frameBorder(other.colorDisabled_frameBorder)
                , colorDisabled_titleFrame(other.colorDisabled_titleFrame), colorDisabled_titleFrameBorder(other.colorDisabled_titleFrameBorder)
                , colorDisabled_text(other.colorDisabled_text), colorDisabled_title(other.colorDisabled_title)
                , selectionColor(other.selectionColor)
                , textNoteColor(other.textNoteColor)
                , gbpFrame(other.gbpFrame ? new CSLINEAR_BIND(*other.gbpFrame) : 0)
                , gbpBkg(other.gbpBkg ? new CSLINEAR_BIND(*other.gbpBkg) : 0)
                , gbpTitle(other.gbpTitle ? new CSLINEAR_BIND(*other.gbpTitle) : 0)
                , gbpIncUp(other.gbpIncUp ? new CSLINEAR_BIND(*other.gbpIncUp) : 0)
                , gbpIncDown(other.gbpIncDown ? new CSLINEAR_BIND(*other.gbpIncDown) : 0)
                , gbpUndo(other.gbpUndo ? new CSLINEAR_BIND(*other.gbpUndo) : 0)
                , gbpRedo(other.gbpRedo ? new CSLINEAR_BIND(*other.gbpRedo) : 0)
                , gbpNoName(other.gbpNoName ? new CSLINEAR_BIND(*other.gbpNoName) : 0)
                , gbpUnroll(other.gbpUnroll ? new CSLINEAR_BIND(*other.gbpUnroll) : 0)
                , frameOn(other.frameOn)
                , bkgOn(other.bkgOn)
                , btnUpOn(other.btnUpOn)
                , btnDownOn(other.btnDownOn)
                , btnUndoOn(other.btnUndoOn)
                , btnRedoOn(other.btnRedoOn)
                , btnUnrollOn(other.btnUnrollOn)
                , btnNoNameOn(other.btnNoNameOn)
                , editable(other.editable)
                , noNameButtonState(other.noNameButtonState)
                , noNameButtonVariableState(other.noNameButtonVariableState)
                , prefix(other.prefix ? _wcsdup(other.prefix) : 0)
                , suffix(other.suffix ? _wcsdup(other.suffix) : 0)
            {
                // Copier les contextes graphiques
                incUpImage1 = other.incUpImage1;
                incDownImage1 = other.incDownImage1;
                undoImage1 = other.undoImage1;
                redoImage1 = other.redoImage1;
                unrollImage1 = other.unrollImage1;
                noNameImage1 = other.noNameImage1;

                incUpImage2 = other.incUpImage2;
                incDownImage2 = other.incDownImage2;
                undoImage2 = other.undoImage2;
                redoImage2 = other.redoImage2;
                unrollImage2 = other.unrollImage2;
                noNameImage2 = other.noNameImage2;

                incUpImage3 = other.incUpImage3;
                incDownImage3 = other.incDownImage3;
                undoImage3 = other.undoImage3;
                redoImage3 = other.redoImage3;
                unrollImage3 = other.unrollImage3;
                noNameImage3 = other.noNameImage3;

                incUpImage4 = other.incUpImage4;
                incDownImage4 = other.incDownImage4;
                undoImage4 = other.undoImage4;
                redoImage4 = other.redoImage4;
                unrollImage4 = other.unrollImage4;
                noNameImage4 = other.noNameImage4;
                editable = other.editable;
                noNameButtonState = other.noNameButtonState;
                noNameButtonVariableState = other.noNameButtonVariableState;
                prefix = other.prefix ? _wcsdup(other.prefix) : 0;
                suffix = other.suffix ? _wcsdup(other.suffix) : 0;
                switchableState = other.switchableState;
            }

            // Destructeur pour libérer les ressources dupliquées
            ~CSINPUT_PARAMS()
            {
                if(title) free(title);
                if(textNote) free(textNote);
                if(gbpFrame) delete gbpFrame;
                if(gbpBkg) delete gbpBkg;
                if(gbpTitle) delete gbpTitle;
                if(gbpIncUp) delete gbpIncUp;
                if(gbpIncDown) delete gbpIncDown;
                if(gbpUndo) delete gbpUndo;
                if(gbpRedo) delete gbpRedo;
                if(gbpNoName) delete gbpNoName;
                if(gbpUnroll) delete gbpUnroll;
                if(prefix) free(prefix);
                if(suffix) free(suffix);
            }

            RECT rect, textRect, titleRect, upRect, downRect, undoRect, redoRect, unrollRect, noNameRect;
            SIZE rsize;
            RECT showedSectionRect, activeCharRect;
            HDC frameDc;
            HDC textDc;
            int textDcWidth, charSpacing;
            HBITMAP textBmp, frameBmp;

            int style, textRectEnd, buttonMarging, titleRectWidth, titleRectHeight, buttonWidth;
            int caretPos, textRectLeftMargin, textRectLeftMargin2, textNoteExtend;
            int textAlign, titleAlign;

            CSGRAPHIC_CONTEXT incUpImage1, incDownImage1, undoImage1, redoImage1, unrollImage1, noNameImage1;
            CSGRAPHIC_CONTEXT incUpImage2, incDownImage2, undoImage2, redoImage2, unrollImage2, noNameImage2;
            CSGRAPHIC_CONTEXT incUpImage3, incDownImage3, undoImage3, redoImage3, unrollImage3, noNameImage3;
            CSGRAPHIC_CONTEXT incUpImage4, incDownImage4, undoImage4, redoImage4, unrollImage4, noNameImage4;

            wchar_t* title, *textNote;
            HFONT titleFont;
            HFONT textFont, textNoteFont;
            wstring textChar;
            vector<RECT> textCharGeometry;
            vector<CSINPUT_CHAR_VISUAL> textCharVisual;
            int charHeight, charWidth, titleHeight, titleWidth;
            float textFontSizeCoef, titleFontSizeCoef;

            wstring allowedChars, forbiddenChars;

            bool multilineStyle;
            bool withItemRollup;
            bool withHistoryRollup;

            /*wchar_t* incUpImage1, incDownImage1, undoImage1, redoImage1;
            wchar_t* incUpImage2, incDownImage2, undoImage2, redoImage2;
            wchar_t* incUpImage3, incDownImage3, undoImage3, redoImage3;
            wchar_t* incUpImage4, incDownImage4, undoImage4, redoImage4;*/
            int incPos;

            CSRGBA activeCharColor, activeCharBkgColor;

            CSRGBA colorNormal_frame, colorNormal_frameBorder;// normal
            CSRGBA colorNormal_titleFrame, colorNormal_titleFrameBorder;
            CSRGBA colorNormal_text, colorNormal_title;

            CSRGBA colorHover_frame, colorHover_frameBorder;// hover
            CSRGBA colorHover_titleFrame, colorHover_titleFrameBorder;
            CSRGBA colorHover_text, colorHover_title;

            CSRGBA colorActive_frame, colorActive_frameBorder;// active
            CSRGBA colorActive_titleFrame, colorActive_titleFrameBorder;
            CSRGBA colorActive_text, colorActive_title;

            CSRGBA colorDisabled_frame, colorDisabled_frameBorder;// disabled
            CSRGBA colorDisabled_titleFrame, colorDisabled_titleFrameBorder;
            CSRGBA colorDisabled_text, colorDisabled_title;

            CSRGBA selectionColor, textNoteColor;

            CSLINEAR_BIND* gbpFrame;
            CSLINEAR_BIND* gbpBkg;
            CSLINEAR_BIND* gbpTitle;
            CSLINEAR_BIND* gbpIncUp;
            CSLINEAR_BIND* gbpIncDown;
            CSLINEAR_BIND* gbpUndo;
            CSLINEAR_BIND* gbpRedo;
            CSLINEAR_BIND* gbpNoName;
            CSLINEAR_BIND* gbpUnroll;

            bool frameOn;
            bool bkgOn;
            bool btnUpOn;
            bool btnDownOn;
            bool btnUndoOn;
            bool btnRedoOn;
            bool btnUnrollOn;
            bool btnNoNameOn;

            bool editable = true;
            bool noNameButtonState = false;
            bool noNameButtonVariableState = false;
            bool switchableState = false;


            wchar_t* prefix, *suffix;
        };
    
    bool startChecking = 0, startGBP=0;
    int checkCount = 0;
    void init(int*idp);
    void createContext(int idInput);
    
    void newInput(wchar_t*title, wchar_t*textNote, RECT geometry, int style, int titleRectWidth, int titleRectHeight);
    void addIncrementButtons(int idInput, wchar_t*path11, wchar_t*path12, wchar_t*path13, wchar_t*path14,
                                  wchar_t*path21, wchar_t*path22, wchar_t*path23, wchar_t*path24);
    void addUndoRedoButtons(int idInput, wchar_t*path11, wchar_t*path12, wchar_t*path13, wchar_t*path14,
                                  wchar_t*path21, wchar_t*path22, wchar_t*path23, wchar_t*path24);
    void addUnrollButton(int idInput, wchar_t*path11, wchar_t*path12, wchar_t*path13, wchar_t*path14);
    void addNoNameButton(int idInput, wchar_t*path11, wchar_t*path12, wchar_t*path13, wchar_t*path14);

    void setGBP(int idInput, int bindLeft, int bindTop, int bindRight, int bindBottom);
    CSINPUT::CSINPUT_PARAMS* getInputParams(int idInput);
    void setActive(int idInput);
    void setLastActive(int idInput);
    int getActive();
    int getLastActive();
    void setText(int idInput, wchar_t*value, bool update=false);
    wstring getText(int idInput);
    void deleteValue(int idInput);
    int getMouseHoverId();
    vector<CSINPUT_PARAMS*> getInputParamsList();
    int getInputsNumber();
    void mouseMoveEvent(int idInput, POINT p);
    RECT updateRect(int idInput, CSLINEAR_BIND*lb);
    void updateGeometry(int idInput);
    void _updateGeometry(int idInputm);
    void updateVisual_state1(int idInput, bool noRedrawActiveChar=0);
    void updateVisual_state2(int idInput, bool noRedrawActiveChar=0);
    void _updateVisual_state1(int idInput, bool noRedrawActiveChar=0);
    void _updateVisual_state2(int idInput, bool noRedrawActiveChar=0);
    void updateAll();
    void updateVisibleBackground();
    void updateBackground();
    int addChar(wchar_t chr);
    int deleteChar(bool frontCaret);
    void updateText(int idInput);
    void updateActiveTextBkg();
    void updateActiveTextBkg(RECT rchar);
    int updateActiveInput();
    int updateLastActiveInput();
    RECT placeCaret(POINT p);
    void textCharGeometryCorrection(int from);
    RECT getFrontEndCharRect(int idInput, int idChar);
    void highlightActiveChar(bool state=0);
    int getActiveInputCharNumber();
    int getId();
    int getActiveInputId();
    HFONT getFont(int idInput);
    void blit();
    void update();
    void setTitleRectWidth(int idInput, int width);
    void setTitleColors(int idInput, CSRGBA colorNormal, CSRGBA colorHover, CSRGBA colorActive={0}, CSRGBA colorDisabled={0});
    void setTitleFrameColors(int idInput, CSRGBA colorNormal, CSRGBA colorHover, CSRGBA colorActive, CSRGBA colorDisabled={0});
    void setTitleFrameBorderColors(int idInput, CSRGBA colorNormal, CSRGBA colorHover, CSRGBA colorActive, CSRGBA colorDisabled={0});
    void setTextColors(int idInput, CSRGBA colorNormal, CSRGBA colorHover, CSRGBA colorActive, CSRGBA colorDisabled={0});
    void setFrameColors(int idInput, CSRGBA colorNormal, CSRGBA colorHover, CSRGBA colorActive, CSRGBA colorDisabled={0});
    void setFrameBorderColors(int idInput, CSRGBA colorNormal, CSRGBA colorHover, CSRGBA colorActive, CSRGBA colorDisabled={0});
    void setEditable(int idInput, bool editable);
    void hide();
    void show();
    HDC getStackDC();
    void updateStackDC();
    void __getEventsGroup();

    void setAllowedChars(int idInput, wstring chars);
    void replaceAllowedChar(int idInput, wchar_t oldChar, wchar_t newChar);
    bool isMouseHoveringNoNameButton(int idInput);
    void switchNoNameButtonState(int idInput);
    void setNoNameButtonState(bool state);
    void setNoNameButtonVariableState(int idInput, bool state);
    void setNoNameButtonVariableState(bool state);
    void setForbiddenChars(int idInput, wstring chars);
    void setGlobalAllowedChars(wstring chars);
    void setGlobalForbiddenChars(wstring chars);
    void resetActiveInput();
    void setLogSection(int* logSection);
    int* getLogSection();
    void setEditable(bool editable);
    void setPrefix(int idInput, wchar_t* prefix);
    void setSuffix(int idInput, wchar_t* suffix);
    void setTitle(int idInput, wchar_t* title);
    void setTextRectLeftMargin(int idInput, int margin);
    void setTextAlign(int idInput, int align);
    void setTextFontSizeCoef(int idInput, float coef);
    void setTitleAlign(int idInput, int align);
    void setTitleFontSizeCoef(int idInput, float coef);
    void setSwitchable(int idInput, bool state);
    bool getSwitchable(int idInput);
    void switchInputState();
    void update(int idInput);

    void moveInput(int idInput, int x, int y, int gbpl=0, int gbpt=0, int gbpr=0, int gbpb=0);

    CSINPUT_ENTITY_COLORS getTitleColors(int idInput);
    bool getNoNameButtonState(int idInput);

    ACTION_PARAMS actionParams;

    protected:
    CSLINEAR_BIND* CSLINEAR_BIND_PTR(CSLINEAR_BIND lb);
    void drawGdiRectangle(HDC& hdc, CSRGBA col, RECT rect);
    void drawGdiRectangle(HDC& hdc, CSRGBA brd, CSRGBA bkg, RECT rect);
    void drawGdiText(HDC& hdc, wchar_t*text, HFONT hf, CSRGBA col, RECT rect);
    void drawImage(HDC& hdc, CSGRAPHIC_CONTEXT dc, RECT r);
    void viewTextNote(int id);

    

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

    wstring globalAllowedChars, globalForbiddenChars;

    int groupMsgPos;

    int* logSection;
    bool globalModifiable = true;
    bool globalNoNameButtonState = false;
    bool globalNoNameButtonVariableState = false;

    vector<CSINPUT_PARAMS*> ip;
};

CSINPUT* csNewInputContext(int*id);
CSINPUT* inputContextExample(int idp);

#endif // CSINPUT_H

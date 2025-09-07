#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include <fstream>
#include <stdexcept>
#include <codecvt>
#include <locale>

#include "csSection.h"
#include "csUIControls.h"

#include "csStrUtils.h"
#include "csSubClasses.h"
#include "readwfile.h"
#include "csListBoxMin.h"

using namespace std;
using namespace CSUTILS;
using namespace CSUIMAN;


typedef struct ADATA
{
    CSARITHMETIC::csRNUMBER number, root, outRoot, error;
    double execTime;
    char* method;
    char* param;
};

vector<ADATA> adata;

extern int CAPTION_AREA_SIZE;

CSARGS forceEventArgs(0);
void forceEventFunction(CSARGS Args);

int ROOT, MENU_SECTION, LEFT_SECTION, MIDDLE_TOP_SECTION, MIDDLE_BOTTOM_SECTION, MIDDLE_SECTION, RIGHT_SECTION, BOTTOM_SECTION;
int NR_SECTION, CS_SECTION, BIS_SECTION;
int BTN_NEW_FILE, BTN_NEW_OUTPUT, BTN_REMOVE_FILE, BNT_RANDOM_NUMBER, BTN_ADD_TO_DATA_LIST;
int BTN_REMOVE_SAVE_1, BTN_REMOVE_UNDO_1, BTN_REMOVE_REDO_1, BTN_REMOVE_SAVE_2, BTN_REMOVE_UNDO_2, BTN_REMOVE_REDO_2;
int STATIC_SEARCH, BUTTON_SEARCH;

int MENU_NS, MENU_BIS, MENU_CS, MENU_PREFERENCES, MENU_ABOUT;
int SYSCOMMAND_SECTION, SYS_CLOSE, SYS_MAX, SYS_MIN, TITLE_SECTION, RIGHT_SECTION_CHILD;

int ABOUT_UI, ABOUT_UI_CLIENT, ABOUT_UI_BOTTOM, ABOUT_SUB_MENU, ABOUT_P1, ABOUT_P2, ABOUT_P3, ABOUT_P4, ABOUT_PTITLE, ABOUT_PINDEX;


int smx = GetSystemMetrics(SM_CXSCREEN);
int smy = GetSystemMetrics(SM_CYSCREEN);
// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    CSUIMAN::_CSIGMA_INIT_(hInstance, forceEventFunction, &forceEventArgs);

    CSUIMAN::setSaveAppSizes(0);
    //float dimFact = GetSystemMetrics(SM_CXSCREEN)*GetSystemMetrics(SM_CYSCREEN)/(1366.0*768);
    float dimFact = 1.5;
    setSizeFactor(dimFact);
    /***************************** Root ********************************** */

    ROOT = createSection(-1, {100,25,600,500},  RGB(30,30,30), {1,1,1,1,1,1,1,1});
    setIcon(sHandle(ROOT), L"icon.ico", L"icon.ico");
    CSUIMAN::enableDarkEdge(ROOT);
    /********************************************************************************** */
    CSUICONTROLS::addTitle(ROOT, L"CSIGMA LIB TEST\0",{150,CAPTION_AREA_SIZE/dimFact}, "img/icon.bmp\0", 22);

    /******************************** Tips popup **************************************** */
    CSUICONTROLS::createTipsPupop(RGB(40,40,40));

    /*************************************** LEFT_SECTION ************************************ */

    CAPTION_AREA_SIZE *= 1.5;
    LEFT_SECTION = createSection(ROOT, {0,CAPTION_AREA_SIZE/dimFact,75,450},  RGB(20,20,20), {0,0,1,0});

    BIND_GEOM_PARAMS bd = {LEFT_SECTION, {0,-1,0,1}, {0,BIND_DEST_BOTTOM_EDGE, 0, BIND_DEST_BOTTOM_EDGE}};
    bindGeometry(ROOT, 1, bd); // C version
    
    /*************************************** MIDDLE_SECTION ************************************ */

    MIDDLE_SECTION = createSection(ROOT, {77,CAPTION_AREA_SIZE/dimFact,515,450+20},  RGB(10,40,30), {0,0,0,0});

    bd = {MIDDLE_SECTION, {0,0,1,0}, {0,0,BIND_DEST_LEFT_EDGE|BIND_DEST_RIGHT_EDGE,0}};
    bindGeometry(LEFT_SECTION, 1, bd);
    bd = {MIDDLE_SECTION, {-1,-1,1,1}, {BIND_DEST_RIGHT_EDGE,BIND_DEST_BOTTOM_EDGE,BIND_DEST_RIGHT_EDGE,BIND_DEST_BOTTOM_EDGE}};
    bindGeometry(ROOT, bd); // cpp version

    /*************************************** MIDDLE_TOP_SECTION ************************************ */

    MIDDLE_TOP_SECTION = createSection(MIDDLE_SECTION, {1,1,517,21},  RGB(30,30,30), {0,0,0,1});
    bd = {MIDDLE_TOP_SECTION, {0,0,1,0}, {BIND_DEST_RIGHT_EDGE,0,BIND_DEST_RIGHT_EDGE,0}};
    bindGeometry(MIDDLE_SECTION, 1, bd);
    bd = {MIDDLE_TOP_SECTION, {0,0,-1,0}, {0,0,BIND_DEST_RIGHT_EDGE,0}};
    bindGeometry(LEFT_SECTION, bd);
    bd = {MIDDLE_TOP_SECTION, {-1,0,1,0}, {BIND_DEST_RIGHT_EDGE,0,BIND_DEST_RIGHT_EDGE,0}};
    bindGeometry(ROOT, bd); // cpp version

    /***********************************MIDDLE_BOTTOM_SECTION *************************************** */

    MIDDLE_BOTTOM_SECTION = createSection(MIDDLE_SECTION, {1,23,515/2-2,395+25},  RGB(25,25,25), {0,0,0,0});

    bd = {MIDDLE_BOTTOM_SECTION, {0,0,0,1}, {0,0, 0, BIND_DEST_TOP_EDGE|BIND_DEST_BOTTOM_EDGE}};
    bindGeometry(MIDDLE_TOP_SECTION, 1, bd);
    bd = {MIDDLE_BOTTOM_SECTION, {0,-1,0,1}, {0,BIND_DEST_BOTTOM_EDGE, 0, BIND_DEST_BOTTOM_EDGE}};
    bindGeometry(ROOT, bd);
    bd = {MIDDLE_BOTTOM_SECTION, {0,0,1,0}, {BIND_DEST_RIGHT_EDGE,0,BIND_DEST_RIGHT_EDGE,0}};
    bindGeometry(MIDDLE_SECTION, bd);
    bd = {MIDDLE_BOTTOM_SECTION, {0,0,-0.5,0}, {0,0,BIND_DEST_RIGHT_EDGE,0}};
    bindGeometry(LEFT_SECTION, bd);

    /*************************************** RIGHT_SECTION ************************************ */

    RIGHT_SECTION = createSection(MIDDLE_SECTION, {515/2,23,515/2-1,395+25},  RGB(30,30,30), {1,0,0,0});

    bd = {MIDDLE_BOTTOM_SECTION, {1,0,0,0}, BIND_DEST_RIGHT_EDGE,0,0,0};
    bindGeometry(RIGHT_SECTION, bd);
    bd = {MIDDLE_TOP_SECTION, {0,0,1,0}, 0,0,BIND_DEST_RIGHT_EDGE,0};
    bindGeometry(ROOT, bd);
    bd = {MIDDLE_BOTTOM_SECTION, {0,0,1,0}, 0,0,BIND_DEST_RIGHT_EDGE,0};
    bindGeometry(ROOT, bd);
    bd = {MIDDLE_SECTION, {0,0,1,0}, 0,0,BIND_DEST_RIGHT_EDGE,0};
    bindGeometry(ROOT, bd);
    bd = {RIGHT_SECTION, {0,0,0,1}, {0,0, 0, BIND_DEST_TOP_EDGE|BIND_DEST_BOTTOM_EDGE}};
    bindGeometry(MIDDLE_TOP_SECTION, 1, bd);
    bd = {RIGHT_SECTION, {-1,-1,1,1}, {BIND_DEST_RIGHT_EDGE,BIND_DEST_BOTTOM_EDGE,
                                       BIND_DEST_LEFT_EDGE,BIND_DEST_BOTTOM_EDGE
                                      }
         };
    bindGeometry(ROOT, bd);
    bd = {RIGHT_SECTION, {0,0,-0.5,0}, {0,0,BIND_DEST_RIGHT_EDGE_OPP|BIND_DEST_LEFT_EDGE,0}};
    bindGeometry(LEFT_SECTION, bd);

    CSUIMAN::updateAfterReceivingResizeMessage(RIGHT_SECTION);

    /**************************************************************************************************** */

    NR_SECTION = createSection(MIDDLE_TOP_SECTION, {2,0,148+136/3,19},  RGB(25,25,25), {0,0,0,0});
    bd = {NR_SECTION, {0,0,0,1}, 0,0,0,BIND_DEST_BOTTOM_EDGE};
    bindGeometry(MIDDLE_TOP_SECTION, bd);
    bd = {NR_SECTION, {0,0,0.5,0}, 0,0,BIND_DEST_RIGHT_EDGE,0};
    bindGeometry(ROOT, bd);
    CSUIMAN::updateAfterReceivingResizeMessage(NR_SECTION);

    CS_NUMERIC_INCREMENTER_PARAMS nrIds01 = CSUICONTROLS::numericIncrementerExt(NR_SECTION, {2,2,70,15}, L"10", L"1", INPUT_FORMAT_INTERGER);
    CS_NUMERIC_INCREMENTER_PARAMS nrIds02 = CSUICONTROLS::numericIncrementerExt(NR_SECTION, {2+73,2,70,15}, L"12", L"1", INPUT_FORMAT_INTERGER);
    CS_NUMERIC_INCREMENTER_PARAMS nrIds03 = CSUICONTROLS::numericIncrementerExt(NR_SECTION, {2+73*2,2,70,15}, L"1", L"10", INPUT_FORMAT_INTERGER);
    CS_NUMERIC_INCREMENTER_PARAMS nrIds04 = CSUICONTROLS::numericIncrementerExt(NR_SECTION, {2+73*3,2,70,15}, L"1", L"10", INPUT_FORMAT_INTERGER);

    nrIds01.setMinBound("1");
    nrIds02.setMinBound("1");
    nrIds03.setMinBound("1");
    nrIds04.setMinBound("1");

    BIS_SECTION = createSection(MIDDLE_TOP_SECTION, r(152+136*1/3,0,151+136/3,19),  RGB(30,30,30), {1,0,0,0});
    bd = {BIS_SECTION, {0,0,0,1}, 0,0,0,BIND_DEST_BOTTOM_EDGE};
    bindGeometry(MIDDLE_TOP_SECTION, bd);
    bd = {NR_SECTION, {1,0,0,0}, {BIND_DEST_RIGHT_EDGE,0,0,0}};
    bindGeometry(BIS_SECTION, bd);
    bd = {BIS_SECTION, {0,0,0.5,0}, 0,0,BIND_DEST_LEFT_EDGE|BIND_DEST_RIGHT_EDGE_ONE,0};
    bindGeometry(ROOT, bd);
    CSUIMAN::updateAfterReceivingResizeMessage(BIS_SECTION);
    CS_NUMERIC_INCREMENTER_PARAMS bisIds01 = CSUICONTROLS::numericIncrementerExt(BIS_SECTION, {2,2,70,15}, L"10", L"1", INPUT_FORMAT_INTERGER);
    CS_NUMERIC_INCREMENTER_PARAMS bisIds02 = CSUICONTROLS::numericIncrementerExt(BIS_SECTION, {2+73,2,70,15}, L"12", L"1", INPUT_FORMAT_INTERGER);
    CS_NUMERIC_INCREMENTER_PARAMS bisIds03 = CSUICONTROLS::numericIncrementerExt(BIS_SECTION, {2+73*2,2,70,15}, L"1", L"10", INPUT_FORMAT_INTERGER);
    CS_NUMERIC_INCREMENTER_PARAMS bisIds04 = CSUICONTROLS::numericIncrementerExt(BIS_SECTION, {2+73*3,2,70,15}, L"1", L"10", INPUT_FORMAT_INTERGER);

    bisIds01.setMinBound("1");
    bisIds02.setMinBound("1");
    bisIds03.setMinBound("1");
    bisIds04.setMinBound("1");

    CS_SECTION = createSection(MIDDLE_TOP_SECTION, {305+136*2/3,0,70+136/3,19},  RGB(25,25,25), {1,0,0,0});
    bd = {BIS_SECTION, {1,0,0,0}, {BIND_DEST_RIGHT_EDGE,0,0,0}};
    bindGeometry(CS_SECTION, bd);
    bd = {CS_SECTION, {0,0,0,1}, 0,0,0,BIND_DEST_BOTTOM_EDGE};
    bindGeometry(MIDDLE_TOP_SECTION, bd);
    bd = {CS_SECTION, {0,0,-1,0}, 0,0,BIND_DEST_RIGHT_EDGE,0};
    bindGeometry(LEFT_SECTION, bd);
    bd = {CS_SECTION, {-1,0,1,0}, BIND_DEST_RIGHT_EDGE,0,BIND_DEST_LEFT_EDGE,0};
    bindGeometry(ROOT, bd);
    CSUIMAN::updateAfterReceivingResizeMessage(CS_SECTION);
    CS_NUMERIC_INCREMENTER_PARAMS csIds01 = CSUICONTROLS::numericIncrementerExt(CS_SECTION, {2,2,70,15}, L"10", L"1", INPUT_FORMAT_INTERGER);
    CS_NUMERIC_INCREMENTER_PARAMS csIds02 = CSUICONTROLS::numericIncrementerExt(CS_SECTION, {2+73,2,70,15}, L"12", L"1", INPUT_FORMAT_INTERGER);
    CS_NUMERIC_INCREMENTER_PARAMS csIds03 = CSUICONTROLS::numericIncrementerExt(CS_SECTION, {2+73*2,2,70,15}, L"1", L"10", INPUT_FORMAT_INTERGER);
    CS_NUMERIC_INCREMENTER_PARAMS csIds04 = CSUICONTROLS::numericIncrementerExt(CS_SECTION, {2+73*3,2,70,15}, L"1", L"10", INPUT_FORMAT_INTERGER);

    csIds01.setMinBound("1");
    csIds02.setMinBound("1");
    csIds03.setMinBound("1");
    csIds04.setMinBound("1");

    /********************************************************************************** */

    BOTTOM_SECTION = createSection(ROOT, {0,500-35,599,25},  RGB(20,20,20), {0,0,0,0});
    bd = {BOTTOM_SECTION, {-1,-1,1,1}, {BIND_DEST_RIGHT_EDGE,BIND_DEST_TOP_EDGE,BIND_DEST_RIGHT_EDGE,BIND_DEST_TOP_EDGE}};
    bindGeometry(ROOT, bd);
    //setBorderColorAndThick(BOTTOM_SECTION, RGB(20,20,20), 1);

    STATIC_SEARCH = createSection(BOTTOM_SECTION, {(600-200)/2,4,200,16},  RGB(15,15,15), {0,0,0,0});
    bd = {STATIC_SEARCH, {-0.5,0,0.5,0}, {BIND_DEST_LEFT_EDGE,0,BIND_DEST_LEFT_EDGE,0}};
    bindGeometry(ROOT, bd);
    CSUIFX::setBorderColorGradient(STATIC_SEARCH, {100,100,100}, {40,40,40}, 2, 2);
    setBorderThick(STATIC_SEARCH, 2);
    setTitle(STATIC_SEARCH, CSTEXT{.Text=L"Search...\0", .Font=L"Arial", .FontSize=16, .Italic=1, .Color={100,100,100},
                                   .Marging={-15,0}, .Align = CS_TA_CENTER, .Show=1});

    BUTTON_SEARCH = CSUICONTROLS::iconButton01(STATIC_SEARCH, "img/search02.bmp\0", "img/search01.bmp\0", {200-14-1,1,14,14});

    int redit = csCreateRichEdit(STATIC_SEARCH, {4,4,35,4}, L"Search...", 0, 0);

    /********************************************************************************** */

    CSSYSCOMMAND_SECTION SYS_CMD= CSUICONTROLS::addSysCommand(ROOT, {600});

    CS_NUMERIC_INCREMENTER_PARAMS fontSizeInc = CSUICONTROLS::numericIncrementerExt1(ROOT, {600-120,0,65,16}, L"16", L"1", INPUT_FORMAT_INTERGER,
                                                            {-1,0,1,0}, {BIND_DEST_LEFT_EDGE,0,BIND_DEST_LEFT_EDGE,0});
    fontSizeInc.setMinBound("8");
    fontSizeInc.setMaxBound("30");
    CSUIMAN::setBorderColorAndThick(fontSizeInc.idSection, RGB(60,60,60), 1);

    void setFontSize(CSARGS Args);
    csSetUpdatingFunction(fontSizeInc.idText, setFontSize, 2, &MIDDLE_BOTTOM_SECTION, &fontSizeInc.idSection);

    CSDYNAMIC_SIMPLE_TEXT tipsMessage0;
    tipsMessage0.marg = {10,10,10,10};
    tipsMessage0.updateGASize = 1;
    tipsMessage0.view = 1;
    wchar_t* txt = makeWcharString(L"Font Size \0");
    tipsMessage0.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri", .FontSize = 18, .Italic=0,
                                   .Bold=FW_BOLD, .Color={150,150,100}});
    tipsMessage0.pSpace.push_back(5);
    txt = makeWcharString(L"Set de font size of text.\0");
    tipsMessage0.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri", .FontSize = 16, .Italic=1,
                                   .Bold=FW_THIN, .Color={150,150,150}});
    tipsMessage0.pSpace.push_back(5);
    CSUIMAN::addTips(fontSizeInc.idSection, {0,0,200/dimFact, 150/dimFact}, (POS_BOOL){.bRBottom=1}, 3, 0, tipsMessage0);


    /********************************************************************************** */
    int cy = CAPTION_AREA_SIZE/dimFact-2;
    MENU_SECTION = createSection(ROOT, {(sRectClient(ROOT).right/dimFact-300)/2,0,300,cy},  RGB(15,15,15), {0,0,0,0});
    bd = {MENU_SECTION, {-0.5,0,0.5,0}, {BIND_DEST_LEFT_EDGE,0,BIND_DEST_LEFT_EDGE,0}};
    bindGeometry(ROOT, bd);
    int cx = 70;

    MENU_NS = CSUICONTROLS::darkImgTextButton01(MENU_SECTION, L"CSigma Menu 1\0", "img/Settings01.bmp", "img/Settings01.bmp", {1,0,cx*1.5,cy-1}, 16);
    MENU_BIS = CSUICONTROLS::darkImgTextButton01(MENU_SECTION, L"CS Menu 2\0", "img/Settings01.bmp", "img/Settings01.bmp", {1+cx*1.5,0,cx,cy-1}, 16);
    MENU_CS = CSUICONTROLS::darkImgTextButton01(MENU_SECTION, L"CS Menu 3\0", "img/Settings01.bmp", "img/Settings01.bmp", {1+cx*2.5,0,cx,cy-1}, 16);
    //MENU_PREFERENCES = CSUICONTROLS::darkImgTextButton01(MENU_SECTION, L"PREFERENCES\0", "img/Settings01.bmp", "img/Settings01.bmp", {1+cx*3,0,cx*1.5,cy-1}, 16);
    MENU_ABOUT = CSUICONTROLS::darkImgTextButton01(MENU_SECTION, L"About\0", "img/About01.bmp", "img/About01.bmp", {1+cx*3.5,0,cx,cy-1}, 16);

    //ABOUT_UI = createSection(-1, {100,25,600,500},  RGB(0,0,0), {1,1,1,1,1,1,1,1}, 1, 1);
    ABOUT_UI = createSection(0, {100,25,600,500},  RGB(40,40,40), {1,1,1,1,1,1,1,1}, 0, 1, 0);
    CSUIMAN::enableDarkEdge(ABOUT_UI);
    CSUICONTROLS::addTitle(ABOUT_UI, L"About",{60,CAPTION_AREA_SIZE/dimFact}, "img/About01.bmp");

    ABOUT_UI_CLIENT = createSection(ABOUT_UI, {10/dimFact,CAPTION_AREA_SIZE/dimFact,590-10/dimFact,500-CAPTION_AREA_SIZE/dimFact-10-20},  RGB(0,0,0), {0}, 1);
    bd = {ABOUT_UI_CLIENT, {-1,-1,1,1}, {BIND_DEST_RIGHT_EDGE, BIND_DEST_BOTTOM_EDGE,
                                        BIND_DEST_RIGHT_EDGE, BIND_DEST_BOTTOM_EDGE}};
    bindGeometry(ABOUT_UI, bd);

    ABOUT_UI_BOTTOM = createSection(ABOUT_UI, {1,(CAPTION_AREA_SIZE+sRectClient(ABOUT_UI_CLIENT).bottom)/dimFact,590,20},  RGB(40,40,40), {0}, 1);
    bd = {ABOUT_UI_BOTTOM, {-1,-1,1,1}, {BIND_DEST_RIGHT_EDGE, BIND_DEST_TOP_EDGE,
                                        BIND_DEST_RIGHT_EDGE, BIND_DEST_TOP_EDGE}};
    bindGeometry(ABOUT_UI, bd);

    CSUIMAN::setTitle(ABOUT_UI_BOTTOM, CSTEXT{.Text=L"Resume 1 : Mining ingineer", .Font=L"calibri", .FontSize = 16, .Italic=0,
                                   .Bold=0, .Color={180,180,180},
                                   .Marging={0,0}, .Align = CS_TA_CENTER, .Show=1});

    int h = (CAPTION_AREA_SIZE-2)/dimFact, w=30, marg = 1, total = w+marg;
    ABOUT_SUB_MENU = createSection(ABOUT_UI, {(sRectClient(ABOUT_UI).right/dimFact-total*4)/2,0,total*4,h},  RGB(20,20,20), {0});
    bd = {ABOUT_SUB_MENU, {-0.5,0,0.5,0}, {BIND_DEST_LEFT_EDGE,0,BIND_DEST_LEFT_EDGE,0}};
    bindGeometry(ABOUT_UI, bd);

    ABOUT_P1 = CSUICONTROLS::darkTextButton01(ABOUT_SUB_MENU, L"1\0", {0,0,w,h});
    ABOUT_P2 = CSUICONTROLS::darkTextButton01(ABOUT_SUB_MENU, L"2\0", {total*1,0,w,h});
    ABOUT_P3 = CSUICONTROLS::darkTextButton01(ABOUT_SUB_MENU, L"3\0", {total*2,0,w,h});
    ABOUT_P4 = CSUICONTROLS::darkTextButton01(ABOUT_SUB_MENU, L"4\0", {total*3,0,w,h});

    CSSYSCOMMAND_SECTION SYS_CMD2 = CSUICONTROLS::addSysCommand(ABOUT_UI, {600});
    CSSCROLLBAR hscrollAbout = CSUICONTROLS::addHScrollBar(&ABOUT_UI_CLIENT, &ABOUT_UI_CLIENT, 0, 10);
    CSSCROLLBAR vscrollAbout = CSUICONTROLS::addVScrollBar(&ABOUT_UI_CLIENT, &ABOUT_UI_CLIENT, 0, 10);
    hscrollAbout.setViewFrameBottomMarging(10);
    vscrollAbout.setViewFrameRightMarging(10);

    int idcv = csGraphics::loadImage(ABOUT_UI_CLIENT, L"img/csigma logo.bmp", {1,1}, {0,0});
    SIZE sizecv = csGraphics::getImageSize(ABOUT_UI_CLIENT, idcv);
    csGraphics::showImage(ABOUT_UI_CLIENT, idcv, {0}, sizecv);

    csGraphics::setGraphicAreaPosition(ABOUT_UI_CLIENT,{0,0});
    csGraphics::setGraphicAreaColor(ABOUT_UI_CLIENT,{50,50,50},{0});
    csGraphics::setGraphicAreaSize(ABOUT_UI_CLIENT,sizecv);
    csGraphics::updateGraphicArea(ABOUT_UI_CLIENT, 1);
    csGraphics::setHzoom(ABOUT_UI_CLIENT, 0.1);
    csGraphics::setVzoom(ABOUT_UI_CLIENT, 0.1);
    hscrollAbout.setPositionRatio(0.5);
    void showAboutUi(CSARGS Args);
    CSUIMAN::addAction(MENU_ABOUT, showAboutUi, 1, &ABOUT_UI);

    /*CSLOCKED_MODE lm;
    lm.Lockable = 1;
    lm.Locked = 1;
    lm.ExternalUnlocking = 0;
    lm.LockingColor = {150,150,150};
    lm.LockingTitleColor = {0};
    lm.LockingBorderColor = {130,130,130};

    CSUIMAN::setLockable(MENU_NS, lm);
    lm.Locked = 0;
    CSUIMAN::setLockable(MENU_BIS, lm);
    CSUIMAN::setLockable(MENU_CS, lm);*/


    /********************************************************************************** */
    h = 16, marg = 4, total = h+marg;
    //BTN_NEW_FILE = CSUICONTROLS::darkTextButton01(LEFT_SECTION, L"New File\0", {5,5,65,h});
    CS_NUMERIC_INCREMENTER_PARAMS internPrec = CSUICONTROLS::numericIncrementerExt1(LEFT_SECTION, {5,5,65,h}, L"1", L"1", INPUT_FORMAT_INTERGER);
    internPrec.setMinBound("1");

    CSDYNAMIC_SIMPLE_TEXT tipsMessage;
    tipsMessage.marg = {10,10,10,10};
    tipsMessage.updateGASize = 1;
    tipsMessage.view = 1;
    txt = makeWcharString(L"Op. Precision \0");
    tipsMessage.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri", .FontSize = 18, .Italic=0,
                                   .Bold=FW_BOLD, .Color={150,150,100}});
    tipsMessage.pSpace.push_back(5);
    txt = makeWcharString(L"Represents the precision of operations. Greater is it, more precise are the results but slower are calculations.\0");
    tipsMessage.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri", .FontSize = 16, .Italic=1,
                                   .Bold=FW_THIN, .Color={150,150,150}});
    tipsMessage.pSpace.push_back(5);
    CSUIMAN::addTips(internPrec.idSection, {0,0,250/dimFact, 200/dimFact}, (POS_BOOL){.bTRight=1}, 3, 0, tipsMessage);

    CS_NUMERIC_INCREMENTER_PARAMS externPrec = CSUICONTROLS::numericIncrementerExt1(LEFT_SECTION, {5,5+total*1,65,h}, L"1", L"1", INPUT_FORMAT_INTERGER);
    externPrec.setMinBound("1");
    CSDYNAMIC_SIMPLE_TEXT tipsMessage1;
    tipsMessage1.marg = {10,10,10,10};
    tipsMessage1.updateGASize = 1;
    tipsMessage1.view = 1;
    txt = makeWcharString(L"Nb. Precision \0");
    tipsMessage1.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri", .FontSize = 18, .Italic=0,
                                   .Bold=FW_BOLD, .Color={150,150,100}});
    tipsMessage1.pSpace.push_back(5);
    txt = makeWcharString(L"Represents the precision of numbers showed.\0");
    tipsMessage1.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri", .FontSize = 16, .Italic=1,
                                   .Bold=FW_THIN, .Color={150,150,150}});
    tipsMessage1.pSpace.push_back(5);
    CSUIMAN::addTips(externPrec.idSection, {0,0,200/dimFact, 150/dimFact}, (POS_BOOL){.bTRight=1}, 3, 0, tipsMessage1);


    CS_NUMERIC_INCREMENTER_PARAMS Nnb = CSUICONTROLS::numericIncrementerExt1(LEFT_SECTION, {5,5+total*2,65,h}, L"500", L"1", INPUT_FORMAT_INTERGER);
    Nnb.setMinBound("1");
    CSDYNAMIC_SIMPLE_TEXT tipsMessage2;
    tipsMessage2.marg = {10,10,10,10};
    tipsMessage2.updateGASize = 1;
    tipsMessage2.view = 1;
    txt = makeWcharString(L"Digits Number \0");
    tipsMessage2.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri", .FontSize = 18, .Italic=0,
                                   .Bold=FW_BOLD, .Color={150,150,100}});
    tipsMessage2.pSpace.push_back(5);
    txt = makeWcharString(L"Represents the number of digits of the number to be generated randomly.\0");
    tipsMessage2.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri", .FontSize = 16, .Italic=1,
                                   .Bold=FW_THIN, .Color={150,150,150}});
    tipsMessage2.pSpace.push_back(5);
    CSUIMAN::addTips(Nnb.idSection, {0,0,200/dimFact, 170/dimFact}, (POS_BOOL){.bTRight=1}, 3, 0, tipsMessage2);

    /********************************************************************************** */
    //BTN_NEW_OUTPUT = CSUICONTROLS::darkTextButton01(LEFT_SECTION, L"New Output\0", {5,5+total*1,65,h});
    CS_NUMERIC_INCREMENTER_PARAMS minDigit = CSUICONTROLS::numericIncrementerExt1(LEFT_SECTION, {5,5+total*3,65,h}, L"0", L"1", INPUT_FORMAT_INTERGER);
    minDigit.setMinBound("0");
    minDigit.setMaxBound("9");
    CSDYNAMIC_SIMPLE_TEXT tipsMessage3;
    tipsMessage3.marg = {10,10,10,10};
    tipsMessage3.updateGASize = 1;
    tipsMessage3.view = 1;
    txt = makeWcharString(L"Digit Boundary \0");
    tipsMessage3.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri", .FontSize = 18, .Italic=0,
                                   .Bold=FW_BOLD, .Color={150,150,100}});
    tipsMessage3.pSpace.push_back(5);
    txt = makeWcharString(L"Represents the lower or higher boundary of the digit to be generated randomly.\0");
    tipsMessage3.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri", .FontSize = 16, .Italic=1,
                                   .Bold=FW_THIN, .Color={150,150,150}});
    tipsMessage3.pSpace.push_back(5);
    CSUIMAN::addTips(minDigit.idSection, {0,0,200/dimFact, 170/dimFact}, (POS_BOOL){.bTRight=1}, 3, 0, tipsMessage3);

    /********************************************************************************** */
    //BTN_REMOVE_FILE = CSUICONTROLS::darkTextButton01(LEFT_SECTION, L"Remove File\0", {5,5+total*2,65,h});

    CS_NUMERIC_INCREMENTER_PARAMS maxDigit = CSUICONTROLS::numericIncrementerExt1(LEFT_SECTION, {5,5+total*4,65,h}, L"9", L"1", INPUT_FORMAT_INTERGER);
    maxDigit.setMinBound("0");
    maxDigit.setMaxBound("9");

    CSUIMAN::addTips(maxDigit.idSection, {0,0,200/dimFact, 170/dimFact}, (POS_BOOL){.bTRight=1}, 3, 0, tipsMessage3);
    /********************************************************************************** */

    void getRandomData(CSARGS Args);
    BNT_RANDOM_NUMBER = CSUICONTROLS::darkTextButton01(LEFT_SECTION, L"Random\0", {5,5+total*5,65,h});
    CSUIMAN::addAction(BNT_RANDOM_NUMBER, getRandomData, 4, &Nnb.idSection, &minDigit.idSection, &maxDigit.idSection, &MIDDLE_BOTTOM_SECTION);

    CSDYNAMIC_SIMPLE_TEXT tipsMSG;
    tipsMSG.marg = {10,10,10,10};
    tipsMSG.updateGASize = 1;
    tipsMSG.view = 1;
    txt = makeWcharString(L"Random \0");
    tipsMSG.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri", .FontSize = 18, .Italic=0,
                                   .Bold=FW_BOLD, .Color={150,150,100}});
    tipsMSG.pSpace.push_back(5);
    txt = makeWcharString(L"Generates a random integer number.\0");
    tipsMSG.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri", .FontSize = 16, .Italic=1,
                                   .Bold=FW_THIN, .Color={150,150,150}});
    tipsMSG.pSpace.push_back(5);
    CSUIMAN::addTips(BNT_RANDOM_NUMBER, {0,0,200/dimFact, 150/dimFact}, (POS_BOOL){.bTRight=1}, 3, 0, tipsMSG);

    /********************************************************************************** */

    BTN_ADD_TO_DATA_LIST = CSUICONTROLS::darkTextButton01(LEFT_SECTION, L"Add\0", {5,5+total*6,65,h});
    CSDYNAMIC_SIMPLE_TEXT tipsMSG1;
    tipsMSG1.marg = {10,10,10,10};
    tipsMSG1.updateGASize = 1;
    tipsMSG1.view = 1;
    txt = makeWcharString(L"Add   \0");
    tipsMSG1.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri", .FontSize = 18, .Italic=0,
                                   .Bold=FW_BOLD, .Color={150,150,100}});
    tipsMSG1.pSpace.push_back(5);
    txt = makeWcharString(L"Add a data to list.\0");
    tipsMSG1.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri", .FontSize = 16, .Italic=1,
                                   .Bold=FW_THIN, .Color={150,150,150}});
    tipsMSG1.pSpace.push_back(5);
    CSUIMAN::addTips(BTN_ADD_TO_DATA_LIST, {0,0,200/dimFact, 150/dimFact}, (POS_BOOL){.bTRight=1}, 3, 0, tipsMSG1);
    /********************************************************************************** */
    //BTN_REMOVE_SAVE_1 = CSUICONTROLS::darkTextButton01(LEFT_SECTION, L"Save 1\0", {5,5+total*6,65,h});
    CS_NUMERIC_INCREMENTER_PARAMS activeNum = CSUICONTROLS::numericIncrementerExt2(LEFT_SECTION, {5,5+total*7,65,h}, L"0", L"1", INPUT_FORMAT_INTERGER);
    activeNum.setMinBound("0");
    activeNum.setMaxBound("9");

    CS_STRING_INCREMENTER_PARAMS strInc = CSUICONTROLS::stringIncrementer(LEFT_SECTION, {5,5+total*8,65,h}, L"string 1", 1, 1,
                                                            {0,0,1,0}, {0,0,BIND_DEST_RIGHT_EDGE,0});
    strInc.newItem(L"string 2");
    strInc.newItem(L"string 3");
    strInc.newItem(L"string 4");
    strInc.newItem(L"string 5");
    strInc.newItem(L"string 6");
    strInc.newItem(L"string 7");
    strInc.newItem(L"string 8");
    strInc.newItem(L"string 9");

    //BTN_REMOVE_UNDO_1 = CSUICONTROLS::darkTextButton01(LEFT_SECTION, L"Undo 1\0", {5,5+total*9,65,h});

    BTN_REMOVE_REDO_1 = CSUICONTROLS::darkTextButton01(LEFT_SECTION, L"Redo 1\0", {5,5+total*10,65,h});
    BTN_REMOVE_SAVE_2 = CSUICONTROLS::darkTextButton01(LEFT_SECTION, L"Save 2\0", {5,5+total*12,65,h});
    BTN_REMOVE_UNDO_2 = CSUICONTROLS::darkTextButton01(LEFT_SECTION, L"Undo 2\0", {5,5+total*13,65,h});
    BTN_REMOVE_REDO_2 = CSUICONTROLS::darkTextButton01(LEFT_SECTION, L"Redo 2\0", {5,5+total*14,65,h});
    // Boucle principale



    /*********************************************************** */

    CSSCROLLBAR hscroll1 = CSUICONTROLS::addHScrollBar(&MIDDLE_BOTTOM_SECTION, &MIDDLE_BOTTOM_SECTION, 0, 10);
    CSSCROLLBAR vscroll1 = CSUICONTROLS::addVScrollBar(&MIDDLE_BOTTOM_SECTION, &MIDDLE_BOTTOM_SECTION, 0, 10);

    hscroll1.setViewFrameBottomMarging(10);
    vscroll1.setViewFrameRightMarging(10);

    csGraphics::setGraphicAreaPosition(MIDDLE_BOTTOM_SECTION,{0,0});
    csGraphics::setGraphicAreaColor(MIDDLE_BOTTOM_SECTION,{10,10,10},{0});
    csGraphics::setGraphicAreaSize(MIDDLE_BOTTOM_SECTION,{2500, 3500});
    csGraphics::updateGraphicArea(MIDDLE_BOTTOM_SECTION, 1);

    hscroll1.setPositionRatio(0);
    vscroll1.setPositionRatio(0);

    CSDYNAMIC_SIMPLE_TEXT dst;
    dst.marg = {10,10};
    dst.updateGASize = 1;
    txt = makeWcharString(L"0\0");
    dst.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri light", .FontSize = 16, .Italic=0,
                                   .Bold=FW_THIN, .Color={150,150,150}});
    dst.pSpace.push_back(12);
    dst.view = 1;
    csGraphics::setDynamicSimpleText(MIDDLE_BOTTOM_SECTION, dst);


    /*********************************************************** */

    //RIGHT_SECTION_CHILD = createSection(RIGHT_SECTION, {5,0,1500/dimFact,1000/dimFact},  RGB(30,30,30), {0,0,0,0});
    RIGHT_SECTION_CHILD = RIGHT_SECTION;

    CSSCROLLBAR hscroll2 = CSUICONTROLS::addHScrollBar(&RIGHT_SECTION, &RIGHT_SECTION_CHILD, 0, 10);
    CSSCROLLBAR vscroll2 = CSUICONTROLS::addVScrollBar(&RIGHT_SECTION, &RIGHT_SECTION_CHILD, 0, 10);

    hscroll2.setViewFrameBottomMarging(10);
    vscroll2.setViewFrameRightMarging(10);
    csGraphics::setHzoom(RIGHT_SECTION_CHILD, 1);
    csGraphics::setVzoom(RIGHT_SECTION_CHILD, 1);

    int idImg1 = csGraphics::loadImage(RIGHT_SECTION_CHILD, L"img/csigma logo.bmp", {1,1}, {0,0});
    SIZE size1 = csGraphics::getImageSize(RIGHT_SECTION_CHILD, idImg1);
    SIZE newSize1 = {size1.cx-200*4, size1.cy-200*4};
    csGraphics::setImageSize(RIGHT_SECTION_CHILD, idImg1, newSize1);
    csGraphics::setImagePos(RIGHT_SECTION_CHILD, idImg1, {200, 200});
    csGraphics::showImage(RIGHT_SECTION_CHILD, idImg1, {200,200}, newSize1);

    int idImg2 = csGraphics::loadImage(RIGHT_SECTION_CHILD, L"img/search02.bmp", {300,500}, {0,0});
    SIZE size2 = csGraphics::getImageSize(RIGHT_SECTION_CHILD, idImg2);
    SIZE newSize2 = {size2.cx*10, size2.cy*10};
    csGraphics::setImageSize(RIGHT_SECTION_CHILD, idImg2, newSize2);
    csGraphics::showImage(RIGHT_SECTION_CHILD, idImg2);

    int fact = std::max(ceil(1.0*size1.cx/smx), ceil(1.0*size1.cy/smy));
    SIZE gaSize = {fact*smx, fact*smy};
    csGraphics::setGraphicAreaColor(RIGHT_SECTION_CHILD, {10,10,10}, {0});
    csGraphics::setGraphicAreaSize(RIGHT_SECTION_CHILD, gaSize);
    csGraphics::updateGraphicArea(RIGHT_SECTION_CHILD, 1);

    hscroll2.setPositionRatio(0);
    vscroll2.setPosition(100);

    CSLISTBOXMIN* lbm = csNewMinimalListBoxPtr(&RIGHT_SECTION_CHILD, 100, 85);
    lbm->setDefaultFont(L"calibri",{12,0});
    lbm->setItemAlign(CS_ALIGN_VERTICAL);
    lbm->setOffset({1,1});

    lbm->setIcon(0, L"img\\search02.bmp",L"img\\search02.bmp", L"img\\search01.bmp", L"img\\search01.bmp");
    //lbm->newItem(L"Locked",{0},1,L"img\\search02.bmp", L"img\\search02.bmp", L"img\\search01.bmp");
    lbm->setMaxTextWidth(150);
    lbm->setDefaultTitle(L"Index");
    lbm->newItem(0,56,0);

    lbm->newIcon(L"img\\Code01.bmp");
    lbm->setDefaultTitle(L"Param");
    lbm->newItem(0,30,1);

    //lbm->newFilePath(L"Bitmap 24-bits\0*.bmp\0");

    lbm->setActiveItem(0);
    lbm->create();

    return CSUIMAN::_CSIGMA_SOFTWARE_();
}



HWND btnTracked;
void forceEventFunction(CSARGS Args)
{
    UINT message = (UINT)Args;
    HWND hwnd = (HWND)Args;
    //WPARAM wParam = (WPARAM)Args;
    //LPARAM lParam = (LPARAM)Args;

    if(message == WM_MOUSEMOVE)
        btnTracked = hwnd;
}

void showAboutUi(CSARGS Args)
{
    if(UINT(Args) == WM_LBUTTONDOWN)
    {
        int idAbout = *(int*)Args[0];
        if(IsWindowVisible(sHandle(idAbout)))
        {
            ShowWindow(sHandle(idAbout), 0);
        }
        else
        {
            ShowWindow(sHandle(idAbout), 1);
        }
    }
}

void getRandomData(CSARGS Args)
{
    static bool exec = 0;
    if(UINT(Args) == WM_LBUTTONUP && !exec)
    {
        exec = 1;

        thread t(
        [](CSARGS Args, bool *exec)
        {
            int idnDigits = *(int*)Args[0];
            int idMinSection = *(int*)Args[1];
            int idMaxSection = *(int*)Args[2];

            int n = strtod(wcharPtrToCharPtr(sTitle(idnDigits).Text).c_str(), 0);
            char mn = strtod(wcharPtrToCharPtr(sTitle(idMinSection).Text).c_str(), 0);
            char mx = strtod(wcharPtrToCharPtr(sTitle(idMaxSection).Text).c_str(), 0);



            if(mn > mx)
            {
                char t = mn;
                mn = mx;
                mx = t;
            }
            CSARITHMETIC::csRNUMBER number;
            number.random(n, mn, mx);

            int idGaphicArea = *(int*)Args[3];




            char *str = CSARITHMETIC::getPrintFormat(number);
            wstring w = charPtrtoWcharPtr(str).c_str();

            sDynSimpleText(idGaphicArea).paragraph[0].setText(w.c_str());
            csGraphics::updateGraphicArea(idGaphicArea, 1);

            free(str);
            number.clear();

            *exec = 0;

        }, Args, &exec);

        t.detach();
    }
}

void setFontSize(CSARGS Args)
{
    //if(UINT(Args) == WM_LBUTTONUP)
    {
        int idGaphicArea = *(int*)Args[0];
        int idFontSizeSection = *(int*)Args[1];

        int fsize = strtod(wcharPtrToCharPtr(sTitle(idFontSizeSection).Text).c_str(), 0);
        sDynSimpleText(idGaphicArea).paragraph[0].FontSize = {fsize};

        csGraphics::updateGraphicArea(idGaphicArea,0);
    }
}

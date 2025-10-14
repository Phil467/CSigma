
#include "csSection.h"
#include "csUIObjects.h"
#include "csMenu.h"

#include "csStrUtils.h"
#include "csSubClasses.h"
#include "readwfile.h"
#include "csListBoxMin.h"
#include "csTranslator.h"

using namespace std;
using namespace CSUTILS;
using namespace CSSECMAN;
using namespace CSSTRUTILS;


extern int CAPTION_AREA_SIZE;

CSARGS forceEventArgs(0);
void forceEventFunction(CSARGS Args);

int ROOT, MENU_SECTION, MIDDLE_LEFT_SECTION, MIDDLE_TOP_SECTION, MIDDLE_MIDDLE_SECTION, MIDDLE_SECTION, RIGHT_SECTION, BOTTOM_SECTION;
int NR_SECTION, CS_SECTION, BIS_SECTION;
int BTN_NEW_FILE, BTN_NEW_OUTPUT, BTN_REMOVE_FILE, BNT_RANDOM_NUMBER, BTN_ADD_TO_DATA_LIST;
int BTN_REMOVE_SAVE_1, BTN_REMOVE_UNDO_1, BTN_1, BTN_2, BTN_3, BTN_4;
int STATIC_SEARCH, BUTTON_SEARCH;
int MENU_NS, MENU_BIS, MENU_CS, MENU_PREFERENCES, MENU_ABOUT;
int SYSCOMMAND_SECTION, SYS_CLOSE, SYS_MAX, SYS_MIN, TITLE_SECTION, RIGHT_SECTION_CHILD;
int ABOUT_UI, ABOUT_UI_CLIENT, ABOUT_UI_BOTTOM, ABOUT_SUB_MENU, ABOUT_P1, ABOUT_P2, ABOUT_P3, ABOUT_P4, ABOUT_PTITLE, ABOUT_PINDEX;
int GA_CLIENT, PROGRESS_INFO_SECTION, IMAGE_LIST_SECTION;

int smx = GetSystemMetrics(SM_CXSCREEN);
int smy = GetSystemMetrics(SM_CYSCREEN);



    
CSIGMA_MAIN_START(L"fr-fr", L"fr-fr", 1, 1)

{

    // //float dimCoef = GetSystemMetrics(SM_CXSCREEN)*GetSystemMetrics(SM_CYSCREEN)/(1366.0*768);
    float dimCoef = 1.5;
    setSizeCoef(dimCoef);
    // /***************************** Root ********************************** */

    ROOT = createSection(-1, {100,25,600,500},  RGB(30,30,30), {1,1,1,1,1,1,1,1});
    int ICON_ROOT  = setIcon(ROOT, L"icon.ico", L"icon.ico", {2,2,21,21});
    CSSECMAN::enableDarkEdge(ROOT);
    CSSECMAN::setMinMaxInfo(ROOT, MINMAXINFO{.ptMinTrackSize={400,250}});

    CSUIOBJECTS::addTitle(ROOT, L"TEST BIBLIOTHEQUE CSIGMA",{0}, 16, L"Arial Black", ICON_ROOT);
    CSSYSCOMMAND_SECTION SYS_CMD = CSUIOBJECTS::addSysCommand(ROOT, {600});

    /******************************** ToolTips **************************************** */
    CSUIOBJECTS::createToolTips(RGB(40,40,40));

    /*************************************** MIDDLE_SECTION ************************************ */
    CSBIND_GEOM_PARAMS bd;
    MIDDLE_SECTION = createSection(ROOT, {0,CAPTION_AREA_SIZE/dimCoef+6,smx/dimCoef,smy/dimCoef},  RGB(40,40,40), {0,0,0,0});
    /*MIDDLE_SECTION = createSection(ROOT, {0,CAPTION_AREA_SIZE/dimCoef+6,515+77,450+10},  RGB(40,5,5), {0,0,0,0});

    bd = {MIDDLE_SECTION, {-1,-1,1,1}, {BIND_DEST_RIGHT_EDGE,BIND_DEST_BOTTOM_EDGE,BIND_DEST_RIGHT_EDGE,BIND_DEST_BOTTOM_EDGE}};
    bindGeometry(ROOT, bd); // cpp version*/

    /*************************************** MIDDLE_LEFT_SECTION ************************************ */

    CAPTION_AREA_SIZE *= 1.5;
    MIDDLE_LEFT_SECTION = createSection(MIDDLE_SECTION, {0,23,77,440-30},  RGB(20,20,20), {0,0,1,0});

    bd = {MIDDLE_LEFT_SECTION, {0,-1,0,1}, {0,BIND_DEST_BOTTOM_EDGE, 0, BIND_DEST_BOTTOM_EDGE}};
    bindGeometry(ROOT, 1, bd); // C version

    CSSECMAN::setMinMaxInfo(MIDDLE_LEFT_SECTION, MINMAXINFO{.ptMinTrackSize={50}, .ptMaxTrackSize={172}});

    /*************************************** MIDDLE_TOP_SECTION ************************************ */

    MIDDLE_TOP_SECTION = createSection(MIDDLE_SECTION, {1,1,517+77,21},  RGB(30,30,30), {0,0,0,1});
    bd = {MIDDLE_LEFT_SECTION, {0,0,0,1}, {0,0,0,BIND_DEST_TOP_EDGE|BIND_DEST_BOTTOM_EDGE}};
    bindGeometry(MIDDLE_TOP_SECTION, bd); // cpp version
    bd = {MIDDLE_TOP_SECTION, {-1,0,1,0}, {BIND_DEST_RIGHT_EDGE,0,BIND_DEST_RIGHT_EDGE,0}};
    bindGeometry(ROOT, bd); // cpp version
    CSSECMAN::setMinMaxInfo(MIDDLE_TOP_SECTION, MINMAXINFO{.ptMinTrackSize={0,17}, .ptMaxTrackSize={0,100}});

    /***********************************MIDDLE_MIDDLE_SECTION *************************************** */

    MIDDLE_MIDDLE_SECTION = createSection(MIDDLE_SECTION, {78,23,440,395+15},  RGB(25,25,25), {0,0,0,0});

    bd = {MIDDLE_MIDDLE_SECTION, {0,0,0,1}, {0,0, 0, BIND_DEST_TOP_EDGE|BIND_DEST_BOTTOM_EDGE}};
    bindGeometry(MIDDLE_TOP_SECTION, 1, bd);
    bd = {MIDDLE_MIDDLE_SECTION, {0,-1,0,1}, {0,BIND_DEST_BOTTOM_EDGE, 0, BIND_DEST_BOTTOM_EDGE}};
    bindGeometry(ROOT, bd);
    bd = {MIDDLE_MIDDLE_SECTION, {0,0,1,0}, {BIND_DEST_RIGHT_EDGE,0,BIND_DEST_RIGHT_EDGE,0}};
    bindGeometry(MIDDLE_SECTION, bd);
    bd = {MIDDLE_MIDDLE_SECTION, {0,0,1,0}, {0,0,BIND_DEST_RIGHT_EDGE|BIND_DEST_LEFT_EDGE,0}};
    bindGeometry(MIDDLE_LEFT_SECTION, bd);

    /*************************************** RIGHT_SECTION ************************************ */

    RIGHT_SECTION = createSection(MIDDLE_SECTION, {442+77,23,70,395+15},  RGB(30,30,30), {1,0,0,0});

    bd = {MIDDLE_MIDDLE_SECTION, {1,0,0,0}, BIND_DEST_RIGHT_EDGE,0,0,0};
    bindGeometry(RIGHT_SECTION, bd);
    bd = {MIDDLE_TOP_SECTION, {0,0,1,0}, 0,0,BIND_DEST_RIGHT_EDGE,0};
    bindGeometry(ROOT, bd);
    bd = {MIDDLE_MIDDLE_SECTION, {-1,0,1,0}, BIND_DEST_RIGHT_EDGE,0,BIND_DEST_RIGHT_EDGE,0};
    bindGeometry(ROOT, bd);
    bd = {RIGHT_SECTION, {0,0,0,1}, {0,0, 0, BIND_DEST_TOP_EDGE|BIND_DEST_BOTTOM_EDGE}};
    bindGeometry(MIDDLE_TOP_SECTION, 1, bd);
    bd = {RIGHT_SECTION, {-1,-1,1,1}, {BIND_DEST_LEFT_EDGE,BIND_DEST_BOTTOM_EDGE,
                                       BIND_DEST_LEFT_EDGE,BIND_DEST_BOTTOM_EDGE
                                      }
         };
    bindGeometry(ROOT, bd);

    CSSECMAN::setMinMaxInfo(RIGHT_SECTION, MINMAXINFO{.ptMinTrackSize={50}, .ptMaxTrackSize={162}});
    CSSECMAN::updateAfterResizing(RIGHT_SECTION);

    /**************************************************************************************************** */

    NR_SECTION = createSection(MIDDLE_TOP_SECTION, {2,0,148+136/3,19},  RGB(25,25,25), {0,0,0,0});
    bd = {NR_SECTION, {0,0,0,1}, 0,0,0,BIND_DEST_BOTTOM_EDGE};
    bindGeometry(MIDDLE_TOP_SECTION, bd);
    bd = {NR_SECTION, {0,0,0.5,0}, 0,0,BIND_DEST_RIGHT_EDGE,0};
    bindGeometry(ROOT, bd);
    CSSECMAN::updateAfterResizing(NR_SECTION);

    CS_NUMERIC_INCREMENTER_PARAMS nrIds01 = CSUIOBJECTS::numericIncrementerExt(NR_SECTION, {2,2,70,15}, L"10", L"1", INPUT_FORMAT_INTERGER);
    CS_NUMERIC_INCREMENTER_PARAMS nrIds02 = CSUIOBJECTS::numericIncrementerExt(NR_SECTION, {2+73,2,70,15}, L"12", L"1", INPUT_FORMAT_INTERGER);
    CS_NUMERIC_INCREMENTER_PARAMS nrIds03 = CSUIOBJECTS::numericIncrementerExt(NR_SECTION, {2+73*2,2,70,15}, L"1", L"10", INPUT_FORMAT_INTERGER);
    CS_NUMERIC_INCREMENTER_PARAMS nrIds04 = CSUIOBJECTS::numericIncrementerExt(NR_SECTION, {2+73*3,2,70,15}, L"1", L"10", INPUT_FORMAT_INTERGER);

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
    CSSECMAN::updateAfterResizing(BIS_SECTION);
    CS_NUMERIC_INCREMENTER_PARAMS bisIds01 = CSUIOBJECTS::numericIncrementerExt(BIS_SECTION, {2,2,70,15}, L"10", L"1", INPUT_FORMAT_INTERGER);
    CS_NUMERIC_INCREMENTER_PARAMS bisIds02 = CSUIOBJECTS::numericIncrementerExt(BIS_SECTION, {2+73,2,70,15}, L"12", L"1", INPUT_FORMAT_INTERGER);
    CS_NUMERIC_INCREMENTER_PARAMS bisIds03 = CSUIOBJECTS::numericIncrementerExt(BIS_SECTION, {2+73*2,2,70,15}, L"1", L"10", INPUT_FORMAT_INTERGER);
    CS_NUMERIC_INCREMENTER_PARAMS bisIds04 = CSUIOBJECTS::numericIncrementerExt(BIS_SECTION, {2+73*3,2,70,15}, L"1", L"10", INPUT_FORMAT_INTERGER);

    bisIds01.setMinBound("1");
    bisIds02.setMinBound("1");
    bisIds03.setMinBound("1");
    bisIds04.setMinBound("1");

    CS_SECTION = createSection(MIDDLE_TOP_SECTION, {305+136*2/3,0,70+136/3+77,19},  RGB(25,25,25), {1,0,0,0});
    bd = {BIS_SECTION, {1,0,0,0}, {BIND_DEST_RIGHT_EDGE,0,0,0}};
    bindGeometry(CS_SECTION, bd);
    bd = {CS_SECTION, {0,0,0,1}, 0,0,0,BIND_DEST_BOTTOM_EDGE};
    bindGeometry(MIDDLE_TOP_SECTION, bd);
    bd = {CS_SECTION, {-1,0,1,0}, BIND_DEST_RIGHT_EDGE,0,BIND_DEST_LEFT_EDGE,0};
    bindGeometry(ROOT, bd);
    CSSECMAN::updateAfterResizing(CS_SECTION);
    CS_NUMERIC_INCREMENTER_PARAMS csIds01 = CSUIOBJECTS::numericIncrementerExt(CS_SECTION, {2,2,70,15}, L"10", L"1", INPUT_FORMAT_INTERGER);
    CS_NUMERIC_INCREMENTER_PARAMS csIds02 = CSUIOBJECTS::numericIncrementerExt(CS_SECTION, {2+73,2,70,15}, L"12", L"1", INPUT_FORMAT_INTERGER);
    CS_NUMERIC_INCREMENTER_PARAMS csIds03 = CSUIOBJECTS::numericIncrementerExt(CS_SECTION, {2+73*2,2,70,15}, L"1", L"10", INPUT_FORMAT_INTERGER);
    CS_NUMERIC_INCREMENTER_PARAMS csIds04 = CSUIOBJECTS::numericIncrementerExt(CS_SECTION, {2+73*3,2,70,15}, L"1", L"10", INPUT_FORMAT_INTERGER);

    csIds01.setMinBound("1");
    csIds02.setMinBound("1");
    csIds03.setMinBound("1");
    csIds04.setMinBound("1");

    /********************************************************************************** */

    BOTTOM_SECTION = createSection(ROOT, {0,500-45,599,35},  RGB(20,20,20), {0,0,0,0});
    bd = {BOTTOM_SECTION, {-1,-1,1,1}, {BIND_DEST_RIGHT_EDGE,BIND_DEST_TOP_EDGE,BIND_DEST_RIGHT_EDGE,BIND_DEST_TOP_EDGE}};
    bindGeometry(ROOT, bd);
    //setBorderColorAndThick(BOTTOM_SECTION, RGB(20,20,20), 1);

    STATIC_SEARCH = createSection(BOTTOM_SECTION, {(600-200)/2,10,200,16},  RGB(15,15,15), {0,0,0,0});
    bd = {STATIC_SEARCH, {-0.5,0,0.5,0}, {BIND_DEST_LEFT_EDGE,0,BIND_DEST_LEFT_EDGE,0}};
    bindGeometry(ROOT, bd);
    CSUIFX::setBorderColorGradient(STATIC_SEARCH, {100,100,100}, {40,40,40}, 2, 2);
    setBorderThick(STATIC_SEARCH, 2);
    setTitle(STATIC_SEARCH, CSTEXT{.Text=L"Rechercher...\0", .Font=L"Arial", .FontSize=14, .Italic=1, .Color={100,100,100},
                                   .Marging={-15,0}, .Align = CS_TA_CENTER, .Show=1});

    BUTTON_SEARCH = CSUIOBJECTS::iconButton01(STATIC_SEARCH, "resourcessearch02.bmp\0", "resourcessearch01.bmp\0", {200-14-1,1,14,14});

    int redit = csCreateRichEdit(STATIC_SEARCH, {4,4,35,4}, L"Search...", 0, 0);

    /********************************************************************************** */

    CS_NUMERIC_INCREMENTER_PARAMS fontSizeInc = CSUIOBJECTS::numericIncrementerExt1(ROOT, {600-150,0,70,20}, L"16", L"1", INPUT_FORMAT_INTERGER,
                                                            {-1,0,1,0}, {BIND_DEST_LEFT_EDGE,0,BIND_DEST_LEFT_EDGE,0});
    fontSizeInc.setMinBound("8");
    fontSizeInc.setMaxBound("30");
    CSSECMAN::setBorderColorAndThick(fontSizeInc.idSection, RGB(60,60,60), 1);

    void setFontSize(CSARGS Args);
    csSetUpdatingFunction(fontSizeInc.idText, setFontSize, 2, &MIDDLE_MIDDLE_SECTION, &fontSizeInc.idSection);

    CSDYNAMIC_SIMPLE_TEXT tips0;
    tips0.marg = {10,10,10,10};
    tips0.updateGASize = 1;
    tips0.view = 1;
    tips0.paragraph.push_back(CSTEXT{.Text=L"Taille de la police\0", .Font=L"calibri", .FontSize = 14, .Italic=0,
                                   .Bold=FW_BOLD, .Color={150,150,100}});
    tips0.pSpace.push_back(5);

    tips0.paragraph.push_back(CSTEXT{.Text=L"Modifie la taille de la police.\0", .Font=L"calibri", .FontSize = 12, .Italic=1,
                                   .Bold=FW_THIN, .Color={150,150,150}});
    tips0.pSpace.push_back(5);
    CSSECMAN::addTips(fontSizeInc.idSection, {0,0,200/dimCoef, 150/dimCoef}, (POS_BOOL){.bRBottom=1}, 3, 0, tips0);


    /********************************************************************************** */
    int cy = CAPTION_AREA_SIZE/dimCoef-2;
    int cx = 70;

    CSMENU menu(ROOT, {(sRectClient(ROOT).right/dimCoef-cx*6)/2,0,0,cy}, 1);

    menu.newGroup( L"Fichier\0", L"resourcesSettings01.bmp", L"resourcesSettings01.bmp");
    menu.newGroup( L"Edition\0", L"resourcesSettings01.bmp", L"resourcesSettings01.bmp");
    menu.newGroup( L"Données\0", L"resourcesSettings01.bmp", L"resourcesSettings01.bmp");
    menu.newGroup( L"Paramètres\0", L"resourcesSettings01.bmp", L"resourcesSettings01.bmp");
    int aboutMenuId = menu.newGroup( L"A propos\0", L"resourcesAbout01.bmp", L"resourcesAbout01.bmp");
    MENU_ABOUT = menu.getIdButton(aboutMenuId);


    ABOUT_UI = createSection(0, {100,25,600,500},  RGB(40,40,40), {1,1,1,1,1,1,1,1}, 0, 1, 0);
    CSSECMAN::enableDarkEdge(ABOUT_UI);
    CSSECMAN::setIcon(ABOUT_UI, ICON_ROOT);
    CSUIOBJECTS::addTitle(ABOUT_UI, L"About",{60,CAPTION_AREA_SIZE/dimCoef}, 18, L"Arial black", ICON_ROOT);

    ABOUT_UI_CLIENT = createSection(ABOUT_UI, {10/dimCoef,CAPTION_AREA_SIZE/dimCoef,590-10/dimCoef,500-CAPTION_AREA_SIZE/dimCoef-10-20},  RGB(0,0,0), {0}, 1);
    bd = {ABOUT_UI_CLIENT, {-1,-1,1,1}, {BIND_DEST_RIGHT_EDGE, BIND_DEST_BOTTOM_EDGE,
                                        BIND_DEST_RIGHT_EDGE, BIND_DEST_BOTTOM_EDGE}};
    bindGeometry(ABOUT_UI, bd);

    ABOUT_UI_BOTTOM = createSection(ABOUT_UI, {1,(CAPTION_AREA_SIZE+sRectClient(ABOUT_UI_CLIENT).bottom)/dimCoef,590,20},  RGB(40,40,40), {0}, 1);
    bd = {ABOUT_UI_BOTTOM, {-1,-1,1,1}, {BIND_DEST_RIGHT_EDGE, BIND_DEST_TOP_EDGE,
                                        BIND_DEST_RIGHT_EDGE, BIND_DEST_TOP_EDGE}};
    bindGeometry(ABOUT_UI, bd);

    CSSECMAN::setTitle(ABOUT_UI_BOTTOM, CSTEXT{.Text=L"Logo CSigma", .Font=L"calibri", .FontSize = 12, .Italic=0,
                                   .Bold=0, .Color={180,180,180},
                                   .Marging={0,0}, .Align = CS_TA_CENTER, .Show=1});

    int h = (CAPTION_AREA_SIZE-2)/dimCoef, w=30, marg = 1, total = w+marg;
    ABOUT_SUB_MENU = createSection(ABOUT_UI, {(sRectClient(ABOUT_UI).right/dimCoef-total*4)/2,0,total*4,h},  RGB(20,20,20), {0});
    bd = {ABOUT_SUB_MENU, {-0.5,0,0.5,0}, {BIND_DEST_LEFT_EDGE,0,BIND_DEST_LEFT_EDGE,0}};
    bindGeometry(ABOUT_UI, bd);

    ABOUT_P1 = CSUIOBJECTS::darkTextButton01(ABOUT_SUB_MENU, L"1\0", {0,0,w,h});
    ABOUT_P2 = CSUIOBJECTS::darkTextButton01(ABOUT_SUB_MENU, L"2\0", {total*1,0,w,h});
    ABOUT_P3 = CSUIOBJECTS::darkTextButton01(ABOUT_SUB_MENU, L"3\0", {total*2,0,w,h});
    ABOUT_P4 = CSUIOBJECTS::darkTextButton01(ABOUT_SUB_MENU, L"4\0", {total*3,0,w,h});

    CSSYSCOMMAND_SECTION SYS_CMD2 = CSUIOBJECTS::addSysCommand(ABOUT_UI, {600});
    CSSCROLLBAR hscrollAbout = CSUIOBJECTS::addHScrollBar(&ABOUT_UI_CLIENT, &ABOUT_UI_CLIENT, 0, 10);
    CSSCROLLBAR vscrollAbout = CSUIOBJECTS::addVScrollBar(&ABOUT_UI_CLIENT, &ABOUT_UI_CLIENT, 0, 10);
    hscrollAbout.setViewFrameBottomMarging(10);
    vscrollAbout.setViewFrameRightMarging(10);

    int idcv = csGraphics::loadImage(ABOUT_UI_CLIENT, L"resourcescsigma logo.bmp", {1,1}, {0,0});
    SIZE sizecv = csGraphics::getImageSize(ABOUT_UI_CLIENT, idcv);
    csGraphics::showImage(ABOUT_UI_CLIENT, idcv, 1, {0}, sizecv);

    csGraphics::setGraphicAreaPosition(ABOUT_UI_CLIENT,{0,0});
    csGraphics::setGraphicAreaColor(ABOUT_UI_CLIENT,{50,50,50},{0});
    csGraphics::setGraphicAreaSize(ABOUT_UI_CLIENT,sizecv);
    csGraphics::updateGraphicArea(ABOUT_UI_CLIENT, 1);
    csGraphics::setHzoom(ABOUT_UI_CLIENT, 0.1);
    csGraphics::setVzoom(ABOUT_UI_CLIENT, 0.1);
    hscrollAbout.setPositionRatio(0.5);
    void showAboutUi(CSARGS Args);
    CSSECMAN::addAction(MENU_ABOUT, showAboutUi, 1, &ABOUT_UI);

    /*CSLOCKED_MODE lm;
    lm.Lockable = 1;
    lm.Locked = 1;
    lm.ExternalUnlocking = 0;
    lm.LockingColor = {150,150,150};
    lm.LockingTitleColor = {0};
    lm.LockingBorderColor = {130,130,130};

    CSSECMAN::setLockable(MENU_NS, lm);
    lm.Locked = 0;
    CSSECMAN::setLockable(MENU_BIS, lm);
    CSSECMAN::setLockable(MENU_CS, lm);*/


    /********************************************************************************** */
    h = 16, marg = 4, total = h+marg;
    //BTN_NEW_FILE = CSUIOBJECTS::darkTextButton01(MIDDLE_LEFT_SECTION, L"New File\0", {5,5,65,h});
    CS_NUMERIC_INCREMENTER_PARAMS internPrec = CSUIOBJECTS::numericIncrementerExt1(MIDDLE_LEFT_SECTION, {5,5,65,h}, L"1", L"1", INPUT_FORMAT_INTERGER);
    internPrec.setMinBound("1");

    CSDYNAMIC_SIMPLE_TEXT tips;
    tips.marg = {10,10,10,10};
    tips.updateGASize = 1;
    tips.view = 1;
    tips.paragraph.push_back(CSTEXT{.Text=L"Précision des opérations\0", .Font=L"calibri", .FontSize = 14, .Italic=0,
                                   .Bold=FW_BOLD, .Color={150,150,100}});
    tips.pSpace.push_back(5);
    wchar_t* txt = L"Représente la précision des opérations. Plus elle est grande, plus les calculs sont précis, et par conséquent lents.\0";
    tips.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri", .FontSize = 12, .Italic=1,
                                   .Bold=FW_THIN, .Color={150,150,150}});
    tips.pSpace.push_back(5);
    CSSECMAN::addTips(internPrec.idSection, {0,0,250/dimCoef, 200/dimCoef}, (POS_BOOL){.bTRight=1}, 3, 0, tips);

    CS_NUMERIC_INCREMENTER_PARAMS externPrec = CSUIOBJECTS::numericIncrementerExt1(MIDDLE_LEFT_SECTION, {5,5+total*1,65,h}, L"1", L"1", INPUT_FORMAT_INTERGER);
    externPrec.setMinBound("1");
    CSDYNAMIC_SIMPLE_TEXT tips1;
    tips1.marg = {10,10,10,10};
    tips1.updateGASize = 1;
    tips1.view = 1;
    tips1.paragraph.push_back(CSTEXT{.Text=L"Précision des nombres\0", .Font=L"calibri", .FontSize = 14, .Italic=0,
                                   .Bold=FW_BOLD, .Color={150,150,100}});
    tips1.pSpace.push_back(5);

    tips1.paragraph.push_back(CSTEXT{.Text=L"Représente la précision d'affichqge des numbres.\0", .Font=L"calibri", .FontSize = 12, .Italic=1,
                                   .Bold=FW_THIN, .Color={150,150,150}});
    tips1.pSpace.push_back(5);
    CSSECMAN::addTips(externPrec.idSection, {0,0,200/dimCoef, 150/dimCoef}, (POS_BOOL){.bTRight=1}, 3, 0, tips1);


    CS_NUMERIC_INCREMENTER_PARAMS Nnb = CSUIOBJECTS::numericIncrementerExt1(MIDDLE_LEFT_SECTION, {5,5+total*2,65,h}, L"500", L"1", INPUT_FORMAT_INTERGER);
    Nnb.setMinBound("1");
    CSDYNAMIC_SIMPLE_TEXT tips2;
    tips2.marg = {10,10,10,10};
    tips2.updateGASize = 1;
    tips2.view = 1;
    tips2.paragraph.push_back(CSTEXT{.Text=L"Nombre de chiffres\0", .Font=L"calibri", .FontSize = 14, .Italic=0,
                                   .Bold=FW_BOLD, .Color={150,150,100}});
    tips2.pSpace.push_back(5);
    txt = L"Représente le numbre de chiffres du nombre aléatoire à générer.\0";
    tips2.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri", .FontSize = 12, .Italic=1,
                                   .Bold=FW_THIN, .Color={150,150,150}});
    tips2.pSpace.push_back(5);
    CSSECMAN::addTips(Nnb.idSection, {0,0,200/dimCoef, 170/dimCoef}, (POS_BOOL){.bTRight=1}, 3, 0, tips2);

    /********************************************************************************** */
    //BTN_NEW_OUTPUT = CSUIOBJECTS::darkTextButton01(MIDDLE_LEFT_SECTION, L"New Output\0", {5,5+total*1,65,h});
    CS_NUMERIC_INCREMENTER_PARAMS minDigit = CSUIOBJECTS::numericIncrementerExt1(MIDDLE_LEFT_SECTION, {5,5+total*3,65,h}, L"0", L"1", INPUT_FORMAT_INTERGER);
    minDigit.setMinBound("0");
    minDigit.setMaxBound("9");
    CSDYNAMIC_SIMPLE_TEXT tips3;
    tips3.marg = {10,10,10,10};
    tips3.updateGASize = 1;
    tips3.view = 1;
    tips3.paragraph.push_back(CSTEXT{.Text=L"Borne du chiffre\0", .Font=L"calibri", .FontSize = 14, .Italic=0,
                                   .Bold=FW_BOLD, .Color={150,150,100}});
    tips3.pSpace.push_back(5);
    txt = L"Represent l'une ou l'autre des deux bornes des chiffres constituant le nombre aléatoire généré.\0";
    tips3.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri", .FontSize = 12, .Italic=1,
                                   .Bold=FW_THIN, .Color={150,150,150}});
    tips3.pSpace.push_back(5);
    CSSECMAN::addTips(minDigit.idSection, {0,0,200/dimCoef, 170/dimCoef}, (POS_BOOL){.bTRight=1}, 3, 0, tips3);

    /********************************************************************************** */
    //BTN_REMOVE_FILE = CSUIOBJECTS::darkTextButton01(MIDDLE_LEFT_SECTION, L"Remove File\0", {5,5+total*2,65,h});

    CS_NUMERIC_INCREMENTER_PARAMS maxDigit = CSUIOBJECTS::numericIncrementerExt1(MIDDLE_LEFT_SECTION, {5,5+total*4,65,h}, L"9", L"1", INPUT_FORMAT_INTERGER);
    maxDigit.setMinBound("0");
    maxDigit.setMaxBound("9");

    CSSECMAN::addTips(maxDigit.idSection, {0,0,200/dimCoef, 170/dimCoef}, (POS_BOOL){.bTRight=1}, 3, 0, tips3);
    /********************************************************************************** */

    void getRandomData(CSARGS Args);
    BNT_RANDOM_NUMBER = CSUIOBJECTS::darkTextButton01(MIDDLE_LEFT_SECTION, L"Nombre aléatoire\0", {5,5+total*5,65,h});
    CSSECMAN::addAction(BNT_RANDOM_NUMBER, getRandomData, 4, &Nnb.idSection, &minDigit.idSection, &maxDigit.idSection, &MIDDLE_MIDDLE_SECTION);

    CSDYNAMIC_SIMPLE_TEXT tipsMSG;
    tipsMSG.marg = {10,10,10,10};
    tipsMSG.updateGASize = 1;
    tipsMSG.view = 1;
    tipsMSG.paragraph.push_back(CSTEXT{.Text=L"Nombre aléatoire\0", .Font=L"calibri", .FontSize = 14, .Italic=0,
                                   .Bold=FW_BOLD, .Color={150,150,100}});
    tipsMSG.pSpace.push_back(5);
    txt = L"Génère un entier aléatoire.\0";
    tipsMSG.paragraph.push_back(CSTEXT{.Text=txt, .Font=L"calibri", .FontSize = 12, .Italic=1,
                                   .Bold=FW_THIN, .Color={150,150,150}});
    tipsMSG.pSpace.push_back(5);
    CSSECMAN::addTips(BNT_RANDOM_NUMBER, {0,0,200/dimCoef, 150/dimCoef}, (POS_BOOL){.bTRight=1}, 3, 0, tipsMSG);

    /********************************************************************************** */

    BTN_ADD_TO_DATA_LIST = CSUIOBJECTS::darkTextButton01(MIDDLE_LEFT_SECTION, L"Ajouter\0", {5,5+total*6,65,h});
    CSDYNAMIC_SIMPLE_TEXT tipsMSG1;
    tipsMSG1.marg = {10,10,10,10};
    tipsMSG1.updateGASize = 1;
    tipsMSG1.view = 1;
    tipsMSG1.paragraph.push_back(CSTEXT{.Text=L"Ajouter\0", .Font=L"calibri", .FontSize = 14, .Italic=0,
                                   .Bold=FW_BOLD, .Color={150,150,100}});
    tipsMSG1.pSpace.push_back(5);
    tipsMSG1.paragraph.push_back(CSTEXT{.Text=L"Ajouter une donnée à la liste.\0", .Font=L"calibri", .FontSize = 12, .Italic=1,
                                   .Bold=FW_THIN, .Color={150,150,150}});
    tipsMSG1.pSpace.push_back(5);
    CSSECMAN::addTips(BTN_ADD_TO_DATA_LIST, {0,0,200/dimCoef, 150/dimCoef}, (POS_BOOL){.bTRight=1}, 3, 0, tipsMSG1);
    /********************************************************************************** */
    //BTN_REMOVE_SAVE_1 = CSUIOBJECTS::darkTextButton01(MIDDLE_LEFT_SECTION, L"Save 1\0", {5,5+total*6,65,h});
    CS_NUMERIC_INCREMENTER_PARAMS activeNum = CSUIOBJECTS::numericIncrementerExt2(MIDDLE_LEFT_SECTION, {5,5+total*7,65,h}, L"0", L"1", INPUT_FORMAT_INTERGER);
    activeNum.setMinBound("0");
    activeNum.setMaxBound("9");

    CS_STRING_INCREMENTER_PARAMS* strInc = CSUIOBJECTS::stringIncrementer(MIDDLE_LEFT_SECTION, {5,5+total*8,65,h}, L"string 1\0", 1, 1,
                                                            {0,0,1,0}, {0,0,BIND_DEST_RIGHT_EDGE,0});
    strInc->newItem(L"string 2\0");
    strInc->newItem(L"string 3\0");
    strInc->newItem(L"string 4\0");
    strInc->newItem(L"string 5\0");
    strInc->newItem(L"string 6\0");
    strInc->newItem(L"string 7\0");
    strInc->newItem(L"string 8\0");
    strInc->newItem(L"string 9\0");

    //BTN_REMOVE_UNDO_1 = CSUIOBJECTS::darkTextButton01(MIDDLE_LEFT_SECTION, L"Undo 1\0", {5,5+total*9,65,h});

    BTN_1 = CSUIOBJECTS::darkTextButton01(MIDDLE_LEFT_SECTION, L"Show/Hide logo", {5,5+total*10,65,h});
    BTN_2 = CSUIOBJECTS::darkTextButton01(MIDDLE_LEFT_SECTION, L"Play/Stop animation", {5,5+total*11,65,h});
    BTN_3 = CSUIOBJECTS::darkTextButton01(MIDDLE_LEFT_SECTION, L"Bouton 3", {5,5+total*12,65,h});
    BTN_4 = CSUIOBJECTS::darkTextButton01(MIDDLE_LEFT_SECTION, L"Bouton 4", {5,5+total*13,65,h});
    // Boucle principale



    /*********************************************************** */

    CSSCROLLBAR hscroll1 = CSUIOBJECTS::addHScrollBar(&MIDDLE_MIDDLE_SECTION, &MIDDLE_MIDDLE_SECTION, 0, 10);
    CSSCROLLBAR vscroll1 = CSUIOBJECTS::addVScrollBar(&MIDDLE_MIDDLE_SECTION, &MIDDLE_MIDDLE_SECTION, 0, 10);

    hscroll1.setViewFrameBottomMarging(10);
    vscroll1.setViewFrameRightMarging(10);

    csGraphics::setGraphicAreaPosition(MIDDLE_MIDDLE_SECTION,{0,0});
    csGraphics::setGraphicAreaColor(MIDDLE_MIDDLE_SECTION,{10,10,10},{0});
    //csGraphics::setGraphicAreaSize(MIDDLE_MIDDLE_SECTION,{2500, 3500});
    csGraphics::updateGraphicArea(MIDDLE_MIDDLE_SECTION, 1);

    vscroll1.update();

    CSDYNAMIC_SIMPLE_TEXT dst;
    dst.marg = {10,10};
    dst.updateGASize = 1;
    dst.paragraph.push_back(CSTEXT{.Text=L"0\0", .Font=L"calibri light", .FontSize = 12, .Italic=0,
                                   .Bold=FW_THIN, .Color={150,150,150}});
    dst.pSpace.push_back(12);
    dst.view = 0;
    csGraphics::setDynamicSimpleText(MIDDLE_MIDDLE_SECTION, dst);

    GA_CLIENT = MIDDLE_MIDDLE_SECTION;

    int idImg1 = csGraphics::loadImage(GA_CLIENT, L"resourcesCSigma Structure.bmp", {1,1}, {0,0});
    SIZE size1 = csGraphics::getImageSize(GA_CLIENT, idImg1);
    SIZE newSize1 = {size1.cx-200*4, size1.cy-200*4};
    //csGraphics::setImageSize(GA_CLIENT, idImg1, newSize1);
    csGraphics::setImagePos(GA_CLIENT, idImg1, {100, 100});
    csGraphics::showImage(GA_CLIENT, idImg1, 1, {0}, {0});

    POINT logoPos = {100,100};
    SIZE logoSize = {350,300};
    int idlogo1 = csGraphics::loadImage(GA_CLIENT, L"resourcescsigma logo3.bmp", logoPos, logoSize);
    csGraphics::showImage(GA_CLIENT, idlogo1, 1, {0}, {0});

    int fact = std::max(ceil(1.0*size1.cx/smx), ceil(1.0*size1.cy/smy));
    SIZE gaSize = {size1.cx+200, size1.cy+200};
    csGraphics::setGraphicAreaColor(GA_CLIENT, {10,10,10}, {0});
    csGraphics::setGraphicAreaSize(GA_CLIENT, gaSize);
    csGraphics::updateGraphicArea(GA_CLIENT, 1);
    hscroll1.setPositionRatio(0.5);
    vscroll1.setPositionRatio(0.5);

    void showHideLogo(CSARGS Args);
    CSSECMAN::addAction(BTN_1, showHideLogo, 2, &GA_CLIENT, &idlogo1);

    void stretchLogo(CSARGS Args);
    CSSECMAN::addAction(BTN_2, stretchLogo, 4, &GA_CLIENT, &idlogo1, &logoPos, &logoSize);


    // /*********************************************************** */

    //RIGHT_SECTION_CHILD = createSection(RIGHT_SECTION, {5,0,1500/dimCoef,1000/dimCoef},  RGB(30,30,30), {0,0,0,0});

    CSSCROLLBAR hscroll2 = CSUIOBJECTS::addHScrollBar(&RIGHT_SECTION, &RIGHT_SECTION, 0, 10);
    CSSCROLLBAR vscroll2 = CSUIOBJECTS::addVScrollBar(&RIGHT_SECTION, &RIGHT_SECTION, 0, 10);

    hscroll2.setViewFrameBottomMarging(10);
    vscroll2.setViewFrameRightMarging(10);
    csGraphics::setGraphicAreaColor(RIGHT_SECTION, {30,30,30}, {0});
    csGraphics::setMouseWheelPreference(RIGHT_SECTION, CS_MOUSEWHEEL_VSCROLL);

    
    CSUIOBJECTS::CS_ABSTRACT_PROGRESSBAR progressbar(BOTTOM_SECTION, {0,0, 300/dimCoef, (sRectClient(BOTTOM_SECTION).bottom/dimCoef)});

    CSLISTBOXMIN* lbm = csNewMinimalListBoxPtr(&RIGHT_SECTION, 100, 220);
    lbm->setDefaultFont(L"calibri",{12,0});
    lbm->setItemAlign(CS_ALIGN_VERTICAL);
    lbm->setOffset({0,0});
    lbm->setDefaultSize({150*dimCoef, 15*dimCoef});
    //lbm->setIconSize(0,{26,26});

    lbm->setIcon(0, L"img\\langIcon2.bmp",L"img\\langIcon.bmp", L"img\\langIcon.bmp", L"img\\langIcon.bmp");
    lbm->setMaxTextWidth(120*dimCoef);
    lbm->setDefaultTitle(L"Index");

    int langCount = 0;
    const wchar_t** languages = CSLANGMAN::getLanguagesW(&langCount);
    const wchar_t** langCodes = CSLANGMAN::getLanguageCodesW(&langCount);

    for(int i=0; i<langCount; i++)
    {
        lbm->newItem((wchar_t*)(wstring(languages[i])+L" ("+ wstring(langCodes[i])+L")").c_str(),1,0);
    }

    //lbm->newFilePath(L"Bitmap 24-bits\0*.bmp\0");

    lbm->setActiveItem(0);
    lbm->create();

    void checkInstalledLanguages(CSLISTBOXMIN*& lbm);
    checkInstalledLanguages(lbm);

    hscroll2.update();
    hscroll2.setPositionRatio(0);
    vscroll2.setPositionRatio(0);


    void appTranslateStrings(CSARGS Args);
    void startTranslation(CSARGS Args);

    bool*execute = csAlloc<bool>(1,0);
    bool*status = csAlloc<bool>(1,0);
    double*maxWorkLevel = csAlloc<double>(1,0);
    double*workLevel = csAlloc<double>(1,0);
    CSSECMAN::addAction(lbm->getId(), startTranslation, 1, execute);
    CSARGS args(4);
    args.regArg(status, maxWorkLevel, workLevel, lbm);
    progressbar.regFunction(appTranslateStrings, args, execute, L"Translating", L"Translated", L"Translation failed");



    IMAGE_LIST_SECTION = createSection(MIDDLE_LEFT_SECTION, {5,300, 65, 110},  RGB(22,22,22), {0});
    bd = {IMAGE_LIST_SECTION, {0,-1,0,1}, {0,BIND_DEST_BOTTOM_EDGE, 0, BIND_DEST_BOTTOM_EDGE}};
    bindGeometry(ROOT, 1, bd); // C version
    bd = {IMAGE_LIST_SECTION, {0,0,1,0}, {0,0, BIND_DEST_RIGHT_EDGE,0}};
    bindGeometry(MIDDLE_LEFT_SECTION, 1, bd);
    bd = {IMAGE_LIST_SECTION, {0,0,0,-1}, {0,0, 0,BIND_DEST_BOTTOM_EDGE}};
    bindGeometry(MIDDLE_TOP_SECTION, 1, bd);

    CSSCROLLBAR hscroll3 = CSUIOBJECTS::addHScrollBar(&IMAGE_LIST_SECTION, &IMAGE_LIST_SECTION, 0, 10);
    CSSCROLLBAR vscroll3 = CSUIOBJECTS::addVScrollBar(&IMAGE_LIST_SECTION, &IMAGE_LIST_SECTION, 0, 10);

    hscroll3.setViewFrameBottomMarging(10);
    vscroll3.setViewFrameRightMarging(10);
    csGraphics::setMouseWheelPreference(IMAGE_LIST_SECTION, CS_MOUSEWHEEL_VSCROLL);


    CSLISTBOXMIN* lbm1 = csNewMinimalListBoxPtr(&IMAGE_LIST_SECTION, 100, 50);
    lbm1->setDefaultFont(L"calibri",{12,0});
    lbm1->setItemAlign(CS_ALIGN_VERTICAL);
    lbm1->setOffset({0,0});
    lbm1->setMarging({2,0});
    lbm1->setDefaultSize({75*dimCoef, 25*dimCoef});
    //lbm1->setAllTitleColors(RGB(200,200,200), RGB(200,200,200), RGB(0,0,0));
    //lbm1->setDefaultTitleColors(RGB(23,23,23), RGB(40,40,40), RGB(100,100,100));
    lbm1->setIconSize(0,{20,20});

    lbm1->setIcon(0, L"img\\img.bmp",L"img\\img2.bmp", L"img\\img2.bmp", L"img\\img2.bmp");
    lbm1->setMaxTextWidth(120*dimCoef);
    lbm1->setDefaultTitle(L"Image");
    lbm1->newItem(0,100,0);
    //lbm->newFilePath(L"Bitmap 24-bits\0*.bmp\0");

    lbm1->setActiveItem(0);
    lbm1->create();

    hscroll3.update();
    hscroll3.setPositionRatio(0);
    vscroll3.setPositionRatio(0);

}

CSIGMA_MAIN_END


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



void appTranslateStrings(CSARGS Args)
{
    CSLISTBOXMIN* lbm = (CSLISTBOXMIN*)Args[3];
    int i = *lbm->getActiveItem();
    static int lastViewLanguageId = -1;

    COLORREF sectionColor1 = RGB(160,160,220), sectionColor2 = RGB(160,160,230), sectionColor3 = RGB(160,160,220);
    COLORREF dColor1 = RGB(30,40,30), dColor2 = RGB(30,50,30), dColor3 = RGB(160,220,160);
    
    CSLANGMAN::setViewLanguage(i);
    CSLANGMAN::translateAppStrings();

    *(bool*)Args[0] = CSLANGMAN::getTranslationProcessStatus();
    
    if(CSLANGMAN::getTranslationProcessStatus())
    {

        int viewLanguageId = CSLANGMAN::getLanguageCodeId(CSLANGMAN::getViewLanguageCode());
        COLORREF color , highlightColor, selectionColor;

        if(viewLanguageId != lastViewLanguageId)
        {
            if(lastViewLanguageId > -1)
            {
                lbm->setItemBackground(lastViewLanguageId, dColor1, dColor2, dColor3);
            }
            lbm->setItemBackground(viewLanguageId, sectionColor1, sectionColor2, sectionColor3);
            lastViewLanguageId = viewLanguageId;
            InvalidateRect(sHandle(lbm->getId()), 0,1);
        }
    }
}

void startTranslation(CSARGS Args)
{
    if(UINT(Args) == WM_LBUTTONDBLCLK)
    {
        *(bool*)Args[0] = 1;
    }
}


COLORREF sectionColor1 = RGB(160,160,220), sectionColor2 = RGB(160,160,230), sectionColor3 = RGB(160,160,220);
COLORREF dColor1 = RGB(30,40,30), dColor2 = RGB(30,50,30), dColor3 = RGB(160,220,160);

void checkInstalledLanguages(CSLISTBOXMIN*& lbm)
{
    thread t(

        [](CSLISTBOXMIN* lbm)
        {
            int n = 0;
            COLORREF color , highlightColor, selectionColor;
            lbm->getItemBackground(0, &color, &highlightColor, &selectionColor);
            vector<int> idsSave;

            wstring unsupportedLangCodes2 = L"";

            while (1)
            {
                std::vector<std::wstring> installedLanguages = CSLANGMAN::getInstalledLanguages();
                if(installedLanguages.size())
                {
                    std::vector<int> langIds = CSLANGMAN::getLanguagesCodesIds(installedLanguages);

                    if(n < langIds.size())
                    {
                        lbm->setMultipleItemsBackground(langIds, dColor1, dColor2, dColor3);

                    }
                    else if(n > langIds.size())
                    {
                        int m = langIds.size(), idRemoved;
                        for(int i=0; i<idsSave.size(); i++)
                        {
                            int j;
                            for(j=0; j<m; j++)
                            {
                                if(langIds[j] == idsSave[i]) break;
                            }
                            if(j == m)
                            {
                                idRemoved = i;
                                break;
                            }
                        }

                        lbm->setItemBackground(idsSave[idRemoved], color, highlightColor, selectionColor);
                    }


                    n = langIds.size();

                    idsSave.clear();
                    idsSave = langIds;
                    langIds.clear();
                    installedLanguages.clear();

                    wstring unsupportedLangCodes = CSLANGMAN::getUnsupportedLanguages();
                    if(unsupportedLangCodes2 != unsupportedLangCodes)
                    {
                        vector<wstring> v = splitWords(unsupportedLangCodes.c_str(), L" ");
                        std::vector<int> uids = CSLANGMAN::getLanguagesCodesIds(v);
                        lbm->setMultipleItemsBackground(uids, RGB(50,30,30), RGB(55,30,30), RGB(220,170,170));
                        v.clear();
                        uids.clear();


                        unsupportedLangCodes2 = unsupportedLangCodes;
                    }
                }

                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }, lbm

    );
    t.detach();
}

void showHideLogo(CSARGS Args)
{
    int id = (int)Args;
    UINT msg = UINT(Args);

    if(msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK)
    {
        int idImg = *(int*)Args[1];
        int idga = *(int*)Args[0];
        if(csGraphics::isImageVisible(idga, idImg))
        {
            csGraphics::showImage(idga, idImg, 0);
        }
        else
        {
            csGraphics::showImage(idga, idImg, 1);
        }
        //CSSECMAN::updateSection(idga);
        csGraphics::updateGraphicArea(idga,1);
        
    }
}

void stretchLogo(CSARGS Args)
{
    int id = (int)Args;
    UINT msg = UINT(Args);
    static bool start = 0, b=0;
    static int maxCount = 60, count = 0;


    if(msg == WM_LBUTTONUP)
    {
        int idImg = *(int*)Args[1];
        int idga = *(int*)Args[0];
        if(csGraphics::isImageVisible(idga, idImg))
        {
            if(!start) start = 1;
            else start = 0;
        }
        
        thread t(
            [](CSARGS Args)
            {
                int idImg = *(int*)Args[1];
                int idga = *(int*)Args[0];
                while(start)
                {
                    if(csGraphics::isImageVisible(idga, idImg))
                    {
                        if(count < maxCount && !b) count++;
                        else b = 1;

                        if(count > 1 && b) count--;
                        else b = 0;

                        POINT imgPos = *(POINT*)Args[2];
                        SIZE imgSz = *(SIZE*)Args[3];
                        
                        int tlen = 2*count;
                        csGraphics::setImageSize(idga, idImg, {imgSz.cx+tlen, imgSz.cy+tlen});
                        csGraphics::setImagePos(idga, idImg, {imgPos.x-tlen/2, imgPos.y-tlen/2});
                        csGraphics::updateGraphicArea(idga,1);
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            }, Args
        );
        t.detach();
    }
    /*else if(msg == WM_TIMER && start)
    {
        int idga = *(int*)Args[0];
        int idImg = *(int*)Args[1];
        if(csGraphics::isImageVisible(idga, idImg))
        {
            if(count < maxCount && !b) count++;
            else b = 1;

            if(count > 1 && b) count--;
            else b = 0;

            POINT imgPos = *(POINT*)Args[2];
            SIZE imgSz = *(SIZE*)Args[3];
            
            //if(count%2 == 0)
            {
                int tlen = 2*count;
                csGraphics::setImageSize(idga, idImg, {imgSz.cx+tlen, imgSz.cy+tlen});
                csGraphics::setImagePos(idga, idImg, {imgPos.x-tlen/2, imgPos.y-tlen/2});
                //CSSECMAN::updateSection(idga);
                csGraphics::updateGraphicArea(idga,1);
            }
        }
    }*/
}

/*================================================================================
### File : mainExample.cpp
### Description : Example of use of the CSigma library
### Author : Phil Az
### Date : 14/10/2025
### Version : 1.0.0
### License : MIT
### Copyright : CSigma
### Contact : azphil@outlook.fr, csigmadev@gmail.com
### Link : https://github.com/Phil467/CSigma
================================================================================

This file shows how to create a simple window with the CSigma library.

*/

#include "csSection.h"
#include "csUIObjects.h"

using namespace std;

CSIGMA_MAIN_BEGIN(L"en", L"en", 0, 0)

float geomCoef = 1.5;
CSSECMAN::setGeometryCoef(geomCoef);

int ROOT = CSSECMAN::createSection( -1,
                                    {100,25,600,500},
                                    RGB(130,130,130),
                                    {1,1,1,1,1,1,1,1}   );

int ROOT_ICON  = CSSECMAN::setIcon( ROOT, 
                                    L"icon.ico", 
                                    L"icon.ico", 
                                    {2,2,21,21} );

int TITLE_SECTION = CSUIOBJECTS::addTitle(  ROOT, 
                                    L"MY APPLICATION",
                                    {0}, 
                                    18, L"Arial Black", 
                                    ROOT_ICON   );

CSSYSCOMMAND_SECTION SYS_CMD = CSUIOBJECTS::addSysCommand(ROOT, {600});
CSSECMAN::setMinMaxInfo(ROOT, MINMAXINFO{.ptMinTrackSize={400,250}});
CSSECMAN::enableDarkEdge(ROOT);

CSIGMA_MAIN_END()

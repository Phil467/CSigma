/*================================================================================
### File : mainExample.cpp
### Description : Example of use of the CSigma library
### Author : Phil Az (Philippe Levang Azeufack)
### Date : 14/10/2025
### Version : 1.0.0
### License : MIT
### Copyright : CSigma
### Contact : azphil@outlook.fr, csigmadev@gmail.com
### Link : https://github.com/Phil467/CSigma
================================================================================

This file shows how to create a simple window with the CSigma library.

You can change the language of the window by changing the language of the main function.
You can change the size of the elements by changing the sizeCoef variable.
You can change the icon of the window by changing the L"icon.ico" string.
You can change the title of the window by changing the L"MY APPLICATION" string.
You can change the font of the window by changing the L"Arial Black" string.
You can change the size of the font by changing the 18 value.
You can change the color of the window by changing the RGB(30,30,30) value.

You can change the size of the window by changing the {100,25,600,500} value.
You can change the size of the icon by changing the {2,2,21,21} value.
You can change the size of the command by changing the {600} value.
You can change the size of the minTrackSize by changing the {400,250} value.
You can change the size of the maxTrackSize by changing the {172} value.

*/

#include "csSection.h"
#include "csUIObjects.h"

using namespace std;

CSIGMA_MAIN_START(L"fr-fr", L"fr-fr", 0, 0)

float sizeCoef = 1.5;
CSSECMAN::setSizeCoef(sizeCoef);

int ROOT = CSSECMAN::createSection( -1,
                                    {100,25,600,500},
                                    RGB(30,30,30),
                                    {1,1,1,1,1,1,1,1}   );

int ROOT_ICON  = CSSECMAN::setIcon( ROOT, 
                                    L"icon.ico", 
                                    L"icon.ico", 
                                    {2,2,21,21} );

CSUIOBJECTS::addTitle(  ROOT, 
                        L"MY APPLICATION",
                        {0}, 
                        18, L"Arial Black", 
                        ROOT_ICON   );

CSSYSCOMMAND_SECTION SYS_CMD = CSUIOBJECTS::addSysCommand(ROOT, {600});
CSSECMAN::setMinMaxInfo(ROOT, MINMAXINFO{.ptMinTrackSize={400,250}});
CSSECMAN::enableDarkEdge(ROOT);

CSIGMA_MAIN_END

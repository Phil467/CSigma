#ifndef CSSECTION_H
#define CSSECTION_H


#include <dwmapi.h>
#include "csUtils.h"
#include "csUIFx.h"
#include "csFilesMan.h"
#include <thread>
#include <functional>

#define CS_NO_SAVE_APP_STRINGS 0
#define CS_SAVE_APP_STRINGS 1
#define CS_NO_SAVE_APP_GEOMETRY 0
#define CS_SAVE_APP_GEOMETRY 1

#define CSIGMA_APP_RUN() CSSECMAN::_CSIGMA_APP_RUN_()

#define CSIGMA_MAIN(sourceLang, targetLang, saveAppStrings, saveAppGeometry) \
    static int csigma_main();\
    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) \
    { \
        CSSECMAN::_CSIGMA_APP_INIT_(hInstance, sourceLang, targetLang, saveAppStrings, saveAppGeometry); \
        return csigma_main(); \
    } \
    static int csigma_main()


#define CSIGMA_MAIN_BEGIN(sourceLang, targetLang, saveAppStrings, saveAppGeometry) \
    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) \
    { \
        CSSECMAN::_CSIGMA_APP_INIT_(hInstance, sourceLang, targetLang, saveAppStrings, saveAppGeometry);

#define CSIGMA_MAIN_END() \
        return CSSECMAN::_CSIGMA_APP_RUN_(); \
    }


using namespace std;
int getId(HWND hwnd);
LRESULT CALLBACK sectionProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Template variadique pour exécuter une fonction si msg correspond à l'un des messages
// Utilise fold expressions (C++17) pour vérifier si msg correspond à l'un des msgs
template<typename Func, typename... Msgs>
void CS_ACT(Func&& func, UINT msg, Msgs... msgs)
{
    if (((msg == msgs) || ...))
    {
        func();
    }
}

// Exemple d'utilisation :
// CS_ACT([&](){ /* code avec capture */ }, msg, WM_LBUTTONDBLCLK, WM_LBUTTONDOWN);
// CS_ACT([](){ /* code sans paramètres */ }, msg, WM_LBUTTONDBLCLK, WM_LBUTTONDOWN);
// void myFunc() { /* ... */ }
// CS_ACT(myFunc, msg, WM_LBUTTONDBLCLK, WM_LBUTTONDOWN);
#endif // CSSECTION_H

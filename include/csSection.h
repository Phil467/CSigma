#ifndef CSSECTION_H
#define CSSECTION_H


#include <dwmapi.h>
#include "csUtils.h"
#include "csUIFx.h"
#include "csFilesMan.h"
#include <thread>
#include <functional>

#define CSIGMA_APP_RUN() CSUIMAN::_CSIGMA_APP_RUN_()

#define CSIGMA_MAIN(sourceLang, targetLang, saveAppStrings, saveAppGeometry) \
    static int csigma_main();\
    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) \
    { \
        CSUIMAN::_CSIGMA_APP_INIT_(hInstance, sourceLang, targetLang, saveAppStrings, saveAppGeometry); \
        return csigma_main(); \
    } \
    static int csigma_main()


#define CSIGMA_MAIN_START(sourceLang, targetLang, saveAppStrings, saveAppGeometry) \
    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) \
    { \
        CSUIMAN::_CSIGMA_APP_INIT_(hInstance, sourceLang, targetLang, 1, 1);

#define CSIGMA_MAIN_END \
        return CSUIMAN::_CSIGMA_APP_RUN_(); \
    }


using namespace std;
int getId(HWND hwnd);
LRESULT CALLBACK sectionProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif // CSSECTION_H

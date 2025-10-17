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

#endif // CSSECTION_H

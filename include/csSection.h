#ifndef CSSECTION_H
#define CSSECTION_H

#include <dwmapi.h>
#include "csUtils.h"
#include "csUIFx.h"
#include "csFilesMan.h"
#include <thread>
#include <functional>

using namespace std;

int getId(HWND hwnd);
LRESULT CALLBACK sectionProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif // CSSECTION_H

#ifndef CSSUBCLASSES_H
#define CSSUBCLASSES_H

#include <windows.h>
#include <richedit.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <uxtheme.h>
#include "csStrUtils.h"
#include "csArgs.h"
#include "csUtils.h"
#include "readwfile.h"


HFONT getDefaultFont();

LRESULT CALLBACK ListSubclassProc(HWND hList, UINT msg, WPARAM wParam, LPARAM lParam);
int csCreateListBox(int idpar, RECT r, int style=0, int hMenuId=0);
HWND listBoxHandle(int idl);
void csRegListBoxActionArgs_(int idl, void**& Args, int& n);
template<typename ..._Args> void csRegListBoxActionArgs_(int idl, void**& Args, int& n, void* arg, _Args... args)
{
    n += 1;
    Args = (void**)realloc(Args, n*sizeof(void*));
    Args[n-1] = arg;
    csRegListBoxActionArgs_(idl, Args, n,  args...);
};

void csAddListBoxAction(int idl, void(*function)(CSARGS), CSARGS args);
template<typename ..._Args> void csAddListBoxAction(int idl, void(*function)(CSARGS), void* arg,_Args... args)
{
    void** Args = 0;
    int n = 0;
    csRegListBoxActionArgs_(idl, Args, n, arg, args...);
    CSARGS _args(n);
    _args.regArg(Args,n);
    csAddListBoxAction(idl, function, _args);
};



LRESULT CALLBACK EditSubclassProc(HWND hEdit, UINT msg, WPARAM wParam, LPARAM lParam);
int csCreateEditBox(int idpar, RECT r, int style=0, int hMenuId=0);
HWND editBoxHandle(int ide);
void csRegEditBoxActionArgs_(int ide, void**& Args, int& n);
template<typename ..._Args> void csRegEditBoxActionArgs_(int ide, void**& Args, int& n, void* arg, _Args... args)
{
    n += 1;
    Args = (void**)realloc(Args, n*sizeof(void*));
    Args[n-1] = arg;
    csRegEditBoxActionArgs_(ide, Args, n,  args...);
};

void csAddEditBoxAction(int ide, void(*function)(CSARGS), CSARGS args);
template<typename ..._Args> void csAddEditBoxAction(int ide, void(*function)(CSARGS), void* arg,_Args... args)
{
    void** Args = 0;
    int n = 0;
    csRegEditBoxActionArgs_(ide, Args, n, arg, args...);
    CSARGS _args(n);
    _args.regArg(Args,n);
    csAddEditBoxAction(ide, function, _args);
};
void getEditLineParams(HWND hEdit, int* cHeight, int*cWidth, int*xOffset, int*yOffset, HFONT* font);



LRESULT CALLBACK richEditSubclassProc(HWND hEdit, UINT msg, WPARAM wParam, LPARAM lParam);
int csCreateRichEdit(int idpar, RECT r, const wchar_t* defaultText, int style=0, int hMenuId=0);
HWND richEditHandle(int ide);
void csSetRichEditFormat(int id, short format);
void csSetUpdatingFunction(int id, void(*f)(CSARGS), CSARGS& Args);
void csSetUpdatingFunction(int id, void(*f)(CSARGS), int nArgs, ...);
void csRegRichEditActionArgs_(int ide, void**& Args, int& n);
template<typename ..._Args> void csRegRichEditActionArgs_(int ide, void**& Args, int& n, void* arg, _Args... args)
{
    n += 1;
    Args = (void**)realloc(Args, n*sizeof(void*));
    Args[n-1] = arg;
    csRegRichEditActionArgs_(ide, Args, n,  args...);
};

void csAddRichEditAction(int ide, void(*function)(CSARGS), CSARGS args);
template<typename ..._Args> void csAddRichEditAction(int ide, void(*function)(CSARGS), void* arg,_Args... args)
{
    void** Args = 0;
    int n = 0;
    csRegRichEditActionArgs_(ide, Args, n, arg, args...);
    CSARGS _args(n);
    _args.regArg(Args,n);
    csAddRichEditAction(ide, function, _args);
};

#endif // CSSUBCLASSES_H

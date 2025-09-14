#include "csSubClasses.h"
#include "csGraphics.h"
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "uxtheme.lib")

#pragma comment(linker,"\"/manifestdependency:type='win32' \
 name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
 processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

extern vector<HWND> SECTION;
extern vector<RECT> RECTCL;
extern HINSTANCE hReditLib;
extern vector<CSTEXT> TITLE;
extern bool CLICK_EFFECT_BOOL;

HFONT font = CreateFontW(
                 -14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                 DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI Emoji" //Segoe UI Emoji
                 );

int getId(HWND hwnd, vector<HWND> v)
{
    int n = v.size();
    for(int i=0; i<n; i++)
    {
        if(hwnd == v[i])
        {
            return i;
        }
    }
    return -1;
}

HFONT getDefaultFont()
{
    return font;
}

/**************************************************EditBox*************************************************************** */

vector<HWND> editBoxes;
vector<vector<void(*)(CSARGS)>> EDITBOX_GROUPED_EVENTS_FUNC;
vector<vector<CSARGS>> EDITBOX_GROUPED_EVENTS_ARGS;
vector<bool> HIDEEDITGROUPMSG;

WNDPROC OldEditProc;

int csCreateEditBox(int idpar, RECT r, int style, int hMenuId)
{
    int s = (style == 0) ? WS_CHILD | WS_VISIBLE | WS_BORDER : style;
    HWND hEditBox = CreateWindowExW(0, L"EDIT", nullptr,
                                    s,r.left, r.top, r.right, r.bottom, SECTION[idpar], (HMENU)hMenuId, nullptr, nullptr);
    SendMessage(hEditBox, WM_SETFONT, (WPARAM)font, TRUE);
    OldEditProc = (WNDPROC)SetWindowLongPtr(hEditBox, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
    editBoxes.push_back(hEditBox);
    vector<void(*)(CSARGS)> v;
    EDITBOX_GROUPED_EVENTS_FUNC.push_back(v);

    vector<CSARGS> arg;
    EDITBOX_GROUPED_EVENTS_ARGS.push_back(arg);

    HIDEEDITGROUPMSG.push_back(0);
    return editBoxes.size()-1;
}

LRESULT CALLBACK EditSubclassProc(HWND hEdit, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static bool b = 0;
    static int id;
    id = getId(hEdit, editBoxes);

    if(id >= 0)
    {
        int n = EDITBOX_GROUPED_EVENTS_ARGS[id].size();
        if(HIDEEDITGROUPMSG[id] == 0)
            for(int i=0; i<n; i++)
            {
                EDITBOX_GROUPED_EVENTS_ARGS[id][i].setProcParams(&hEdit,&msg,&wParam,&lParam,&id);
                (EDITBOX_GROUPED_EVENTS_FUNC[id][i])(EDITBOX_GROUPED_EVENTS_ARGS[id][i]);
                LRESULT lr = *(LRESULT*)EDITBOX_GROUPED_EVENTS_ARGS[id][i].getLRESULT();
                if(lr)
                {
                    *(LRESULT*)EDITBOX_GROUPED_EVENTS_ARGS[id][i].getLRESULT() = 0;
                    return lr;
                }
            }
    }

    return CallWindowProc(OldEditProc, hEdit, msg, wParam, lParam);
}

HWND editBoxHandle(int id)
{
    if(id < 0) return 0;
    else return editBoxes[id];
}

void csRegEditBoxActionArgs_(int id, void**& Args, int& n)
{

}

void csAddEditBoxAction(int idl, void(*function)(CSARGS), CSARGS args)
{
    EDITBOX_GROUPED_EVENTS_FUNC[idl].push_back(function);
    EDITBOX_GROUPED_EVENTS_ARGS[idl].push_back(args);
}

// Obtenir la hauteur d'une ligne en pixels
void getEditLineParams(HWND hEdit, int* cHeight, int*cWidth, int*xOffset, int*yOffset, HFONT* font)
{
    // Créer un DC pour le contrôle Edit
    HDC hDC = GetDC(hEdit);

    // Récupérer la police utilisée par le contrôle
    HFONT hFont = (HFONT)SendMessage(hEdit, WM_GETFONT, 0, 0);
    HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

    // Obtenir les métriques du texte
    TEXTMETRIC tm;
    GetTextMetrics(hDC, &tm);

    // Nettoyer
    SelectObject(hDC, hOldFont);
    ReleaseDC(hEdit, hDC);

    // Hauteur de ligne = hauteur de la police + espacement entre lignes
    *cHeight = tm.tmAveCharWidth;
    *cWidth = tm.tmAveCharWidth;
    *xOffset = tm.tmInternalLeading;
    *yOffset = tm.tmExternalLeading;
    *font = hFont;
}

/**************************************************RichEdit*************************************************************** */

typedef struct
{
    COLORREF bkg;
    COLORREF txt;
    HBRUSH bkgBrush;
}RICHEDITCOLORS;



vector<HWND> richEdits;
vector<int> richEditParId;
vector<bool> hookRichEditSignal;
vector<RICHEDITCOLORS> recol;
vector<vector<void(*)(CSARGS)>> RICHEDIT_GROUPED_EVENTS_FUNC;
vector<vector<CSARGS>> RICHEDIT_GROUPED_EVENTS_ARGS;
vector<bool> HIDERICHEDITGROUPMSG;
vector<RECT> richEditMarging;
vector<wchar_t*> richEditDefaultText;
vector<short> richEditTextFormat;
vector<CS_RICHEDIT_NUMERIC_BOUNDS> richEditNumericBounds;
vector<void(*)(CSARGS)> richEditUpdatingFunction;
vector<CSARGS*> richEditUpdatingFunctionArgs;

WNDPROC OldRichEditProc;

void _showRichEditAction(CSARGS Args);

int csCreateRichEdit(int idpar, RECT marging, const wchar_t* defaultText, int style, int hMenuId)
{

    //int s = (style == 0) ? ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL : style;
    int s = (style == 0) ? WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL : style;
    HWND hRichEdit;
    hReditLib = LoadLibraryW(L"msftedit.dll");
    if(hReditLib)
    {
        hRichEdit = CreateWindowExW(0,L"RICHEDIT50W", nullptr,
                                    s,marging.left, marging.top, marging.right, marging.bottom, SECTION[idpar], (HMENU)hMenuId, GetModuleHandle(NULL), nullptr);
    }
    else
    {
        hRichEdit = CreateWindowExW(0,L"EDIT", nullptr,
                                    s,marging.left, marging.top, marging.right, marging.bottom, SECTION[idpar], (HMENU)hMenuId, nullptr, nullptr);
    }

    SetWindowTheme(hRichEdit, L"", L""); 

    SendMessageW(hRichEdit, WM_SETFONT, (WPARAM)font, TRUE);
    OldRichEditProc = (WNDPROC)SetWindowLongPtr(hRichEdit, GWLP_WNDPROC, (LONG_PTR)richEditSubclassProc);
    richEdits.push_back(hRichEdit);
    hookRichEditSignal.push_back(0);
    richEditTextFormat.push_back(0);
    richEditParId.push_back(idpar);
    recol.push_back({RGB(20,20,20), RGB(200,200,200), CreateSolidBrush(RGB(20,20,20))});
    richEditMarging.push_back(marging);
    richEditNumericBounds.push_back({0,0});
    richEditUpdatingFunction.push_back(0);
    richEditUpdatingFunctionArgs.push_back(0);

    if(defaultText)
    {
        wchar_t* text = csAlloc<wchar_t>((wcslen(defaultText)+1)*sizeof(wchar_t));
        wsprintf(text, L"%s\0", defaultText);
        richEditDefaultText.push_back(text);

        TITLE[idpar].Text = csAlloc<wchar_t>((wcslen(defaultText)+1)*sizeof(wchar_t));
        wsprintf(TITLE[idpar].Text, L"%s\0", defaultText);
    }
    else
    {
        wchar_t* text = csAlloc<wchar_t>(2*sizeof(wchar_t));
        wsprintf(text, L"\0");
        richEditDefaultText.push_back(text);
        
        TITLE[idpar].Text = csAlloc<wchar_t>(2*sizeof(wchar_t));
        wsprintf(TITLE[idpar].Text, L"\0");
    }

    vector<void(*)(CSARGS)> v;
    RICHEDIT_GROUPED_EVENTS_FUNC.push_back(v);

    vector<CSARGS> arg;
    RICHEDIT_GROUPED_EVENTS_ARGS.push_back(arg);
    HIDERICHEDITGROUPMSG.push_back(0);

    ShowWindow(hRichEdit,0);
    CSUIMAN::addAction(idpar, _showRichEditAction, 1, csAlloc<int>(1, richEdits.size()-1));

    return richEdits.size()-1;
}


LRESULT CALLBACK richEditSubclassProc(HWND hEdit, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static bool b = 0;
    static int id;
    id = getId(hEdit, richEdits);

    if(id >= 0)
    {
        if(msg==WM_MOUSEMOVE)
        {
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_HOVER | TME_LEAVE;
            tme.hwndTrack = hEdit;
            tme.dwHoverTime = 5;
            TrackMouseEvent(&tme);
        }

        else if(msg==WM_MOUSEHOVER)
        {
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hEdit;
            TrackMouseEvent(&tme);

        }

        else if(msg==WM_MOUSELEAVE)
        {
            wchar_t str[5000];
            GetWindowTextW(hEdit, str, 5000);

            if(wcslen(str))
            {
                bool b = 1;
                if(richEditTextFormat[id] == INPUT_FORMAT_INTERGER || richEditTextFormat[id] == INPUT_FORMAT_POSITIVE_INTERGER
                   ||richEditTextFormat[id] == INPUT_FORMAT_NEGATIVE_INTERGER)
                {
                    /*char*chr = (char*)wcharPtrToCharPtr((const wchar_t*)str).c_str();
                    long long num = strtoll(chr,&chr + strlen(chr),0);*/
                    int n = wcslen(str);
                    if(str[0] != '-' && str[0] != '+' && str[0] != '1' && str[0] != '2' && str[0] != '3' && 
                        str[0] != '4' && str[0] != '5' && str[0] != '6' && str[0] != '7' && str[0] != '8' && 
                        str[0] != '9' && str[0] != '0')
                        b = 0;
                    for(int i=1; i<n; i++)
                    {
                        if(str[i] != '1' && str[i] != '2' && str[i] != '3' && 
                        str[i] != '4' && str[i] != '5' && str[i] != '6' && str[i] != '7' && str[i] != '8' && 
                        str[i] != '9' && str[i] != '0')
                        {
                            b = 0;
                            break;
                        }
                    }

                    if(richEditTextFormat[id] == INPUT_FORMAT_POSITIVE_INTERGER && str[0] == '-') b = 0;
                    if(richEditTextFormat[id] == INPUT_FORMAT_NEGATIVE_INTERGER && str[0] != '-') b = 0;

                    if(b && richEditNumericBounds[id].minBound)
                    {
                        CSARITHMETIC::csRNUMBER rn(0, wcharPtrToCharPtr(str).c_str());
                        CSARITHMETIC::csRNUMBER rnMin(0, richEditNumericBounds[id].minBound);

                        if(rn < rnMin)
                            b = 0;
                        
                        rn.clear();
                        rnMin.clear();
                    }

                    if(b && richEditNumericBounds[id].maxBound)
                    {
                        CSARITHMETIC::csRNUMBER rn(0, wcharPtrToCharPtr(str).c_str());
                        CSARITHMETIC::csRNUMBER rnMax(0, richEditNumericBounds[id].maxBound);

                        if(rn > rnMax)
                            b = 0;
                        
                        rn.clear();
                        rnMax.clear();
                    }
                    
                }
                else if(richEditTextFormat[id] == INPUT_FORMAT_FLOAT||richEditTextFormat[id] == INPUT_FORMAT_POSITIVE_FLOAT||
                        richEditTextFormat[id] == INPUT_FORMAT_NEGATIVE_FLOAT)
                {
                    int n = wcslen(str);
                    bool bb = 0;
                    if(str[0] != '-' && str[0] != '+' && str[0] != '1' && str[0] != '2' && str[0] != '3' && 
                        str[0] != '4' && str[0] != '5' && str[0] != '6' && str[0] != '7' && str[0] != '8' && 
                        str[0] != '9' && str[0] != '0')
                        b = 0;
                    for(int i=1; i<n; i++)
                    {
                        if(str[i] != '1' && str[i] != '2' && str[i] != '3' && 
                        str[i] != '4' && str[i] != '5' && str[i] != '6' && str[i] != '7' && str[i] != '8' && 
                        str[i] != '9' && str[i] != '0')
                        {
                            if(bb == 0)
                            {
                                if(str[i] != '.')
                                {
                                    b = 0;
                                    break;
                                }
                                else
                                    bb = 1;
                            }
                            else
                            {
                                b = 0;
                                break;
                            }
                        }
                    }

                    if(richEditTextFormat[id] == INPUT_FORMAT_POSITIVE_FLOAT && str[0] == '-') b = 0;
                    if(richEditTextFormat[id] == INPUT_FORMAT_NEGATIVE_FLOAT && str[0] != '-') b = 0;

                    if(b && richEditNumericBounds[id].minBound)
                    {
                        CSARITHMETIC::csRNUMBER rn(0, wcharPtrToCharPtr(str).c_str());
                        CSARITHMETIC::csRNUMBER rnMin(0, richEditNumericBounds[id].minBound);

                        if(rn < rnMin)
                            b = 0;
                        
                        rn.clear();
                        rnMin.clear();
                    }

                    if(b && richEditNumericBounds[id].maxBound)
                    {
                        CSARITHMETIC::csRNUMBER rn(0, wcharPtrToCharPtr(str).c_str());
                        CSARITHMETIC::csRNUMBER rnMax(0, richEditNumericBounds[id].maxBound);

                        if(rn > rnMax)
                            b = 0;
                        
                        rn.clear();
                        rnMax.clear();
                    }
                }

                if(b)
                {
                    free(TITLE[richEditParId[id]].Text);
                    int len = wcslen(str)+1;
                    TITLE[richEditParId[id]].Text = csAlloc<wchar_t>(len);
                    wcscpy_s(TITLE[richEditParId[id]].Text, len, str);
                    
                    if(richEditUpdatingFunction[id])
                        richEditUpdatingFunction[id](*(richEditUpdatingFunctionArgs[id]));
                }
                else
                {
                    SetWindowTextW(richEdits[id], TITLE[richEditParId[id]].Text);
                }
                
            }
            ShowWindow(hEdit, 0);
            SetForegroundWindow(SECTION[0]);
            
            //SendMessage(GetParent(SECTION[richEditParId[id]]), WM_LBUTTONDOWN, MAKELPARAM(0,0),0);
            //SendMessage(SECTION[richEditParId[id]], WM_MOUSEHOVER, 0,0);
            //SendMessage(SECTION[richEditParId[id]], WM_MOUSELEAVE, 0,0);
            InvalidateRect(SECTION[richEditParId[id]], 0,1);
            CLICK_EFFECT_BOOL = 0;
        }
        /*else if(msg == WM_CHAR)
        {
            if(richEditTextFormat[id] == INPUT_FORMAT_INTERGER)
            wchar_t str[5000];
            GetWindowTextW(hEdit, str, 5000);
            cout<<WPARAM(Args)<<" === \n";
        }*/

        int n = RICHEDIT_GROUPED_EVENTS_ARGS[id].size();
        if(HIDERICHEDITGROUPMSG[id] == 0)
            for(int i=0; i<n; i++)
            {
                RICHEDIT_GROUPED_EVENTS_ARGS[id][i].setProcParams(&hEdit,&msg,&wParam,&lParam,&id);
                (RICHEDIT_GROUPED_EVENTS_FUNC[id][i])(RICHEDIT_GROUPED_EVENTS_ARGS[id][i]);
                LRESULT lr = *(LRESULT*)RICHEDIT_GROUPED_EVENTS_ARGS[id][i].getLRESULT();
                if(lr)
                {
                    *(LRESULT*)RICHEDIT_GROUPED_EVENTS_ARGS[id][i].getLRESULT() = 0;
                    return lr;
                }
            }

    }

    return CallWindowProc(OldRichEditProc, hEdit, msg, wParam, lParam);
}

void _showRichEditAction(CSARGS Args)
{
    UINT msg = UINT(Args);
    int idEdit = *(int*)Args[0];
    if(msg == WM_MOUSELEAVE)
    {
        POINT p;
        GetCursorPos(&p);
        RECT r;
        GetWindowRect(richEdits[idEdit], &r);
        if(!PtInRect(&r, p))
            ShowWindow(richEdits[idEdit], 0);

    }
    
    if(msg == WM_CTLCOLOREDIT)
    {
        cout<<"kjkjkl\n";
        LRESULT* lr = (LRESULT*)Args.getLRESULT();
        HDC hdcStatic = (HDC)(WPARAM)Args;
        SetTextColor(hdcStatic, recol[idEdit].txt);      
        SetBkColor(hdcStatic, recol[idEdit].bkg);            
        *lr = (LRESULT)(recol[idEdit].bkgBrush);
    }

    if(msg == WM_LBUTTONDOWN)
    {
        RECT r = RECTCL[(int)Args];
        RECT marg = richEditMarging[idEdit];
        MoveWindow(richEdits[idEdit], marg.left, marg.top, r.right-marg.right-marg.left, r.bottom-marg.bottom-marg.top, 0);
        ShowWindow(richEdits[idEdit], 1);
        SetFocus(richEdits[idEdit]);
    }

}

void setRichEditColors(int idEdit, COLORREF textColor, COLORREF bkgColor)
{
    recol[idEdit] = {bkgColor, textColor};
    recol[idEdit].bkgBrush = CreateSolidBrush(bkgColor);
}

HWND richEditHandle(int id)
{
    if(id < 0) return 0;
    else return richEdits[id];
}

void csRegRichEditActionArgs_(int id, void**& Args, int& n)
{

}

void csAddRichEditAction(int idl, void(*function)(CSARGS), CSARGS args)
{
    RICHEDIT_GROUPED_EVENTS_FUNC[idl].push_back(function);
    RICHEDIT_GROUPED_EVENTS_ARGS[idl].push_back(args);
}

void csSetRichEditFormat(int id, short format)
{
    richEditTextFormat[id] = format;
}

void csSetUpdatingFunction(int id, void(*f)(CSARGS), CSARGS& Args)
{
    richEditUpdatingFunction[id] = f;
    richEditUpdatingFunctionArgs[id] = &Args;
}

void csSetUpdatingFunction(int id, void(*f)(CSARGS), int nArgs, ...)
{
    va_list arg;
    va_start(arg, nArgs);

    void* l[nArgs];

    for(int i=0; i<nArgs; i++)
    {
        l[i] = (va_arg(arg, void*));
    }

    va_end(arg);

    CSARGS Args(nArgs);
    Args.regArg(l, nArgs);
    richEditUpdatingFunction[id] = f;
    richEditUpdatingFunctionArgs[id] = csAlloc<CSARGS>(1,Args);

}

/*************************************************ListBox************************************************************* */

vector<HWND> listBoxes;
vector<vector<void(*)(CSARGS)>> LISTBOX_GROUPED_EVENTS_FUNC;
vector<vector<CSARGS>> LISTBOX_GROUPED_EVENTS_ARGS;
vector<bool> HIDELISTGROUPMSG;

WNDPROC OldListProc;

int csCreateListBox(int idpar, RECT r, int style, int hMenuId)
{
    int s = (style == 0) ? WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP | LBS_EXTENDEDSEL : style;
    HWND hListBox = CreateWindowExW(0, L"LISTBOX", nullptr,
                                    s,r.left, r.top, r.right, r.bottom, SECTION[idpar], (HMENU)hMenuId, nullptr, nullptr);
    SendMessage(hListBox, WM_SETFONT, (WPARAM)font, TRUE);
    OldListProc = (WNDPROC)SetWindowLongPtr(hListBox, GWLP_WNDPROC, (LONG_PTR)ListSubclassProc);
    listBoxes.push_back(hListBox);
    vector<void(*)(CSARGS)> v;
    LISTBOX_GROUPED_EVENTS_FUNC.push_back(v);

    vector<CSARGS> arg;
    LISTBOX_GROUPED_EVENTS_ARGS.push_back(arg);
    HIDELISTGROUPMSG.push_back(0);

    return listBoxes.size()-1;
}

LRESULT CALLBACK ListSubclassProc(HWND hList, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static bool b = 0;
    static int id;
    id = getId(hList, listBoxes);

    if(id >= 0)
    {
        int n = LISTBOX_GROUPED_EVENTS_ARGS[id].size();
        if(HIDELISTGROUPMSG[id] == 0)
            for(int i=0; i<n; i++)
            {
                LISTBOX_GROUPED_EVENTS_ARGS[id][i].setProcParams(&hList,&msg,&wParam,&lParam,&id);
                (LISTBOX_GROUPED_EVENTS_FUNC[id][i])(LISTBOX_GROUPED_EVENTS_ARGS[id][i]);
                LRESULT lr = *(LRESULT*)LISTBOX_GROUPED_EVENTS_ARGS[id][i].getLRESULT();
                if(lr)
                {
                    *(LRESULT*)LISTBOX_GROUPED_EVENTS_ARGS[id][i].getLRESULT() = 0;
                    return lr;
                }
            }
    }

    return CallWindowProc(OldListProc, hList, msg, wParam, lParam);
}

HWND listBoxHandle(int id)
{
    if(id < 0) return 0;
    else return listBoxes[id];
}

void csRegListBoxActionArgs_(int id, void**& Args, int& n)
{

}

void csAddListBoxAction(int idl, void(*function)(CSARGS), CSARGS args)
{
    LISTBOX_GROUPED_EVENTS_FUNC[idl].push_back(function);
    LISTBOX_GROUPED_EVENTS_ARGS[idl].push_back(args);
}

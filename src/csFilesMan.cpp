#include "csFilesMan.h"
#include "readwfile.h"
#include "csTranslator.h"

extern vector<HWND> SECTION;
extern vector<RECT> RECTPARREFSAVED;
extern vector<CSTEXT> TITLE;
extern  vector<RECT> RF;

wchar_t* appSizesFilePath = L"appSizes.txt\0";
extern bool saveAppSizes;

wchar_t* appTitleParamsFilePath = L"lang/titles/prm.txt\0";
wchar_t* appTitleFilePath = L"lang/titles/tr.txt\0";
vector<CSTEXT> TITLEFILE;
vector<bool> setTitleInit;
bool saveAppTitles;

using namespace CSSTRUTILS;

bool CSFILESMAN::fileExists(const wchar_t* filename) 
{
    DWORD attributes = GetFileAttributesW(filename);
    
    // Vérifier que le fichier existe ET que ce n'est pas un répertoire
    return (attributes != INVALID_FILE_ATTRIBUTES && 
            !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

void CSFILESMAN::__saveAppSizes()
{
    if(!saveAppSizes)
        return;

    FILE* f = _wfopen(appSizesFilePath,L"w+");
    //FILE* f = fopen(wcharPtrToCharPtr(filePath).c_str(),"w+");

    int n = SECTION.size();

    fwprintf(f, L"%ld %ld\n", (long)GetSystemMetrics(SM_CXSCREEN), (long)GetSystemMetrics(SM_CYSCREEN));

    for(int i=0; i<n; i++)
    {
        //RECT r = rectInParentRef(i);
        RECT r = RECTPARREFSAVED[i];
        fwprintf(f, L"%ld %ld %ld %ld\n", r.left, r.top, r.right-r.left, r.bottom-r.top);
    }
    fclose(f);
}
void CSFILESMAN::__getAppSizes()
{
    if(!(fileExists(appSizesFilePath) && saveAppSizes))
        return;

    FILE* f = _wfopen(appSizesFilePath,L"r");
    wchar_t str[1001];

    fgetws(str, 1000, f);
    vector<wstring> words = splitWords(str);
    vector<long> v = wordsToLong(words);

    long cxmax = v[0], cymax = v[1];
    words.clear();
    v.clear();

    float xcf = 1.0*GetSystemMetrics(SM_CXSCREEN)/cxmax;
    float ycf = 1.0*GetSystemMetrics(SM_CYSCREEN)/cymax;

    while(fgetws(str, 1000, f) != NULL)
    {
        words = splitWords(str);
        v = wordsToLong(words);
        RECT r = {v[0]*xcf,v[1]*ycf, v[2]*xcf, v[3]*ycf};
        //CSFILESMAN::printRect(r, "r = ");
        RF.push_back(r);
        words.clear();
        v.clear();
    }
    fclose(f);  // bug
}

void CSFILESMAN::__setAppSizes()
{
    int n = min(SECTION.size(), RF.size());

    if(SECTION.size() != RF.size())
        cout<<"Warning : some sections or windows may not be resized !";
    for(int i=0; i<n; i++)
    {
        RECT r = RF[i];
        MoveWindow(SECTION[i], r.left, r.top, r.right, r.bottom, 1);
    }
}


int MAX_TRANSLATION_TEXT_LENGTH_REQUESTED = 500-1;

void CSFILESMAN::__saveAppTitles()
{
    
    if(!saveAppTitles)
        return;
    
    FILE* f = _wfopen(appTitleParamsFilePath,L"w+");

    int n = SECTION.size();

    for(int i=0; i<n; i++)
    {
        CSTEXT& t = TITLE[i];
        RECT r = t.TextRect;
        RECT rm = t.Marging;
        CSRGBA c = t.Color;

        wchar_t str[1000];


        fwprintf(f, L"%ld %ld %ld %ld "
                L"%ls,"
                L"%ld %ld "
                L"%d "
                L"%d "
                L"%d "
                L"%ld "
                L"%ld "
                L"%d %d %d %lf "
                L"%ld %ld %ld %ld "
                L"%d "
                L"%d "
                L"%d\n", 
                r.left, r.top, r.right, r.bottom,
                t.Font != 0 ? t.Font : L"-",
                t.FontSize.cx, t.FontSize.cy,
                t.Italic,
                t.Underline,
                t.Orientation,
                t.Bold,
                t.Charset,
                (int)c.r, (int)c.g, (int)c.b, (int)c.a,
                rm.left, rm.top, rm.right, rm.bottom,
                t.Align,
                t.Show,
                t.ShowEntierText);


    }
    fclose(f);

    f = _wfopen(appTitleFilePath,L"wb+");
    unsigned char bom[2] = {0xFF, 0xFE};
    fwrite(bom, 1, 2, f);
    wstring titles = L"";


    for(int i=0; i<n; i++)
    {
        int sz = 0;
        if(TITLE[i].Text) sz = wcslen(TITLE[i].Text);
        if(titles.size() + sz > MAX_TRANSLATION_TEXT_LENGTH_REQUESTED)
        {
            titles.erase(titles.begin()+titles.size()-1);
            fwrite(titles.c_str(), sizeof(wchar_t), titles.size(), f);
            fwrite(L"\n", sizeof(wchar_t), 1, f);
            titles.clear();
            titles = L"";
        }

        if(TITLE[i].Text)
        //fwrite(TITLE[i].Text, sizeof(wchar_t), lstrlenW(TITLE[i].Text), f);
        titles += wstring(TITLE[i].Text) + L"|";
        else
        titles += L"-|";
        //fwrite(L"-", sizeof(wchar_t), 6, f);
        //fwrite(L"\n", sizeof(wchar_t), 1, f);
    }
    if(titles.size())
    {
        titles.erase(titles.begin()+titles.size()-1);
        fwrite(titles.c_str(), sizeof(wchar_t), titles.size(), f);
    }
    fclose(f);

}

#include <fcntl.h>
void CSFILESMAN::__getAppTitles()
{   
    _setmode(_fileno(stdout), _O_U16TEXT);
    if(!(saveAppTitles && fileExists(appTitleFilePath) && fileExists(appTitleParamsFilePath)))
        return;

    if(TITLEFILE.size())
    {
        int n = TITLEFILE.size();
        for(int i=0; i<n; i++)
        {
            TITLEFILE[i].clear();
        }
        TITLEFILE.clear();
    }


    /*auto tl = [](wstring s)
    {
        wchar_t* endPtr;
        return wcstol(s.c_str(), &endPtr, 10);
    };
    auto td = [](wstring s)
    {
        wchar_t* endPtr;
        return wcstod(s.c_str(), &endPtr);
    };*/

    auto tl = [](const std::wstring& s) -> long {
        try {
            return std::stol(s);
        } catch(...) {
            return 0;  // Valeur par défaut en cas d'erreur
        }
    };
    
    auto td = [](const std::wstring& s) -> double {
        try {
            return std::stod(s);
        } catch(...) {
            return 0.0;  // Valeur par défaut en cas d'erreur
        }
    };

    
    auto _splitWords = [](const wchar_t* text) 
    {
        // Copier la chaîne car wcstok() la modifie
        vector<wstring> words;

        size_t len = wcslen(text);
        wchar_t* copy = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
        wcscpy_s(copy, len+1, text);
        
        wchar_t* token;
        wchar_t* context = NULL;  // Pour wcstok_s (version sécurisée)
        
        // Premier appel
        token = wcstok_s(copy, L" ", &context);  // Sépare par espace, tab, newline
        //token = wcstok(copy, L" ");  // Sépare par espace, tab, newline
        
        int i = 1;
        while (token != NULL) {
            
            words.push_back(token);
            // Appels suivants
            if(i == 4)
                token = wcstok_s(NULL, L",", &context);
            else
                token = wcstok_s(NULL, L" ", &context);

            i++;
        }
        
        free(copy);

        return words;
    };

    FILE* f = _wfopen(appTitleParamsFilePath,L"r+");
    wchar_t str[1001] = {0};

    vector<wstring> w;
    while(fgetws(str, 1001, f) != NULL)
    {
        w = _splitWords(str);
        CSTEXT t;
        
        int i = 0;
        t.TextRect = {tl(w[i++]), tl(w[i++]), tl(w[i++]), tl(w[i++])};
        
        t.Font = makeWcharString(w[i++].c_str()); //------------------------------------- memory...
        if(wcsicmp(t.Font, L"-")==0)
        {
            free(t.Font);
            t.Font = 0;
        }
        t.FontSize = {tl(w[i++]), tl(w[i++])};
        t.Italic = tl(w[i++]);
        t.Underline = tl(w[i++]);
        t.Orientation = tl(w[i++]);
        t.Bold = tl(w[i++]);
        t.Charset = tl(w[i++]);
        t.Color = {tl(w[i++]), tl(w[i++]), tl(w[i++]), td(w[i++])};
        t.Marging = {tl(w[i++]), tl(w[i++]), tl(w[i++]), tl(w[i++])};
        t.Align = tl(w[i++]);
        t.Show = tl(w[i++]);
        t.ShowEntierText = tl(w[i++]);
        //cout<<i<<"\n";
        TITLEFILE.push_back(t);
        
        w.clear();
    }
    if (ferror(f)) {
        wprintf(L"Erreur de lecture du fichier\n");
    }
    fclose(f);  

    f = _wfopen(appTitleFilePath,L"rb+");
    int i=0, n = TITLEFILE.size();
    // Lire le BOM (Byte Order Mark)
    unsigned char bom[2];
    fread(bom, 1, 2, f);

    if (!(bom[0] == 0xFF && bom[1] == 0xFE)) {
        wprintf(L"Attention: pas de BOM UTF-16 LE détecté\n");
        rewind(f); // revenir au début si pas de BOM
    }

    /*while(fgetws(str, 1000, f) != NULL)
    {
        if(i >= n)
            break;

        wchar_t* _str = 0;
        if(wcsicmp(str, L"-\r\n") != 0)//
        {
            wcout<<"["<<i<<"]  "<<str;
            _str = makeWcharString(str);
        }
        
        TITLEFILE[i++].Text = _str;
        
    }*/
    fseek(f,0,SEEK_END);
    long size = ftell(f);
    fseek(f,0,SEEK_SET);

    wchar_t* str2 = csAlloc<wchar_t>(MAX_TRANSLATION_TEXT_LENGTH_REQUESTED);
    vector<wstring> wt;
    while(fgetws(str2, size, f) != 0)
    {
        vector<wstring> titles = splitWords(str2, L"|");
        int m = titles.size();

        for(int i=0; i<m; i++)
        {
            wt.push_back(titles[i].c_str());
        }
        //titles.clear();
    }
    free(str2);

    if(wt.size() != n)
    {
        MessageBoxW(0, L"File corrupted !", L"Error", MB_OK|MB_ICONERROR);
        exit(0);
    }

    for(int i=0; i<n; i++)
    {
        if(wt[i] != L"-")
        {
            TITLEFILE[i].Text = makeWcharString(wt[i].c_str());
        }
    }

    wt.clear();

    fclose(f);

}

void CSFILESMAN::setSaveAppTitles(bool b)
{
    saveAppTitles = b;
}

void CSFILESMAN::setSaveAppSizes(bool b)
{
    saveAppSizes = b;
}


#include "readwfile.h"
#include "csLIST.h"

bool __translateTitles() 
{
    // === PARAMÈTRES À MODIFIER ===
    std::string INPUT_FILE = "lang\\titles\\en.txt";
    std::string OUTPUT_FILE = wcharPtrToCharPtr(appTitleFilePath); //"D:\\projects\\CSigma\\lang\\titles\\hi.txt";

    csLIST<char*> l1;
    l1.insertEnd((char*)OUTPUT_FILE.c_str());
    csLIST<char> l2 = l1.toList(0);
    int i1 = l2.findLast('/') ; if(i1 == -1) i1 = l2.findLast('\\') ; if(i1 == -1) i1 = 0;
    int i2 = l2.findLast('.');
    char* tar = l2.toString(i1+1, i2-1);

    l1.clear();
    l2.clear();

    if (!tar)
    {
        return 0;
    }
    
    //std::cout<<"\n"<<tar<<"    -----\n";

    std::string SOURCE_LANG = "en";
    std::string TARGET_LANG = tar;
    // =============================
    
    // Configurer la console pour UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    CSTRANSLATOR translator(INPUT_FILE, OUTPUT_FILE, SOURCE_LANG, TARGET_LANG);

    if (!translator.translate()) {
        std::cout << "Échec de la traduction" << std::endl;
        return 0;
    }
    
    return 1;
}
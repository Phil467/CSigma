#include "csFilesMan.h"
#include "readwfile.h"
#include "csTranslator.h"
#include "csUtils.h"

extern vector<HWND> SECTION;
extern vector<RECT> RECTPARREFSAVED;
extern vector<CSTEXT> TITLE;
extern  vector<RECT> RF;
extern vector<TIPS_POPUP_PARAMS> TipsPopupParams;

wchar_t* appSizesFilePath = L"appSizes.txt\0";
extern bool saveAppSizes;

wchar_t* appTitleParamsFilePath = L"lang/titles/prm.txt\0";
wchar_t* appTitleFilePath = L"lang/titles/ar.txt\0";
vector<CSTEXT> TITLEFILE;
vector<bool> setTitleInit;
bool saveAppTitles;

wchar_t* appTipsFilePath = L"lang/tips/ar.txt\0";
vector<vector<vector<wchar_t*>>> TIPSFILE;
bool saveAppTips;


extern const wchar_t* languagesW[];
extern const wchar_t* langCodesW[];
extern int LANGUAGES_COUNT;


extern wchar_t* originalLanguage;
extern wchar_t* viewLanguage;

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
            titles += wstring(TITLE[i].Text) + L"|";
        else
            titles += L"-|";
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
   // _setmode(_fileno(stdout), _O_U16TEXT);
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
        t.Text = 0;
        
        int i = 0;
        t.TextRect = {tl(w[i++]), tl(w[i++]), tl(w[i++]), tl(w[i++])};
        
        t.Font = makeWString(w[i++].c_str()); //------------------------------------- memory...
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
    //wcout<<"title file size : "<<TITLEFILE.size()<<"\n";
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

    /*fseek(f,0,SEEK_END);
    long size = ftell(f);
    fseek(f,0,SEEK_SET);*/

    wchar_t* str2 = csAlloc<wchar_t>(MAX_TRANSLATION_TEXT_LENGTH_REQUESTED+1);
    vector<wstring> wt;
    while(fgetws(str2, MAX_TRANSLATION_TEXT_LENGTH_REQUESTED, f) != 0)
    {
        vector<wstring> titles = splitWords(str2, L"|");
        int m = titles.size();

        for(int i=0; i<m; i++)
        {
            wt.push_back(titles[i].c_str());
        }
        //titles.clear();
    }

    if(wt.size() != n)
    {
        MessageBoxW(0, L"File corrupted !", L"Error", MB_OK|MB_ICONERROR);
        exit(0);
    }

    for(int i=0; i<n; i++)
    {
        if(wt[i] != L"-" && wt[i] != L"-\n")
        {
            TITLEFILE[i].Text = makeWString(wt[i].c_str());
        }
    }

    wt.clear();
    free(str2);

    fclose(f);

}

void CSFILESMAN::__setAppTitles()
{
    int n = TITLEFILE.size();
    //cout<<n<<"  "<<SECTION.size()<<"  ---text\n";
    for(int i=0; i<n; i++)
    {
        if(TITLEFILE[i].Text)
        {
            wchar_t* t = TITLE[i].Text;
            TITLE[i].Text = makeWString(TITLEFILE[i].Text);
            free(t);
            InvalidateRect(SECTION[i],0,1);
        }
        //CSUIMAN::setTitle(i, TITLEFILE[i], 0);
    }
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
    std::string INPUT_FILE = "lang\\titles\\" + utf16_to_utf8(originalLanguage) + ".txt";
    std::string OUTPUT_FILE = wcharPtrToCharPtr(appTitleFilePath); //"D:\\projects\\CSigma\\lang\\titles\\hi.txt";

    if(!CSFILESMAN::fileExists(utf8_to_utf16(INPUT_FILE).c_str()))
        return 0;

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

    std::string SOURCE_LANG = utf16_to_utf8(originalLanguage);
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

void CSFILESMAN::__saveAppTips(wchar_t* path)
{
    if(!saveAppTips)
        return;

    wchar_t* _path = path != 0 ? path : appTipsFilePath;

    FILE *f = _wfopen(_path,L"wb+");
    // Lire le BOM (Byte Order Mark)
    unsigned char bom[2] = {0xFF,0xFE};
    fwrite(bom, 1, 2, f);

    int n = SECTION.size();

    csLIST<wchar_t> str; 
    csLIST<wchar_t> strStruct;
    for(int i=0; i<n; i++)
    {
        int m = TipsPopupParams[i].message.size();
        
        wstring tmp= (to_wstring(m)+L" ");
        strStruct.insertTableAt((wchar_t*)tmp.c_str(), strStruct.size(), tmp.size());
        for(int j=0; j<m; j++)
        {
            int o = TipsPopupParams[i].message[j].paragraph.size();
            wstring tmp= (to_wstring(o)+L" ");
            strStruct.insertTableAt((wchar_t*)tmp.c_str(), strStruct.size(), tmp.size());
            int pos = 0;
            for(int k=0; k<o; k++)
            {
                wchar_t* t = TipsPopupParams[i].message[j].paragraph[k].Text;
                str.insertTableAt(t, str.size(), wcslen(t));
                str.insertEnd('|');

            }
        }
        strStruct.deleteEnd();
        strStruct.insertEnd(L'|');
    }
    str.deleteEnd();
    strStruct.deleteEnd();
    
    str.insertEnd('\0');
    strStruct.insertEnd('\n');
    strStruct.insertEnd('\0');
    
    wstring tmp=(to_wstring(strStruct.size())+L"\n");
    fwrite((tmp).c_str(), sizeof(wchar_t), tmp.size(), f);
    fwrite(strStruct.getTable(), sizeof(wchar_t), strStruct.size()-1, f);
    tmp = (to_wstring(str.size())+L"\n");
    fwrite((tmp).c_str(), sizeof(wchar_t), tmp.size(), f);
    fwrite(str.getTable(), sizeof(wchar_t), str.size()-1, f);

    str.clear();
    strStruct.clear();

    fclose(f);
}

bool CSFILESMAN::__getAppTips(wchar_t* path)
{
    wchar_t* _path = path != 0 ? path : appTipsFilePath;
    //wcout<<L"path = " <<fileExists(_path)<<L"\n";
    if(!(saveAppTips && fileExists(_path)))
        return 0;

    int m = TIPSFILE.size();
    for(int i=0; i<m; i++)
    {
        int n = TIPSFILE[i].size();
        for(int j=0; j<n; j++)
        {
            int o = TIPSFILE[i][j].size();
            for(int k=0; k<o; k++)
            {
                free(TIPSFILE[i][j][k]);
            }
            TIPSFILE[i][j].clear();
        }
        TIPSFILE[i].clear();
    }
    TIPSFILE.clear();

    FILE *f = _wfopen(_path,L"rb+");
    
    // Lire le BOM (Byte Order Mark)
    unsigned char bom[3];
    fread(bom, 1, 2, f);

    if (!(bom[0] == 0xFF && bom[1] == 0xFE)) {
        wprintf(L"Attention: pas de BOM UTF-16 LE détecté\n");
        rewind(f); // revenir au début si pas de BOM
    }

    wchar_t strStructSize[30];
    fgetws(strStructSize, 30, f);

    wchar_t*t;
    int sz;
    wchar_t strStruct[(sz=wcstol(strStructSize, &t, 10))+1];
    fgetws(strStruct, sz, f);

    wchar_t strSize[30];
    fgetws(strSize, 30, f);

    wchar_t str[(sz=wcstol(strSize, &t, 10))+1];
    fgetws(str, sz, f);

    vector<wstring> vstruct = splitWords(strStruct, L"|");
    vector<wstring> vstr = splitWords(str, L"|");
    int n = vstruct.size();

    long strIter = 0;
    for(int i=0; i<n; i++)
    {
        vector<vector<wchar_t*>> v;
        vector<wstring> pstruct = splitWords(vstruct[i].c_str());
        int psz;
        if((psz = pstruct.size()) > 1)
        {
            int groupNum = wcstol(pstruct[0].c_str(), &t, 10);
            for(int j=1; j<=groupNum; j++)
            {
                vector<wchar_t*> pragGroup;
                int paragNum = wcstol(pstruct[j].c_str(), &t, 10);
                for(int k=0; k<paragNum; k++)
                {
                    pragGroup.push_back(makeWString((wchar_t*)vstr[strIter].c_str()));
                    //wcout<<vstr[strIter]<<L"\n";
                    strIter++;
                }
                v.push_back(pragGroup);
            }
        }
        pstruct.clear();
        TIPSFILE.push_back(v);
    }

    fclose(f);

    return 1;
}

void CSFILESMAN::__setAppTips()
{
    int n = SECTION.size();

    for(int i=0; i<n; i++)
    {
        int m = TipsPopupParams[i].message.size();
        
        for(int j=0; j<m; j++)
        {
            int o = TipsPopupParams[i].message[j].paragraph.size();
            for(int k=0; k<o; k++)
            {
                wchar_t* t = (TipsPopupParams[i].message[j].paragraph[k].Text);
                TipsPopupParams[i].message[j].paragraph[k].Text = CSSTRUTILS::makeWString(TIPSFILE[i][j][k]);
                wprintf(L"%ls\n",TIPSFILE[i][j][k]);
                free(t);

            }
        }
    }
}

void CSFILESMAN::setSaveAppTips(bool b)
{
    saveAppTips = b;
}


bool __translateTips() 
{
    
    std::string INPUT_FILE = string("lang\\tips\\") + utf16_to_utf8(originalLanguage) + ".txt";
    std::string OUTPUT_FILE = wcharPtrToCharPtr(appTipsFilePath); 

    if(!CSFILESMAN::__getAppTips((wchar_t*)charPtrtoWcharPtr(INPUT_FILE.c_str()).c_str()))
    {
        wcout<< L"Pas de fichier à traduire. Veuillez créer un fichier de langue initial.\n";
        return 0;
    }
    

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
    

    std::string SOURCE_LANG = utf16_to_utf8(originalLanguage);
    std::string TARGET_LANG = tar;
    // =============================
    
    
    CSTRANSLATOR translator(INPUT_FILE, OUTPUT_FILE, SOURCE_LANG, TARGET_LANG);
    
    int m = TIPSFILE.size();
    for(int i=0; i<m; i++)
    {
        if(TIPSFILE[i].size())
        {
            int n = TIPSFILE[i].size();
            for(int j=0; j<n; j++)
            {
                vector<wchar_t*> ret;
                ret = translator.translate(TIPSFILE[i][j]);

                int o = ret.size();
                for(int k=0; k<o; k++)
                {
                    wchar_t*t=(TIPSFILE[i][j][k]);
                    TIPSFILE[i][j][k] = ret[k];
                    free(t);
                }
            }
        }
    }
    std::cout << std::endl << "Traduction terminee ! " << std::endl;
    
    return 1;
}

void CSFILESMAN::translateAppStrings()
{
    if(!fileExists(appTitleFilePath))
        __translateTitles();
    __getAppTitles();

    __translateTips();

    __setAppTitles();
    __setAppTips();
}

void CSFILESMAN::setViewLanguage(unsigned int idLang)
{
    if(idLang < LANGUAGES_COUNT)
    {
        viewLanguage = (wchar_t*)langCodesW[idLang];

        wstring s = appTitleFilePath;
        size_t pos1 = s.find_last_of(L"/");
        size_t pos2 = s.find_last_of(L".");

        appTitleFilePath = CSSTRUTILS::makeWString((wchar_t*)(s.substr(0,pos1+1) + viewLanguage + s.substr(pos2, s.size()-pos2)).c_str());

        s = appTipsFilePath;
        pos1 = s.find_last_of(L"/");
        pos2 = s.find_last_of(L".");

        appTipsFilePath = CSSTRUTILS::makeWString((wchar_t*)(s.substr(0,pos1+1) + viewLanguage + s.substr(pos2, s.size()-pos2)).c_str());
    }
}

const wchar_t** CSFILESMAN::getLanguagesW(int* langCount)
{
    if(langCount) *langCount = LANGUAGES_COUNT;

    /*wchar_t* lang[LANGUAGES_COUNT];

    for(int i=0; i<LANGUAGES_COUNT; i++)
    {
        lang[i] = (wchar_t*)languagesW[i];
    }*/

    return (const wchar_t**)languagesW;
}
const wchar_t** CSFILESMAN::getLanguageCodesW(int* langCount)
{
    if(langCount) *langCount = LANGUAGES_COUNT;

    /*wchar_t* lang[LANGUAGES_COUNT];

    for(int i=0; i<LANGUAGES_COUNT; i++)
    {
        lang[i] = (wchar_t*)languagesW[i];
    }*/

    return (const wchar_t**)langCodesW;
}
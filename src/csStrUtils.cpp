#include "csStrUtils.h"


std::wstring CSSTRUTILS::getFileText(const std::wstring& path)
{
    // Convertir le chemin en string pour l'ouverture
    std::string cheminUtf8(path.begin(), path.end());


    std::wifstream file(cheminUtf8);
    file.imbue(std::locale(file.getloc(), new std::codecvt_utf16<wchar_t>));
    if (!file.is_open())
    {
        return L"";
    }

    std::wstring result;
    std::wstring line;

    while (std::getline(file, line))
    {
        // Ajouter la ligne
        result += line;
        // Ajouter \r\n à la fin
        result += L"\r\n";
    }

    return result;
}


vector<wstring> CSSTRUTILS::splitWords(const wchar_t* text, const wchar_t* delims) 
{
    // Copier la chaîne car wcstok() la modifie
    vector<wstring> words;

    size_t len = wcslen(text);
    wchar_t* copy = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
    wcscpy(copy, text);
    
    wchar_t* token;
    wchar_t* context = NULL;  // Pour wcstok_s (version sécurisée)
    
    // Premier appel
    //token = wcstok_s(copy, delims, &context);  // Sépare par espace, tab, newline
    token = wcstok(copy, delims);  // Sépare par espace, tab, newline
    
    while (token != NULL) {
        
        words.push_back(token);
        // Appels suivants
        token = wcstok(NULL, delims);
    }
    
    free(copy);

    return words;
}

vector<long> CSSTRUTILS::wordsToLong(vector<wstring> words)
{
    vector<long> ret;
    int n = words.size();

    for(int i=0; i<n; i++)
    {
        wchar_t* endPtr;
        ret.push_back(wcstol(words[i].c_str(), &endPtr, 10));
    }
    return ret;
}


wchar_t* CSSTRUTILS::makeWcharString(const wchar_t* _str)
{
    int len = wcslen(_str)+1;
    wchar_t* str = (wchar_t*)malloc(sizeof(wchar_t)*len);
    wcscpy_s(str, len, _str);
    return str;
}
wchar_t* CSSTRUTILS::makeWcharString(const char* _str)
{
    int len = strlen(_str)+1;
    wchar_t* str = (wchar_t*)malloc(sizeof(wchar_t)*len);
    wcscpy_s(str, len, charPtrtoWcharPtr(_str).c_str());
    return str;
}
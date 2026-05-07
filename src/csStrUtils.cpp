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


wchar_t* CSSTRUTILS::makeWString(const wchar_t* _str)
{
    int len = wcslen(_str)+1;
    wchar_t* str = (wchar_t*)malloc(sizeof(wchar_t)*len);
    wcscpy_s(str, len, _str);
    return str;
}
wchar_t* CSSTRUTILS::makeWString(const char* _str)
{
    int len = strlen(_str)+1;
    wchar_t* str = (wchar_t*)malloc(sizeof(wchar_t)*len);
    wcscpy_s(str, len, charPtrtoWcharPtr(_str).c_str());
    return str;
}



std::wstring CSSTRUTILS::utf8_to_utf16(const std::string& utf8) 
{
    if (utf8.empty()) return std::wstring();
    
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &utf8[0], (int)utf8.size(), NULL, 0);
    std::wstring result(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &utf8[0], (int)utf8.size(), &result[0], size_needed);
    return result;
}

// Convertir UTF-16 vers UTF-8
std::string CSSTRUTILS::utf16_to_utf8(const std::wstring& utf16) 
{
    if (utf16.empty()) return std::string();
    
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &utf16[0], (int)utf16.size(), NULL, 0, NULL, NULL);
    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &utf16[0], (int)utf16.size(), &result[0], size_needed, NULL, NULL);
    return result;
}



wchar_t* CSSTRUTILS::getStrNumericFormat(long double value, const wchar_t* unit, unsigned int precision, unsigned int grouping, wchar_t separator, int maxDigits)
{
    wchar_t raw[maxDigits];

    if(precision > maxDigits-3)
        precision = maxDigits-3;

    if((long long)(value * precision) % precision == 0)
        swprintf(raw, maxDigits, L"%lld", static_cast<long long>(value));
    else
    {
        wchar_t precisionFormat[100] = {0};
        swprintf(precisionFormat, 100, L"%%.%uLf", precision);
        swprintf(raw, maxDigits, precisionFormat, value);
    }

    wstring number(raw);
    bool isNegative = false;
    if(!number.empty() && number[0] == L'-')
    {
        isNegative = true;
        number.erase(0, 1);
    }

    size_t dotPos = number.find(L'.');
    wstring integerPart = (dotPos == wstring::npos) ? number : number.substr(0, dotPos);
    wstring fractionalPart = (dotPos == wstring::npos) ? L"" : number.substr(dotPos);

    wstring grouped;
    grouped.reserve(integerPart.size() + integerPart.size() / grouping + 4);
    if(isNegative)
        grouped.push_back(L'-');

    for(size_t i = 0; i < integerPart.size(); ++i)
    {
        grouped.push_back(integerPart[i]);
        size_t remaining = integerPart.size() - i - 1;
        if(remaining > 0 && remaining % grouping == 0)
            grouped.push_back(separator);
    }

    wchar_t separatorStr[2] = {separator, 0};
    wstring outputStr = grouped + fractionalPart + wstring(separatorStr) + wstring(unit);
    wchar_t* output = (wchar_t*)malloc((outputStr.size() + 1) * sizeof(wchar_t));
    wcscpy(output, outputStr.c_str());
    return output;
}

/** Parses a number from UI text (spaces, currency, grouping, FR/US separators). */
long double CSSTRUTILS::parseNumericWide(const wstring& text)
{
    wstring raw;
    raw.reserve(text.size());
    for(wchar_t c : text)
    {
        if(c == L' ' || c == L'\t' || c == L'\u00A0' || c == L'\u202F')
            continue;
        if(c >= L'0' && c <= L'9')
            raw.push_back(c);
        else if(c == L'-' && raw.empty())
            raw.push_back(c);
        else if(c == L'.' || c == L',')
            raw.push_back(c);
    }
    if(raw.empty() || raw == L"-")
        return 0.0L;

    const bool hasDot = raw.find(L'.') != wstring::npos;
    size_t commaCount = 0;
    for(wchar_t c : raw)
    {
        if(c == L',')
            commaCount++;
    }
    if(hasDot)
    {
        wstring t;
        for(wchar_t c : raw)
        {
            if(c != L',')
                t.push_back(c);
        }
        raw.swap(t);
    }
    else if(commaCount == 1)
    {
        for(size_t i = 0; i < raw.size(); ++i)
        {
            if(raw[i] == L',')
                raw[i] = L'.';
        }
    }
    else if(commaCount > 1)
    {
        wstring t;
        for(wchar_t c : raw)
        {
            if(c != L',')
                t.push_back(c);
        }
        raw.swap(t);
    }

    wchar_t* endPtr = nullptr;
    const long double v = std::wcstold(raw.c_str(), &endPtr);
    if(endPtr == raw.c_str())
        return 0.0L;
    return v;
}
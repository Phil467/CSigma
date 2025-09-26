#ifndef CSTRANSLATOR_H_INCLUDED
#define CSTRANSLATOR_H_INCLUDED

#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <codecvt>
#include <locale>
#include <thread>
#include <chrono>
#include <map>
#include "csLIST.h"
#include "csStrUtils.h"

#define CSTRANSLATOR_ERR_INVALID_LANGUAGE 0

using namespace std;

struct CSTRANSLATIONRESULT 
{
    std::string translatedText;
    std::string originalText;
    double confidence;
    std::string status;
    bool success;
};

class CSTRANSLATOR
{
    public:
    CSTRANSLATOR(const std::string& input, const std::string& output, 
               const std::string& source, const std::string& target) 
        : inputFile(input), outputFile(output), sourceLang(source), targetLang(target) {};
    
    bool translate() ;
    std::vector<wchar_t*> translate(std::vector<wchar_t*> v);
    bool translateFileAsSingleString(wchar_t* linesDelims);

    bool success = 1;
    int errType = 0;

    private:
    
    std::string inputFile;
    std::string outputFile;
    std::string sourceLang;
    std::string targetLang;

    std::string urlEncode(const std::string& value) ;
    std::string httpRequest(const std::string& url) ;
    std::string extractTranslation(const std::string& jsonResponse);
    std::vector<std::string> readInputFile();
    std::string readInputFileAsSingleString();
    void writeLineUtf16(std::wofstream& file, const std::string& line);
};

class CSJSONPARSER 
{   
    private:
    std::string jsonData;
    size_t pos;

    void skipWhitespace();
    std::string parseString();
    static std::string escapeString(const std::string& str);

    public:
    CSJSONPARSER(const std::string& json) : jsonData(json), pos(0) {};
    std::string extractValue(const std::string& key);
    static std::string createSimpleJson(const std::map<std::string, std::string>& data);
};

class CSAITRANSLATOR 
{
    private:
    HINTERNET hInternet;
    std::string serverUrl;
    std::string apiKey;
    std::string userInstructions;
    bool useHttps;
    
    struct TranslationConfig 
    {
        std::string sourceLanguage;
        std::string targetLanguage;
        std::string customInstructions;
        bool preserveFormatting;
        std::string tone; // formal, casual, technical, etc.
        std::string domain; // medical, legal, technical, general, etc.
    };

    public:
    CSAITRANSLATOR(const std::string& server, const std::string& key = "", bool https = true) 
        : serverUrl(server), apiKey(key), useHttps(https), hInternet(nullptr) 
    {
        
        // Initialiser WinINet
        hInternet = InternetOpenA("TranslationAgent/1.0", 
                                    INTERNET_OPEN_TYPE_DIRECT, 
                                    nullptr, nullptr, 0);
        
        if (!hInternet) {
            throw std::runtime_error("Impossible d'initialiser WinINet");
        }
    };

    ~CSAITRANSLATOR() 
    {
        if (hInternet) {
            InternetCloseHandle(hInternet);
        }
    };
    void setUserInstructions(const std::string& instructions);
    std::string translate(const std::string& text, 
                        const std::string& sourceLang = "auto",
                        const std::string& targetLang = "en",
                        const std::string& customInstructions = "",
                        const std::string& tone = "general",
                        const std::string& domain = "general",
                        bool preserveFormatting = true);
    std::string translateWithInstructions(const std::string& text,
        const std::string& sourceLang,
        const std::string& targetLang,
        const std::string& instructions);
    std::string translateContextual(const std::string& text,
        const std::string& sourceLang,
        const std::string& targetLang,
        const std::string& context,
        const std::string& domain = "general");
    std::string translateWithStyle(const std::string& text,
        const std::string& sourceLang,
        const std::string& targetLang,
        const std::string& styleInstructions);
    std::string performTranslation(const std::string& text, const TranslationConfig& config);

    private:
    std::string prepareEnrichedText(const std::string& originalText, const TranslationConfig& config);
    std::string sendHttpRequest(const std::string& endpoint, const std::string& jsonData);
    std::string readHttpResponse(HINTERNET hRequest);
    std::string postProcessTranslation(const std::string& translatedText);
    void parseUrl(const std::string& url, std::string& hostname, std::string& path, int& port);

};

std::wstring utf8_to_utf16(const std::string& utf8) ;
std::string utf16_to_utf8(const std::wstring& utf16) ;


#endif // CSTRANSLATOR_H_INCLUDED

#include "csTranslator.h"
#include <iomanip>
#include <nlohmann/json.hpp>
#include <regex>
#include <unordered_map>

#pragma comment(lib, "wininet.lib")

// Nombre total de langues et codes
int LANGUAGES_COUNT = 220;

const char* languages[] = {
    "abkhazian", "acehnese", "acholi", "afar", "afrikaans", "aghem", "akan", "albanian", "alemannic_german", "amharic",
    "arabic", "aragonese", "armenian", "assamese", "asturian", "avaric", "awadhi", "aymara", "azerbaijani", "balinese",
    "balti", "bambara", "banjar", "bashkir", "basque", "belarusian", "bemba", "bengali", "bhojpuri", "bislama",
    "bosnian", "breton", "bulgarian", "burmese", "cantonese", "catalan", "cebuano", "chamorro", "chechen", "chichewa",
    "chinese_simplified", "chinese_traditional", "choctaw", "chuvash", "corsican", "cree", "crimean_tatar", "croatian", "czech", "danish",
    "dari", "dhivehi", "dinka", "dogri", "dutch", "dzongkha", "eastern_mari", "english","english_usa","english_uk", "erzya", "esperanto",
    "estonian", "ewe", "faroese", "fijian", "filipino", "finnish", "french", "frisian", "fula", "ga",
    "galician", "ganda", "georgian", "german", "gilbertese", "greek", "guarani", "gujarati", "haitian_creole", "hausa",
    "hawaiian", "hebrew", "hiligaynon", "hindi", "hmong", "hungarian", "icelandic", "igbo", "ilocano", "indonesian",
    "inuktitut", "irish", "italian", "japanese", "javanese", "jingpho", "kalaallisut", "kannada", "kanuri", "kashmiri",
    "kazakh", "khmer", "kinyarwanda", "kirghiz", "komi", "konkani", "korean", "krio", "kurdish_kurmanji", "kurdish_sorani",
    "kyrgyz", "lao", "latin", "latvian", "ligurian", "limburgish", "lingala", "lithuanian", "luganda", "luo",
    "luxembourgish", "macedonian", "madurese", "magahi", "maithili", "makassar", "malagasy", "malay", "malayalam", "maltese",
    "manipuri_meitei", "manx", "maori", "marathi", "marshallese", "minangkabau", "mirandese", "mizo", "mongolian", "myanmar",
    "nahuatl", "ndau", "ndebele", "nepali", "newari", "nko", "northern_sotho", "norwegian_bokmal", "norwegian_nynorsk", "nuer",
    "occitan", "odia", "oromo", "ossetian", "pangasinan", "papiamento", "pashto", "persian", "polish", "portuguese",
    "punjabi_gurmukhi", "punjabi_shahmukhi", "quechua", "romani", "romanian", "rundi", "russian", "samoan", "sango", "sanskrit",
    "sardinian", "scots_gaelic", "serbian", "sesotho", "setswana", "shona", "sindhi", "sinhala", "slovak", "slovenian",
    "somali", "spanish", "sundanese", "swahili", "swati", "swedish", "tajik", "tamil", "tatar", "telugu",
    "tetum", "thai", "tibetan", "tigrinya", "tiv", "tok_pisin", "tongan", "tsonga", "tswana", "tulu",
    "turkish", "turkmen", "twi", "ukrainian", "urdu", "uyghur", "uzbek", "venda", "vietnamese", "waigali",
    "welsh", "western_frisian", "wolof", "xhosa", "yiddish", "yoruba", "yue_chinese", "zulu"
};


const char* langCodes[] = {
    "ab", "ace", "ach", "aa", "af", "agq", "ak", "sq", "gsw", "am",
    "ar", "an", "hy", "as", "ast", "av", "awa", "ay", "az", "ban",
    "bft", "bm", "bjn", "ba", "eu", "be", "bem", "bn", "bho", "bi",
    "bs", "br", "bg", "my", "yue", "ca", "ceb", "ch", "ce", "ny",
    "zh-cn", "zh-tw", "cho", "cv", "co", "cr", "crh", "hr", "cs", "da",
    "prs", "dv", "din", "doi", "nl", "dz", "mhr", "en","en-us","en-gb", "myv", "eo",
    "et", "ee", "fo", "fj", "tl", "fi", "fr", "fy", "ff", "gaa",
    "gl", "lg", "ka", "de", "gil", "el", "gn", "gu", "ht", "ha",
    "haw", "he", "hil", "hi", "hmn", "hu", "is", "ig", "ilo", "id",
    "iu", "ga", "it", "ja", "jv", "kac", "kl", "kn", "kr", "ks",
    "kk", "km", "rw", "ky", "kmi", "kok", "ko", "kri", "ku", "ckb",
    "ky", "lo", "la", "lv", "lij", "li", "ln", "lt", "lg", "luo",
    "lb", "mk", "mad", "mai", "mai", "mak", "mg", "ms", "ml", "mt",
    "mni", "gv", "mi", "mr", "mh", "min", "mwl", "lus", "mn", "my",
    "nah", "ndc", "nr", "ne", "new", "nqo", "nso", "nb", "nn", "nus",
    "oc", "or", "om", "os", "pag", "pap", "ps", "fa", "pl", "pt",
    "pa", "pnb", "qu", "rom", "ro", "rn", "ru", "sm", "sg", "sa",
    "sc", "gd", "sr", "st", "tn", "sn", "sd", "si", "sk", "sl",
    "so", "es", "su", "sw", "ss", "sv", "tg", "ta", "tt", "te",
    "tet", "th", "bo", "ti", "tiv", "tpi", "to", "ts", "tn", "tcy",
    "tr", "tk", "tw", "uk", "ur", "ug", "uz", "ve", "vi", "wbl",
    "cy", "fy", "wo", "xh", "yi", "yo", "yue", "zu"
};

const wchar_t* langCodesW[] = {
    L"ab", L"ace", L"ach", L"aa", L"af", L"agq", L"ak", L"sq", L"gsw", L"am",
    L"ar", L"an", L"hy", L"as", L"ast", L"av", L"awa", L"ay", L"az", L"ban",
    L"bft", L"bm", L"bjn", L"ba", L"eu", L"be", L"bem", L"bn", L"bho", L"bi",
    L"bs", L"br", L"bg", L"my", L"yue", L"ca", L"ceb", L"ch", L"ce", L"ny",
    L"zh-cn", L"zh-tw", L"cho", L"cv", L"co", L"cr", L"crh", L"hr", L"cs", L"da",
    L"prs", L"dv", L"din", L"doi", L"nl", L"dz", L"mhr", L"en", L"en-us", L"en-gb", L"myv", L"eo",
    L"et", L"ee", L"fo", L"fj", L"tl", L"fi", L"fr", L"fy", L"ff", L"gaa",
    L"gl", L"lg", L"ka", L"de", L"gil", L"el", L"gn", L"gu", L"ht", L"ha",
    L"haw", L"he", L"hil", L"hi", L"hmn", L"hu", L"is", L"ig", L"ilo", L"id",
    L"iu", L"ga", L"it", L"ja", L"jv", L"kac", L"kl", L"kn", L"kr", L"ks",
    L"kk", L"km", L"rw", L"ky", L"kmi", L"kok", L"ko", L"kri", L"ku", L"ckb",
    L"ky", L"lo", L"la", L"lv", L"lij", L"li", L"ln", L"lt", L"lg", L"luo",
    L"lb", L"mk", L"mad", L"mai", L"mai", L"mak", L"mg", L"ms", L"ml", L"mt",
    L"mni", L"gv", L"mi", L"mr", L"mh", L"min", L"mwl", L"lus", L"mn", L"my",
    L"nah", L"ndc", L"nr", L"ne", L"new", L"nqo", L"nso", L"nb", L"nn", L"nus",
    L"oc", L"or", L"om", L"os", L"pag", L"pap", L"ps", L"fa", L"pl", L"pt",
    L"pa", L"pnb", L"qu", L"rom", L"ro", L"rn", L"ru", L"sm", L"sg", L"sa",
    L"sc", L"gd", L"sr", L"st", L"tn", L"sn", L"sd", L"si", L"sk", L"sl",
    L"so", L"es", L"su", L"sw", L"ss", L"sv", L"tg", L"ta", L"tt", L"te",
    L"tet", L"th", L"bo", L"ti", L"tiv", L"tpi", L"to", L"ts", L"tn", L"tcy",
    L"tr", L"tk", L"tw", L"uk", L"ur", L"ug", L"uz", L"ve", L"vi", L"wbl",
    L"cy", L"fy", L"wo", L"xh", L"yi", L"yo", L"yue", L"zu"
};

// Tableau de langues pour accès individuel
const wchar_t* languagesW[] = {
    L"abkhazian", L"acehnese", L"acholi", L"afar", L"afrikaans", L"aghem", L"akan", L"albanian", L"alemannic_german", L"amharic",
    L"arabic", L"aragonese", L"armenian", L"assamese", L"asturian", L"avaric", L"awadhi", L"aymara", L"azerbaijani", L"balinese",
    L"balti", L"bambara", L"banjar", L"bashkir", L"basque", L"belarusian", L"bemba", L"bengali", L"bhojpuri", L"bislama",
    L"bosnian", L"breton", L"bulgarian", L"burmese", L"cantonese", L"catalan", L"cebuano", L"chamorro", L"chechen", L"chichewa",
    L"chinese_simplified", L"chinese_traditional", L"choctaw", L"chuvash", L"corsican", L"cree", L"crimean_tatar", L"croatian", L"czech", L"danish",
    L"dari", L"dhivehi", L"dinka", L"dogri", L"dutch", L"dzongkha", L"eastern_mari", L"english", L"english_usa", L"english_uk", L"erzya", L"esperanto",
    L"estonian", L"ewe", L"faroese", L"fijian", L"filipino", L"finnish", L"french", L"frisian", L"fula", L"ga",
    L"galician", L"ganda", L"georgian", L"german", L"gilbertese", L"greek", L"guarani", L"gujarati", L"haitian_creole", L"hausa",
    L"hawaiian", L"hebrew", L"hiligaynon", L"hindi", L"hmong", L"hungarian", L"icelandic", L"igbo", L"ilocano", L"indonesian",
    L"inuktitut", L"irish", L"italian", L"japanese", L"javanese", L"jingpho", L"kalaallisut", L"kannada", L"kanuri", L"kashmiri",
    L"kazakh", L"khmer", L"kinyarwanda", L"kirghiz", L"komi", L"konkani", L"korean", L"krio", L"kurdish_kurmanji", L"kurdish_sorani",
    L"kyrgyz", L"lao", L"latin", L"latvian", L"ligurian", L"limburgish", L"lingala", L"lithuanian", L"luganda", L"luo",
    L"luxembourgish", L"macedonian", L"madurese", L"magahi", L"maithili", L"makassar", L"malagasy", L"malay", L"malayalam", L"maltese",
    L"manipuri_meitei", L"manx", L"maori", L"marathi", L"marshallese", L"minangkabau", L"mirandese", L"mizo", L"mongolian", L"myanmar",
    L"nahuatl", L"ndau", L"ndebele", L"nepali", L"newari", L"nko", L"northern_sotho", L"norwegian_bokmal", L"norwegian_nynorsk", L"nuer",
    L"occitan", L"odia", L"oromo", L"ossetian", L"pangasinan", L"papiamento", L"pashto", L"persian", L"polish", L"portuguese",
    L"punjabi_gurmukhi", L"punjabi_shahmukhi", L"quechua", L"romani", L"romanian", L"rundi", L"russian", L"samoan", L"sango", L"sanskrit",
    L"sardinian", L"scots_gaelic", L"serbian", L"sesotho", L"setswana", L"shona", L"sindhi", L"sinhala", L"slovak", L"slovenian",
    L"somali", L"spanish", L"sundanese", L"swahili", L"swati", L"swedish", L"tajik", L"tamil", L"tatar", L"telugu",
    L"tetum", L"thai", L"tibetan", L"tigrinya", L"tiv", L"tok_pisin", L"tongan", L"tsonga", L"tswana", L"tulu",
    L"turkish", L"turkmen", L"twi", L"ukrainian", L"urdu", L"uyghur", L"uzbek", L"venda", L"vietnamese", L"waigali",
    L"welsh", L"western_frisian", L"wolof", L"xhosa", L"yiddish", L"yoruba", L"yue_chinese", L"zulu"
};

wchar_t* KEY = L"";

wchar_t* originalLanguageCode = L"en-us";
wchar_t* viewLanguageCode = L"fr";

extern int MAX_TRANSLATION_TEXT_LENGTH_REQUESTED;


std::wstring utf8_to_utf16(const std::string& utf8) 
{
    if (utf8.empty()) return std::wstring();
    
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &utf8[0], (int)utf8.size(), NULL, 0);
    std::wstring result(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &utf8[0], (int)utf8.size(), &result[0], size_needed);
    return result;
}

// Convertir UTF-16 vers UTF-8
std::string utf16_to_utf8(const std::wstring& utf16) 
{
    if (utf16.empty()) return std::string();
    
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &utf16[0], (int)utf16.size(), NULL, 0, NULL, NULL);
    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &utf16[0], (int)utf16.size(), &result[0], size_needed, NULL, NULL);
    return result;
}

// Encoder une chaîne pour URL
std::string CSTRANSLATOR::urlEncode(const std::string& value) 
{
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;
    
    for (char c : value) 
    {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') 
        {
            escaped << c;
        } 
        else 
        {
            escaped << std::uppercase;
            escaped << '%' << std::setw(2) << int((unsigned char)c);
            escaped << std::nouppercase;
        }
    }
    
    return escaped.str();
}

// Effectuer requête HTTP
std::string CSTRANSLATOR::httpRequest(const std::string& url) 
{
    HINTERNET hInternet = InternetOpenA("Translator/1.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return "";
    
    HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) 
    {
        InternetCloseHandle(hInternet);
        return "";
    }
    
    std::string result;
    char buffer[4096];
    DWORD bytesRead;
    
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) 
    {
        result.append(buffer, bytesRead);
    }
    
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return result;
}

std::string translateWithLibreTranslate(const std::string& text, 
        const std::string& sourceLang, 
        const std::string& targetLang) 
{
    std::string result;

    // 1. Initialiser WinINet
    HINTERNET hInternet = InternetOpen(L"LibreTranslate Client", 
    INTERNET_OPEN_TYPE_PRECONFIG, 
    NULL, NULL, 0);
    if (!hInternet) {
    return "Erreur: Impossible d'initialiser WinINet";
    }

    // 2. Se connecter au serveur
    HINTERNET hConnect = InternetConnect(hInternet,
        L"libretranslate.com",
        INTERNET_DEFAULT_HTTPS_PORT,
        NULL, NULL,
        INTERNET_SERVICE_HTTP,
        0, 0);
    if (!hConnect) {
    InternetCloseHandle(hInternet);
    return "Erreur: Impossible de se connecter au serveur";
    }

    // 3. Créer la requête HTTP POST
    HINTERNET hRequest = HttpOpenRequest(hConnect,
        L"POST",
        L"/translate",
        NULL,
        NULL,
        NULL,
        INTERNET_FLAG_SECURE, // HTTPS
        0);
    if (!hRequest) {
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return "Erreur: Impossible de créer la requête";
    }

    // 4. Construire le JSON avec les paramètres fournis
    std::string headers = "Content-Type: application/json\r\n";
    std::string jsonData = "{\"q\":\"" + text + 
    "\",\"source\":\"" + sourceLang + 
    "\",\"target\":\"" + targetLang + 
    "\",\"format\":\"text\"}";

    // 5. Envoyer la requête
    BOOL requestSent = HttpSendRequestA(hRequest,
        headers.c_str(),
        headers.length(),
        (LPVOID)jsonData.c_str(),
        jsonData.length());

    if (!requestSent) 
    {
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return "Erreur: Impossible d'envoyer la requête";
    }

    // 6. Lire la réponse
    char buffer[4096];
    DWORD bytesRead;

    while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) 
    {
    buffer[bytesRead] = '\0';
    result += buffer;
    }

    // 7. Nettoyer les handles
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return result;
}

size_t findLastOccurence(const char* str, const char* source, size_t sourceSize, bool includeStr)
{
    size_t pos, strSize1 = strlen(str)-1;
    bool b;
    for(int i=sourceSize-1; i>=0; i--)
    {
        b = 1;
        if(source[i] == str[strSize1])
        {
            int k=1;
            for(int j=strSize1-1; j>=0; j--, k++)
            {
                if(source[i-k] != str[j])
                {
                    b = 0;
                    break;
                }
            }
            
            if(b) 
            {
                if(includeStr) return i-k+1;
                else return i+1;
            }
        }
    }

    return MAXLONG;
}

// Parser JSON simple pour extraire translatedText
std::string CSTRANSLATOR::extractTranslation(const std::string& jsonResponse) 
{
    // Recherche simple de "translatedText":"..." // ------ moins sure
    std::string searchStr = "\"translatedText\":\"";
    size_t start = jsonResponse.find(searchStr);
    if (start == std::string::npos) return "";
    
    start += searchStr.length();
    size_t end;
    if(jsonResponse.substr(start, 7) != "<entry>")
    {
        end = jsonResponse.find("\"", start);
        if (end == std::string::npos) return "";
    }
    else // pour contourner le bug du a l'apparition des balises xml dans le resultat de "translatedText"
    {
        // Recherche simple de "translated":"..." // ------ plus sure
        start = findLastOccurence("\"translation\":\"", jsonResponse.c_str(), jsonResponse.size(), 0);
        if (start == MAXLONG) return "";
        
        end = findLastOccurence("\",\"source\"", jsonResponse.c_str(), jsonResponse.size(), 1);
        if (end == MAXLONG) return "";
    }
    
    std::string translation = jsonResponse.substr(start, end - start);
    
    // Décoder les échappements JSON
    std::string decoded;
    size_t i = 0;
    while (i < translation.length()) 
    {
        if (translation[i] == '\\' && i + 1 < translation.length()) 
        {
            char next = translation[i + 1];
            if (next == '\"') {
                decoded += '\"';
                i += 2;
            } else if (next == '\\') {
                decoded += '\\';
                i += 2;
            } else if (next == '/') {
                decoded += '/';
                i += 2;
            } else if (next == 'b') {
                decoded += '\b';
                i += 2;
            } else if (next == 'f') {
                decoded += '\f';
                i += 2;
            } else if (next == 'n') {
                decoded += '\n';
                i += 2;
            } else if (next == 'r') {
                decoded += '\r';
                i += 2;
            } else if (next == 't') {
                decoded += '\t';
                i += 2;
            } 
            else if (next == 'u' && i + 5 < translation.length()) 
            {
                // Décoder \uXXXX
                std::string hexStr = translation.substr(i + 2, 4);
                try {
                    unsigned int codepoint = std::stoul(hexStr, nullptr, 16);
                    
                    // Convertir le codepoint Unicode en UTF-8
                    if (codepoint <= 0x7F) 
                    {
                        decoded += (char)codepoint;
                    } else if (codepoint <= 0x7FF) 
                    {
                        decoded += (char)(0xC0 | (codepoint >> 6));
                        decoded += (char)(0x80 | (codepoint & 0x3F));
                    } else if (codepoint <= 0xFFFF) 
                    {
                        decoded += (char)(0xE0 | (codepoint >> 12));
                        decoded += (char)(0x80 | ((codepoint >> 6) & 0x3F));
                        decoded += (char)(0x80 | (codepoint & 0x3F));
                    } else 
                    {
                        // Pour les codepoints > 0xFFFF, il faudrait gérer les paires de substitution
                        // mais c'est rare dans les traductions courantes
                        decoded += '?'; // Caractère de remplacement
                    }
                    i += 6;
                } catch (...) 
                {
                    // En cas d'erreur, garder la séquence originale
                    decoded += translation[i];
                    i++;
                }
            } else {
                decoded += translation[i];
                i++;
            }
        } else {
            decoded += translation[i];
            i++;
        }
    }
    
    return decoded;
}







/*********************************  json parsing ******************************************************* */

using json = nlohmann::json;

// Table de correspondance des entités HTML courantes
std::unordered_map<std::string, std::string> htmlEntities = {
    {"&#10;", "\n"},     // Line Feed
    {"&#13;", "\r"},     // Carriage Return
    {"&#9;", "\t"},      // Tab
    {"&#32;", " "},      // Space
    {"&#34;", "\""},     // Quote
    {"&#38;", "&"},      // Ampersand
    {"&#39;", "'"},      // Apostrophe
    {"&#60;", "<"},      // Less than
    {"&#62;", ">"},      // Greater than
    {"&lt;", "<"},       // Less than (named)
    {"&gt;", ">"},       // Greater than (named)
    {"&amp;", "&"},      // Ampersand (named)
    {"&quot;", "\""},    // Quote (named)
    {"&apos;", "'"},     // Apostrophe (named)
    {"&nbsp;", " "},     // Non-breaking space
};
std::unordered_map<std::string, std::string> htmlEntities2 = {
    {"&#10; ", "\n"},     // Line Feed
    {"&#13; ", "\r"},     // Carriage Return
    {"&#9; ", "\t"},      // Tab
    {"&#32; ", " "},      // Space
    {"&#34; ", "\""},     // Quote
    {"&#38; ", "&"},      // Ampersand
    {"&#39; ", "'"},      // Apostrophe
    {"&#60; ", "<"},      // Less than
    {"&#62; ", ">"},      // Greater than
    {"&lt; ", "<"},       // Less than (named)
    {"&gt; ", ">"},       // Greater than (named)
    {"&amp; ", "&"},      // Ampersand (named)
    {"&quot; ", "\""},    // Quote (named)
    {"&apos; ", "'"},     // Apostrophe (named)
    {"&nbsp; ", " "},     // Non-breaking space
};

// Fonction pour décoder les entités HTML numériques (&#xxx;)
std::string decodeNumericEntities(const std::string& text, const std::regex& numericPattern) 
{
    //std::regex numericPattern(R"(&#(\d+);)");
    //std::regex numericPattern(R"(&#(\d+); )"); // espace a la fin
    std::string result = text;
    
    std::sregex_iterator iter(text.begin(), text.end(), numericPattern);
    std::sregex_iterator end;
    
    // Traiter les correspondances en ordre inverse pour éviter les problèmes d'index
    std::vector<std::pair<size_t, size_t>> matches;
    std::vector<int> codes;
    
    for (auto it = iter; it != end; ++it) {
        matches.push_back({it->position(), it->length()});
        codes.push_back(std::stoi((*it)[1].str()));
    }
    
    // Traiter de la fin vers le début
    for (int i = matches.size() - 1; i >= 0; i--) {
        size_t pos = matches[i].first;
        size_t len = matches[i].second;
        int code = codes[i];
        
        // Convertir le code ASCII en caractère
        if (code >= 0 && code <= 127) {
            char character = static_cast<char>(code);
            result.replace(pos, len, 1, character);
        }
    }
    
    return result;
}

// Fonction pour décoder les entités HTML nommées et numériques
std::string decodeHtmlEntities(const std::string& text) 
{
    std::string result = text;
    
    // D'abord, décoder les entités numériques
    std::regex numericPattern(R"(&#(\d+); )");
    result = decodeNumericEntities(result, numericPattern);

    std::regex numericPattern2(R"(&#(\d+);)");
    result = decodeNumericEntities(result, numericPattern2);
    
    // Ensuite, décoder les entités nommées courantes
    for (const auto& entity : htmlEntities) {
        size_t pos = 0;
        while ((pos = result.find(entity.first, pos)) != std::string::npos) {
            result.replace(pos, entity.first.length(), entity.second);
            pos += entity.second.length();
        }
    }
    for (const auto& entity : htmlEntities2) {
        size_t pos = 0;
        while ((pos = result.find(entity.first, pos)) != std::string::npos) {
            result.replace(pos, entity.first.length(), entity.second);
            pos += entity.second.length();
        }
    }
    
    return result;
}

// Version améliorée de la fonction d'extraction avec décodage HTML
std::string extractTranslationWithDecoding(const std::string& jsonResponse) 
{
    try {
        json parsed = json::parse(jsonResponse);
        
        if (parsed.contains("responseData") && 
            parsed["responseData"].contains("translatedText")) {
            
            std::string translatedText = parsed["responseData"]["translatedText"];
            
            // Décoder les entités HTML
            translatedText = decodeHtmlEntities(translatedText);
            
            return translatedText;
        } else {
            std::cerr << "Erreur: Structure JSON inattendue" << std::endl;
            return "";
        }
        
    } catch (const json::parse_error& e) {
        std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
        return "";
    }
}


CSTRANSLATIONRESULT extractDetailedTranslationWithDecoding(const std::string& jsonResponse) 
{
    CSTRANSLATIONRESULT result = {"", "", 0.0, "", false};
    
    try {
        json parsed = json::parse(jsonResponse);
        
        // Vérifier le statut
        if (parsed.contains("responseStatus")) {
            int responseStatus = parsed["responseStatus"];
            if (responseStatus == 200) {
                result.success = true;
                result.status = "OK";
            } else {
                result.success = false;
                result.status = "Error: " + std::to_string(responseStatus);
                return result;
            }
        }
        
        // Extraire et décoder la traduction
        if (parsed.contains("responseData")) {
            auto responseData = parsed["responseData"];
            
            if (responseData.contains("translatedText")) {
                result.translatedText = responseData["translatedText"];
                // Décoder les entités HTML
                result.translatedText = decodeHtmlEntities(result.translatedText);
            }
        }
        
        // Traiter les matches
        if (parsed.contains("matches") && parsed["matches"].is_array() && 
            !parsed["matches"].empty()) {
            auto firstMatch = parsed["matches"][0];
            if (firstMatch.contains("segment")) {
                result.originalText = firstMatch["segment"];
                // Décoder aussi le texte original si nécessaire
                result.originalText = decodeHtmlEntities(result.originalText);
            }
            if (firstMatch.contains("quality")) {
                if (firstMatch["quality"].is_string()) {
                    std::string qualityStr = firstMatch["quality"];
                    result.confidence = std::stod(qualityStr) / 100.0;
                } else if (firstMatch["quality"].is_number()) {
                    result.confidence = firstMatch["quality"];
                }
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Erreur: " << e.what() << std::endl;
        result.success = false;
        result.status = "Error";
    }
    
    return result;
}



/************************************************************************************************************************************************ */

// Lire fichier avec détection d'encodage
std::vector<std::string> CSTRANSLATOR::readInputFile() 
{
    std::vector<std::string> lines;
    
    // Essayer d'abord UTF-16 LE
    std::wifstream file16(inputFile, std::ios::binary);
    file16.imbue(std::locale(std::locale(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));
    
    if (file16.is_open()) 
    {
        // Vérifier le BOM UTF-16 LE
        wchar_t bom;
        file16.read(&bom, 1);
        if (bom != 0xFEFF) 
        {
            file16.seekg(0); // Pas de BOM, retourner au début
        }
        
        std::wstring line;
        while (std::getline(file16, line)) 
        {
            lines.push_back(utf16_to_utf8(line));
        }
        file16.close();
        
        if (!lines.empty()) return lines;
    }
    
    // Si UTF-16 échoue, essayer UTF-8
    std::ifstream file8(inputFile);
    if (file8.is_open()) 
    {
        std::string line;
        while (std::getline(file8, line)) 
        {
            lines.push_back(line);
        }
        file8.close();
    }
    
    return lines;
}

std::string CSTRANSLATOR::readInputFileAsSingleString() 
{
    std::string retLine = "";
    
    // Essayer d'abord UTF-16 LE
    std::wifstream file16(inputFile, std::ios::binary);
    file16.imbue(std::locale(std::locale(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));
    
    if (file16.is_open()) 
    {
        // Vérifier le BOM UTF-16 LE
        wchar_t bom;
        file16.read(&bom, 1);
        if (bom != 0xFEFF) 
        {
            file16.seekg(0); // Pas de BOM, retourner au début
        }
        
        std::wstring line;
        while (std::getline(file16, line)) 
        {
            retLine += utf16_to_utf8(line) + "\n";
        }
        file16.close();
        
        if (retLine.size()) return retLine;
    }
    
    // Si UTF-16 échoue, essayer UTF-8
    std::ifstream file8(inputFile);
    if (file8.is_open()) 
    {
        std::string line;
        while (std::getline(file8, line)) 
        {
            retLine += line + "\n";
        }
        file8.close();
    }
    
    return retLine;
}

// Écrire une ligne en UTF-16 LE avec BOM
void CSTRANSLATOR::writeLineUtf16(std::wofstream& file, const std::string& line) 
{
    std::wstring wline = utf8_to_utf16(line);
    file << wline << std::endl;
}


std::vector<wchar_t*> CSTRANSLATOR::translate(std::vector<wchar_t*> lines)
{
    int totalLines = lines.size();
    std::vector<wchar_t*> ret;
    csLIST<wchar_t> line;
    int lineCount = 0;
    for (int i=0; i<totalLines; i++) 
    {
        int n = wcslen(lines[i]);
        line.insertTableEnd(lines[i], n);
        lineCount++;
        
        /*if (line.size() == 0) {
            std::wcout << L"Ligne " << lineCount << "/" << totalLines 
                        << L": (ligne vide) - conservee" << std::endl;
            ret.push_back(0);
        }
        else if (wcscmp(line.getTable(), L"-") == 0) {
            std::wcout << L"Ligne " << lineCount << "/" << totalLines 
                        << L": - '-' non traduite" << std::endl;
            ret.push_back(CSSTRUTILS::makeWString(L"-"));
        }
        else */
        {
            int len = (i==totalLines-1 ? -1 : wcslen(lines[i+1]));
            
            if(len > -1 && (line.size() + len) < MAX_TRANSLATION_TEXT_LENGTH_REQUESTED)
            {
                line.insertEnd(L'\n');
                continue;
            }
            line.endStringW();

            /*wchar_t* part=0;
            std::wstring preview = line.size() > 50 ? 
                wstring(part=line.toStringW(0, 50)) + L"..." : line.getTable();
            std::wcout << L"Ligne " << lineCount << L"/" << totalLines 
                        << L": " << preview << std::endl;
            if(part) free(part);*/
            //wcout<<L"\n"<<line.getTable() << L"    path = ---------------------------------------------" <<L"\n";
            
            // Construire l'URL de l'API
            std::string encodedText = urlEncode(utf16_to_utf8(line.getTable()));

            std::string url = "https://api.mymemory.translated.net/get?q=" + 
                            encodedText + "&langpair=" + sourceLang + "|" + targetLang + "&de=azphil@outlook.fr";
            
            // Effectuer la requête
            std::string response = httpRequest(url);

            //std::string response = translateWithLibreTranslate(encodedText, sourceLang, targetLang);
            //std::string translation = extractTranslation(response);
            CSTRANSLATIONRESULT translation = extractDetailedTranslationWithDecoding(response);

            printf("%s\n",response.c_str());

            //if (!translation.empty() && translation != "null") 
            if(translation.success)
            {
                vector<wstring> vline = CSSTRUTILS::splitWords(utf8_to_utf16(translation.translatedText).c_str(), L"\n");
                int n = vline.size();
                for(int i=0; i<n; i++)
                {
                    ret.push_back(CSSTRUTILS::makeWString(vline[i].c_str()));
                }
                vline.clear();
            } 
            else 
            {
                std::wcout << L"Erreur de traduction, conservation du texte original" << std::endl;
                vector<wstring> vline = CSSTRUTILS::splitWords((const wchar_t*)line.getTable(), L"\n");
                int n = vline.size();
                for(int i=0; i<vline.size(); i++)
                {
                    ret.push_back((wchar_t*)CSSTRUTILS::makeWString(vline[i].c_str()));
                }
                vline.clear();
                success = 0;

                if(strstr("IS AN INVALID TARGET LANGUAGE", translation.translatedText.c_str()) != 0)
                {
                    errType = CSTRANSLATOR_ERR_INVALID_LANGUAGE;
                    wcout<<L"invalid language! ------------------------------------\n";
                }
                else 
                    errType = 1;
            }
            
            // Pause d'1 seconde
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            line.clear();
        }
    }
    
    
    return ret;
    
}

bool CSTRANSLATOR::translate() 
{
    std::cout << "Traduction de: " << inputFile << std::endl;
    std::cout << "Vers: " << outputFile << std::endl;
    std::cout << "Langue cible: " << targetLang << std::endl << std::endl;
    
    // Vérifier que le fichier d'entrée existe
    if (GetFileAttributesA(inputFile.c_str()) == INVALID_FILE_ATTRIBUTES) {
        std::cout << "ERREUR: Fichier introuvable: " << inputFile << std::endl;
        return false;
    }
    
    std::cout << "Traduction en cours..." << std::endl << std::endl;
    
    // Lire le fichier d'entrée
    std::vector<std::string> lines = readInputFile();
    if (lines.empty()) 
    {
        std::cout << "ERREUR: Impossible de lire le fichier d'entrée" << std::endl;
        return false;
    }
    
    // Créer le fichier de sortie UTF-16 LE avec BOM
    std::wofstream outFile(outputFile, std::ios::binary);
    outFile.imbue(std::locale(std::locale(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));
    
    if (!outFile.is_open()) {
        std::cout << "ERREUR: Impossible de créer le fichier de sortie" << std::endl;
        return false;
    }
    
    // Écrire le BOM UTF-16 LE
    outFile.put(0xFEFF);
    
    int totalLines = (int)lines.size();
    int lineCount = 0;
    
    std::string line;
    bool b = 1;
    for (int i=0; i<totalLines; i++) 
    {
        line = lines[i];
        lineCount++;
        
        if (line.empty()) {
            std::cout << "Ligne " << lineCount << "/" << totalLines 
                        << ": (ligne vide) - conservee" << std::endl;
            outFile << std::endl;
        }
        else if (line == "-") {
            std::cout << "Ligne " << lineCount << "/" << totalLines 
                        << ": - '-' non traduite" << std::endl;
            writeLineUtf16(outFile, "-");
        }
        else {
            int n = line.size();
            for(int i=0; i<n; i++)
            {
                if(line[i] == '|')
                    line[i] = '\n';
            }
            std::string preview = line.length() > 50 ? 
                line.substr(0, 50) + "..." : line;
            std::cout << "Ligne " << lineCount << "/" << totalLines 
                        << ": " << preview << std::endl;
            
            // Construire l'URL de l'API
            std::string encodedText = urlEncode(line);
            std::string url = "https://api.mymemory.translated.net/get?q=" + 
                            encodedText + "&langpair=" + sourceLang + "|" + targetLang + "&de=azphil@outlook.fr";
                            
            
            // Effectuer la requête
            std::string response = httpRequest(url);
            //std::string translation = extractTranslation(response);
            CSTRANSLATIONRESULT translation = extractDetailedTranslationWithDecoding(response);
            
            //if (!translation.empty() && translation != "null") 
            if(translation.success)
            {
                n = translation.translatedText.size();
                for(int i=0; i<n; i++)
                {
                    if(translation.translatedText[i] == '\n')
                    {
                        translation.translatedText[i] = '|';
                    }
                }
                cout<<translation.translatedText<<"\n";
                writeLineUtf16(outFile, translation.translatedText);
            } 
            else 
            {
                std::cout << "Erreur de traduction, conservation du texte original" << std::endl;
                for(int i=0; i<n; i++)
                {
                    if(line[i] == '\n')
                        line[i] = '|';
                }
                writeLineUtf16(outFile, line);
                b = 0;
            }
            
            // Pause d'1 seconde
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    outFile.close();
    
    std::cout << std::endl << "Traduction terminee: " << outputFile << std::endl;
    std::cout << "Fichier encode en UTF-16 LE avec BOM" << std::endl;
    
    return b;
};

bool CSTRANSLATOR::translateFileAsSingleString(wchar_t* linesDelims)
{
    std::cout << "Traduction de: " << inputFile << std::endl;
    std::cout << "Vers: " << outputFile << std::endl;
    std::cout << "Langue cible: " << targetLang << std::endl << std::endl;

    if (GetFileAttributesA(inputFile.c_str()) == INVALID_FILE_ATTRIBUTES) {
        std::cout << "ERREUR: Fichier introuvable: " << inputFile << std::endl;
        return false;
    }
    
    std::cout << "Traduction en cours..." << std::endl << std::endl;
    
    // Lire le fichier d'entrée
    std::string line = readInputFileAsSingleString();

    if (!line.size()) 
    {
        std::cout << "ERREUR: Impossible de lire le fichier d'entrée" << std::endl;
        return false;
    }
    
    // Créer le fichier de sortie UTF-16 LE avec BOM
    std::wofstream outFile(outputFile, std::ios::binary);
    outFile.imbue(std::locale(std::locale(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));
    
    if (!outFile.is_open()) {
        std::cout << "ERREUR: Impossible de créer le fichier de sortie" << std::endl;
        return false;
    }
    
    // Écrire le BOM UTF-16 LE
    outFile.put(0xFEFF);
    
    std::string encodedText = urlEncode(line);
    std::string url = "https://api.mymemory.translated.net/get?q=" + 
                    encodedText + "&langpair=" + sourceLang + "|" + targetLang + "&de=azphil237@outlook.com";
    
    // Effectuer la requête
    std::string response = httpRequest(url);
    std::string translation = extractTranslation(response);
    
    if (!translation.empty() && translation != "null") {
        writeLineUtf16(outFile, translation);
    } else {
        std::cout << "Erreur de traduction, conservation du texte original" << std::endl;
        writeLineUtf16(outFile, line);
    }
    
    // Pause d'1 seconde
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    
    outFile.close();
    
    std::cout << std::endl << "Traduction terminee: " << outputFile << std::endl;
    std::cout << "Fichier encode en UTF-16 LE avec BOM" << std::endl;
    
    return true;
}


void CSJSONPARSER::skipWhitespace() 
{
    while (pos < jsonData.length() && 
            (jsonData[pos] == ' ' || jsonData[pos] == '\t' || 
            jsonData[pos] == '\n' || jsonData[pos] == '\r')) 
    {
        pos++;
    }
}

std::string CSJSONPARSER::parseString() 
{
    if (jsonData[pos] != '"') return "";
    pos++; // Skip opening quote
    
    std::string result;
    while (pos < jsonData.length() && jsonData[pos] != '"') 
    {
        if (jsonData[pos] == '\\' && pos + 1 < jsonData.length()) 
        {
            pos++; // Skip escape character
            switch (jsonData[pos]) 
            {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case '\\': result += '\\'; break;
                case '"': result += '"'; break;
                default: result += jsonData[pos]; break;
            }
        } 
        else 
        {
            result += jsonData[pos];
        }
        pos++;
    }
    
    if (pos < jsonData.length()) pos++; // Skip closing quote
    return result;
}
    
    
std::string CSJSONPARSER::extractValue(const std::string& key) 
{
    pos = 0;
    std::string searchKey = "\"" + key + "\"";
    
    size_t keyPos = jsonData.find(searchKey);
    if (keyPos == std::string::npos) return "";
    
    pos = keyPos + searchKey.length();
    skipWhitespace();
    
    if (pos >= jsonData.length() || jsonData[pos] != ':') return "";
    pos++; // Skip ':'
    skipWhitespace();
    
    if (pos >= jsonData.length()) return "";
    
    if (jsonData[pos] == '"') 
    {
        return parseString();
    }
    
    return "";
}
    
std::string CSJSONPARSER::createSimpleJson(const std::map<std::string, std::string>& data) 
{
    std::string json = "{";
    bool first = true;
    
    for (const auto& pair : data) 
    {
        if (!first) json += ",";
        json += "\"" + pair.first + "\":\"" + escapeString(pair.second) + "\"";
        first = false;
    }
    
    json += "}";
    return json;
}
    
std::string CSJSONPARSER::escapeString(const std::string& str) 
{
    std::string escaped;
    for (char c : str) 
    {
        switch (c) 
        {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}
    

/******************************************************************************************************************************************* */
    
// Définir des instructions générales pour l'agent
void CSAITRANSLATOR::setUserInstructions(const std::string& instructions) 
{
    userInstructions = instructions;
}

// Méthode principale de traduction avec configuration détaillée
std::string CSAITRANSLATOR::translate(const std::string& text, 
                        const std::string& sourceLang,
                        const std::string& targetLang,
                        const std::string& customInstructions,
                        const std::string& tone,
                        const std::string& domain,
                        bool preserveFormatting) {
    
    TranslationConfig config;
    config.sourceLanguage = sourceLang;
    config.targetLanguage = targetLang;
    config.customInstructions = customInstructions.empty() ? userInstructions : customInstructions;
    config.preserveFormatting = preserveFormatting;
    config.tone = tone;
    config.domain = domain;
    
    return performTranslation(text, config);
}

// Traduction avec instructions spécifiques
std::string CSAITRANSLATOR::translateWithInstructions(const std::string& text,
                                    const std::string& sourceLang,
                                    const std::string& targetLang,
                                    const std::string& instructions) {
    return translate(text, sourceLang, targetLang, instructions);
}

// Traduction contextuelle (pour des domaines spécifiques)
std::string CSAITRANSLATOR::translateContextual(const std::string& text,
                                const std::string& sourceLang,
                                const std::string& targetLang,
                                const std::string& context,
                                const std::string& domain) {
    
    std::string contextualInstructions = "Contexte: " + context + 
                                        ". Domaine: " + domain + 
                                        ". " + userInstructions;
    
    return translate(text, sourceLang, targetLang, contextualInstructions, "contextual", domain);
}

// Traduction avec préservation de style
std::string CSAITRANSLATOR::translateWithStyle(const std::string& text,
                                const std::string& sourceLang,
                                const std::string& targetLang,
                                const std::string& styleInstructions) {
    
    std::string fullInstructions = "Style requis: " + styleInstructions + ". " + userInstructions;
    return translate(text, sourceLang, targetLang, fullInstructions);
}


std::string CSAITRANSLATOR::performTranslation(const std::string& text, const TranslationConfig& config) 
{
    if (!hInternet) {
        return "Erreur: Agent non initialisé";
    }

    // Préparer le texte enrichi avec les instructions
    std::string enrichedText = prepareEnrichedText(text, config);
    
    // Créer la requête JSON
    std::map<std::string, std::string> jsonData;
    jsonData["q"] = enrichedText;
    jsonData["source"] = config.sourceLanguage;
    jsonData["target"] = config.targetLanguage;
    jsonData["format"] = config.preserveFormatting ? "html" : "text";
    
    if (!config.domain.empty() && config.domain != "general") {
        jsonData["domain"] = config.domain;
    }

    std::string jsonPayload = CSJSONPARSER::createSimpleJson(jsonData);
    
    // Effectuer la requête HTTP
    return sendHttpRequest("/translate", jsonPayload);
}

std::string CSAITRANSLATOR::prepareEnrichedText(const std::string& originalText, const TranslationConfig& config) 
{
    std::string enrichedText = originalText;
    
    // Ajouter les instructions au contexte si nécessaire
    if (!config.customInstructions.empty()) 
    {
        // Pour certains services, on peut préfixer le texte avec des instructions
        // Exemple: "[INSTRUCTION: Traduire en gardant un ton formel] " + originalText
        std::string instructionPrefix = "[INSTRUCTIONS: " + config.customInstructions + "] ";
        enrichedText = instructionPrefix + originalText;
    }
    
    return enrichedText;
}

std::string CSAITRANSLATOR::sendHttpRequest(const std::string& endpoint, const std::string& jsonData) 
{
    // Parser l'URL du serveur
    std::string hostname, path;
    int port;
    parseUrl(serverUrl, hostname, path, port);
    
    // Se connecter au serveur
    HINTERNET hConnect = InternetConnectA(hInternet,
                                            hostname.c_str(),
                                            port,
                                            nullptr, nullptr,
                                            INTERNET_SERVICE_HTTP,
                                            0, 0);
    
    if (!hConnect) {
        return "Erreur: Impossible de se connecter au serveur";
    }

    // Créer la requête
    std::string fullPath = path + endpoint;
    DWORD flags = useHttps ? INTERNET_FLAG_SECURE : 0;
    
    HINTERNET hRequest = HttpOpenRequestA(hConnect,
                                        "POST",
                                        fullPath.c_str(),
                                        nullptr,
                                        nullptr,
                                        nullptr,
                                        flags,
                                        0);

    std::string response;
    if (hRequest) {
        // Préparer les en-têtes
        std::string headers = "Content-Type: application/json\r\n";
        if (!apiKey.empty()) {
            headers += "Authorization: Bearer " + apiKey + "\r\n";
        }

        // Envoyer la requête
        BOOL result = HttpSendRequestA(hRequest,
                                        headers.c_str(),
                                        headers.length(),
                                        (LPVOID)jsonData.c_str(),
                                        jsonData.length());

        if (result) {
            // Lire la réponse
            response = readHttpResponse(hRequest);
            
            // Parser la réponse JSON pour extraire la traduction
            CSJSONPARSER parser(response);
            std::string translatedText = parser.extractValue("translatedText");
            
            if (!translatedText.empty()) {
                response = postProcessTranslation(translatedText);
            } else {
                // Essayer d'autres champs possibles selon le service
                translatedText = parser.extractValue("translation");
                if (!translatedText.empty()) {
                    response = postProcessTranslation(translatedText);
                } else {
                    std::string error = parser.extractValue("error");
                    response = error.empty() ? "Erreur: Réponse inattendue du service" : "Erreur: " + error;
                }
            }
        } else {
            response = "Erreur: Échec de l'envoi de la requête";
        }

        InternetCloseHandle(hRequest);
    } else {
        response = "Erreur: Impossible de créer la requête";
    }

    InternetCloseHandle(hConnect);
    return response;
}

std::string CSAITRANSLATOR::readHttpResponse(HINTERNET hRequest) 
{
    std::string response;
    char buffer[4096];
    DWORD bytesRead;

    while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) 
    {
        buffer[bytesRead] = '\0';
        response += buffer;
    }

    return response;
}

std::string CSAITRANSLATOR::postProcessTranslation(const std::string& translatedText) 
{
    std::string result = translatedText;
    
    // Nettoyer les instructions qui pourraient être restées dans la traduction
    size_t instructionStart = result.find("[INSTRUCTIONS:");
    if (instructionStart != std::string::npos) 
    {
        size_t instructionEnd = result.find("] ", instructionStart);
        if (instructionEnd != std::string::npos) 
        {
            result.erase(instructionStart, instructionEnd - instructionStart + 2);
        }
    }
    
    return result;
}

void CSAITRANSLATOR::parseUrl(const std::string& url, std::string& hostname, std::string& path, int& port) 
{
    std::string tempUrl = url;
    
    // Retirer le protocole
    if (tempUrl.find("https://") == 0) {
        tempUrl = tempUrl.substr(8);
        port = 443;
    } else if (tempUrl.find("http://") == 0) {
        tempUrl = tempUrl.substr(7);
        port = 80;
    } else {
        port = useHttps ? 443 : 80;
    }
    
    // Trouver le chemin
    size_t pathPos = tempUrl.find('/');
    if (pathPos != std::string::npos) {
        hostname = tempUrl.substr(0, pathPos);
        path = tempUrl.substr(pathPos);
    } else {
        hostname = tempUrl;
        path = "/";
    }
    
    // Gérer le port personnalisé
    size_t portPos = hostname.find(':');
    if (portPos != std::string::npos) {
        port = std::stoi(hostname.substr(portPos + 1));
        hostname = hostname.substr(0, portPos);
    }
}

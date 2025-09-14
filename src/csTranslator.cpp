#include "csTranslator.h"
#include <iomanip>

#pragma comment(lib, "wininet.lib")


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

// Parser JSON simple pour extraire translatedText
std::string CSTRANSLATOR::extractTranslation(const std::string& jsonResponse) 
{
    // Recherche simple de "translatedText":"..."
    std::string searchStr = "\"translatedText\":\"";
    size_t start = jsonResponse.find(searchStr);
    if (start == std::string::npos) return "";
    
    start += searchStr.length();
    size_t end = jsonResponse.find("\"", start);
    if (end == std::string::npos) return "";
    
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
                            encodedText + "&langpair=" + sourceLang + "|" + targetLang;
            
            // Effectuer la requête
            std::string response = httpRequest(url);
            std::string translation = extractTranslation(response);
            
            if (!translation.empty() && translation != "null") {
                n = translation.size();
                for(int i=0; i<n; i++)
                {
                    if(translation[i] == '\n')
                        translation[i] = '|';
                }
                writeLineUtf16(outFile, translation);
            } else {
                std::cout << "Erreur de traduction, conservation du texte original" << std::endl;
                writeLineUtf16(outFile, line);
            }
            
            // Pause d'1 seconde
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    outFile.close();
    
    std::cout << std::endl << "Traduction terminee: " << outputFile << std::endl;
    std::cout << "Fichier encode en UTF-16 LE avec BOM" << std::endl;
    
    return true;
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
                    encodedText + "&langpair=" + sourceLang + "|" + targetLang;
    
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

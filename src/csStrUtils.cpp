#include "csStrUtils.h"


std::vector<std::wstring> filePaths;

// Ajoute des fichiers � la liste
void AddFiles(HWND hListBox)
{
    OPENFILENAMEW ofn = {};
    wchar_t szFile[1024] = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hListBox;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All Files\0*.*\0";
    ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    if (GetOpenFileNameW(&ofn))
    {
        wchar_t* p = szFile + wcslen(szFile) + 1;
        if (*p == '\0')
        {
            filePaths.push_back(szFile);
            SendMessageW(hListBox, LB_ADDSTRING, 0, (LPARAM)szFile);
        }
        else
        {
            std::wstring folder = szFile;
            while (*p)
            {
                std::wstring fullPath = folder + L"\\" + p;
                filePaths.push_back(fullPath);
                SendMessageW(hListBox, LB_ADDSTRING, 0, (LPARAM)fullPath.c_str());
                p += wcslen(p) + 1;
            }
        }
    }
}

// Supprime les fichiers s�lectionn�s
void RemoveSelectedFiles(HWND hListBox)
{
    int count = (int)SendMessageW(hListBox, LB_GETSELCOUNT, 0, 0);
    if (count <= 0) return;
    std::vector<int> indices(count);
    SendMessageW(hListBox, LB_GETSELITEMS, (WPARAM)count, (LPARAM)indices.data());

    for (int i = count - 1; i >= 0; --i)
    {
        int idx = indices[i];
        wchar_t buf[512];
        SendMessageW(hListBox, LB_GETTEXT, idx, (LPARAM)buf);
        for (auto it = filePaths.begin(); it != filePaths.end(); ++it)
        {
            if (*it == buf)
            {
                filePaths.erase(it);
                break;
            }
        }
        SendMessageW(hListBox, LB_DELETESTRING, idx, 0);
    }
}

vector<wchar_t*> GetSelectedPathsFromListBox(HWND hListBox);
// Appelle la fonction de g�n�ration
void Generate(HWND hListBox, HWND hComboBox)
{
    int sel = (int)SendMessage(hListBox, LB_GETCURSEL, 0, 0);
    int modelIndex = (int)SendMessageW(hComboBox, CB_GETCURSEL, 0, 0);
    if (sel == LB_ERR || sel >= filePaths.size()) return;


    vector<wchar_t*> paths = GetSelectedPathsFromListBox(hListBox);

    for(int i=0; i<paths.size(); i++)
    {

    }
    /*
        std::string selectedFile = filePaths[sel];
        // Ici tu peux appeler ta fonction de g�n�ration
        std::ostringstream oss;
        oss << "G�n�ration pour :\n" << selectedFile << "\nMod�le : " << modelIndex;
        MessageBoxA(nullptr, oss.str().c_str(), "G�n�ration", MB_OK);*/
}


vector<wchar_t*> GetSelectedPathsFromListBox(HWND hListBox)
{
    int count = SendMessage(hListBox, LB_GETSELCOUNT, 0, 0);
    vector<wchar_t*> paths;

    if (count > 0)
    {
        std::vector<int> selectedIndices(count);
        SendMessage(hListBox, LB_GETSELITEMS, (WPARAM)count, (LPARAM)selectedIndices.data());

        for (int i = 0; i < count; ++i)
        {
            int index = selectedIndices[i];

            wchar_t buffer[256];
            SendMessage(hListBox, LB_GETTEXT, index, (LPARAM)buffer);

            paths.push_back(buffer);
            // Utilise le texte ici
            //wprintf("Item s�lectionn� : %s\n", buffer);
        }
    }
    return paths;
}

std::string opentextFile(const std::string& path)
{
    std::ifstream fichier(path);
    if (!fichier.is_open())
    {
        throw std::runtime_error("Cannot Open File : " + path);
    }

    // Lire tout le contenu dans une chaîne
    std::string contenu((std::istreambuf_iterator<char>(fichier)),
                        std::istreambuf_iterator<char>());

    return contenu;
}

std::string opentextFile(const std::wstring& path)
{
    // Convertir le chemin en string pour l'ouverture du fichier
    std::string cheminUtf8(path.begin(), path.end());

    // Ouvrir le fichier
    std::ifstream fichier(cheminUtf8, std::ios::binary);
    if (!fichier.is_open())
    {
        throw std::runtime_error("Impossible d'ouvrir le fichier");
    }

    // Lire le contenu
    std::string content((std::istreambuf_iterator<char>(fichier)),
                        std::istreambuf_iterator<char>());

    return content;
}

std::string lireFichierLigneParLigne(const std::string& cheminFichier)
{
    // Convertir le chemin en string pour l'ouverture
    std::string cheminUtf8(cheminFichier.begin(), cheminFichier.end());

    std::ifstream fichier(cheminUtf8);
    if (!fichier.is_open())
    {
        return "";
    }

    std::string resultat;
    std::string ligne;

    while (std::getline(fichier, ligne))
    {
        // Ajouter la ligne
        resultat += ligne;
        // Ajouter \r\n à la fin
        resultat += "\r\n";
    }

    return resultat;
}
std::wstring getFileText(const std::wstring& cheminFichier)
{
    // Convertir le chemin en string pour l'ouverture
    std::string cheminUtf8(cheminFichier.begin(), cheminFichier.end());


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

std::vector<std::wstring> chargerLignesUTF8(const char* nomFichier)
{
    std::ifstream fichier(nomFichier, std::ios::binary);
    std::vector<std::wstring> lignes;

    std::string ligne;
    while (std::getline(fichier, ligne))
    {
        int len = MultiByteToWideChar(CP_UTF8, 0, ligne.c_str(), -1, NULL, 0);
        if (len > 0)
        {
            std::wstring wligne(len, 0);
            MultiByteToWideChar(CP_UTF8, 0, ligne.c_str(), -1, &wligne[0], len);
            lignes.push_back(wligne);
        }
    }

    return lignes;
}

std::wstring getFileTextUTF8(const char* nomFichier)
{
    std::ifstream fichier(nomFichier, std::ios::binary);
    std::wstring lignes;

    std::string ligne;
    while (std::getline(fichier, ligne))
    {
        int len = MultiByteToWideChar(CP_UTF8, 0, ligne.c_str(), -1, NULL, 0);
        if (len > 0)
        {
            std::wstring wligne(len - 1, 0); // -1 pour ne pas inclure le caractère nul
            MultiByteToWideChar(CP_UTF8, 0, ligne.c_str(), -1, &wligne[0], len);
            // wcout<<wligne.c_str()<<"\n";
            lignes += wligne;
            lignes += L"\r\n";
        }
    }

    return lignes;
}

#include <windows.h>
#include <fstream>
#include <vector>
#include <string>

std::wstring ReadHTMLFileAsUTF16(const std::string& filePath)
{
    // Ouvrir le fichier en mode binaire
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        return L"Erreur : Impossible d'ouvrir le fichier.";
    }

    // Lire la taille du fichier
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Lire le contenu dans un vecteur de bytes
    std::vector<char> buffer(fileSize);
    if (!file.read(buffer.data(), fileSize))
    {
        return L"Erreur : Lecture du fichier échouée.";
    }

    // Détecter et ignorer le BOM UTF-8 si présent (EF BB BF)
    const char* contentStart = buffer.data();
    if (buffer.size() >= 3 &&
            static_cast<unsigned char>(buffer[0]) == 0xEF &&
            static_cast<unsigned char>(buffer[1]) == 0xBB &&
            static_cast<unsigned char>(buffer[2]) == 0xBF)
    {
        contentStart += 3;
    }

    // Convertir UTF-8 → UTF-16
    int utf16Length = MultiByteToWideChar(
                          CP_UTF8,
                          MB_ERR_INVALID_CHARS,  // Échoue si l'UTF-8 est invalide
                          contentStart,
                          -1,  // Taille automatique (jusqu'au NULL)
                          nullptr,
                          0
                      );

    if (utf16Length <= 0)
    {
        DWORD error = GetLastError();
        return L"Erreur : Conversion UTF-8 → UTF-16 échouée (Code " + std::to_wstring(error) + L")";
    }

    std::wstring utf16Text(utf16Length, L'\0');
    MultiByteToWideChar(
        CP_UTF8,
        0,
        contentStart,
        -1,
        utf16Text.data(),
        utf16Length
    );

    return utf16Text;
}

vector<wstring> splitWords(const wchar_t* text, const wchar_t* delims) 
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

vector<long> wordsToLong(vector<wstring> words)
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
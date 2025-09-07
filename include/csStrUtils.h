#ifndef CSSTRUTILS_H
#define CSSTRUTILS_H


#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include <fstream>
#include <stdexcept>
#include <codecvt>
#include <windows.h>

using namespace std;

void AddFiles(HWND hListBox);
void RemoveSelectedFiles(HWND hListBox);
void Generate(HWND hListBox, HWND hComboBox);

std::string opentextFile(const std::string& path);
//std::string opentextFile(const std::wstring& path);
std::string opentextFile(const std::wstring& path);
std::wstring normaliserRetoursLigne(const std::wstring& texte);
std::wstring normaliserRetoursLigne2(const std::wstring& texte) ;
std::string lireFichierLigneParLigne(const std::string& cheminFichier);
std::wstring getFileText(const std::wstring& cheminFichier);
std::wstring getFileTextUTF8(const char* nomFichier);
std::wstring ReadHTMLFileAsUTF16(const std::string& filePath);
vector<wstring> splitWords(const wchar_t* text, const wchar_t* delims=L" \t\n") ;
vector<long> wordsToLong(vector<wstring> words);
#endif // CSSTRUTILS_H

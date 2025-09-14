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

namespace CSSTRUTILS
{
std::wstring getFileText(const std::wstring& path);
vector<wstring> splitWords(const wchar_t* text, const wchar_t* delims=L" ") ;
vector<long> wordsToLong(vector<wstring> words);
wchar_t* makeWcharString(const wchar_t* _str);
}

#endif // CSSTRUTILS_H

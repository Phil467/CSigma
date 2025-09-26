#ifndef CSFILESMAN_H
#define CSFILESMAN_H

#include "csTypes.h"
#include <wchar.h>
#include "csStrUtils.h"

namespace CSFILESMAN
{
bool fileExists(const wchar_t* filename);
bool fileExists(const char* filename);
void __saveAppSizes();
void __getAppSizes();
void __setAppSizes();
void setSaveAppSizes(bool b);

void __saveAppTitles();
void __getAppTitles();
void __setAppTitles();
void setSaveAppTitles(bool b);

void __saveAppTips(wchar_t* path=0);
bool __getAppTips(wchar_t* path=0);
void __setAppTips();
void setSaveAppTips(bool b);

std::vector<std::wstring> getTxtFiles(const std::wstring& folderPath) ;
};

namespace CSLANGMAN
{
void translateTitle(int id, bool b);
void translateAppStrings();
void setViewLanguage(unsigned int idLang);

const wchar_t** getLanguagesW(int* langCount);
const wchar_t** getLanguageCodesW(int* langCount);

std::vector<std::wstring> getInstalledLanguages();
vector<int> getLanguagesCodesIds(vector<wstring> _langCodes);
wstring openUnsupportedLanguages();
wstring& getUnsupportedLanguages();
int getLanguageCodeId(const wchar_t* langCode);
const wchar_t* getViewLanguageCode();
bool getTranslationProcessStatus();
};

#endif // CSFILESMAN_H

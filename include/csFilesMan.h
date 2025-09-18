#ifndef CSFILESMAN_H
#define CSFILESMAN_H

#include "csTypes.h"
#include <wchar.h>
#include "csStrUtils.h"

namespace CSFILESMAN
{
bool fileExists(const wchar_t* filename);
void __saveAppSizes();
void __getAppSizes();
void __setAppSizes();
void setSaveAppSizes(bool b);

void __saveAppTitles();
void __getAppTitles();
void setSaveAppTitles(bool b);

void __saveAppTips(wchar_t* path=0);
bool __getAppTips(wchar_t* path=0);
void setSaveAppTips(bool b);
};

#endif // CSFILESMAN_H

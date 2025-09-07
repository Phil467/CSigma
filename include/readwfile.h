#pragma once

#if defined _WIN32 || defined __CYGWIN__
#ifdef BUILDING_READWFILE_DLL
#define READWFILE_API __declspec(dllexport)
#else
#define READWFILE_API __declspec(dllimport)
#endif
#else
#ifdef BUILDING_CSPARALLEL_DLL
#define READWFILE_API __attribute__ ((visibility ("default")))
#else
#define READWFILE_API
#endif
#endif

#ifndef READWFILE_H_INCLUDED
#define READWFILE_H_INCLUDED

std::wstring READWFILE_API readWFile(const wchar_t* filename);
std::wstring readWFile2(const char* file, int*lineNb=0);
std::wstring READWFILE_API charPtrtoWcharPtr(const char* str);
std::string READWFILE_API wcharPtrToCharPtr(const wchar_t* wstr);

#endif // READWFILE_H_INCLUDED

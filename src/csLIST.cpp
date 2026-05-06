#include "csList.h"
#include "csUtils.h"
#include <string.h>
#include <iostream>
#include <windows.h>


template<> void csLIST<char*>::moveCharPtr(int from, int to)
{
    char*data = csAlloc<char>(strlen((const char*)Array[from])+1);
    sprintf(data,"%s",Array[from]);

    if(from < to)
    {
        for(int i=from+1; i<=to; i++) 
        {
            int j = i-1;
            Array[j] = (char*)realloc(Array[j], strlen((const char*)Array[i])+1);
            sprintf(Array[j],"%s",Array[i]);
        }
        free(Array[to]);
        Array[to] = data;
    }
    else if(from > to)
    {
        for(int i=from-1; i>=to; i--) 
        {
            int j = i+1;
            Array[j] = (char*)realloc(Array[j], strlen((const char*)Array[i])+1);
            sprintf(Array[j],"%s",Array[i]);
        }
        free(Array[to]);
        Array[to] = data;
    }
}

template<> void csLIST<csLIST<POINT> >::duplicatePointList(int from, int to)
{
    if(arraySize)
    {
        int n = Array[from].size();
        csLIST<POINT> data;
        data.full(n, {0,0});
        for(int i=0; i<n; i++)
        {
            data[i] = Array[from][i];
        }
        placeAt(data, to);
    }
}

template<> char* csLIST<char>::toString(int from, int to)
{
    char* ret = 0;
    if(from < to)
    {
        int end = to + 1;
        if(to >= arraySize)
            end = arraySize;
        ret = (char*)malloc(end - from+1);
        sprintf(ret,"\0");
        char c[2];
        int j=0;
        for(int i=from; i<end; i++, j++)
        {
            ret[j] = Array[i];
        }
        ret[j] = '\0';
    }
    else
    {
        int end = from + 1;
        if(from >= arraySize)
            end = arraySize;
        ret = (char*)malloc(end - to+1);
        sprintf(ret,"\0");
        char c[2];
        int j=0;
        for(int i=end-1; i>=to; i--, j++)
        {
            ret[j] = Array[i];
        }
        ret[j] = '\0';
    }
    return ret;
}

template<> wchar_t* csLIST<wchar_t>::toStringW(int from, int to)
{
    wchar_t* ret = 0;
    if(from < to)
    {
        int end = to + 1;
        if(to >= arraySize)
            end = arraySize;
        ret = (wchar_t*)malloc((end - from+1)*sizeof(wchar_t));
        wsprintf(ret,L"\0");
        wchar_t c[2];
        int j=0;
        for(int i=from; i<end; i++, j++)
        {
            ret[j] = Array[i];
        }
        ret[j] = '\0';
    }
    else
    {
        int end = from + 1;
        if(from >= arraySize)
            end = arraySize;
        ret = (wchar_t*)malloc((end - to+1)*sizeof(wchar_t));
        wsprintf(ret,L"\0");
        wchar_t c[2];
        int j=0;
        for(int i=end-1; i>=to; i--, j++)
        {
            ret[j] = Array[i];
        }
        ret[j] = '\0';
    }
    return ret;
}

template<> void csLIST<char>::endString()
{
    if(arraySize >= memSize)
    {
        memSize += inc;
        Array = (char*)realloc(Array, memSize*sizeof(char));
    }
    Array[arraySize] = '\0';
}

template<> void csLIST<wchar_t>::endStringW()
{
    if(arraySize >= memSize)
    {
        memSize += inc;
        Array = (wchar_t*)realloc(Array, memSize*sizeof(wchar_t));
    }
    Array[arraySize] = '\0';
}


template<> csLIST<char> csLIST<char*>::toList(int id)
{
    int len = strlen(Array[id]);
    csLIST<char> list;
    list.initForce(len);
    for(int i=0; i<len; i++)
    {
        list[i] = Array[id][i];

    }
    list[len] = '\0';
    return list;
}
template<> csLIST<wchar_t> csLIST<wchar_t*>::toListW(int id)
{
    int len = wcslen(Array[id]);
    csLIST<wchar_t> list;
    list.initForce(len);
    for(int i=0; i<len; i++)
    {
        list[i] = Array[id][i];
    }
    list[len] = '\0'; // sans crainte car espace additionnel de 2 (variable _inc);
    return list;
}

template<class T> csLIST<T> csLIST<T>::breakList(int from, int to)
{
    csLIST<T> ret;
    ret.init(1);
    int begin, end; 
    if(from < to)
    {
        end = (to < arraySize) ? to+1 : arraySize;
        begin = (from < 0) ? 0 : from;
    }
    else
    {
        end = (from < arraySize) ? from+1 : arraySize;
        begin = (to < 0) ? 0 : to;
    }
    int len = end-begin;
    ret.placeTablePartAt(0, Array, begin, len);
    deleteTableAt(begin, len);

    return ret;
}

/*template<class T> int csLIST<T>::find(T data)
{
    int id = -1;
    for(int i=0; i<arraySize; i++)
    {
        if(Array[i] == data)
        {
            id = data;
            break;
        }
    }
    return id;
}*/
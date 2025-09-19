#include "csList.h"
#include "csUtils.h"
#include <string.h>
#include <iostream>
#include <windows.h>


template<> void csLIST<char*>::moveCharPtr(int from, int to)
{
    char*data = csAlloc<char>(strlen((const char*)DataTab[from])+1);
    sprintf(data,"%s",DataTab[from]);

    if(from < to)
    {
        for(int i=from+1; i<=to; i++) 
        {
            int j = i-1;
            DataTab[j] = (char*)realloc(DataTab[j], strlen((const char*)DataTab[i])+1);
            sprintf(DataTab[j],"%s",DataTab[i]);
        }
        free(DataTab[to]);
        DataTab[to] = data;
    }
    else if(from > to)
    {
        for(int i=from-1; i>=to; i--) 
        {
            int j = i+1;
            DataTab[j] = (char*)realloc(DataTab[j], strlen((const char*)DataTab[i])+1);
            sprintf(DataTab[j],"%s",DataTab[i]);
        }
        free(DataTab[to]);
        DataTab[to] = data;
    }
}

template<> void csLIST<csLIST<POINT> >::duplicatePointList(int from, int to)
{
    if(tabSize)
    {
        int n = DataTab[from].size();
        csLIST<POINT> data;
        data.full(n, {0,0});
        for(int i=0; i<n; i++)
        {
            data[i] = DataTab[from][i];
        }
        insertAt(data, to);
    }
}

template<> char* csLIST<char>::toString(int from, int to)
{
    char* ret = 0;
    if(from < to)
    {
        int end = to + 1;
        if(to >= tabSize)
            end = tabSize;
        ret = (char*)malloc(end - from+1);
        sprintf(ret,"\0");
        char c[2];
        int j=0;
        for(int i=from; i<end; i++, j++)
        {
            ret[j] = DataTab[i];
        }
        ret[j] = '\0';
    }
    else
    {
        int end = from + 1;
        if(from >= tabSize)
            end = tabSize;
        ret = (char*)malloc(end - to+1);
        sprintf(ret,"\0");
        char c[2];
        int j=0;
        for(int i=end-1; i>=to; i--, j++)
        {
            ret[j] = DataTab[i];
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
        if(to >= tabSize)
            end = tabSize;
        ret = (wchar_t*)malloc(end - from+1);
        wsprintf(ret,L"\0");
        wchar_t c[2];
        int j=0;
        for(int i=from; i<end; i++, j++)
        {
            ret[j] = DataTab[i];
        }
        ret[j] = '\0';
    }
    else
    {
        int end = from + 1;
        if(from >= tabSize)
            end = tabSize;
        ret = (wchar_t*)malloc(end - to+1);
        wsprintf(ret,L"\0");
        wchar_t c[2];
        int j=0;
        for(int i=end-1; i>=to; i--, j++)
        {
            ret[j] = DataTab[i];
        }
        ret[j] = '\0';
    }
    return ret;
}

template<> void csLIST<char>::endString()
{
    if(tabSize >= memSize)
    {
        memSize += inc;
        DataTab = (char*)realloc(DataTab, memSize*sizeof(char));
    }
    DataTab[tabSize] = '\0';
}

template<> void csLIST<wchar_t>::endStringW()
{
    if(tabSize >= memSize)
    {
        memSize += inc;
        DataTab = (wchar_t*)realloc(DataTab, memSize*sizeof(wchar_t));
    }
    DataTab[tabSize] = '\0';
}


template<> csLIST<char> csLIST<char*>::toList(int id)
{
    int len = strlen(DataTab[id]);
    csLIST<char> list;
    list.initForce(len);
    for(int i=0; i<len; i++)
    {
        list[i] = DataTab[id][i];

    }
    list[len] = '\0';
    return list;
}
template<> csLIST<wchar_t> csLIST<wchar_t*>::toListW(int id)
{
    int len = wcslen(DataTab[id]);
    csLIST<wchar_t> list;
    list.initForce(len);
    for(int i=0; i<len; i++)
    {
        list[i] = DataTab[id][i];
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
        end = (to < tabSize) ? to+1 : tabSize;
        begin = (from < 0) ? 0 : from;
    }
    else
    {
        end = (from < tabSize) ? from+1 : tabSize;
        begin = (to < 0) ? 0 : to;
    }
    int len = end-begin;
    ret.insertTablePartAt(0, DataTab, begin, len);
    deleteTableAt(begin, len);

    return ret;
}

/*template<class T> int csLIST<T>::find(T data)
{
    int id = -1;
    for(int i=0; i<tabSize; i++)
    {
        if(DataTab[i] == data)
        {
            id = data;
            break;
        }
    }
    return id;
}*/
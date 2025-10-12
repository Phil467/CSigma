#ifndef CSTYPES_H
#define CSTYPES_H

#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <csArithmetic/csARITHMETIC.h>

#define AUTO_TRANSF_LENGTH   1
#define AUTO_TRANSF_RECT   2

#define CSG_ENTITY_IMAGE 10
#define CSG_ENTITY_POLYGON 11
#define CSG_ENTITY_MESH 12
#define CSG_ENTITY_POLYLINE 13
#define CSG_ENTITY_POINT 14

#define INPUT_FORMAT_INTERGER 1
#define INPUT_FORMAT_POSITIVE_INTERGER 2
#define INPUT_FORMAT_NEGATIVE_INTERGER 3
#define INPUT_FORMAT_FLOAT 4
#define INPUT_FORMAT_POSITIVE_FLOAT 5
#define INPUT_FORMAT_NEGATIVE_FLOAT 6

using namespace std;

namespace CSTYPES
{
typedef struct
{
    unsigned char r,g,b;
    double a;
} CSRGBA;

typedef struct
{
    bool closeWhenMinimize;
    int delay;
} CSTIMER_PARAMS;

typedef struct 
{
    POINT outPos, inPos;
    SIZE size, outSize, inSize;
    COLORREF bkgColor;
    HDC dc, outDc, maskDc;
    HBITMAP bmp, outBmp, maskBmp;
    unsigned char* bits;
    bool show, eraseBkg;
} CSLOADED_IMAGE;

typedef struct TASKBAR_INFO
{
    RECT rect;
    bool autoHide;
    UINT edge;
};

typedef struct
{
    char type;
    long tid, group, gid;

}CSGRAPHIC_ENTITY;

typedef struct CSENTITY_ID_MAP
{
    vector<long> ids;
    SIZE size;
};

typedef struct
{
    int GradientType;
    CSRGBA Color1;
    CSRGBA Color2;
    CSRGBA ActiveColor;
    CSRGBA TitleColor1;
    CSRGBA TitleColor2;
    CSRGBA ActiveTitleColor;
    CSRGBA BorderColor1;
    CSRGBA BorderColor2;
    CSRGBA ActiveBorderColor;
    CSRGBA HightLightColor;
    float HightLightLevel;
    float ZoomLevel;
    char* Image1;
    char* Image2;
    SIZE Size;
    POINT Position;
    int ActiveImageIndex;
    unsigned int GradientNumber;
    unsigned int TimeStep;
    float ImageRotationAngle;
    RECT validArea;
    bool BackgroundGradient;
    bool BorderGradient;
    bool TitleGradient;
    bool ImageGradient;
    bool init;
} CSIMAGE_GRADIENT;

typedef struct
{
    int GradientType;
    CSRGBA Color;
    CSRGBA BorderColor;
    CSRGBA TitleColor;

    char* Image;
    unsigned int GradientNumber;
    unsigned int TimeStep;
    unsigned int MaxIter;
} CSBKG_CLICK_GRADIENT;


typedef struct
{
    bool Lockable;
    bool Locked;
    bool LockingActivated;
    bool ExternalUnlocking;//utile pour les boutons en groupes
    CSRGBA LockingColor;
    CSRGBA LockingTitleColor;
    CSRGBA LockingBorderColor;
    char* LockingImagePath;
    bool ExtBool;
} CSLOCKED_MODE;

typedef struct
{
    HDC dc;
    HBITMAP hbmp;
    BITMAP bm;
} CSGRAPHIC_CONTEXT_EXT;

typedef struct
{
    HDC dc;
    HBITMAP hbmp;
    SIZE sz;
} CSGRAPHIC_CONTEXT;

typedef struct
{
    RECT  TextRect;
    wchar_t* Text;
    wchar_t* Font;
    SIZE  FontSize;
    bool  Italic;
    bool  Underline;
    int   Orientation;
    DWORD  Bold;
    DWORD  Charset;
    CSRGBA  Color;
    RECT  Marging;
    int  Align;
    bool  Show;
    bool ShowEntierText;

    void setText(const wchar_t* str)
    {
        int len = wcslen(str);
        Text = (wchar_t*)realloc(Text, (len+1)*sizeof(wchar_t));
        //wsprintf(Text, L"%s\0", str);
        wcscpy_s(Text, len+1, str);
    }
    /*void setText(char* str)
    {
        Text = (wchar_t*)realloc(Text, (strlen(str)+1)*sizeof(wchar_t));
        wsprintf(Text, L"%s\0", str);
    }*/
   void clear()
   {
        free(Text);
        free(Font);
   }
} CSTEXT;

typedef struct BOOL_RECT
{
    bool l, t, r, b, tl, tr, bl, br;
};

typedef struct
{
    float v0, ang, g, cosa, tana;
    void Init()
    {
        cosa = cos(3.14159*ang/180);
        float sina = sin(3.14159*ang/180);
        tana = sina/cosa;
    }
    float Get(float t,float Tmax, float DX, float Xmin)
    {
        return DX*(3-2*t/Tmax)*t*t/(Tmax*Tmax) + Xmin;
    }
} POS_FUNC;

typedef struct
{
    int Type;
    RECT InitialRect;
    RECT FinalRect;
    int Time;
    int Count;
    bool Start;
    POS_FUNC PosFunc;
    POS_FUNC SizeFunc;
    RECT InstRect;
    bool EmergencyStop;
    bool waitUntilAnimEnds;
    HWND Range = HWND_TOP;
    bool Update = 0;
} AUTO_TRANSFORMATION;

template<class T> T* csAlloc(size_t n)
{
    T*t = (T*)malloc(n*sizeof(T));
    return t;
}

template<class T> T* csAlloc(size_t n, T init)
{
    T*t = (T*)malloc(n*sizeof(T));
    for(size_t i=0; i<n; i++)
    {
        t[i] = init;
    }
    return t;
}

typedef struct COEFS4
{
    float lcf, tcf, rcf, bcf;
};

typedef struct FLAGS4
{
    int lFlag,tFlag,rFlag,bFlag;
};

typedef struct CSBIND_GEOM_PARAMS
{
    int id;
    COEFS4 coef;
    FLAGS4 flag;
};

typedef struct BIND_DIM_GEOM_PARAMS
{
    int id;
    float cf;
    int flag;
};

typedef struct CSSYSCOMMAND_SECTION
{
    int SYSCOMMAND_SECTION, SYS_MIN, SYS_MAX, SYS_CLOSE;
};

typedef struct
{
    vector<CSTEXT> paragraph;
    vector<int> pSpace;
    RECT marg;
    bool updateGASize;
    bool view;
}CSDYNAMIC_SIMPLE_TEXT;

typedef struct
{
    bool bTLeft,bMLeft,bBLeft, bLTop,bMTop, bRTop;
    bool bTRight,bMRight,bBRight, bLBottom, bMBottom,bRBottom;
    bool bOther;
} POS_BOOL;

typedef struct
{
    vector<int> Ids;
    vector<int>* Ids_src;
    vector<RECT> Geometry;
    vector<POS_BOOL> Bpos;
    vector<CSDYNAMIC_SIMPLE_TEXT> text;
    unsigned int Delay;
    unsigned MouseHoverCount;
    unsigned MouseLeaveCount;
    bool Lock;
} TIPS_POPUP_PARAMS;

typedef struct
{
    float hmin, hmax;
    float vmin, vmax;
    float hZoom, vZoom;
    int marging;
    SIZE targetContextSize;
    POINT focus;
}CSZOOM_PARAMS;

typedef struct
{
    vector<wchar_t*> str;
    vector<unsigned long> secId;
    vector<unsigned long> tipId;
    vector<unsigned long> mlbId;

}REG_STRINGS;

class CSPARAARGS
{
    private:
    HWND hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    int id;
    void**paraArgs;
    int nbArgs;

    public:
    CSPARAARGS()
    {
        init();
    };
    void init()
    {
        paraArgs = 0;
        nbArgs = 0;
        //paraArgs = csAlloc<void**>(1);
    }
    void regArg(int _nbArgs, ...)
    {
        int a = nbArgs;
        nbArgs += _nbArgs;
        
        paraArgs = (void**)realloc(paraArgs, nbArgs*sizeof(void*));

        va_list adArgs ;
        va_start (adArgs, _nbArgs);
        for (int i=a ; i<nbArgs ; i++)
        {
            paraArgs[i] = va_arg (adArgs, void*) ;
        }
        va_end(adArgs);
    };
    void setProcParams(HWND _hwnd, UINT _msg, WPARAM _wp, LPARAM _lp, int _id)
    {
        hwnd = _hwnd; message = _msg; wParam = _wp; lParam = _lp; id = _id;
    }
    void**& getArgs()
    {
        return paraArgs;
    }
    void setArgs(void** args, int n)
    {
        paraArgs = args;
        nbArgs = n;
    }
    operator HWND()
    {
        return hwnd;
    };
    operator UINT()
    {
        return message;
    }
    operator WPARAM()
    {
        return wParam;
    }
    operator LPARAM()
    {
        return lParam;
    }
    operator int()
    {
        return id;
    }
    void* operator [](int id)
    {
        return paraArgs[id];
    }
};

class CSARGS
{
public:

    CSARGS(unsigned int nArgs=0);
    void init(unsigned int _nbArgs);
    void* getArg(unsigned int i);
    void setArg(unsigned int i, void*arg);
    void setArg_int(unsigned int i, int arg);
    void setArgNumber(unsigned int nbArgs);
    void setProcParams(HWND* hwnd, UINT* msg, WPARAM* wp, LPARAM* lp, int* id);
    void setProcParams(void* hwnd, void* msg, void* wp, void* lp, void*id);
    void setId(void*id);
    void setHwnd(void*hwnd);
    void* getHwnd();
    void* getMessage();
    void* getWParam();
    void* getLParam();
    void* getId();
    void* getLRESULT();
    int regArg(void*arg);
    void  regArg(...);
    void  regArg(void**args, int nbArgs);
    void  clear();
    int getArgNumber();

    operator HWND()
    {
        return *((HWND*)Args[0]);
    };
    operator UINT()
    {
        return *((UINT*)Args[1]);
    };
    operator WPARAM()
    {
        return *((WPARAM*)Args[2]);
    };
    operator LPARAM()
    {
        return *((LPARAM*)Args[3]);
    };
    operator int()
    {
        return *((int*)Args[4]);
    };
    void* operator[](unsigned int i)
    {
        return Args[i+6];
    };
    operator int*()
    {
        return ((int*)Args[4]);
    };

private:
    void** Args;
    int nbArgs;
    LRESULT* lr;

    int argIncrement;
};
}

using namespace CSTYPES;


typedef struct
{
    int idSection, idText, idUp, idDown, idValidate;
    bool useMinBound, useMaxBound;
    CSARITHMETIC::csRNUMBER step, min, max;

    void setMinBound(const char* num);
    void setMaxBound(const char* num);

}CS_NUMERIC_INCREMENTER_PARAMS;

typedef struct
{
    int idSection, idText, idUp, idDown, idValidate;
    bool loopStyle;
    long step, currentItem;
    vector<wstring> item;
    vector<bool> status;
    
    void newItem(const wchar_t* title, bool status=1);

}CS_STRING_INCREMENTER_PARAMS;

typedef struct
{
    char* minBound, *maxBound;
}CS_RICHEDIT_NUMERIC_BOUNDS;

typedef struct
{
    HICON smallIcon, bigIcon;
    RECT rectSmall;
    bool centered;
}CSAPP_ICON;

template<class T> vector<T> newVector()
{
    vector<T> v;
    return v;
};

typedef struct
{
    wchar_t ch;
    LPSIZE psize;
    int cumulLen;
    int topLocalMarging;
    int rightSpace;
    bool isDash;
    wchar_t* fontName;
    COLORREF color;
    COLORREF bkgColor;
    bool noBkg;

    DWORD itatic;
    DWORD underlined;
    DWORD weight;

    HFONT hf;
    
}CSTEXT_CHAR;

typedef struct
{
    int idEnd, width, height;
    bool isDash;
}CSTEXT_LINE_PARAMS;
    
typedef struct
{
    vector<CSTEXT_CHAR> chr;
    vector<CSTEXT_LINE_PARAMS> line;
    wchar_t* dfltFont;
    COLORREF dfltColor;
    wchar_t* vowelList = L"aeiouyAEIOUY";// 129-154, 160-163, 210-212, 224-229, 233-237
    int vowelListLen = 12;


    void getLatineVowels()
    {
        vowelList = csAlloc<wchar_t>(57);
        wcscpy_s(vowelList, 13, L"aeiouyAEIOUY");
        for(int i=129, j=13; i<238; i++, j++)
        {
            if((i>=129 && i<155) || (i>=160 && i<164) || (i>=210 && i<213) || (i>=233 && i<238))
            {
                vowelList[j] = (wchar_t)i;
            }
        }
        vowelListLen = 56;
    };

    bool isCharInVowelList(wchar_t ch)
    {
        for(int i=0; i<vowelListLen; i++)
        {
            if(ch == vowelList[i])
            {
                return 1;
            }
        }
        return 0;
    }

    int canDashBeInserted(int pos, int& cumulLen)
    {
        if(pos > 0)
        {
            for(int i=pos; i>0; i--)
            {
                bool b1 = isCharInVowelList(chr[i].ch);
                bool b2 = isCharInVowelList(chr[i-1].ch);
                if((b1&&b2) || (!b1&&!b2))
                {
                    cumulLen += chr[i].psize->cx;
                    return i;
                }
                cumulLen += (chr[i].psize->cx + chr[i].rightSpace);
            }
        }
        else
            return 0;
        return 0;
    }

    void getLines(int len, int GAWidth)
    {
        if(len > 0)
        {
            int width = 0;
            int l = 0;
            for(int i=0; i<len; i++)
            {
                l = (chr[i].psize->cx + chr[i].rightSpace);
                width += l;
                if(width > GAWidth)
                {
                    width -= l;
                    i--;
                    int clen = 0;
                    int pos = canDashBeInserted(i, clen);
                    if(pos)
                    {
                        
                    }
                }
            }
        }
    }

}CSTEXT_PARAGRAPH;


typedef CSIMAGE_GRADIENT CSMOUSE_HOVER_EFFECT;
typedef BOOL_RECT CSRESIZE_EDGE;
#endif // CSTYPES_H

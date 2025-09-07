#include "csTypes.h"

CSARGS::CSARGS(unsigned int _nbArgs)
{
    init(_nbArgs);
};

void CSARGS::init(unsigned int _nbArgs)
{
    Args = 0;
    lr = (LRESULT*)malloc(sizeof(LRESULT));
    *lr = 0;
    nbArgs = _nbArgs;
    argIncrement = 0;
    Args = (void**)malloc(sizeof(void*)*(nbArgs+6));
    Args[5] = (void*)lr;
}
void* CSARGS::getArg(unsigned int i)
{
    return Args[i+6];
}
void* CSARGS::getHwnd()
{
    return Args[0];
}
void* CSARGS::getMessage()
{
    return Args[1];
}
void* CSARGS::getWParam()
{
    return Args[2];
}
void* CSARGS::getLParam()
{
    return Args[3];
}
void* CSARGS::getLID()
{
    return Args[4];
}
void* CSARGS::getLRESULT()
{
    return Args[5];
}
void CSARGS::setProcParams(HWND* hwnd, UINT* msg, WPARAM* wp, LPARAM* lp, int*id)
{
    Args[0]=(void*)hwnd;
    Args[1]=(void*)msg;
    Args[2]=(void*)wp;
    Args[3]=(void*)lp;
    Args[4]=(void*)id;
    //Args[5]=(void*)&lr;
}

int CSARGS::getArgNumber()
{
    return nbArgs;
}

void CSARGS::setArg(unsigned int i, void*arg)
{
    //free(Args[i+4]);
    Args[i+6]=arg;
}
void CSARGS::setId(void*id)
{
    //free(Args[i+4]);
    Args[4]=id;
}
void CSARGS::setHwnd(void*hwnd)
{
    //free(Args[i+4]);
    Args[0]=hwnd;
}
void CSARGS::setArg_int(unsigned int i, int arg)
{
    static int a = 0;
    a=arg;
    Args[i+6]=(void*)&a;
}

void CSARGS::setArgNumber(unsigned int _nbArgs)
{
    nbArgs = _nbArgs;
    int m = nbArgs + 6;
    Args = (void**)realloc(Args, m*sizeof(void*));
}
/*void CSARGS::setArg(unsigned int i, void*arg)
{
    Args[i+4][0]=arg[0];
}*/

void CSARGS::regArg(...)
{
    va_list adArgs ;
    void* parv;
    va_start (adArgs, 0);
    for (int i=0 ; i<nbArgs ; i++)
    {
        parv = va_arg (adArgs, void*) ;
        Args[i+6]=parv;
    }
    va_end(adArgs);
    argIncrement += nbArgs;

}

int CSARGS::regArg(void*arg)
{
    if(nbArgs > argIncrement + 1)
    {
        Args[argIncrement+6] = arg;
        argIncrement += 1;
    }
    else
    {
        std::cout<<"Warning : Args number increased.\n";
        argIncrement += 1;
        nbArgs += 1;
        Args = (void**)realloc(Args, nbArgs*sizeof(void*));
        Args[argIncrement-1] = arg;
    }
    return argIncrement-1;
}

void CSARGS::regArg(void**args, int nbArgs)
{
    int id=0;
    for (int i=0 ; i<nbArgs ; i++)
    {
        Args[i+6]=args[i];
    }
    argIncrement = nbArgs;
}

void CSARGS::clear()
{
    free(Args);
    Args = 0;
    nbArgs = 0;
    argIncrement = 0;
    free(lr);
}

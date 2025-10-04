#include "csMenu.h"
#include "csUtils.h"
#include "csScrollBar.h"
#include "csUIControls.h"

extern vector<HWND> SECTION;
extern vector<CSTEXT> TITLE;
extern vector<RECT> RECTWND;
extern vector<RECT> RECTCL;
extern vector<RECT> RECTPARREF;
extern vector<vector<WPARAM>> sectionMouseHook;

void lastMenuItemRepos(CSARGS Args);
void resizeMenu(CSARGS Args);
void showHideLbm(CSARGS Args);

extern float dimFact;

void initLbm(CSLISTBOXMIN*& lbm, int nbItem, wchar_t*defltTitle);
void hideMenuContainer(CSPARAARGS pArgs);

CSLISTBOXMIN* lastlbm;

CSMENU::CSMENU(int _idp, RECT r, bool _autoResize)
{
    autoResize = _autoResize;
    bLastMenuItemRepos = csAlloc<bool>(1,0);
    idp = _idp;
    id = CSUIMAN::createSection(idp, r,  RGB(15,15,15), {0,0,0,0});
    CSBIND_GEOM_PARAMS bd = {id, {-0.5,0,0.5,0}, {BIND_DEST_LEFT_EDGE,0,BIND_DEST_LEFT_EDGE,0}};
    CSUIMAN::bindGeometry(_idp, bd);
 
    if(autoResize)
        CSUIMAN::addAction(id, resizeMenu, 3, &idSection, &idp, bLastMenuItemRepos);

    textPar = CSTEXT{.Font=L"Segoe UI", .FontSize=16, .Color={150,150,150},
                                .Marging={32/dimFact,-5}, .Align = CS_TA_CENTER_LEFT, .Show=1};
    
    int i;
    idPopup.push_back(i=CSUIMAN::createSection(0, {0,0,200,300},  RGB(100,100,100), {0}, 0, 0, 0));
    SetTimer(SECTION[i], 0, 20, 0);

    //CSUIMAN::newMouseHook(i);
    csGraphics::setGraphicAreaColor(i, {30,30,30}, {0});
    csGraphics::setGraphicAreaSize(i, {200*dimFact,300*dimFact});
    csGraphics::updateGraphicArea(i, 1);
    sectionMouseHook[i].push_back(0);
    
        

}

int CSMENU::getId()
{
    return id;
}

int CSMENU::getIdParent()
{
    return idp;
} 

int CSMENU::getIdButton(int idMenuItem)
{
    if(idMenuItem < idSection.size())
    {
        return idSection[idMenuItem];
    }
    return -1;
}

void CSMENU::setTextParams(CSTEXT _textPar)
{
    textPar = _textPar;
}

int CSMENU::newGroup(const wchar_t* title, const wchar_t* iconPath1, const wchar_t* iconPath2)
{
    int BUTTON = CSUIMAN::createSection(id, {0,0,0,RECTCL[id].bottom-1},  RGB(30,30,30), {0,0,0,0});
    CSUIFX::setImageGradient(BUTTON, (char*)wcharPtrToCharPtr(iconPath1).c_str(), (char*)wcharPtrToCharPtr(iconPath2).c_str(), {4,4}, {20,20}, 0.1, 2, 2);
    CSUIFX::setBackgroundColorGradient(BUTTON, {5,5,5}, {20,20,20}, 2, 2);
    CSUIFX::setBackgroundColorClickEffect(BUTTON, {10,10,10});
    CSUIFX::setTitleColorGradient(BUTTON, {220,200,100}, {200,200,200}, 2, 2);
    CSUIFX::setTitleColorClickEffect(BUTTON, {220,220,120});
    CSUIFX::setBorderColorGradient(BUTTON, {10,10,10}, {25,25,25}, 2, 2);
    CSUIFX::setBorderColorClickEffect(BUTTON, {20,20,20});
    CSUIMAN::setBorderThick(BUTTON, 2);
    CSUIMAN::setTitle(BUTTON, CSTEXT{.Text=(wchar_t*)title, .Font=textPar.Font, .FontSize=textPar.FontSize, .Color=textPar.Color,
                            .Marging=textPar.Marging, .Align = textPar.Align, .Show=1});

    CSUIMAN::autoFitToTitle(BUTTON, 10);

    static int ID_ACTION = 0;

    if(idSection.size() >= 1)
    {
        //CSUIMAN::removeLastAction(idSection[idSection.size()-1]);
        CSUIMAN::removeAction(idSection[idSection.size()-1], ID_ACTION);
    }

    CSUIMAN::joinPopup(BUTTON, idPopup[0], {0,0,160,255}, (POS_BOOL){.bLBottom=1}, 1, 1,0, &idSection);
    idSection.push_back(BUTTON);
    int id = idSection.size()-1;

    int lbmId = newlbm(&idPopup[0], &idSection[id], 8, (wchar_t*)(wstring(L"Menu-")+to_wstring(lbm.size())+L"-").c_str());
    if(lastlbm) lastlbm->hide();
    lbm[lbmId]->show();
    lastlbm = lbm[lbmId];

    ID_ACTION = CSUIMAN::addAction(BUTTON, lastMenuItemRepos, 1, bLastMenuItemRepos);
    CSUIMAN::addAction(BUTTON, showHideLbm, 1, lbm[lbmId]);

    
    

    return id;
}

extern vector<vector<void(*)(CSARGS)>> GROUPED_EVENTS_FUNC;
extern vector<vector<CSARGS>> GROUPED_EVENTS_ARGS;

int CSMENU::newlbm(int* idp, int* idSec, int nbItem, wchar_t*defltTitle)
{
    lbm.push_back(csNewMinimalListBoxPtr(idp, 100, 1000));
    csLIST<RECT> lr;
    lbmItemRectLock.push_back(lr);
    int l = lbm.size()-1;
    
    lbmItemRectLock[l].insertEnd(RECT{0,10,100,100});
    CSPARAARGS* pargs = csAlloc<CSPARAARGS>(1);
    lpargs.push_back(pargs);
    pargs->init();
    pargs->regArg(3,lbmItemRectLock[l].getTable(), csAlloc(1,lbmItemRectLock[l].size()), csAlloc(1,*idSec));
    lbm[l]->addExternalFunction(hideMenuContainer, pargs);

    initLbm(lbm[l], nbItem, defltTitle);

    //lbmItemRectLock[l].push_back(lbm[l]->getItemRectPtr(0));

    
    /*CSARGS& args2 = GROUPED_EVENTS_ARGS[*idp][0];
    pargs->setProcParams(args2.getHwnd(), args2.getMessage(), args2.getWParam(), args2.getLParam(), args2.getId());*/

    return l;
}

void initLbm(CSLISTBOXMIN*& lbm, int nbItem, wchar_t*defltTitle)
{
    lbm->setDefaultFont(L"calibri",{16,0});
    lbm->setItemAlign(CS_ALIGN_VERTICAL);
    lbm->setMarging({8,8});
    lbm->setOffset({0,1});
    lbm->setDefaultSize({150*dimFact, 30*dimFact});
    lbm->setDefaultTitleColors(RGB(0,0,0), RGB(0,0,0), RGB(0,0,0), RGB(100,100,100));
    lbm->setDefaultBackgroundColors(RGB(120,120,120), RGB(140,140,140), RGB(120,120,120), RGB(100,100,100));
    lbm->setIconSize(0,{30,30});

    lbm->setIcon(0, L"img\\menulogo.bmp",L"img\\menulogo2.bmp", L"img\\menulogo2.bmp", L"img\\menulogo2.bmp");
    lbm->setMaxTextWidth(120*dimFact);
    lbm->setDefaultTitle(defltTitle);
    lbm->newItem(0,nbItem,0);

    lbm->setActiveItem(0);
    lbm->create();
}

bool CSMENU::newItem(vector<int> _hierarchy, wchar_t* title, wchar_t*iconPath1, wchar_t*iconPath2)
{
    int n = _hierarchy.size();
    if(n < 1) return 0;

    //if(_hierarchy[0] >= idSection.size()) return 0;

    vector<vector<int>> v = hierarchy;
    vector<int> ids;
    for(int i=0; i<v.size(); i++) ids.push_back(i);

    n-=1;
    int pos = 0;
    for(int i=0; i<n; i++)
    {
        int m = v.size();
        for(int j=pos; j<m; j++)
        {
            int o = v[j].size();
            if(o <= i)
            {
                continue;
            }
            
            if(v[j][i] == _hierarchy[i]) 
            {
                v.push_back(v[j]);
                ids.push_back(ids[j]);
            }
        }
        pos = m;
    }

    vector<int> vlast = v[pos-1];
    int idLast = ids[pos-1];

    int m = std::min(size_t(n), vlast.size()), i;
    
    for(i=0; i<m; i++)
    {
        if(vlast[i] != _hierarchy[i]) break;
    }

    vector<int>vh;
    for(int k=0; k<i; k++)
        vh.push_back(_hierarchy[k]);
    for(int k=i; k<m; k++)
    {
        /*lbm.push_back(csNewMinimalListBoxPtr(&idPopup[k], 100, 1000));
        lbmItemRectLock.push_back(vector<RECT*>());
        idLast = lbm.size()-1;
        initLbm(lbm[idLast], _hierarchy[k]+1);*/
        
        idLast = newlbm(&idPopup[k], &idSection[_hierarchy[0]], _hierarchy[k]+1);
        hierarchy.push_back(vh);
        lbmOfHierarchy.push_back(&lbm[idLast]);
        vh.push_back(_hierarchy[k]);
    }

    for(int k=vlast.size(); k<n; k++)
    {
        int j = idPopup.size();
        idPopup.push_back(CSUIMAN::createSection(0, {0,0,200,300},  RGB(15,15,15), {0,0,0,0}, 0, 0, 0));
        SetTimer(SECTION[j], 0, 20, 0);
        /*lbm.push_back(csNewMinimalListBoxPtr(&idPopup[j], 100, 1000));
        lbmItemRectLock.push_back(vector<RECT*>());
        hierarchy.push_back({k,_hierarchy[k]});
        idLast = lbm.size()-1;
        lbmOfHierarchy.push_back(&lbm[idLast]);
        initLbm(lbm[idLast], _hierarchy[k]+1);*/
        
        idLast = newlbm(&idPopup[k], &idSection[_hierarchy[0]], _hierarchy[k]+1);
        hierarchy.push_back(vh);
        lbmOfHierarchy.push_back(&lbm[idLast]);
        vh.push_back(_hierarchy[k]);
    }

    if(lbm[idLast]->getSize() <  _hierarchy[n])
    {
        lbm[idLast]->newItem(0,_hierarchy[n]+1-lbm[idLast]->getSize(), 0);
        lbm[idLast]->setTitle(_hierarchy[n], title);
        //lbm[idLast]->u
    }
    else
    {
        lbm[idLast]->setTitle(_hierarchy[n], title);
    }
    
}

void lastMenuItemRepos(CSARGS Args)
{

    if((UINT)Args == WM_SIZE || (UINT)Args == WM_MOVE)
    {
        int id = (int)Args;
        bool* bLastMenuItemRepos = (bool*)Args[0];
        *bLastMenuItemRepos = 1;
    }
}

void showHideLbm(CSARGS Args)
{
    static int delay = 8, count = 0;
    static bool b = 0;
    if((UINT)Args == WM_MOUSEHOVER)
    {
        CSLISTBOXMIN* lbm = (CSLISTBOXMIN*)Args[0];
        lbm->show();
        lbm->update();
        if(lastlbm && lbm != lastlbm) lastlbm->hide();
        lastlbm = lbm;

        count = 0;
        b = 1;
    }
    if((UINT)Args == WM_TIMER)
    {
        if(b)
        {
            count++;
            if(count == delay)
            {
                lastlbm->update();
                //InvalidateRect(HWND(Args), 0, 1);
                count = 0;
                b = 0;
            }
        }

    }
}

extern vector<int> autoSizeFromTitle;
extern vector<bool> autoSizeComplete;

void resizeMenu(CSARGS Args)
{
    static int count = 1, a=0, count1 = 0;

    UINT msg = (UINT)Args;
    int id = (int)Args;

    if(msg == WM_TIMER )
    {
//cout<<<<"  "<<autoSizeFromTitle[id]<<"     ..............\n";
        vector<int> idSection = *(vector<int>*)Args[0];
        int n = idSection.size();

        bool* bLastMenuItemRepos = (bool*)Args[2];
        
        for(count = 1; count<n; count++)
        {
            int idPrev = idSection[count-1];
            int i = idSection[count];
            RECT r1, r2;
            GetWindowRect(SECTION[idPrev], &r1);
            GetWindowRect(SECTION[i], &r2);
            
            if(r1.right != r2.left && (!autoSizeFromTitle[id] || (autoSizeFromTitle[id] && autoSizeComplete[id])))
            {
                SetWindowPos(SECTION[i], 0, r1.right-RECTWND[id].left, r1.top-RECTWND[id].top, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
                a = 1;
                autoSizeComplete[i] = 0;
            }
        }
        if(n && a && *bLastMenuItemRepos)
        {
            //if(count1 == 5)
            {
                int i = idSection[n-1];
                //GetWindowRect(SECTION[i], &RECTWND[i]);
                RECTPARREF[i] = CSUTILS::rectInParentRef(i);
                int diff = abs(RECTPARREF[i].right - RECTCL[id].right);
                if(diff > 3)
                {
                    int idp = *(int*)Args[1];

                    SetWindowPos(HWND(Args), 0, (RECTCL[idp].right-RECTPARREF[i].right)/2, RECTPARREF[id].top, RECTPARREF[i].right, RECTCL[id].bottom, SWP_NOZORDER);
                }
                a = 0;
                *bLastMenuItemRepos = 0;

                //count1 = 0;
            }

            //count1++;
        }

        autoSizeComplete[id] = 0;
    }

}

void hideMenuContainer(CSPARAARGS pArgs)
{
    int id = (int)pArgs;
    HWND hwnd = (HWND)pArgs;
    if((UINT)pArgs == WM_TIMER && CSUIMAN::getMouseHook(id,0) && IsWindowVisible(hwnd))
    {
        RECT* rlock = (RECT*)pArgs[0];
        int n = *(int*)pArgs[1];
        int ids = *(int*)pArgs[2];
        POINT p, p0;
        GetCursorPos(&p);
        p0 = p;
        ScreenToClient(hwnd, &p);

        bool b = 0;
        for(int i=0; i<n; i++)
        {
            if(PtInRect(&rlock[i], p))
            {
                CSUIMAN::printRect(rlock[i], "r = ");
                b = 1;
                break;
            }
        }

        if(!b && !PtInRect(&RECTWND[ids], p0)) 
        {
            ShowWindow(hwnd,0);
        }
        
        CSUIMAN::resetMouseHook(id,0);
    }
}
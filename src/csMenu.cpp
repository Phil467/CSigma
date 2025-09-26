#include "csMenu.h"
#include "csUtils.h"

extern vector<HWND> SECTION;
extern vector<CSTEXT> TITLE;
extern vector<RECT> RECTWND;
extern vector<RECT> RECTCL;
extern vector<RECT> RECTPARREF;

void lastMenuItemRepos(CSARGS Args);
void resizeMenu(CSARGS Args);

extern float dimFact;

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
    
    /*idPopup.push_back(CSUIMAN::createSection(0, {0},  RGB(15,15,15), {0,0,0,0}, 0, 0, 0));
    lbm.push_back(csNewMinimalListBoxPtr(&idPopup[0], 100, 1000));*/

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

    if(idSection.size() > 1)
    {
        CSUIMAN::removeLastAction(idSection[idSection.size()-1]);
    }
    CSUIMAN::addAction(BUTTON, lastMenuItemRepos, 1, bLastMenuItemRepos);

    idSection.push_back(BUTTON);

    return idSection.size()-1;
}

void initLbm(CSLISTBOXMIN*& lbm, int nbItem)
{
    lbm->setDefaultFont(L"calibri",{14,0});
    lbm->setItemAlign(CS_ALIGN_VERTICAL);
    lbm->setOffset({0,0});
    lbm->setDefaultSize({150*dimFact, 15*dimFact});
    //lbm->setIconSize(0,{26,26});

    lbm->setIcon(0, L"img\\langIcon2.bmp",L"img\\langIcon.bmp", L"img\\langIcon.bmp", L"img\\langIcon.bmp");
    lbm->setMaxTextWidth(120*dimFact);
    lbm->setDefaultTitle(L"Item");
    lbm->newItem(0,nbItem,0);

    lbm->setActiveItem(0);
    lbm->create();
}

bool CSMENU::newItem(vector<BYTE> _hierarchy, wchar_t* title, wchar_t*iconPath1, wchar_t*iconPath2)
{
    int n = _hierarchy.size();
    if(n < 2) return 0;

    if(_hierarchy[0] >= idSection.size()) return 0;

    vector<vector<int>> v = hierarchy;
    vector<int> ids;
    for(int i=0; i<v.size(); i++) ids.push_back(i);

    n-=1;
    int pos = 0;
    for(int i=1; i<n; i++)
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
    
    for(i=1; i<m; i++)
    {
        if(vlast[i] != _hierarchy[i]) break;
    }

    for(int k=i; k<m; k++)
    {
        lbm.push_back(csNewMinimalListBoxPtr(&idPopup[k], 100, 1000));
        hierarchy.push_back({k,_hierarchy[k]});
        idLast = hierarchy.size()-1;
        lbmOfHierarchy.push_back(&lbm[idLast]);
        initLbm(lbm[idLast], _hierarchy[k]+1);
        //lbm[idLast]->setTitle(_hierarchy[k], title);
    }

    for(int k=vlast.size(); k<n; k++)
    {
        int j = idPopup.size()-1;
        idPopup.push_back(CSUIMAN::createSection(0, {0},  RGB(15,15,15), {0,0,0,0}, 0, 0, 0));
        lbm.push_back(csNewMinimalListBoxPtr(&idPopup[j], 100, 1000));
        hierarchy.push_back({k,_hierarchy[k]});
        idLast = hierarchy.size()-1;
        lbmOfHierarchy.push_back(&lbm[idLast]);
        initLbm(lbm[idLast], _hierarchy[k]+1);
        //lbm[idLast]->setTitle(_hierarchy[k], title);
    }

    if(lbm[idLast]->getSize() <  _hierarchy[n])
    {
        lbm[idLast]->newItem(0,_hierarchy[n]+1-lbm[idLast]->getSize(), 0);
        lbm[idLast]->setTitle(_hierarchy[n], title);
        //lbm[idLast]->u
    }
    else
    {
        lbm[idLast]->newItem(title,1,0);
        lbm[idLast]->moveItem(lbm[idLast]->getSize()-1, _hierarchy[n]);
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

void resizeMenu(CSARGS Args)
{
    static int count = 1, a=0, count1 = 0;

    UINT msg = (UINT)Args;

    if(msg == WM_TIMER)
    {
        int id = (int)Args;

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
            
            if(r1.right != r2.left)
            {
                SetWindowPos(SECTION[i], 0, r1.right-RECTWND[id].left, r1.top-RECTWND[id].top, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
                a = 1;
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


    }

}

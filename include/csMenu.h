#ifndef CSMENU_H
#define CSMENU_H

#include "csSection.h"
#include "csListBoxMin.h"

class CSMENU
{
    public:
        CSMENU(int idp, RECT r, bool autoResize);
        int getId();
        int getIdParent();
        int getIdButton(int idMenuItem);
        int newGroup(const wchar_t* title, const wchar_t* iconPath1, const wchar_t* iconPath2);
        bool newItem(vector<BYTE> _hierarchy, wchar_t* title, wchar_t*iconPath1, wchar_t*iconPath2);
        void setTextParams(CSTEXT textPar);
        //virtual ~CSMENU();

    protected:

    private:
        int id;
        int idp;
        vector<int> idPopup;
        bool autoResize;
        vector<int> idSection;
        CSTEXT textPar;
        bool* bLastMenuItemRepos;
        vector<CSLISTBOXMIN*> lbm;
        vector<CSLISTBOXMIN**> lbmOfHierarchy;
        vector<vector<int>> hierarchy;
        int selectedItem;
        
};

#endif // CSMENU_H

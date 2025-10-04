#include "csUIFx.h"

extern vector<HWND> SECTION;
extern vector<bool> WAITANIMEENDSIN;
extern vector<bool> WAITANIMEENDSOUT;
extern vector<int> MOUSEHOVERCOUNT;
extern vector<int> MOUSELEAVECOUNT;
extern vector<int> MOUSEHOVERSIGNAL;
extern vector<CSIMAGE_GRADIENT> mhgradient;
extern vector<CSLOCKED_MODE> lockedMode;
extern vector<bool> mouseLeaveRemoveBkg;
extern vector<int> backgroundColor;
extern vector<vector<CSGRAPHIC_CONTEXT>> imageGradients;
extern vector<vector<CSGRAPHIC_CONTEXT>> imageGradients;
extern vector<CSBKG_CLICK_GRADIENT> clkGradient;
extern vector<int> CLICKGRADIENTCOUNT;
extern vector<RECT> RECTWND;
extern vector<RECT> RECTCL;
extern vector<HDC> hdcontext;
extern vector<HBITMAP> hStackBmp;
extern vector<SIZE> hdcSize;
extern vector<SIZE> hdcontextExtSize;
extern vector<RECT> RECTPARREF;
extern vector<RECT> bltRect;

extern vector<CSTEXT> TITLE;

extern vector<HDC> hdStackContext;

extern vector<TIPS_POPUP_PARAMS> TipsPopupParams;
extern vector<AUTO_TRANSFORMATION> AutoTransform;
extern vector<int> AUTOTRANSCOUNT;

extern vector<CSDYNAMIC_SIMPLE_TEXT> dynSimpleText;

bool CLICK_EFFECT_BOOL = 0;
extern POINT TIMER_POINT;

extern float xdimFact;
extern float ydimFact;

void CSUIFX::_mouseHover_colorGradient(HWND hwnd, int id)
{
    //ctor
    if((mhgradient[id].GradientType == CS_GRAD_COLOR) && (!lockedMode[id].Locked) && !CLICK_EFFECT_BOOL)
    {
        unsigned int GradientNumber = mhgradient[id].GradientNumber;
        unsigned int TimeStep = mhgradient[id].TimeStep;
        unsigned int mx = GradientNumber*TimeStep;

        if(MOUSEHOVERSIGNAL[id] && MOUSEHOVERCOUNT[id] <= mx)
        {
            //std::cout<<"in\n";
            WAITANIMEENDSIN[id]=1;

            float f = float(MOUSEHOVERCOUNT[id])/mx;
            float f1 = 1.0-f;
            CSRGBA c3;
            if(mhgradient[id].BackgroundGradient)
            {
                CSRGBA c1=mhgradient[id].Color2;
                CSRGBA c2=mhgradient[id].Color1;
                c3.r = (unsigned char)(c2.r*f + c1.r*f1);
                c3.g = (unsigned char)(c2.g*f + c1.g*f1);
                c3.b = (unsigned char)(c2.b*f + c1.b*f1);
                mhgradient[id].ActiveColor = c3;
            }

            if(mhgradient[id].BorderGradient)
            {
                CSRGBA c1=mhgradient[id].BorderColor2;
                CSRGBA c2=mhgradient[id].BorderColor1;
                c3.r = (unsigned char)(c2.r*f + c1.r*f1);
                c3.g = (unsigned char)(c2.g*f + c1.g*f1);
                c3.b = (unsigned char)(c2.b*f + c1.b*f1);
                mhgradient[id].ActiveBorderColor = c3;
            }

            if(mhgradient[id].TitleGradient)
            {
                CSRGBA c1=mhgradient[id].TitleColor2;
                CSRGBA c2=mhgradient[id].TitleColor1;
                c3.r = (unsigned char)(c2.r*f + c1.r*f1);
                c3.g = (unsigned char)(c2.g*f + c1.g*f1);
                c3.b = (unsigned char)(c2.b*f + c1.b*f1);
                mhgradient[id].ActiveTitleColor = c3;
            }

            if(mhgradient[id].ImageGradient)
            {
                int i = MOUSEHOVERCOUNT[id]/(TimeStep);
                //cout<<i<<"\n";
                if(i < GradientNumber)
                    mhgradient[id].ActiveImageIndex = i;
            }
            //SendMessage(hwnd,WM_PAINT,0,0);
            InvalidateRect(hwnd,0,1);
            if(MOUSEHOVERCOUNT[id] == mx)
            {
                WAITANIMEENDSIN[id]=0;
            }
            MOUSEHOVERCOUNT[id]++;

        }
        else if(!MOUSEHOVERSIGNAL[id] &&
                MOUSELEAVECOUNT[id] <= mx)
        {
            //std::cout<<"out\n";
            WAITANIMEENDSOUT[id]=1;
            CSRGBA c3;
            float f = float(MOUSELEAVECOUNT[id])/mx;
            float f1 = 1.0-f;

            if(mhgradient[id].BackgroundGradient)
            {
                CSRGBA c1=mhgradient[id].Color2;
                CSRGBA c2=mhgradient[id].Color1;
                if(lockedMode[id].ExtBool)
                    c2 = lockedMode[id].LockingColor;
                c3.r = (unsigned char)(c1.r*f + c2.r*f1);
                c3.g = (unsigned char)(c1.g*f + c2.g*f1);
                c3.b = (unsigned char)(c1.b*f + c2.b*f1);
                mhgradient[id].ActiveColor = c3;
            }

            if(mhgradient[id].BorderGradient)
            {
                CSRGBA c1=mhgradient[id].BorderColor2;
                CSRGBA c2=mhgradient[id].BorderColor1;
                if(lockedMode[id].ExtBool)
                    c2 = lockedMode[id].LockingBorderColor;
                c3.r = (unsigned char)(c1.r*f + c2.r*f1);
                c3.g = (unsigned char)(c1.g*f + c2.g*f1);
                c3.b = (unsigned char)(c1.b*f + c2.b*f1);
                mhgradient[id].ActiveBorderColor = c3;
            }

            if(mhgradient[id].TitleGradient)
            {
                CSRGBA c1=mhgradient[id].TitleColor2;
                CSRGBA c2=mhgradient[id].TitleColor1;
                if(lockedMode[id].ExtBool)
                    c2 = lockedMode[id].LockingTitleColor;
                c3.r = (unsigned char)(c1.r*f + c2.r*f1);
                c3.g = (unsigned char)(c1.g*f + c2.g*f1);
                c3.b = (unsigned char)(c1.b*f + c2.b*f1);
                mhgradient[id].ActiveTitleColor = c3;
            }

            if(mhgradient[id].ImageGradient)
            {
                int i = GradientNumber-1-MOUSELEAVECOUNT[id]/(TimeStep);
                if(i > -1)
                    mhgradient[id].ActiveImageIndex = i;
            }

            if(MOUSELEAVECOUNT[id] == mx)
            {
                if(mouseLeaveRemoveBkg[id])
                    mhgradient[id].ActiveColor = CSUTILS::toRGBA(backgroundColor[id]);
                WAITANIMEENDSOUT[id]=0;
                if(lockedMode[id].ExtBool)
                    lockedMode[id].ExtBool=0;
                //SetTimer(hwnd, 0,100,0);
            }
            //SendMessage(hwnd,WM_PAINT,0,0);
            InvalidateRect(hwnd,0,1);
            MOUSELEAVECOUNT[id]++;
        }
    }

    if((clkGradient[id].GradientType == CS_GRAD_COLOR) &&
            CLICKGRADIENTCOUNT[id] < clkGradient[id].MaxIter)
    {
        unsigned int mx2 = clkGradient[id].MaxIter;
        RECT r = RECTWND[id];
        POINT p;
        GetCursorPos(&p);
        //CSRGBA c2=BackgroundColor;
        CSRGBA c2=mhgradient[id].Color2;
        CSRGBA c21=mhgradient[id].BorderColor2;
        CSRGBA c22=mhgradient[id].TitleColor2;
        if(mhgradient[id].GradientType==CS_GRAD_COLOR/* &&
                PtInRect(&r,p)*/)
        {
            if(lockedMode[id].Lockable && lockedMode[id].Lockable)
            {
                c2 = lockedMode[id].LockingColor;
                c21 = lockedMode[id].LockingBorderColor;
                c22 = lockedMode[id].LockingTitleColor;
            }
            else
            {
                c2 = mhgradient[id].Color1;
                c21 = mhgradient[id].BorderColor1;
                c22 = mhgradient[id].TitleColor1;
            }
        }
        CSRGBA c3;
        float f = float(CLICKGRADIENTCOUNT[id])/mx2;
        float f1 = 1.0-f;

        if(mhgradient[id].BackgroundGradient)
        {
            CSRGBA c1=clkGradient[id].Color;
            c3.r = (unsigned char)(c2.r*f + c1.r*f1);
            c3.g = (unsigned char)(c2.g*f + c1.g*f1);
            c3.b = (unsigned char)(c2.b*f + c1.b*f1);
            mhgradient[id].ActiveColor = c3;
        }

        if(mhgradient[id].BorderGradient)
        {
            CSRGBA c1=clkGradient[id].BorderColor;
            c3.r = (unsigned char)(c21.r*f + c1.r*f1);
            c3.g = (unsigned char)(c21.g*f + c1.g*f1);
            c3.b = (unsigned char)(c21.b*f + c1.b*f1);
            mhgradient[id].ActiveBorderColor = c3;
        }
        if(mhgradient[id].TitleGradient)
        {
            CSRGBA c1=clkGradient[id].TitleColor;
            c3.r = (unsigned char)(c22.r*f + c1.r*f1);
            c3.g = (unsigned char)(c22.g*f + c1.g*f1);
            c3.b = (unsigned char)(c22.b*f + c1.b*f1);
            mhgradient[id].ActiveTitleColor = c3;
        }
        //DeleteBrush(brush); //-----------------------attention--------------
        CLICKGRADIENTCOUNT[id]++;
        if(CLICKGRADIENTCOUNT[id] == mx2)
        {
            if(lockedMode[id].Lockable && lockedMode[id].Locked)
            {
                mhgradient[id].ActiveColor = lockedMode[id].LockingColor;
                mhgradient[id].ActiveBorderColor = lockedMode[id].LockingBorderColor;
                mhgradient[id].ActiveTitleColor = lockedMode[id].LockingTitleColor;
            }
        }
        InvalidateRect(hwnd,0,1);
    }
}


void CSUIFX::_mouseHover_clickEffect(HWND hwnd, UINT msg, int id)
{
    static POINT p;
    if(msg == WM_LBUTTONDOWN)
    {
        //std::cout<<id<<"\n";
        //SetCapture(hwnd);
        if(!MOUSEHOVERSIGNAL[id])
            SendMessage(hwnd, WM_MOUSEHOVER, 0, 0);
        GetCursorPos(&p);
        if(clkGradient[id].GradientType==CS_GRAD_COLOR && clkGradient[id].TimeStep)
        {
            CLICK_EFFECT_BOOL = 1;
            InvalidateRect(hwnd,0,1);

            if(mhgradient[id].BackgroundGradient)
            {
                mhgradient[id].ActiveColor = clkGradient[id].Color;
            }
            if(mhgradient[id].BorderGradient)
            {
                mhgradient[id].ActiveBorderColor = clkGradient[id].BorderColor;
            }
            if(mhgradient[id].TitleGradient)
            {
                mhgradient[id].ActiveTitleColor = clkGradient[id].TitleColor;
            }
            SendMessage(hwnd,WM_ERASEBKGND,0,0);
        }
        /*else if(clkGradient[id].GradientType== CS_GRAD_IMAGE &&
                    (!lockedMode[id].Lockable))
        {
            MOUSEHOVERSIGNAL[id] = 1;
            MOUSEHOVERCOUNT[id] = 0;
        }*/
    }
    else if(msg == WM_LBUTTONUP)
    {
        // verifier que le point n'est pas hors du cadre du conteneur
        CLICK_EFFECT_BOOL = 0;
        if(PtInRect(&RECTWND[id], p))
        {
            if(clkGradient[id].GradientType==CS_GRAD_COLOR &&
                    clkGradient[id].TimeStep)
            {
                CLICKGRADIENTCOUNT[id] = 0;
                InvalidateRect(hwnd,0,1);
            }
            if(lockedMode[id].Lockable)
            {
                if(lockedMode[id].Locked)
                {
                    if((!lockedMode[id].ExternalUnlocking))
                    {
                        lockedMode[id].Locked = 0;
                        lockedMode[id].LockingActivated = 0;
                    }
                }
                else
                {
                    //LockedMode.Locked = 1;
                    lockedMode[id].Locked = 1;
                    lockedMode[id].LockingActivated = 1;
                    /*if(LockedMode.LockingImagePath)
                    {
                        int id0 = id;
                        if(SameDesignAs) id0 = SameDesignAs;
                        HDC dc=cv.CTX[id].getDwgSurface();
                        CSGRAPHIC_CONTEXT bdc = cv.DCLOCK[id0];
                        BitBlt(dc,cv.cp[id0]->MHGradient.Position.x,cv.cp[id0]->MHGradient.Position.y,
                            bdc.sz.width,bdc.sz.height,
                            bdc.dc,0,0,SRCCOPY);
                        SendMessage(hwnd,WM_ERASEBKGND,0,0);
                    }*/
                }
                //evite le clignotement apres le clic
                unsigned int GradientNumber = mhgradient[id].GradientNumber;
                unsigned int TimeStep = mhgradient[id].TimeStep;
                unsigned int mx = GradientNumber*TimeStep+1;
                MOUSEHOVERCOUNT[id] = mx;
                InvalidateRect(hwnd,0,1);

            }
        }
    }
    //ReleaseCapture();
}
//extern vector<bool> initBool;
void CSUIFX::_mouseHoverLeave(HWND hwnd, UINT msg, int id)
{
    if(msg==WM_MOUSEMOVE/* || msg==WM_NCMOUSEMOVE*/ && !CLICK_EFFECT_BOOL)
    {
        if (!MOUSEHOVERSIGNAL[id] /*&& ((cp[id]->MHGradient.GradientType!=GRADIENT_TYPE_NONE)
                                         ||TipsPopupParams[id].size())*/)
        {
            // start tracking if we aren't already
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_HOVER | TME_LEAVE;
            tme.hwndTrack = hwnd;
            /*unsigned int GradientNumber = mhgradient[id].GradientNumber;
            unsigned int TimeStep = mhgradient[id].TimeStep;
            unsigned int time = GradientNumber*TimeStep;*/
            tme.dwHoverTime = 5;
            MOUSEHOVERSIGNAL[id] = TrackMouseEvent(&tme);
            MOUSEHOVERCOUNT[id] = 0;
        }
    }

    else if(msg==WM_MOUSEHOVER && !CLICK_EFFECT_BOOL/*||msg==WM_NCMOUSEHOVER*/)
    {
        //int max_count = cp[id]->MHGradient.GradientNumber*cp[id]->MHGradient.TimeStep
        //+ cp[id]->MouseHoverTime;
        //SetTimer(hwnd, 0,1,0);
        MOUSEHOVERSIGNAL[id] = FALSE; // tracking now cancelled
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = hwnd;
        MOUSEHOVERSIGNAL[id] = TrackMouseEvent(&tme);

        TIPS_POPUP_PARAMS m = TipsPopupParams[id];
        if(m.Delay)
        {
            TipsPopupParams[id].MouseHoverCount = TipsPopupParams[id].Delay;
            TipsPopupParams[id].MouseLeaveCount = 0;
            
        }

        /*if(cp[id]->MouseLeaveTransparency)
        {
            cp[id]->AutoAlphaFade.State = CS_AAF_SHOW_CNTR;
            cp[id]->AutoAlphaFade.Count = cp[id]->AutoAlphaFade.FadeTime;
            cp[id]->AutoAlphaFade.Activate = 1;
        }*/
        MOUSEHOVERCOUNT[id] = 0;
    }

    else if(msg==WM_MOUSELEAVE/*||msg==WM_NCMOUSELEAVE*/)
    {
        MOUSEHOVERSIGNAL[id] = FALSE; // tracking now cancelled
        //MOUSEHOVERCOUNT[id] = 0;
        MOUSELEAVECOUNT[id] = 0;

        TIPS_POPUP_PARAMS m = TipsPopupParams[id];
        if(m.Delay)
        {
            
            for(int i=0; i<m.Geometry.size(); i++)
            {
                TipsPopupParams[id].Geometry[i].left = RECTWND[i].left;
                TipsPopupParams[id].Geometry[i].top = RECTWND[i].top;
            }
            TipsPopupParams[id].MouseHoverCount = 0;
            TipsPopupParams[id].MouseLeaveCount = TipsPopupParams[id].Delay + 3;
        }

        /*if(HWNDCAPTURE == CNTLIST[0])
            HWNDCAPTURE = 0;
        HWNDSIZING = 0;
        CURRENT_HITTEST_AREA = 0;

        if(cp[id]->MouseLeaveTransparency)
        {
            cp[id]->AutoAlphaFade.State = CS_AAF_HIDE_CNTR;
            cp[id]->AutoAlphaFade.Count = 0;
            cp[id]->AutoAlphaFade.Activate = 1;
        }*/
    }

}


void __createStackContext(int id)
{
    if(!hdStackContext[id])
    {
        hdStackContext[id] = (CreateCompatibleDC(hdcontext[id]));
        hStackBmp[id] = (CreateCompatibleBitmap(hdcontext[id],hdcSize[id].cx, hdcSize[id].cy));
        SelectBitmap(hdStackContext[id], hStackBmp[id]);
    }
}

void CSUIFX::setBackgroundColorGradient(int id, CSRGBA color1, CSRGBA color2, unsigned int gradientNumber, unsigned int timeStep)
{
    __createStackContext(id);
    mhgradient[id].GradientType = CS_GRAD_COLOR;
    mhgradient[id].Color1 = color1;
    mhgradient[id].Color2 = color2;
    mhgradient[id].ActiveColor = color2;
    mhgradient[id].GradientNumber = gradientNumber;
    mhgradient[id].TimeStep = timeStep;
    mhgradient[id].BackgroundGradient = 1;
}
void CSUIFX::setBorderColorGradient(int id, CSRGBA color1, CSRGBA color2, unsigned int gradientNumber, unsigned int timeStep)
{
    __createStackContext(id);
    mhgradient[id].GradientType = CS_GRAD_COLOR;
    mhgradient[id].BorderColor1 = color1;
    mhgradient[id].BorderColor2 = color2;
    mhgradient[id].ActiveBorderColor = color2;
    mhgradient[id].GradientNumber = gradientNumber;
    mhgradient[id].TimeStep = timeStep;
    mhgradient[id].BorderGradient = 1;
}

void CSUIFX::setTitleColorGradient(int id, CSRGBA color1, CSRGBA color2, unsigned int gradientNumber, unsigned int timeStep)
{
    __createStackContext(id);
    mhgradient[id].GradientType = CS_GRAD_COLOR;
    mhgradient[id].TitleColor1 = color1;
    mhgradient[id].TitleColor2 = color2;
    mhgradient[id].ActiveTitleColor = color2;
    mhgradient[id].GradientNumber = gradientNumber;
    mhgradient[id].TimeStep = timeStep;
    mhgradient[id].TitleGradient = 1;
}

void CSUIFX::setImageGradient(int id, char*path1, char*path2, POINT position, SIZE size, float zoom, unsigned int gradientNumber, unsigned int timeStep)
{
    __createStackContext(id);
    mhgradient[id].Image1 = path1;
    mhgradient[id].Image2 = path2;
    mhgradient[id].ZoomLevel = zoom;
    mhgradient[id].Size = {size.cx*xdimFact, size.cy*ydimFact};
    mhgradient[id].Position = {position.x*xdimFact, position.y*ydimFact};
    mhgradient[id].GradientNumber = gradientNumber;
    mhgradient[id].TimeStep = timeStep;

    imageGradients[id] = csGraphics::getImageGradient(mhgradient[id], CSUTILS::toRGBA(backgroundColor[id]));
    mhgradient[id].ActiveImageIndex = 0;
    mhgradient[id].ImageGradient = 1;
    mhgradient[id].GradientType = CS_GRAD_COLOR;

}

CSMOUSE_HOVER_EFFECT* CSUIFX::getMouseHoverEffectPtr(int id)
{
    return &mhgradient[id];
}

void CSUIFX::setBackgroundColorClickEffect(int id, CSRGBA color)
{
    clkGradient[id].Color = color;
    clkGradient[id].TimeStep = mhgradient[id].TimeStep;
    clkGradient[id].GradientNumber = mhgradient[id].GradientNumber;
    clkGradient[id].GradientType = mhgradient[id].GradientType;

    clkGradient[id].MaxIter = mhgradient[id].GradientNumber*mhgradient[id].TimeStep;

}

void CSUIFX::setBorderColorClickEffect(int id, CSRGBA color)
{
    clkGradient[id].BorderColor = color;
    clkGradient[id].TimeStep = mhgradient[id].TimeStep;
    clkGradient[id].GradientNumber = mhgradient[id].GradientNumber;
    clkGradient[id].GradientType = mhgradient[id].GradientType;

    clkGradient[id].MaxIter = mhgradient[id].GradientNumber*mhgradient[id].TimeStep;

}

void CSUIFX::setTitleColorClickEffect(int id, CSRGBA color)
{
    clkGradient[id].TitleColor = color;
    clkGradient[id].TimeStep = mhgradient[id].TimeStep;
    clkGradient[id].GradientNumber = mhgradient[id].GradientNumber;
    clkGradient[id].GradientType = mhgradient[id].GradientType;

    clkGradient[id].MaxIter = mhgradient[id].GradientNumber*mhgradient[id].TimeStep;

}

RECT lastRect;

bool ptInSecRect(vector<int> v)
{
    int n=v.size();
    POINT p;
    GetCursorPos(&p);
    for(int i=0; i<n; i++)
    {
        if(PtInRect(&RECTWND[v[i]], p))
            return 1;
    }
    return 0;
}

void CSUIFX::_mouseHover_movePopup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, int id)
{
    if(msg == WM_TIMER)
    {
        TIPS_POPUP_PARAMS m = TipsPopupParams[id];
        if(m.Delay)
        {
            if(TipsPopupParams[id].MouseHoverCount > 0)
            {
                if(TipsPopupParams[id].MouseHoverCount == 1)
                {
                    //RECT r = RECTPARREF[id]; //in desktop coords
                    RECT r = RECTWND[id]; //in desktop coords
                    GetWindowRect(hwnd, &r);
                    RECT rc = RECTCL[id]; //in desktop coords
                    GetClientRect(hwnd, &rc);
                    wchar_t*t = TITLE[id].Text;
                    for(int i = 0; i<m.Ids.size(); i++)
                    {
                        HWND w=SECTION[m.Ids[i]];
                        RECT r1=m.Geometry[i]; //sizes
                        
                        POS_BOOL b=m.Bpos[i];

                        int L = (b.bTLeft|b.bBLeft|b.bMLeft)*(r.left-r1.right)
                                +(b.bLTop|b.bLBottom)*(r.left)+(b.bRTop|b.bRBottom)*(r.right-r1.right)
                                +(b.bTRight|b.bBRight|b.bMRight)*(r.right) + b.bOther*r1.left
                                +(b.bMBottom|b.bMTop)*(r.left+(rc.right-r1.right)/2);
                        int T = (b.bLTop|b.bRTop|b.bMTop)*(r.top-r1.bottom)+
                                (b.bTRight|b.bTLeft)*(r.top)+(b.bBRight|b.bBLeft)*(r.bottom-r1.bottom)
                                +(b.bRBottom|b.bLBottom|b.bMBottom)*(r.bottom) + b.bOther*r1.top
                                +(b.bMLeft|b.bMRight)*(r.top+(rc.bottom-r1.bottom)/2);

                        int j = m.Ids[i];
                        TITLE[j].Text = t;
                        if(IsWindowVisible(w))
                        {
                            RECT rp = RECTWND[j];
                            //RECT rp = CSUTILS::rectInParentRef(j);

                            SetWindowPos(w,0,rp.left,rp.top,r1.right, r1.bottom,SWP_SHOWWINDOW);
                            InvalidateRect(w,0,1);
                            AutoTransform[j].InitialRect=rp;
                            AutoTransform[j].FinalRect={L-rp.left,T-rp.top,r1.right-rp.right+rp.left,r1.bottom-rp.bottom+rp.top};
                            AutoTransform[j].Time=3;
                            AutoTransform[j].Type = AUTO_TRANSF_LENGTH;
                            //TipsPopupParams[id].Geometry[i].left = L;
                            //TipsPopupParams[id].Geometry[i].top = T;
                            AUTOTRANSCOUNT[j] = 0;

                        }
                        else
                        {
                            /*int aafmove = 30;
                            SetWindowPos(w,0,L,T,r1.right,r1.bottom,SWP_SHOWWINDOW);
                            cv.CNTRECT_SAVED[j]={L,T,r1.right,r1.bottom};
                            cv.cp[j]->AutoAlphaFade.Geometry = {0,0,0,0};
                            cv.cp[j]->AutoAlphaFade.FadeTime = 8;
                            cv.cp[j]->AutoAlphaFade.AlphaMin = 50;
                            cv.cp[j]->AutoAlphaFade.ShowHide = 1;
                            cv.cp[j]->AutoAlphaFade.State = CS_AAF_SHOW_CNTR;
                            cv.cp[j]->AutoAlphaFade.Activate = 1;*/
                            SetWindowPos(w,0,L,T,r1.right,r1.bottom,SWP_SHOWWINDOW);
                        }
                        
                        GetWindowRect(w, &lastRect);
                        //SetWindowPos(w,0,L,T,r1.right,r1.bottom,SWP_SHOWWINDOW);
                        bltRect[j].right = r1.right-m.text[i].marg.right-m.text[i].marg.left;
                        bltRect[j].left = 0;
                        bltRect[j].bottom = r1.bottom-m.text[i].marg.bottom;
                        bltRect[j].top = 0;
                        if(m.text.size())
                        {
                            dynSimpleText[j] = m.text[i];
                            hdcontextExtSize[j].cx = r1.right;
                            csGraphics::updateGraphicArea(j, 1);
                        }
                    }
                    //cv.BHOOK_LBUTNDOWN_POPUP = 1;
                }
                TipsPopupParams[id].MouseHoverCount--;
            }
            if(TipsPopupParams[id].MouseLeaveCount>0)
            {
                RECT r, r1;
                if(TipsPopupParams[id].MouseLeaveCount==TipsPopupParams[id].Delay+3)
                {
                    //entre cette condition et la suivante, il y'a un espce de temps pendant lequel
                    //la fenetre peut se deplacer. Dans cas il ne faut pa la masquer.
                    for(int i = 0; i<m.Ids.size(); i++)
                    {
                        GetWindowRect(SECTION[m.Ids[i]],&r1);
                        TipsPopupParams[id].Geometry[i].left=r1.left;
                        TipsPopupParams[id].Geometry[i].top=r1.top;
                    }
                }
                if(TipsPopupParams[id].MouseLeaveCount==1)
                {
                    POINT p = TIMER_POINT;
                    for(int i = 0; i<m.Ids.size(); i++)
                    {
                        r = TipsPopupParams[id].Geometry[i];
                        GetWindowRect(SECTION[m.Ids[i]],&r1);

                        if((r1.left==r.left && r1.top==r.top) &&
                            !(PtInRect(&r1,p)&& TipsPopupParams[id].Lock))
                        {
                            if(lastRect.left == r1.left && lastRect.top == r1.top)                           
                            {
                                ShowWindow(SECTION[m.Ids[i]],SW_HIDE);

                            }
                            else
                            {
                                if(m.Ids_src && !ptInSecRect(*m.Ids_src))
                                    ShowWindow(SECTION[m.Ids[i]],SW_HIDE);
                                lastRect = r1;
                            }
                        }
                            //cv.cp[m.Ids[i]]->Show = 0;
                    }
                }
                TipsPopupParams[id].MouseLeaveCount--;
            }

        }

    }
}

void CSUIFX::_autoTransformation(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, int id)
{
    if(msg == WM_TIMER && !AutoTransform[id].EmergencyStop)
    {
        //RECT rw = rectInParentRef(id);
        if(AutoTransform[id].Type==AUTO_TRANSF_LENGTH)
        {
            unsigned int Time=AutoTransform[id].Time;
            //unsigned int Count=AutoTransform[id].Count;//nombre de fois que l'action est sollicitee
            if(AUTOTRANSCOUNT[id] <= Time)
            {
                RECT rw = AutoTransform[id].InitialRect;
                RECT r = AutoTransform[id].FinalRect;
                float t = AUTOTRANSCOUNT[id];

                POS_FUNC pf = AutoTransform[id].PosFunc;
                int x = pf.Get(t, Time, r.left, rw.left);
                int y = pf.Get(t, Time, r.top, rw.top);
                int cx = pf.Get(t, Time, r.right, rw.right-rw.left);
                int cy = pf.Get(t, Time, r.bottom, rw.bottom-rw.top);

                if(AUTOTRANSCOUNT[id]<Time)
                    SetWindowPos(hwnd,AutoTransform[id].Range, x, y, cx, cy, SWP_NOZORDER);
                else
                {
                    SetWindowPos(hwnd,AutoTransform[id].Range, rw.left+r.left, rw.top+r.top,
                    rw.right-rw.left+r.right, rw.bottom-rw.top+r.bottom,SWP_NOZORDER);
                    //std::cout<<id<<"{"<<r.right<<"}  ";
                }
                if(AutoTransform[id].Update)
                    InvalidateRect(hwnd, 0,1);
                AUTOTRANSCOUNT[id]++;
                AutoTransform[id].waitUntilAnimEnds=1;
            }
            else
            {
                AutoTransform[id].waitUntilAnimEnds=0;//nouveau----=1  -----------------------
                //std::cout<<545;
            }
        }
        else if(AutoTransform[id].Type==AUTO_TRANSF_RECT)
        {
            unsigned int Time=AutoTransform[id].Time;
            if(AUTOTRANSCOUNT[id] <= Time)
            {
                RECT rw = AutoTransform[id].InitialRect;
                RECT r = AutoTransform[id].FinalRect;
                float t = AUTOTRANSCOUNT[id];

                POS_FUNC pf = AutoTransform[id].PosFunc;
                int x = pf.Get(t, Time, r.left-rw.left, rw.left);
                int y = pf.Get(t, Time, r.top-rw.top, rw.top);
                int cx = pf.Get(t, Time, r.right-rw.right, rw.right);
                int cy = pf.Get(t, Time, r.bottom-rw.bottom, rw.bottom);

                if(AUTOTRANSCOUNT[id] == Time)
                {
                    SetWindowPos(hwnd,AutoTransform[id].Range, r.left, r.top,
                                    r.right-r.left, r.bottom-r.top, SWP_NOZORDER);
                }
                else
                    SetWindowPos(hwnd, AutoTransform[id].Range ,x, y, cx, cy, SWP_NOZORDER);

                if(AutoTransform[id].Update)
                    InvalidateRect(hwnd, 0,1);

                AUTOTRANSCOUNT[id]++;
                AutoTransform[id].waitUntilAnimEnds=1;
            }
            else
            {
                AutoTransform[id].waitUntilAnimEnds=0;//nouveau----=1  -----------------------
            }
        }
    }

}

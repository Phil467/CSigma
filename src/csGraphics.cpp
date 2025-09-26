#include "csGraphics.h"
#include "math.h"

extern int CAPTION_AREA_SIZE;
extern vector<RECT> RECTWND;
extern vector<RECT> RECTCL;
extern vector<RECT> bltRect;
extern vector<bool> SECTIONSTYLE;
extern vector<HWND> SECTION;

extern vector<HDC> hdcontext;
extern vector<HDC> hdcontextExt;
extern vector<HDC> hdBkgContext;
extern vector<HBITMAP> hBkgBmp;
extern vector<COLORREF> backgroundColor;
extern vector<COLORREF> borderColor;

extern vector<POINT> hdcontextExtInPos;
extern vector<POINT> hdcontextExtOutPos;
extern vector<SIZE> hdcontextExtSize;
extern vector<SIZE> hdcSize;
extern vector<HBITMAP> hdBmpExt;
extern vector<CSRGBA> hdcontextExtBkgColor;
extern vector<CSRGBA> hdcontextExtBrdColor;
extern vector<CSENTITY_ID_MAP> entityMap;
extern vector<vector<CSGRAPHIC_ENTITY>> entity;

extern vector<float> hZoom;
extern vector<float> vZoom;
extern vector<BYTE> mouseWheelPreference;

extern vector<bool> bltUpdate;

extern vector<CSDYNAMIC_SIMPLE_TEXT> dynSimpleText;

extern vector<vector<CSLOADED_IMAGE>> loadedImage;

BYTE interpByte(char b1, char b2, char b3, char b4,float dx, float dy);


CSGRAPHIC_CONTEXT csGraphics::getImageGraphicContext(char*path)
{
    CSGRAPHIC_CONTEXT_EXT gce;
    gce.hbmp=(HBITMAP)LoadImageA(NULL,path,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
    GetObjectA((HGDIOBJ)gce.hbmp, sizeof(gce.bm), (PVOID)&gce.bm);
    //char*buf0 = (char*)gce.bm.bmBits;
    HDC dc=GetDC(0);
    gce.dc=CreateCompatibleDC(dc);
    SelectBitmap(gce.dc,gce.hbmp);
    ReleaseDC(0,dc);
    DeleteBitmap(gce.hbmp);

    CSGRAPHIC_CONTEXT gc;
    gc.dc = gce.dc;
    gc.sz = {gce.bm.bmWidth, gce.bm.bmHeight};

    return gc;
}

CSGRAPHIC_CONTEXT csGraphics::getImageGraphicContextW(wchar_t*path)
{
    CSGRAPHIC_CONTEXT_EXT gce;
    gce.hbmp=(HBITMAP)LoadImageW(NULL,path,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
    GetObjectW((HGDIOBJ)gce.hbmp, sizeof(gce.bm), (PVOID)&gce.bm);
    //char*buf0 = (char*)gce.bm.bmBits;
    HDC dc=GetDC(0);
    gce.dc=CreateCompatibleDC(dc);
    SelectBitmap(gce.dc,gce.hbmp);
    ReleaseDC(0,dc);
    DeleteBitmap(gce.hbmp);

    CSGRAPHIC_CONTEXT gc;
    gc.dc = gce.dc;
    gc.sz = {gce.bm.bmWidth, gce.bm.bmHeight};

    return gc;
}

CSGRAPHIC_CONTEXT csGraphics::getImageGraphicContextResizedW(wchar_t*path, SIZE size)
{
    CSGRAPHIC_CONTEXT_EXT gce;
    gce.hbmp=(HBITMAP)LoadImageW(NULL,path,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
    GetObjectW((HGDIOBJ)gce.hbmp, sizeof(gce.bm), (PVOID)&gce.bm);
    //char*buf0 = (char*)gce.bm.bmBits;
    HDC dc=GetDC(0);
    gce.dc=CreateCompatibleDC(dc);
    SelectBitmap(gce.dc,gce.hbmp);
    DeleteBitmap(gce.hbmp);

    if(size.cx == 0 && size.cy == 0) 
    {
        size.cx = gce.bm.bmWidth;
        size.cy = gce.bm.bmHeight;
    }
    HDC rdc = CreateCompatibleDC(dc);
    HBITMAP bm = CreateCompatibleBitmap(dc, size.cx, size.cy);
    SelectBitmap(rdc, bm);
    SetStretchBltMode(rdc, HALFTONE);
    
    StretchBlt(rdc, 0,0, size.cx, size.cy, gce.dc, 0,0, gce.bm.bmWidth, gce.bm.bmHeight, SRCCOPY);
    //cout<<size.cx<<" -- "<<size.cy<<"\n";
    DeleteDC(gce.dc);
    ReleaseDC(0,dc);

    CSGRAPHIC_CONTEXT gc;
    gc.dc = rdc;
    gc.sz = {size.cx, size.cy};

    return gc;
}

CSGRAPHIC_CONTEXT csGraphics::getImageGraphicContextResized(char*path, SIZE size)
{
    CSGRAPHIC_CONTEXT_EXT gce;
    gce.hbmp=(HBITMAP)LoadImageA(NULL,path,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
    GetObjectA((HGDIOBJ)gce.hbmp, sizeof(gce.bm), (PVOID)&gce.bm);
    //char*buf0 = (char*)gce.bm.bmBits;
    HDC dc=GetDC(0);
    gce.dc=CreateCompatibleDC(dc);
    SelectBitmap(gce.dc,gce.hbmp);
    DeleteBitmap(gce.hbmp);

    if(size.cx == 0 && size.cy == 0) 
    {
        size.cx = gce.bm.bmWidth;
        size.cy = gce.bm.bmHeight;
    }
    HDC rdc = CreateCompatibleDC(dc);
    HBITMAP bm = CreateCompatibleBitmap(dc, size.cx, size.cy);
    SelectBitmap(rdc, bm);
    SetStretchBltMode(rdc, HALFTONE);
    StretchBlt(rdc, 0,0, size.cx, size.cy, gce.dc, 0,0, gce.bm.bmWidth, gce.bm.bmHeight, SRCCOPY);

    DeleteDC(gce.dc);
    ReleaseDC(0,dc);

    CSGRAPHIC_CONTEXT gc;
    gc.dc = rdc;
    gc.sz = {size.cx, size.cy};

    return gc;
}

CSGRAPHIC_CONTEXT_EXT csGraphics::getImageGraphicContextExt(char*path, BITMAPINFO*bmi)
{
    CSGRAPHIC_CONTEXT_EXT gce;
    gce.hbmp=(HBITMAP)LoadImageA(NULL,path,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
    GetObjectA((HGDIOBJ)gce.hbmp,sizeof(gce.bm),(PVOID)&gce.bm);
    //char*buf0 = (char*)gce.bm.bmBits;
    HDC dc=GetDC(0);
    gce.dc=CreateCompatibleDC(dc);
    SelectBitmap(gce.dc,gce.hbmp);
    /*DrawStateA(gce.dc,NULL,NULL,
               (long long)gce.hbmp,NULL,0,0,0,0,DST_BITMAP);*/
    gce.bm.bmBits = (LPVOID)malloc(bmi->bmiHeader.biSizeImage*sizeof(void));
    int bres = GetDIBits(gce.dc,gce.hbmp,0,gce.bm.bmHeight,gce.bm.bmBits,bmi,DIB_RGB_COLORS);
    ReleaseDC(0,dc);
    return gce;
}

CSGRAPHIC_CONTEXT_EXT csGraphics::getImageGraphicContextExtW(wchar_t*path, BITMAPINFO*bmi)
{
    CSGRAPHIC_CONTEXT_EXT gce;
    gce.hbmp=(HBITMAP)LoadImageW(NULL,path,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
    GetObjectW((HGDIOBJ)gce.hbmp,sizeof(gce.bm),(PVOID)&gce.bm);
    //char*buf0 = (char*)gce.bm.bmBits;
    HDC dc=GetDC(0);
    gce.dc=CreateCompatibleDC(dc);
    SelectBitmap(gce.dc,gce.hbmp);
    /*DrawStateA(gce.dc,NULL,NULL,
               (long long)gce.hbmp,NULL,0,0,0,0,DST_BITMAP);*/
    gce.bm.bmBits = (LPVOID)malloc(bmi->bmiHeader.biSizeImage*sizeof(void));
    int bres = GetDIBits(gce.dc,gce.hbmp,0,gce.bm.bmHeight,gce.bm.bmBits,bmi,DIB_RGB_COLORS);
    ReleaseDC(0,dc);
    return gce;
}

CSGRAPHIC_CONTEXT_EXT csGraphics::createGraphicContextExt(SIZE sz, BITMAPINFO*bmi)
{
    CSGRAPHIC_CONTEXT_EXT gce;
    CSGRAPHIC_CONTEXT  ndc = createGraphicContext(sz);
    gce.dc = ndc.dc;
    gce.hbmp = ndc.hbmp;
    GetObjectA((HGDIOBJ)gce.hbmp,sizeof(gce.bm),(PVOID)&gce.bm);
    gce.bm.bmBits = (LPVOID)malloc((bmi->bmiHeader.biSizeImage+1)*sizeof(void));
    int bres = GetDIBits(gce.dc,gce.hbmp,0,gce.bm.bmHeight,gce.bm.bmBits,bmi,DIB_RGB_COLORS);
    return gce;
}

CSGRAPHIC_CONTEXT csGraphics::createGraphicContext(SIZE sz)
{
    CSGRAPHIC_CONTEXT dc;
    HDC hdc = GetDC(0);
    dc.dc=CreateCompatibleDC(hdc);
    dc.hbmp = CreateCompatibleBitmap(hdc, sz.cx, sz.cy);
    SelectBitmap(dc.dc,dc.hbmp);
    /*DrawStateA(dc.dc,NULL,NULL,
               (long long)dc.hbmp,NULL,0,0,0,0,DST_BITMAP);*/
    ReleaseDC(0,hdc);
    dc.sz = sz;
    return dc;
}

CSGRAPHIC_CONTEXT csGraphics::createCompatibleGraphicContext(HDC hdc, SIZE sz, CSRGBA bkgColor, CSRGBA brdColor)
{
    CSGRAPHIC_CONTEXT dc;
    dc.dc= CreateCompatibleDC(hdc);
    dc.hbmp = CreateCompatibleBitmap(hdc, sz.cx, sz.cy);
    SelectBitmap(dc.dc,dc.hbmp);
    CSUTILS::drawGDIRectangle(dc.dc, RGB(bkgColor.r, bkgColor.g, bkgColor.b), RGB(brdColor.r, brdColor.g, brdColor.b), 1, {0,0,sz.cx,sz.cy});
    dc.sz = sz;
    return dc;
}
CSGRAPHIC_CONTEXT csGraphics::createCompatibleGraphicContext(HDC hdc, SIZE sz, COLORREF bkgColor, COLORREF brdColor)
{
    CSGRAPHIC_CONTEXT dc;
    dc.dc= CreateCompatibleDC(hdc);
    dc.hbmp = CreateCompatibleBitmap(hdc, sz.cx, sz.cy);
    SelectBitmap(dc.dc,dc.hbmp);
    CSUTILS::drawGDIRectangle(dc.dc, bkgColor, brdColor, 1, {0,0,sz.cx,sz.cy});
    dc.sz = sz;
    return dc;
}

void csGraphics::releaseGraphicContext(CSGRAPHIC_CONTEXT dc)
{
    DeleteDC(dc.dc);
    DeleteObject(dc.hbmp);
    dc.sz = {0,0};
}

void csGraphics::releaseGraphicContextExt(CSGRAPHIC_CONTEXT_EXT gce)
{
    DeleteDC(gce.dc);
    DeleteObject(gce.hbmp);
    if(gce.bm.bmBits)
        free(gce.bm.bmBits);
}


BITMAPINFO csGraphics::setBMI(SIZE sz)
{
    BITMAPINFO bi;
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = sz.cx;
    bi.bmiHeader.biHeight = -sz.cy;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    bi.bmiHeader.biSizeImage = sz.cx * 4 * sz.cy;
    bi.bmiHeader.biClrUsed = 0;
    bi.bmiHeader.biClrImportant = 0;
    return bi;
}

/*vector<CSGRAPHIC_CONTEXT> csGraphics::getImageGradient(char*path1,char*path2, SIZE sz, unsigned int n, float rotAng, float zoom,
                           CSRGBA bkgColor, CSRGBA teint, float teintLevel)*/
vector<CSGRAPHIC_CONTEXT> csGraphics::getImageGradient(CSIMAGE_GRADIENT mhg, CSRGBA bkgColor)
{
//cout<<"in\n";
    char*path1 = mhg.Image1;
    char*path2 = mhg.Image2;
    
    SIZE sz = mhg.Size;
    unsigned int n = mhg.GradientNumber;
    float rotAng = mhg.ImageRotationAngle;
    float zoom = mhg.ZoomLevel;
    CSRGBA teint = mhg.HightLightColor;
    float teintLevel = mhg.HightLightLevel;

    vector<CSGRAPHIC_CONTEXT> dcs;
    BITMAPINFO bmi = setBMI(sz);
    
    CSGRAPHIC_CONTEXT_EXT bmp1 = getImageGraphicContextExt(path1, &bmi);
    CSGRAPHIC_CONTEXT_EXT bmp2 = getImageGraphicContextExt(path2, &bmi);
    CSGRAPHIC_CONTEXT_EXT fbmp1 = createGraphicContextExt(sz, &bmi);
    CSGRAPHIC_CONTEXT_EXT fbmp2 = createGraphicContextExt(sz, &bmi);

    SetStretchBltMode(fbmp1.dc, HALFTONE);
    SetStretchBltMode(fbmp2.dc, HALFTONE);
    StretchBlt(fbmp1.dc, 0,0,sz.cx, sz.cy, bmp1.dc, 0,0,bmp1.bm.bmWidth,bmp1.bm.bmHeight,SRCCOPY);
    StretchBlt(fbmp2.dc, 0,0,sz.cx, sz.cy, bmp2.dc, 0,0,bmp2.bm.bmWidth,bmp2.bm.bmHeight,SRCCOPY);
    GetDIBits(fbmp1.dc,fbmp1.hbmp,0,fbmp1.bm.bmHeight,fbmp1.bm.bmBits,&bmi,DIB_RGB_COLORS);
    GetDIBits(fbmp2.dc,fbmp2.hbmp,0,fbmp2.bm.bmHeight,fbmp2.bm.bmBits,&bmi,DIB_RGB_COLORS);

    char*buf1 = (char*)fbmp1.bm.bmBits;
    char*buf2 = (char*)fbmp2.bm.bmBits;

    long lval=0,nCount=0, r, g, b, a, szmax=sz.cx*sz.cy*4;
    float rrat, grat, brat, rat, arat, da, toRad=3.141516/180, xc=sz.cx/2, yc=sz.cy/2;
    int j0, i0;
    float j0f, i0f, dx, dy;
    long nCount11, nCount12, nCount21, nCount22, nCount0;
    BYTE bi1, bi2;

    for (int k=0; k<n; k++)
    {
        CSGRAPHIC_CONTEXT ndc;
        CSGRAPHIC_CONTEXT_EXT mbmp = createGraphicContextExt(sz, &bmi);
        char*buf = (char*)mbmp.bm.bmBits;
        buf = (char*)realloc(buf, szmax+1);
        buf[szmax] = '\0';

        for (int j=0; j<sz.cy; j++)
        {
            for (int i=0; i<sz.cx; i++)
            {
                nCount=(j*sz.cx+i)*4;
                rat = (float)k/(n-1);
                da = -rotAng*rat*toRad;
                i0 = int(i0f = xc*rat*zoom+(xc+(i-xc)*cos(da)+(j-yc)*sin(da))*(1-rat*zoom));
                j0 = int(j0f = yc*rat*zoom+(yc-(i-xc)*sin(da)+(j-yc)*cos(da))*(1-rat*zoom));
                dx = i0f-i0;
                dy = j0f-j0;
                if(i0==sz.cx-1) i0 --;
                else if(i0<0) i0++;
                if(j0==sz.cy-1) j0 --;
                else if(j0<0) j0++;

                nCount0=(j0*sz.cx+i0)*4;
                nCount11=(j0*sz.cx+i0)*4;
                nCount12=(j0*sz.cx+(i0+1))*4;
                nCount21=((j0+1)*sz.cx+i0)*4;
                nCount22=((j0+1)*sz.cx+(i0+1))*4;
                if(nCount0>0&&nCount0<szmax)
                {
                    bi2 = interpByte(buf2[nCount11], buf2[nCount12],buf2[nCount21],buf2[nCount22], dx,dy);
                    bi1 = interpByte(buf1[nCount11], buf1[nCount12],buf1[nCount21],buf1[nCount22], dx,dy);
                    r = (1-rat*teintLevel)*(bi2*rat + bi1*(1-rat)) + rat*teint.r*teintLevel;
                    bi2 = interpByte(buf2[nCount11+1], buf2[nCount12+1],buf2[nCount21+1],buf2[nCount22+1], dx,dy);
                    bi1 = interpByte(buf1[nCount11+1], buf1[nCount12+1],buf1[nCount21+1],buf1[nCount22+1], dx,dy);
                    g = (1-rat*teintLevel)*(bi2*rat + bi1*(1-rat)) + rat*teint.g*teintLevel;
                    bi2 = interpByte(buf2[nCount11+2], buf2[nCount12+2],buf2[nCount21+2],buf2[nCount22+2], dx,dy);
                    bi1 = interpByte(buf1[nCount11+2], buf1[nCount12+2],buf1[nCount21+2],buf1[nCount22+2], dx,dy);
                    b = (1-rat*teintLevel)*(bi2*rat + bi1*(1-rat)) + rat*teint.b*teintLevel;
                    //a = BYTE(buf2[nCount11+3])*(rat) + BYTE(buf1[nCount11+3])*(1-rat);
                }
                else
                {
                    r=bkgColor.r;
                    g=bkgColor.g;
                    b=bkgColor.b;
                }

                buf[nCount] = (BYTE)r;
                buf[nCount+1] = (BYTE)g;
                buf[nCount+2] = (BYTE)b;

            }
        }

        SetDIBits(mbmp.dc, mbmp.hbmp, 0, sz.cy, buf,  &bmi, DIB_RGB_COLORS);
        //StretchBlt(GetDC(0), 0,0,sz.width, sz.height, mbmp.dc, 0,0,mbmp.bm.bmWidth,mbmp.bm.bmHeight,SRCCOPY);
        free(buf);
        ndc.dc = mbmp.dc;
        ndc.hbmp = mbmp.hbmp;
        ndc.sz = sz;
        //StretchBlt(GetDC(0), 0,0,sz.width, sz.height, ndc.dc, 0,0,mbmp.bm.bmWidth,mbmp.bm.bmHeight,SRCCOPY);
        dcs.push_back(ndc);
    }
    releaseGraphicContextExt(bmp1);
    releaseGraphicContextExt(bmp2);
    releaseGraphicContextExt(fbmp1);
    releaseGraphicContextExt(fbmp2);
//cout<<"out\n";
    return dcs;
}

BYTE interpByte(char b1, char b2, char b3, char b4,float dx, float dy)
{
    //float f = ((1.0-dy)*(1.0-dx)*((BYTE)b1) + dx*(1.0-dy)*((BYTE)b2) + (1.0-dx)*dy*((BYTE)b3) + dx*dy*((BYTE)b4));
    float f = (1.0-dy)*((1.0-dx)*((BYTE)b1) + dx*((BYTE)b2)) + dy*((1-dx)*((BYTE)b3) + dx*((BYTE)b4));
    return (BYTE)f;
}



void csGraphics::setHzoom(int id, float zoom)
{
    hZoom[id] = zoom;
}

void csGraphics::setVzoom(int id, float zoom)
{
    vZoom[id] = zoom;
}

void csGraphics::setMouseWheelPreference(int id, BYTE pref)
{
    mouseWheelPreference[id] = pref;
}

void csGraphics::setGraphicAreaPosition(int id, POINT pos)
{
    hdcontextExtOutPos[id] = pos;
}

void csGraphics::setGraphicAreaSize(int id, SIZE size)
{
    hdcontextExtSize[id] = size;
}
void csGraphics::setGraphicAreaXSize(int id, long cx)
{
    hdcontextExtSize[id].cx = cx;
}
void csGraphics::setGraphicAreaYSize(int id, long cy)
{
    hdcontextExtSize[id].cy = cy;
}

void csGraphics::setGraphicAreaColor(int id, CSRGBA background, CSRGBA border)
{
    hdcontextExtBkgColor[id] = background;
    hdcontextExtBrdColor[id] = border;
}

void csGraphics::updateGraphicArea(int id, bool reCreate)
{
    if(reCreate)
    {
        SIZE size = hdcontextExtSize[id];
        DeleteBitmap(hdBmpExt[id]);
        DeleteDC(hdcontextExt[id]);
        HDC dc = GetDC(0);
        hdcontextExt[id] = CreateCompatibleDC(dc);
        hdBmpExt[id] = CreateCompatibleBitmap(dc, size.cx, size.cy);
        SelectBitmap(hdcontextExt[id], hdBmpExt[id]);
        CSRGBA bc = hdcontextExtBkgColor[id];
        CSRGBA pc = hdcontextExtBrdColor[id];
        HBRUSH hb = CreateSolidBrush(RGB(bc.r, bc.g, bc.b));
        HPEN hp = CreatePen(0,1,RGB(pc.r, pc.g, pc.b));
        SelectPen(hdcontextExt[id], hp);
        SelectBrush(hdcontextExt[id], hb);
        Rectangle(hdcontextExt[id],0,0,size.cx, size.cy);
        DeletePen(hp);
        DeleteBrush(hb);
        ReleaseDC(0,dc);
    }
    bltUpdate[id] = 1;
    InvalidateRect(SECTION[id], &bltRect[id], 1);
}

void csGraphics::updateBackground(int id)
{
    DeleteDC(hdBkgContext[id]);
    DeleteBitmap(hBkgBmp[id]);
    
    hdBkgContext[id] = (CreateCompatibleDC(hdcontext[id]));
    hBkgBmp[id] = (CreateCompatibleBitmap(hdcontext[id],hdcSize[id].cx, hdcSize[id].cy));
    SelectBitmap(hdBkgContext[id], hBkgBmp[id]);

    HBRUSH hb = CreateSolidBrush(backgroundColor[id]);
    HPEN hp = CreatePen(0,1,borderColor[id]);
    SelectPen(hdBkgContext[id], hp);
    SelectBrush(hdBkgContext[id], hb);
    Rectangle(hdBkgContext[id],0,0,hdcSize[id].cx, hdcSize[id].cy);
    DeletePen(hp);
    DeleteBrush(hb);
    
}

CSGRAPHIC_CONTEXT csGraphics::viewImage(int id, wchar_t*path, POINT position, SIZE size)
{
    if(hdcontextExt[id])
    {
        CSGRAPHIC_CONTEXT gc = csGraphics::getImageGraphicContextResizedW(path, size);
        BitBlt(hdcontextExt[id],position.x,position.y,gc.sz.cx,gc.sz.cy, gc.dc,0,0, SRCCOPY);
        InvalidateRect(SECTION[id], 0, 1);
        return gc;
    }
    else
        return{0};
}

int csGraphics::loadImage(int id, wchar_t*path, POINT position, SIZE size)
{
    CSGRAPHIC_CONTEXT gc = csGraphics::getImageGraphicContextResizedW(path, size);
        CSLOADED_IMAGE li = {0};
        li.outPos = position;
        li.size = gc.sz;
        li.inSize = gc.sz;
        li.outSize = gc.sz;
        li.dc = gc.dc;
        li.outDc = gc.dc;
        li.bmp = gc.hbmp;
        li.outBmp = gc.hbmp;
        //li.show = 1;
        loadedImage[id].push_back(li);

        int idi = loadedImage[id].size()-1;

        CSGRAPHIC_ENTITY gi;
        gi.group = 0;
        gi.tid = idi;
        gi.type = CSG_ENTITY_IMAGE;

        entity[id].push_back(gi);

    return entity[id].size()-1;
}

void csGraphics::showImage(int id, int idEnt, POINT posCrop, SIZE sizeCrop, bool show)
{
    int idImg = entity[id][idEnt].tid;
    CSLOADED_IMAGE& li = loadedImage[id][idImg];
    if(sizeCrop.cx != 0 && sizeCrop.cy != 0)
        li.inSize = sizeCrop;
    li.inPos = posCrop;

    li.show = show;
    bltUpdate[id] = 1;
    InvalidateRect(SECTION[id], &bltRect[id],1);
}

POINT csGraphics::getImagePos(int id, int idEnt)
{
    int idImg = entity[id][idEnt].tid;
    return loadedImage[id][idImg].outPos;
}

SIZE csGraphics::getImageOriginalSize(int id, int idEnt)
{
    int idImg = entity[id][idEnt].tid;
    return loadedImage[id][idImg].size;
}

SIZE csGraphics::getImageSize(int id, int idEnt)
{
    int idImg = entity[id][idEnt].tid;
    return loadedImage[id][idImg].outSize;
}

void csGraphics::setImagePos(int id, int idEnt, POINT pos)
{
    int idImg = entity[id][idEnt].tid;
    loadedImage[id][idImg].outPos = pos;
}

void csGraphics::setImageSize(int id, int idEnt, SIZE size)
{
    int idImg = entity[id][idEnt].tid;
    loadedImage[id][idImg].outSize = size;
}

void csGraphics::setMap(int id, SIZE size)
{
    if(size.cx == 0 && size.cy == 0)
    {
        size.cx = hdcontextExtSize[id].cx;
        size.cy = hdcontextExtSize[id].cy;
    }
    
    int n = size.cx*size.cy;

    entityMap[id].ids = vector<long>(n,-1);
    entityMap[id].size = size;
}

void csGraphics::mapImageEntity(int id, int idEnt)
{
    
    if(entityMap[id].ids.size())
    {
        int idImg = entity[id][idEnt].tid;
        SIZE size = loadedImage[id][idImg].outSize;
        POINT position = loadedImage[id][idImg].outPos;

        int nx = size.cx;
        int ny = size.cy;
        long l;

        //int n = entityMap[id].size.cx * entityMap[id].size.cy;
        int nxm = entityMap[id].size.cx;
        int nym = entityMap[id].size.cy;
        int px = position.x;
        int py = position.y;

        vector<long>& v = entityMap[id].ids;
        
        for(int j=py; j<ny; j++)
        {
            for(int i=px; i<nx; i++)
            {
                l = j*nx + i;
                v[l] = idEnt;
            }
        }
    }
}


void csGraphics::viewText(int id, wchar_t* text, CSRGBA color, POINT pos, wchar_t* fontName, int fontSize, bool italic, DWORD weight, int width)
{
    HDC dc = hdcontextExt[id];
    SetBkMode(dc,TRANSPARENT);
    HFONT hf = CreateFontW(CSUTILS::getAdjustedFontSizeX(fontSize),
                            0,
                            0, 0, weight,italic,0,0,DEFAULT_CHARSET,
                            OUT_DEFAULT_PRECIS,CLIP_STROKE_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH|FF_DONTCARE, fontName);
    SelectFont(dc,hf);
    SetTextColor(dc,RGB(color.r, color.g, color.b));
    if(width)
    {
        vector<wchar_t*> tlines;
        
        int h0 = CSUTILS::unilineTextToPolylineText(text, hf, width, tlines);
        int d = pos.y, n = tlines.size();
        for(int i=0; i<n; i++)
        {
            TextOutW(dc,pos.x,d,(LPCWSTR)tlines[i],wcslen(tlines[i]));
            free(tlines[i]);
            d += h0;
        }
        tlines.clear();
    }
    else
        TextOutW(dc,pos.x,pos.y,text,wcslen(text));
    DeleteFont(hf);

}

void csGraphics::setDynamicSimpleText(int id, CSDYNAMIC_SIMPLE_TEXT dst)
{
    dynSimpleText[id] = dst;
    bltUpdate[id] = 1;
    //dynSimpleText[id].view = 1;
}

extern float dimFact;
void viewDynamicSimpleText(int id, vector<CSTEXT> paragraph, vector<int> pSpace, RECT marg, bool updateGASize)
{
    int n = paragraph.size();
    HFONT hf[n];

    int len = (bltRect[id].right-bltRect[id].left)/hZoom[id];
    //int len = (bltRect[id].right-bltRect[id].left);
    if(len > hdcontextExtSize[id].cx) len = hdcontextExtSize[id].cx;

    SIZE sz = {len-marg.left-marg.right};
    vector<wchar_t*> lines[n];
    int h0[n];


    for(int i=0; i<n; i++)
    {
        hf[i] = CreateFontW(CSUTILS::getAdjustedFontSizeX(paragraph[i].FontSize.cx),0,0,paragraph[i].Underline,
                            paragraph[i].Bold, paragraph[i].Italic,FALSE,FALSE,DEFAULT_CHARSET,
                          OUT_DEFAULT_PRECIS, CLIP_MASK, CLEARTYPE_QUALITY,
                          DEFAULT_PITCH, paragraph[i].Font);

        
        h0[i] = CSUTILS::unilineTextToPolylineText(paragraph[i].Text, hf[i], sz.cx, lines[i]);
        h0[i] += 1;

        sz.cy += h0[i]*lines[i].size() + pSpace[i]*dimFact;
    }

    CSGRAPHIC_CONTEXT dc = csGraphics::createGraphicContext(sz);

    if(updateGASize)
    {
        csGraphics::setGraphicAreaXSize(id, RECTCL[id].right);
        //if(sz.cy + marg.top > hdcontextExtSize[id].cy)
        int l = sz.cy + marg.top;
        if(l < bltRect[id].bottom-bltRect[id].top) l = bltRect[id].bottom-bltRect[id].top;
        csGraphics::setGraphicAreaYSize(id, l);
        csGraphics::updateGraphicArea(id, 1);
        SendMessage(SECTION[id], WM_SIZE, 0, 0);
    }
    BitBlt(dc.dc, 0,0, sz.cx, sz.cy, hdcontextExt[id], marg.left, marg.top, SRCCOPY);

    int d = 0;
    for(int i=0; i<n; i++)
    {
        SelectFont(dc.dc, hf[i]);
        SetBkMode(dc.dc,TRANSPARENT);
        SetTextColor(dc.dc, RGB(paragraph[i].Color.r, paragraph[i].Color.g, paragraph[i].Color.b));

        int ls = lines[i].size();
        for(int j=0; j<ls; j++)
        {
            TextOutW(dc.dc,0,d,(LPCWSTR)lines[i][j],wcslen(lines[i][j]));
            free(lines[i][j]);
            d += h0[i];
        }
        d += pSpace[i]*dimFact;
        lines[i].clear();
        DeleteFont(hf[i]);

    }
    DeleteBitmap(dc.hbmp);
    //BitBlt(hdcontextExt[id],marg.left, marg.top, sz.cx, sz.cy, dc.dc, 0,0, SRCCOPY);
    BitBlt(hdcontextExt[id],marg.left, marg.top, sz.cx, sz.cy, dc.dc, 0,0, SRCCOPY);
    DeleteDC(dc.dc);
}


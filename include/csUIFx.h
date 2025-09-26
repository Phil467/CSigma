#ifndef CSUIFX_H
#define CSUIFX_H

#include <windows.h>
#include <vector>
#include <iostream>
#include <thread>
#include "csGraphics.h"

using namespace std;

#define CS_GRAD_COLOR 1


namespace CSUIFX
{
void setBackgroundColorGradient(int id, CSRGBA color1, CSRGBA color2, unsigned int gradientNumber, unsigned int timeStep);
void setBorderColorGradient(int id, CSRGBA color1, CSRGBA color2, unsigned int gradientNumber, unsigned int timeStep);
void setTitleColorGradient(int id, CSRGBA color1, CSRGBA color2, unsigned int gradientNumber, unsigned int timeStep);
void setImageGradient(int id, char*path1, char*path2, POINT position, SIZE size, float zoom, unsigned int gradientNumber, unsigned int timeStep);
void setBackgroundColorClickEffect(int id, CSRGBA color);
void setBorderColorClickEffect(int id, CSRGBA color);
void setTitleColorClickEffect(int id, CSRGBA color);

CSMOUSE_HOVER_EFFECT* getMouseHoverEffectPtr(int id);


void _mouseHover_colorGradient(HWND hwnd, int id);
void _mouseHoverLeave(HWND hwnd, UINT msg, int id);
void _mouseHover_clickEffect(HWND hwnd, UINT msg, int id);
void _mouseHover_movePopup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, int id);
void _autoTransformation(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, int id);
};

#endif // CSUIFX_H

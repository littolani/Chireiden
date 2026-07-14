#pragma once
#include "Chireiden.h"

class TextHelper
{
public:
    static bool invertTextureAlpha(int height);

    static bool drawTextHelper_00443860(int height);

    static void drawTextBorder(
        RECT* pDestRect,
        int xPos,
        int fontSize,
        COLORREF colorRef,
        const char* string,
        IDirect3DTexture9* tex,
        int style);
};
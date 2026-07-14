#include "Globals.h"
#include "TextHelper.h"

bool TextHelper::invertTextureAlpha(int height)
{
    // Pitch * Height = total bytes of the locked texture area
    int totalBytes = g_lockedTexPitch * height;
    uint8_t* pBits = static_cast<uint8_t*>(g_lockedTexBits);

    if (g_lockedTexFormat == D3DFMT_A8R8G8B8)
    {
        // 32-bit: Alpha is the 4th byte (offset 3).
        for (int i = 3; i < totalBytes; i += 4) 
            pBits[i] = ~pBits[i];
        return true;
    }
    
    if (g_lockedTexFormat == D3DFMT_A1R5G5B5)
    {
        if (totalBytes > 0) 
        {
            int numPixels = totalBytes / 2;
            uint16_t* pPixels = reinterpret_cast<uint16_t*>(pBits);
            
            for (int i = 0; i < numPixels; ++i) 
            {
                pPixels[i] ^= 0x8000;
                
                // If the new alpha bit is 0, clear the RGB data entirely
                if ((pPixels[i] & 0x8000) == 0) 
                    pPixels[i] = 0;
            }
        }
        return true;
    }
    
    if (g_lockedTexFormat == D3DFMT_A4R4G4B4) // Handled unlike in the original
    {
        for (int i = 1; i < totalBytes; i += 2) 
            pBits[i] ^= 0xF0;
        return true;
    }

    return false; // Unhandled format
}

bool TextHelper::drawTextHelper_00443860(int height)
{
    return false;
}

void TextHelper::drawTextBorder(
    RECT* pDestRect, 
    int xPos, 
    int fontSize, 
    COLORREF colorRef, 
    const char* string, 
    IDirect3DTexture9* tex, 
    int style)
{
    HFONT fontToUse;
    if (style == 0)
        fontToUse = g_gdiObject_0;
    else if (style == 1)
        fontToUse = g_gdiObject_1;
    else if (style == 2)
        fontToUse = g_gdiObject_2;
    else
        fontToUse = g_gdiObject_3;

    if (fontSize < 17)
        fontSize = 17;

    // The assembly extracts the top 4 bits of the B, G, and R components of the COLORREF
    // and packs them into a 12-bit (0x0BGR) value for what looks like a 16-bit texture format
    uint16_t packedColor = (((colorRef >> 20) & 0xF) << 8) |  // Blue
                           (((colorRef >> 12) & 0xF) << 4) |  // Green
                           ((colorRef >> 4)  & 0xF);          // Red

    uint16_t* destBits = (uint16_t*)g_lockedTexBits;
    for (int bytesCount = 0; bytesCount < g_lockedTexBufferSize; bytesCount += 2)
        *destBits++ = packedColor;

    HFONT oldFont = (HFONT)SelectObject(g_softwareCanvas, fontToUse);
    
    int newSize = (fontSize * 2) + 6;
    invertTextureAlpha(newSize);
    SetBkMode(g_softwareCanvas, TRANSPARENT);
    SetTextColor(g_softwareCanvas, colorRef);
    TextOutA(g_softwareCanvas, xPos * 2, 0, string, strlen(string));
    SelectObject(g_softwareCanvas, oldFont);
    invertTextureAlpha(newSize);
    drawTextHelper_00443860(newSize);
    // SelectObject(g_softwareCanvas, oldFont); // The assembly does this twice

    // Calculate Source Surface Boundaries
    int width = pDestRect->right - pDestRect->left;
    int srcRight = (width * 2) + 22; // lea 0x16(%eax,%eax,1)
    if (srcRight > 1024)
        srcRight = 1024;

    int srcBottom = (fontSize * 2) + 2;

    RECT srcRect;
    srcRect.left = 0;
    srcRect.top = 0;
    srcRect.right = srcRight;
    srcRect.bottom = srcBottom;

    IDirect3DSurface9* outSurface = nullptr;
    tex->GetSurfaceLevel(0, &outSurface);

    D3DXLoadSurfaceFromMemory(
        outSurface,
        NULL,
        pDestRect,
        g_lockedTexBits,
        g_lockedTexFormat,
        g_lockedTexPitch,
        NULL,
        &srcRect,
        D3DX_FILTER_TRIANGLE, 
        0 // ColorKey
    );

    if (outSurface != nullptr)
        outSurface->Release();
}
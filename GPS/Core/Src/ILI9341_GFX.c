#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"

// Draws a single character
void DrawChar(char ch, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor)
{
    if ((ch < 32) || (ch > 126)) return;

    // Get font metrics
    uint8_t fOffset = font[0];
    uint8_t fWidth = font[1];
    uint8_t fHeight = font[2];
    uint8_t fBPL = font[3];

    const uint8_t *tempChar = &font[((ch - 0x20) * fOffset) + 4];

    DrawRectangle(X, Y, fWidth, fHeight, bgcolor);

    // Draw the character pixels
    for (int j = 0; j < fHeight; j++)
    {
        for (int i = 0; i < fWidth; i++)
        {
            uint8_t z = tempChar[fBPL * i + ((j & 0xF8) >> 3) + 1];
            uint8_t b = 1 << (j & 0x07);
            if ((z & b) != 0x00)
            {
                DrawPixel(X + i, Y + j, color);
            }
        }
    }
}

// Draws a text string
void DrawText(const char* str, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor)
{
    uint8_t fWidth = font[1];
    uint16_t startX = X;

    while (*str)
    {
        DrawChar(*str, font, startX, Y, color, bgcolor);
        startX += fWidth; // Move to the next character position
        str++;
    }
}

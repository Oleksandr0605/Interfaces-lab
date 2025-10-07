#include "ILI9341_STM32_Driver.h"

volatile uint16_t LCD_WIDTH  = ILI9341_SCREEN_WIDTH;
volatile uint16_t LCD_HEIGHT = ILI9341_SCREEN_HEIGHT;

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == HSPI_INSTANCE)
    {
        HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
    }
}

static void WriteCommand(uint8_t cmd)
{
    HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(HSPI_INSTANCE, &cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

static void WriteData(uint8_t data)
{
    HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(HSPI_INSTANCE, &data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

static void WriteBuffer_DMA(uint8_t *buffer, uint16_t len)
{
    HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit_DMA(HSPI_INSTANCE, buffer, len);
}

static void SetAddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    WriteCommand(0x2A); // Column Address Set
    WriteData(x1 >> 8);
    WriteData(x1 & 0xFF);
    WriteData(x2 >> 8);
    WriteData(x2 & 0xFF);

    WriteCommand(0x2B); // Page Address Set
    WriteData(y1 >> 8);
    WriteData(y1 & 0xFF);
    WriteData(y2 >> 8);
    WriteData(y2 & 0xFF);

    WriteCommand(0x2C); // Memory Write
}

void Init(void)
{
    // Hardware reset
    HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(10);

    // Initialization sequence
    WriteCommand(0x01);
    HAL_Delay(10);
    WriteCommand(0x11);
    HAL_Delay(120);

    WriteCommand(0x36); // Memory Access Control
    WriteData(0x48);    // Default to horizontal orientation

    WriteCommand(0x3A); // Pixel Format Set
    WriteData(0x55);    // 16 bits per pixel

    WriteCommand(0x29); // Display ON
}

void SetRotation(uint8_t rotation)
{
    WriteCommand(0x36); // Memory Access Control
    switch(rotation)
    {
        case SCREEN_VERTICAL_1:
            WriteData(0x40 | 0x08);
            LCD_WIDTH = 240; LCD_HEIGHT = 320;
            break;
        case SCREEN_HORIZONTAL_1:
            WriteData(0x20 | 0x08);
            LCD_WIDTH = 320; LCD_HEIGHT = 240;
            break;
        default:
            break;
    }
}

void DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    if((x >= LCD_WIDTH) || (y >= LCD_HEIGHT)) return;
    if((x + width - 1) >= LCD_WIDTH) width = LCD_WIDTH - x;
    if((y + height - 1) >= LCD_HEIGHT) height = LCD_HEIGHT - y;

    SetAddress(x, y, x + width - 1, y + height - 1);

    uint32_t total_pixels = width * height;
    uint16_t burst_size;

    // Prepare a buffer for burst transfers
    uint8_t BurstBuffer[BURST_MAX_SIZE];
    for (uint32_t i = 0; i < BURST_MAX_SIZE; i += 2) {
        BurstBuffer[i] = color >> 8;
        BurstBuffer[i+1] = color & 0xFF;
    }

    // Transfer data in bursts
    while (total_pixels > 0)
    {
        burst_size = (total_pixels * 2 > BURST_MAX_SIZE) ? BURST_MAX_SIZE : (total_pixels * 2);
        WriteBuffer_DMA(BurstBuffer, burst_size);
        total_pixels -= (burst_size / 2);
        while (HAL_SPI_GetState(HSPI_INSTANCE) == HAL_SPI_STATE_BUSY_TX); // Wait for DMA to finish
    }
}

void FillScreen(uint16_t color)
{
    DrawRectangle(0, 0, LCD_WIDTH, LCD_HEIGHT, color);
}

void DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if ((x >= LCD_WIDTH) || (y >= LCD_HEIGHT)) return;
    SetAddress(x, y, x, y);
    uint8_t data[] = { color >> 8, color & 0xFF };
    WriteBuffer_DMA(data, 2);
}



#ifndef ST7789_H
#define ST7789_H

#include "TFT_config.h"

#if defined(TFT_DRIVER_ST7789)

#include <stdbool.h>
#include <stdint.h>
#include "../TFT_define.h"

#if defined(TFT_PLATFORM_STM32)
#include "main.h"
#include "spi.h"
#endif

/****************************/
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define ST_CMD_DELAY   0x80    // special signifier for command lists

#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01
#define ST7789_RDDID   0x04
#define ST7789_RDDST   0x09

#define ST7789_SLPIN   0x10
#define ST7789_SLPOUT  0x11
#define ST7789_PTLON   0x12
#define ST7789_NORON   0x13

#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON  0x29
#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_RAMRD   0x2E

#define ST7789_PTLAR   0x30
#define ST7789_COLMOD  0x3A
#define ST7789_MADCTL  0x36

#define ST7789_MADCTL_MY  0x80
#define ST7789_MADCTL_MX  0x40
#define ST7789_MADCTL_MV  0x20
#define ST7789_MADCTL_ML  0x10
#define ST7789_MADCTL_RGB 0x00

#define ST7789_RDID1   0xDA
#define ST7789_RDID2   0xDB
#define ST7789_RDID3   0xDC
#define ST7789_RDID4   0xDD

#define ST77XX_SWRESET        0x01
#define ST77XX_SLPOUT         0x11
#define ST77XX_NORON          0x13
#define ST77XX_INVON          0x21
#define ST77XX_GAMSET         0x26
#define ST77XX_DISPON         0x29
#define ST77XX_CASET          0x2A
#define ST77XX_RASET          0x2B
#define ST77XX_RAMWR          0x2C
#define ST77XX_COLMOD         0x3A
#define ST77XX_MADCTL         0x36
#define ST77XX_DGMEN          0xBA
#define ST77XX_MADCTL_MY      0x80
#define ST77XX_MADCTL_MX      0x40
#define ST77XX_MADCTL_MV      0x20
#define ST77XX_MADCTL_ML      0x10
#define ST77XX_MADCTL_RGB     0x00

// ============================================================================
// ST7789 FRAME RATE CONTROL (FRCTRL2 0xC6) DEFINITIONS
// ============================================================================
#define ST7789_FRCTRL2_119HZ  0x00 // ~120 Hz (Maximum refresh rate)
#define ST7789_FRCTRL2_114HZ  0x01
#define ST7789_FRCTRL2_106HZ  0x02
#define ST7789_FRCTRL2_100HZ  0x03 // 100 Hz
#define ST7789_FRCTRL2_94HZ   0x04
#define ST7789_FRCTRL2_89HZ   0x05 // ~90 Hz
#define ST7789_FRCTRL2_85HZ   0x06
#define ST7789_FRCTRL2_81HZ   0x07 // ~80 Hz
#define ST7789_FRCTRL2_77HZ   0x08
#define ST7789_FRCTRL2_74HZ   0x09 // ~75 Hz
#define ST7789_FRCTRL2_71HZ   0x0A
#define ST7789_FRCTRL2_69HZ   0x0B
#define ST7789_FRCTRL2_66HZ   0x0C
#define ST7789_FRCTRL2_64HZ   0x0D
#define ST7789_FRCTRL2_62HZ   0x0E
#define ST7789_FRCTRL2_60HZ   0x0F // Standard default (60 Hz)
#define ST7789_FRCTRL2_50HZ   0x15
#define ST7789_FRCTRL2_39HZ   0x1F

// Translate human-readable ST7789_FRAMERATE (e.g. 120, 100, 60) to register value
#if defined(ST7789_FRAMERATE)
  #if (ST7789_FRAMERATE == 120 || ST7789_FRAMERATE == 119)
    #define ST7789_FRCTRL2_VAL ST7789_FRCTRL2_119HZ
  #elif (ST7789_FRAMERATE == 100)
    #define ST7789_FRCTRL2_VAL ST7789_FRCTRL2_100HZ
  #elif (ST7789_FRAMERATE == 90 || ST7789_FRAMERATE == 89)
    #define ST7789_FRCTRL2_VAL ST7789_FRCTRL2_89HZ
  #elif (ST7789_FRAMERATE == 80 || ST7789_FRAMERATE == 81)
    #define ST7789_FRCTRL2_VAL ST7789_FRCTRL2_81HZ
  #elif (ST7789_FRAMERATE == 75 || ST7789_FRAMERATE == 74)
    #define ST7789_FRCTRL2_VAL ST7789_FRCTRL2_74HZ
  #elif (ST7789_FRAMERATE == 60)
    #define ST7789_FRCTRL2_VAL ST7789_FRCTRL2_60HZ
  #elif (ST7789_FRAMERATE == 50)
    #define ST7789_FRCTRL2_VAL ST7789_FRCTRL2_50HZ
  #else
    #define ST7789_FRCTRL2_VAL (ST7789_FRAMERATE)
  #endif
#elif defined(ST7789_FRCTRL2_CONFIG)
  #define ST7789_FRCTRL2_VAL ST7789_FRCTRL2_CONFIG
#else
  #define ST7789_FRCTRL2_VAL ST7789_FRCTRL2_60HZ // Default backward-compatible 60 Hz
#endif

#endif /* TFT_Driver_ST7789 */

#endif /* _U8G_H */

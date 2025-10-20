
#include "gfxfont.h"
#include "gfx_fonts_includes.h"

GFXFONT gfxfont;

gfxfont.init(&tft);

gfxfont.setFont(&FreeMono9pt7bNew);
gfxfont.set_delta_x(-3);

tft.GotoXY(2, 60);
gfxfont.Puts(str);
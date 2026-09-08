#include "transformation.h"

#include <cmath>
#include "string.h"

bool transformation::pushRotated(TFT * tft ,TFT * tft_src, int16_t angle, int32_t transp) {
	//_xPivot = LCD->TFT_WIDTH / 2;
	//_yPivot = LCD->TFT_HEIGHT / 2;

	// Bounding box parameters
	int16_t min_x = 0;	int16_t min_y = 0;	int16_t max_x = 0;	int16_t max_y = 0;

	// Get the bounding box of this rotated source Sprite relative to Sprite pivot
	if (!getRotatedBounds(tft_src, angle, &min_x, &min_y, &max_x, &max_y))
		return false;

	//Фиксированный буфер вместо VLA (диагональ экрана + запас)
	#define TFT_ROT_LINE_BUF_MAX 720
	if (max_x - min_x + 1 > TFT_ROT_LINE_BUF_MAX) return false;
	static uint16_t sline_buffer[TFT_ROT_LINE_BUF_MAX];

	int32_t xt = min_x - tft->_xPivot;
	int32_t yt = min_y - tft->_yPivot;

	uint32_t xe = (tft_src->LCD->TFT_WIDTH)  << FP_SCALE;
	uint32_t ye = (tft_src->LCD->TFT_HEIGHT) << FP_SCALE;
	uint16_t tpcolor = transp;  // convert to unsigned

	//if (_bpp == 4) tpcolor = _colorMap[transp & 0x0F];

	tpcolor = tpcolor >> 8 | tpcolor << 8; // Working with swapped color bytes

	// Scan destination bounding box and fetch transformed pixels from source Sprite
	for (int32_t y = min_y; y <= max_y; y++, yt++) {
		int32_t x = min_x;

		uint32_t xs = (_cosra * xt  - (_sinra * yt - (tft_src->_xPivot << FP_SCALE))+ (1 << (FP_SCALE - 1)));
		uint32_t ys = (_sinra * xt	+ (_cosra * yt + (tft_src->_yPivot << FP_SCALE))+ (1 << (FP_SCALE - 1)));

		while ((xs >= xe || ys >= ye) && x < max_x) {	x++; xs += _cosra; ys += _sinra; }
		if (x == max_x)	continue;

		uint32_t pixel_count = 0;
		uint16_t *pd;
		do {
			uint16_t rp;
			int32_t xp = xs >> FP_SCALE;
			int32_t yp = ys >> FP_SCALE;

			//if (_bpp == 16)
			//{
			pd = &tft_src->LCD->buffer16[0] + xp + (yp * tft_src->LCD->TFT_WIDTH);
			rp = *pd;//TFT_dest->GetPixel(xp, yp);//*pd;
			//}
			//else
			//  { rp = readPixel(xp, yp); rp = rp>>8 | rp<<8; }

			if (tpcolor == rp) {
				if (pixel_count) {
					for (uint16_t i = 0; i < pixel_count; i++)
						tft->SetPixel(x-pixel_count+i, y, sline_buffer[i]);
					pixel_count = 0; //СБРОС: без него буфер строки переполнялся
				}
			} else {
				sline_buffer[pixel_count++] = rp;
			}
		}

		while (++x < max_x && (xs += _cosra) < xe && (ys += _sinra) < ye);
		if (pixel_count) {
			// TFT window is already clipped, so this is faster than pushImage()
			//_tft->setWindow(x - pixel_count, y, x, y);
			//_tft->pushPixels(sline_buffer, pixel_count);
			//uint16_t *p;
			//p = &LCD->buffer16[0] + x - pixel_count + y * LCD->TFT_WIDTH;
			for (uint16_t i = 0; i < pixel_count; i++)
				//*p++ = sline_buffer[i];
				tft->SetPixel(x-pixel_count+i, y, sline_buffer[i]);

		}
	}

	return true;
}


/***************************************************************************************
** Function name:           setScrollRect
** Description:             Set scroll area within the sprite and the gap fill colour
***************************************************************************************/
// Intentionally not constrained to viewport area
void transformation::setScrollRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
{
  if ((x >= _iwidth) || (y >= _iheight)) return;

  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }

  if ((x + w) > _iwidth ) w = _iwidth  - x;
  if ((y + h) > _iheight) h = _iheight - y;

  if ( w < 1 || h < 1) return;

  _sx = x;
  _sy = y;
  _sw = w;
  _sh = h;

  _scolor = color;
}


/***************************************************************************************
** Function name:           scroll
** Description:             Scroll dx,dy pixels, positive right,down, negative left,up
***************************************************************************************/
void transformation::scroll(int16_t dx, int16_t dy)
{
	  if (abs(dx) >= _sw || abs(dy) >= _sh)
	  {
	    tft->RectangleFilled(_sx, _sy, _sw, _sh, _scolor);
	    return;
	  }

	  // Fetch the scroll area width and height set by setScrollRect()
	  uint32_t w  = _sw - abs(dx);       // line width to copy
	  uint32_t h  = _sh - abs(dy);       // lines to copy
	  int32_t iw  = _iwidth;       // rounded up width of sprite

	  // Fetch the x,y origin set by setScrollRect()
	    int32_t tx = _sx; // to x
	    int32_t fx = _sx; // from x
	    int32_t ty = _sy; // to y
	    int32_t fy = _sy; // from y

	    // Adjust for x delta
	    if (dx <= 0) fx -= dx;
	    else tx += dx;

	    // Adjust for y delta
	    if (dy <= 0) fy -= dy;
	    else
	    { // Scrolling down so start copy from bottom
	      ty = ty + _sh - 1; // "To" pointer
	      iw = -iw;          // Pointer moves backwards
	      fy = ty - dy;      // "From" pointer
	    }

	    // Calculate "from y" and "to y" pointers in RAM
	    uint32_t fyp = fx + fy * _iwidth;
	    uint32_t typ = tx + ty * _iwidth;

	    // Now move the pixels in RAM
	    if (tft->LCD->Bit == 16)
	    {
	      while (h--)
	      { // move pixel lines (to, from, byte count)
	        memmove( (&tft->LCD->buffer16[0] + typ) ,  (&tft->LCD->buffer16[0] + fyp), w<<1);
	        typ += iw;
	        fyp += iw;
	      }
	    }

	    // Fill the gap left by the scrolling
	    if (dx > 0) tft->RectangleFilled(_sx, _sy, dx, _sh, _scolor);
	    if (dx < 0) tft->RectangleFilled(_sx + _sw + dx, _sy, -dx, _sh, _scolor);
	    if (dy > 0) tft->RectangleFilled(_sx, _sy, _sw, dy, _scolor);
	    if (dy < 0) tft->RectangleFilled(_sx, _sy + _sh + dy, _sw, -dy, _scolor);

}


/***************************************************************************************
 ** Function name:           getRotatedBounds
 ** Description:             Get TFT bounding box of a rotated Sprite wrt pivot
 ***************************************************************************************/
bool transformation::getRotatedBounds(TFT * TFT_src, int16_t angle, int16_t *min_x,
		int16_t *min_y, int16_t *max_x, int16_t *max_y) {

	//TFT_dest->_xPivot = TFT_dest->LCD->TFT_WIDTH / 2;
	//TFT_dest->_yPivot = TFT_dest->LCD->TFT_HEIGHT / 2;

	// Get the bounding box of this rotated source Sprite relative to Sprite pivot
	getRotatedBounds(angle, TFT_src->LCD->TFT_WIDTH, TFT_src->LCD->TFT_HEIGHT,
			TFT_src->_xPivot, TFT_src->_yPivot, min_x, min_y, max_x, max_y);

	// Move bounding box so source Sprite pivot coincides with TFT pivot
	*min_x += tft->_xPivot;
	*max_x += tft->_xPivot;
	*min_y += tft->_yPivot;
	*max_y += tft->_yPivot;

	// Return if bounding box is outside of TFT viewport
	//if (*min_x > _vpW) return false;
	// if (*min_y > _vpH) return false;
	// if (*max_x < _vpX) return false;
//  if (*max_y < _vpY) return false;

	// Clip bounding box to be within TFT viewport
	// if (*min_x < _vpX) *min_x = _vpX;
	// if (*min_y < _vpY) *min_y = _vpY;
	// if (*max_x > _vpW) *max_x = _vpW;
	// if (*max_y > _vpH) *max_y = _vpH;

	return true;
}

/***************************************************************************************
 ** Function name:           rotatedBounds
 ** Description:             Get bounding box of a rotated Sprite wrt pivot
 ***************************************************************************************/
void transformation::getRotatedBounds(int16_t angle, int16_t w, int16_t h, int16_t xp,
		int16_t yp, int16_t *min_x, int16_t *min_y, int16_t *max_x,
		int16_t *max_y) {
	// Trig values for the rotation
	float radAngle = -angle * 0.0174532925; // Convert degrees to radians
	float sina = sin(radAngle);
	float cosa = cos(radAngle);

	w -= xp; // w is now right edge coordinate relative to xp
	h -= yp; // h is now bottom edge coordinate relative to yp

	// Calculate new corner coordinates
	int16_t x0 = -xp * cosa - yp * sina;
	int16_t y0 = xp * sina - yp * cosa;

	int16_t x1 = w * cosa - yp * sina;
	int16_t y1 = -w * sina - yp * cosa;

	int16_t x2 = h * sina + w * cosa;
	int16_t y2 = h * cosa - w * sina;

	int16_t x3 = h * sina - xp * cosa;
	int16_t y3 = h * cosa + xp * sina;

	// Find bounding box extremes, enlarge box to accomodate rounding errors
	*min_x = x0 - 2;
	if (x1 < *min_x)
		*min_x = x1 - 2;
	if (x2 < *min_x)
		*min_x = x2 - 2;
	if (x3 < *min_x)
		*min_x = x3 - 2;

	*max_x = x0 + 2;
	if (x1 > *max_x)
		*max_x = x1 + 2;
	if (x2 > *max_x)
		*max_x = x2 + 2;
	if (x3 > *max_x)
		*max_x = x3 + 2;

	*min_y = y0 - 2;
	if (y1 < *min_y)
		*min_y = y1 - 2;
	if (y2 < *min_y)
		*min_y = y2 - 2;
	if (y3 < *min_y)
		*min_y = y3 - 2;

	*max_y = y0 + 2;
	if (y1 > *max_y)
		*max_y = y1 + 2;
	if (y2 > *max_y)
		*max_y = y2 + 2;
	if (y3 > *max_y)
		*max_y = y3 + 2;

	_sinra = round(sina * (1 << FP_SCALE));
	_cosra = round(cosa * (1 << FP_SCALE));
}





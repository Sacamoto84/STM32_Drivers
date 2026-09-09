#include "transformation.h"

#include <cmath>
#include "string.h"

bool transformation::pushRotated(TFT * tft ,TFT * tft_src, int16_t angle, int32_t transp) {
	//Прямое чтение buffer16 источника - поддерживается только 16 бит
	if (tft_src->LCD->Bit != 16) return false;

	//_xPivot = LCD->TFT_WIDTH / 2;
	//_yPivot = LCD->TFT_HEIGHT / 2;

	// Bounding box parameters
	int16_t min_x = 0;	int16_t min_y = 0;	int16_t max_x = 0;	int16_t max_y = 0;

	// Get the bounding box of this rotated source Sprite relative to Sprite pivot
	if (!getRotatedBounds(tft_src, angle, &min_x, &min_y, &max_x, &max_y))
		return false;

	int32_t xt = min_x - tft->_xPivot;
	int32_t yt = min_y - tft->_yPivot;

	uint32_t xe = (tft_src->LCD->TFT_WIDTH)  << FP_SCALE;
	uint32_t ye = (tft_src->LCD->TFT_HEIGHT) << FP_SCALE;
	uint16_t tpcolor = (uint16_t)transp;  // convert to unsigned

	// Scan destination bounding box and fetch transformed pixels from source Sprite
	for (int32_t y = min_y; y <= max_y; y++, yt++) {
		int32_t x = min_x;

		uint32_t xs = (_cosra * xt  - (_sinra * yt - (tft_src->_xPivot << FP_SCALE))+ (1 << (FP_SCALE - 1)));
		uint32_t ys = (_sinra * xt	+ (_cosra * yt + (tft_src->_yPivot << FP_SCALE))+ (1 << (FP_SCALE - 1)));

		while ((xs >= xe || ys >= ye) && x < max_x) {	x++; xs += _cosra; ys += _sinra; }
		if (x == max_x)	continue;

		do {
			int32_t xp = xs >> FP_SCALE;
			int32_t yp = ys >> FP_SCALE;

			uint16_t rp = tft_src->LCD->buffer16[xp + (yp * tft_src->LCD->TFT_WIDTH)];
			if (rp != tpcolor) {
				tft->SetPixel(x, y, rp);
			}
		} while (++x < max_x && (xs += _cosra) < xe && (ys += _sinra) < ye);
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
	    else if (tft->LCD->Bit == 8)
	    {
	      //8 бит: 1 байт на пиксель - строки можно двигать memmove
	      while (h--)
	      {
	        memmove( (&tft->LCD->buffer8[0] + typ) ,  (&tft->LCD->buffer8[0] + fyp), w);
	        typ += iw;
	        fyp += iw;
	      }
	    }
	    else
	    {
	      //1/4 бита: пиксели не выровнены по байтам - копируем попиксельно.
	      //Раньше для этих глубин пиксели вообще не двигались, а заливка
	      //"зазора" выполнялась - экран просто портился.
	      //Направление обхода строк (iw может быть отрицательным при
	      //скролле вниз) задают typ/fyp, как и в путях выше
	      while (h--)
	      {
	        int32_t tox   = (int32_t)(typ % _iwidth);
	        int32_t toy   = (int32_t)(typ / _iwidth);
	        int32_t fromx = (int32_t)(fyp % _iwidth);
	        int32_t fromy = (int32_t)(fyp / _iwidth);
	        if (fromx < tox) {
	          for (int32_t i = (int32_t)w - 1; i >= 0; i--)
	            tft->SetPixel(tox + i, toy, tft->GetPixel(fromx + i, fromy));
	        } else {
	          for (uint32_t i = 0; i < w; i++)
	            tft->SetPixel(tox + (int32_t)i, toy, tft->GetPixel(fromx + (int32_t)i, fromy));
	        }
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
	int16_t mnx = x0;
	if (x1 < mnx) mnx = x1;
	if (x2 < mnx) mnx = x2;
	if (x3 < mnx) mnx = x3;
	*min_x = mnx - 2;

	int16_t mxx = x0;
	if (x1 > mxx) mxx = x1;
	if (x2 > mxx) mxx = x2;
	if (x3 > mxx) mxx = x3;
	*max_x = mxx + 2;

	int16_t mny = y0;
	if (y1 < mny) mny = y1;
	if (y2 < mny) mny = y2;
	if (y3 < mny) mny = y3;
	*min_y = mny - 2;

	int16_t mxy = y0;
	if (y1 > mxy) mxy = y1;
	if (y2 > mxy) mxy = y2;
	if (y3 > mxy) mxy = y3;
	*max_y = mxy + 2;

	_sinra = round(sina * (1 << FP_SCALE));
	_cosra = round(cosa * (1 << FP_SCALE));
}





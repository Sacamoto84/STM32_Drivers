#include <stdio.h>
#include <string.h>
#include "main.h"

extern int CDC_Transmit_FS(unsigned char* Buf, uint16_t Len);

void Terminal_cls(void)
{
	char str_tx[21];
	sprintf(str_tx,"\033[2J");
	CDC_Transmit_FS((unsigned char*)str_tx, strlen(str_tx));
}

void Terminal_locate(int column, int row) {
	  char str_tx[21];
    // Cursor Home    <ESC>[{ROW};{COLUMN}H
  	sprintf(str_tx,"\033[%d;%dH%c", row + 1, column + 1);
	  CDC_Transmit_FS((unsigned char*)str_tx, strlen(str_tx));
}

static int rgb888tobgr111(int colour) {
    int r = (colour >> 23) & 1;
    int g = (colour >> 15) & 1;
    int b = (colour >> 7) & 1;
    return (b << 2) | (g << 1) | (r << 0);
}
 
void Terminal_foreground(int colour) {
   	char str_tx[21];
    // Set Attribute Mode    <ESC>[{n}m
    // Foreground Colours : 30 + bgr
    int c = 30 + rgb888tobgr111(colour);
    sprintf(str_tx,"\033[%dm", c);
	  CDC_Transmit_FS((unsigned char*)str_tx, strlen(str_tx));
}
 
void Terminal_background(int colour) {
	  char str_tx[21];
    // Set Attribute Mode    <ESC>[{n}m
    // Background Colours : 40 + bgr
    int c = 40 + rgb888tobgr111(colour);
    sprintf(str_tx,"\033[%dm", c);
	  CDC_Transmit_FS((unsigned char*)str_tx, strlen(str_tx));
}
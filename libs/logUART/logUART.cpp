#include "logUART.h"

void classLog::setBold(void)
{
	print("\33[01m");
}

void classLog::setItalic(void)
{
	print("\33[03m");
}

void classLog::setUnderline(void)
{
	print("\33[04m");
}

void classLog::setRevers(void)
{
	print("\33[07m");
}

void classLog::setFlash(void)
{
	print("\33[08m");
}

void classLog::color(int color)
{
	char s[16];
	sprintf(s, "\33[38;05;%dm", color);
	print(s);
}

void classLog::bgcolor(int color)
{
	char s[16];
	sprintf(s, "\33[48;05;%dm", color);
	print(s);
}

void classLog::colorString(int colorValue, char const *const format)
{
	color(colorValue);
	print(format);
	print("\33[0m");
}

void classLog::colorString(int color, int bgcolor, char const *const format)
{
	char s[32];
	sprintf(s, "\33[38;05;%d;48;05;%dm", color, bgcolor);
	print(s);
	print(format);
	print("\33[0m");
}

void classLog::colorStringln(int colorValue, char const *const format)
{
	color(colorValue);
	print(format);
	print("\33[0m\n");
}

void classLog::colorStringln(int color, int bgcolor, char const *const format)
{
	char s[32];
	sprintf(s, "\33[38;05;%d;48;05;%dm", color, bgcolor);
	print(s);
	print(format);
	print("\33[0m\n");
}

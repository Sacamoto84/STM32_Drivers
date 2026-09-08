#include "timber.h"

Timber timber;

void Timber::setBold(void)
{
	print("\33[01m");
}

void Timber::setItalic(void)
{
	print("\33[03m");
}

void Timber::setUnderline(void)
{
	print("\33[04m");
}

void Timber::setRevers(void)
{
	print("\33[07m");
}

void Timber::setFlash(void)
{
	print("\33[08m");
}

void Timber::color(int color)
{
	char s[16];
	sprintf(s, "\33[38;05;%dm", color);
	print(s);
}

void Timber::bgcolor(int color)
{
	char s[16];
	sprintf(s, "\33[48;05;%dm", color);
	print(s);
}

void Timber::colorString(int colorValue, char const *const format)
{
	color(colorValue);
	print(format);
	print("\33[0m");
}

void Timber::colorString(int color, int bgcolor, char const *const format)
{
	char s[32];
	sprintf(s, "\33[38;05;%d;48;05;%dm", color, bgcolor);
	print(s);
	print(format);
	print("\33[0m");
}

void Timber::colorStringln(int colorValue, char const *const format)
{
	color(colorValue);
	print(format);
	print("\33[0m\n");
}

void Timber::colorStringln(int color, int bgcolor, char const *const format)
{
	char s[32];
	sprintf(s, "\33[38;05;%d;48;05;%dm", color, bgcolor);
	print(s);
	print(format);
	print("\33[0m\n");
}

/*
 * err_log.c
 *
 *  Created on: 2017年9月17日
 *      Author: cheetah
 *      参   考: http://blog.csdn.net/xfeng88/article/details/6695848
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "err_log.h"

void write_log_file(char *msg);
void printch(char ch);
void printdec(int dec);
void printflt(double flt);
void printbin(int bin);
void printhex(int hex);
void printstr(char* str);

#define console_print(ch) putchar(ch)

/*
 * 写log到日志文件
 */
void write_log_file(char *msg)
{

}

/*
 * 输出日志或者错误
 */
void err_log(char* fmt, ...)
{
	double vargflt = 0;
	int vargint = 0;
	char* vargpch = NULL;
	char vargch = 0;
	char* pfmt = NULL;
	va_list vp;
	va_start(vp, fmt);
	pfmt = fmt;
	while (*pfmt)
	{
		if (*pfmt == '%')
		{
			switch (*(++pfmt))
			{
			case 'c':
				vargch = va_arg(vp, int);
				/*    va_arg(ap, type), if type is narrow type (char, short, float) an error is given in strict ANSI
				 mode, or a warning otherwise.In non-strict ANSI mode, 'type' is allowed to be any expression. */
				printch(vargch);
				break;
			case 'd':
			case 'i':
				vargint = va_arg(vp, int);
				printdec(vargint);
				break;
			case 'f':
				vargflt = va_arg(vp, double);
				/*    va_arg(ap, type), if type is narrow type (char, short, float) an error is given in strict ANSI
				 mode, or a warning otherwise.In non-strict ANSI mode, 'type' is allowed to be any expression. */
				printflt(vargflt);
				break;
			case 's':
				vargpch = va_arg(vp, char*);
				printstr(vargpch);
				break;
			case 'b':
			case 'B':
				vargint = va_arg(vp, int);
				printbin(vargint);
				break;
			case 'x':
			case 'X':
				vargint = va_arg(vp, int);
				printhex(vargint);
				break;
			case '%':
				printch('%');
				break;
			default:
				break;
			}
			pfmt++;
		}
		else
		{
			printch(*pfmt++);
		}
	}
	va_end(vp);
}

/*
 * 输出内容到console
 */
void printch(char ch)
{
	console_print(ch);
}

void printdec_do(int dec)
{
	if (dec == 0)
	{
		return;
	}
	printdec(dec / 10);
	printch((char) (dec % 10 + '0'));
}

void printdec(int dec)
{
	if(dec < 0)
	{
		dec = -dec;
		printch('-');
		printdec_do(dec);
	}
	else
	{
		printdec_do(dec);
	}
}

void printflt(double flt)
{
	int tmpint = 0;
	tmpint = (int) flt;
	printdec(tmpint);
	printch('.');
	flt = flt - tmpint;
	tmpint = (int) (flt * 1000000);
	printdec(tmpint);
}

void printstr(char* str)
{
	while (*str)
	{
		printch(*str++);
	}
}

void printbin(int bin)
{
	if (bin == 0)
	{
		printstr("0b");
		return;
	}
	printbin(bin / 2);
	printch((char) (bin % 2 + '0'));
}

void printhex(int hex)
{
	if (hex == 0)
	{
		printstr("0x");
		return;
	}
	printhex(hex / 16);
	if (hex < 10)
	{
		printch((char) (hex % 16 + '0'));
	}
	else
	{
		printch((char) (hex % 16 - 10 + 'a'));
	}
}

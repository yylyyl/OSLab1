#include "string.h"
#include "assert.h"
#include "common.h"
#include "stdarg.h"

/*
 * use putchar_func to print a string
 *   @putchar_func is a function pointer to print a character
 *   @format is the control format string (e.g. "%d + %d = %d")
 *   @data is the address of the first variable argument
 * please implement it.
 */
int
vfprintf(void (*putchar_func)(char), const char *format, va_list arg) {
	const char *s, *s2;
	int num = 0;
	for (s = format; *s; s ++) {
		if(*s=='%') {
			s ++;
			switch (*s) {
				case 'd':
					assert(arg);
					num += vfprintf(putchar_func, itoa(va_arg(arg, int), 10), (va_list)NULL);
					break;
				case 'x':
					assert(arg);
					num += vfprintf(putchar_func, utoa(va_arg(arg, int), 16), (va_list)NULL);
					break;
				case 's':
					assert(arg);
					for(s2=va_arg(arg, char*); *s2; s2 ++) {
						putchar_func(*s2);
						num++;
					}
					break;
				case 'c':
					assert(arg);
					putchar_func(va_arg(arg, char));
					num ++; 
					break;
				case '%':
					putchar_func(*s);
					num ++;
					break;
				case 0:
					//it ends?! wtf!
					s --;
					break;
				default:
					//not implemented
					putchar_func(*s);
					num ++;
			}
		} else {
			putchar_func(*s);
			num ++;
		}
	}
	return num;
}


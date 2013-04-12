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
	const char *s;
	for (s = format; *s; s ++) {
		if(*s=='%') {
			s ++;
			switch (*s) {
				case 'd':
					vfprintf(putchar_func, itoa(va_ar (arg, int), 10), (va_list)NULL);
					break;
				case 'x':
					vfprintf(putchar_func, utoa(va_arg(arg, int), 16), (va_list)NULL);
					break;
				case 's':
					vfprintf(putchar_func, va_arg(arg, char*), (va_list)NULL);
					break;
				case 'c':
					putchar_func(va_arg(arg, char));
					break;
				case 0:
					//it ends?! wtf!
					s --;
					break;
				default:
					//not implemented
					putchar_func(*s);
			}
		} else {
			putchar_func(*s);
		}
	}
	return 0;
}


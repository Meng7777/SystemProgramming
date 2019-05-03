/*
 *  linux/kernel/vsprintf.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

/* vsprintf.c -- Lars Wirzenius & Linus Torvalds. */
/*
 * Wirzenius wrote this portably, Torvalds fucked it up :-)
 */
 
#include <stdarg.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/ctype.h>

unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base)
{
	unsigned long result = 0,value;

	if (!base) {
		base = 10;
		if (*cp == '0') {
			base = 8;
			cp++;
			if ((*cp == 'x') && isxdigit(cp[1])) {
				cp++;
				base = 16;
			}
		}
	}
	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
	    ? toupper(*cp) : *cp)-'A'+10) < base) {
		result = result*base + value;
		cp++;
	}
	if (endp)
		*endp = (char *)cp;
	return result;
}

/* we use this so that we can do without the ctype library */
#define is_digit(c)	((c) >= '0' && (c) <= '9')  //判断给定字符是否是0-9的数字

static int skip_atoi(const char **s)  //将给定数字字符串转换成整型
{
	int i=0;

	while (is_digit(**s))
		i = i*10 + *((*s)++) - '0';
	return i;
}  //判断字符是否是数字，如果是就累加

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define SMALL	64		/* use 'abcdef' instead of 'ABCDEF' */

#define do_div(n,base) ({ \
int __res; \
__asm__("divl %4":"=a" (n),"=d" (__res):"0" (n),"1" (0),"r" (base)); \
__res; })
//内嵌汇编，定义除法运算，输出寄存器是eax和edx,一个存放商一个存放余数，除数是base，被除数放在eax中

static char * number(char * str, int num, int base, int size, int precision
	,int type)  //将指定数字转换成不同格式的数字字符串
{
	char c,sign,tmp[36];
	const char *digits="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i;

	if (type&SMALL) digits="0123456789abcdefghijklmnopqrstuvwxyz";  //如果是小写则吧数字字母转换表改写
	if (type&LEFT) type &= ~ZEROPAD;  //如果是左对齐，则需要吧ZEROPAD标志去掉
	if (base<2 || base>36)  //检查基数范围是否正确
		return 0;
	c = (type & ZEROPAD) ? '0' : ' ' ;  //如果设置了标志位ZEROPAD，则表示用0去填充他，否则用空格
	if (type&SIGN && num<0) {
		sign='-';
		num = -num;  //如果设置了符号位，并且数字是负数，需要加上符号字符
	} else
		sign=(type&PLUS) ? '+' : ((type&SPACE) ? ' ' : 0);  //如果标志位PLUS设置，需要在正数前面加上+号，否则如果设置了SPACE，则在符号为用空格填充，要不就用0填充
	if (sign) size--;  //如果符号位被设置成非0值，需要将size大小减去1
	if (type&SPECIAL)
		if (base==16) size -= 2;
		else if (base==8) size--;  //如果设置了SPECIAL标志位，需要在八进制前面加0字符，需要在十六进制前面加0x字符串，并相应的调整size的大小
	i=0;
	if (num==0)
		tmp[i++]='0';
	else while (num!=0)
		tmp[i++]=digits[do_div(num,base)];  //将数值部分转换成字符串存入tmp中
	if (i>precision) precision=i;  //如果超过了精度规定的长度，则充值精度值
	size -= precision;  //调整size的值 
	if (!(type&(ZEROPAD+LEFT)))
		while(size-->0)
			*str++ = ' ';  //如果既没设置ZEROPAD标志，也没设置LEFT标志，将剩下是部分用空格填充，也就是将字符串最开始的size个字符位设置为空格
	if (sign)
		*str++ = sign;  //设置符号位
	if (type&SPECIAL)
		if (base==8)
			*str++ = '0';
		else if (base==16) {
			*str++ = '0';
			*str++ = digits[33];
		}  //如果是八进制或则十六进制，需要设置0或则0x字符
	if (!(type&LEFT))
		while(size-->0)
			*str++ = c;  //如果没有设置左对齐，则需要进行填充
	while(i<precision--)
		*str++ = '0';  //填充精度范围部分，用‘0’进行填充
	while(i-->0)
		*str++ = tmp[i];  //设置数字部分
	while(size-->0)
		*str++ = ' ';  //用空格填剩余部分
	return str;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
	int len;
	int i;
	char * str;
	char *s;
	int *ip;

	int flags;		/* flags to number() */

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
				   number of chars for from string */
	int qualifier;		/* 'h', 'l', or 'L' for integer fields */
//格式化输出：%[flags][width][.precision][|h|l|L][type]
	for (str=buf ; *fmt ; ++fmt) {  //对格式字符串进行扫描，把va_list中的数进行格式化，存入str中
		if (*fmt != '%') {  //如果不是%号，那么仅做简单的赋值
			*str++ = *fmt;
			continue;
		}
			
		/* process flags */
		flags = 0;
		repeat:
			++fmt;		/* this also skips first '%' */
			switch (*fmt) {
				case '-': flags |= LEFT; goto repeat;
				case '+': flags |= PLUS; goto repeat;
				case ' ': flags |= SPACE; goto repeat;
				case '#': flags |= SPECIAL; goto repeat;
				case '0': flags |= ZEROPAD; goto repeat;
				}  //如果是%，判断flags,并设置相应的位
		
		/* get field width */
		field_width = -1;
		if (is_digit(*fmt))  //判断下一个字符是不是数字，如果是则表示域宽
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			/* it's the next argument */
			field_width = va_arg(args, int);  //如果是*,表示下一个参数制定域宽，用va_arg来取得下一个参数值
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;  //取得精度
		if (*fmt == '.') {
			++fmt;	
			if (is_digit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				/* it's the next argument */
				precision = va_arg(args, int);
			}  //如果下一个字符是‘.’，表示有精度值，同样精度值如果是一个数字就直接表示精度了，如果是字符*，表示从参数列表中取得精度
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;  //取修饰符
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
			qualifier = *fmt;
			++fmt;
		}

		switch (*fmt) {  //最后取得格式
		case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0)
					*str++ = ' ';  //如果不是左对齐在前面填充域宽个空格
			*str++ = (unsigned char) va_arg(args, int);  //接着填充参数列表的值
			while (--field_width > 0)
				*str++ = ' ';  //如果是左对齐，域宽的填充应放在后面
			break;

		case 's':  //如果格式化成字符串
			s = va_arg(args, char *);  //取参数列表中的字符串
			if (!s)
				s = "<NULL>";
			len = strlen(s);
			if (precision < 0)
				precision = len;
			else if (len > precision)
				len = precision;  //对精度或字符串长度重新赋值

			if (!(flags & LEFT))  //如果不是左对齐
				while (len < field_width--)
					*str++ = ' ';  //在域宽前面填充空格
			for (i = 0; i < len; ++i)
				*str++ = *s++;  //填充字符串
			while (len < field_width--)
				*str++ = ' ';  //如果不是左对齐，需要在后面填充空格
			break;

		case 'o':  //如果格式化成八进制数
			str = number(str, va_arg(args, unsigned long), 8,
				field_width, precision, flags);
			break;

		case 'p':  //如果格式化成指针
			if (field_width == -1) {
				field_width = 8;
				flags |= ZEROPAD;
			}  //如果没有设置域宽，则需要设置域宽为8，0填充标志
			str = number(str,
				(unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags);
			break;

		case 'x':  //如果格式化成十六进制
			flags |= SMALL;
		case 'X':
			str = number(str, va_arg(args, unsigned long), 16,
				field_width, precision, flags);
			break;

		case 'd':  //如果格式化成整型
		case 'i':  //带符号数
			flags |= SIGN;
		case 'u':
			str = number(str, va_arg(args, unsigned long), 10,
				field_width, precision, flags);
			break;

		case 'n':
			ip = va_arg(args, int *);
			*ip = (str - buf);  //将到目前为止转换的字符个数保存到ip指针指定的位置
			break;

		default:
			if (*fmt != '%')
				*str++ = '%';
			if (*fmt)
				*str++ = *fmt;
			else
				--fmt;
			break;
		}  //如果格式转换符不是%，表示有错，将一个%写入，如果格式字符串还没遍历完直接将字符写入，否则退出循环
	}
	*str = '\0';
	return str-buf;
}

int sprintf(char * buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i=vsprintf(buf,fmt,args);
	va_end(args);
	return i;
}


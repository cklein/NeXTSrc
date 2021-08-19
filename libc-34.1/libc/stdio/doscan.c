#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)doscan.c	5.2 (Berkeley) 3/9/86";
#endif /* LIBC_SCCS and not lint */

#include <stdio.h>
#include	<ctype.h>

#define	SPC	01
#define	STP	02

#define	SHORT	0
#define	REGULAR	1
#define	LONG	2
#define	INT	0
#define	FLOAT	1

static char *_getccl(register unsigned char *s);
static _innum(int **ptr, int type, int len, int size, FILE *iop, int *eofptr);
static _instr(register char *ptr, int type, int len, register FILE *iop, int *eofptr);

static char _sctab[256] = {
	0,0,0,0,0,0,0,0,
	0,SPC,SPC,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	SPC,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
};
static int getcount = 0;

_doscan(iop, fmt, argp)
FILE *iop;
register char *fmt;
register int **argp;
{
	register int ch;
	int nmatch, len, ch1;
	int **ptr, fileended, size;

	nmatch = 0;
	fileended = 0;
        getcount = 0;
	for (;;) switch (ch = *fmt++) {
	case '\0': 
		return (nmatch);
	case '%': 
		if ((ch = *fmt++) == '%')
			goto def;
		if (ch == 'n') {
		       **argp++ = getcount;
		       break;
		}
		ptr = 0;
		if (ch != '*')
			ptr = argp++;
		else
			ch = *fmt++;
		len = 0;
		size = REGULAR;
		while (isdigit(ch)) {
			len = len*10 + ch - '0';
			ch = *fmt++;
		}
		if (len == 0)
			len = 30000;
		if (ch=='l') {
			size = LONG;
			ch = *fmt++;
		} else if (ch=='h') {
			size = SHORT;
			ch = *fmt++;
#if	NeXT
		} else if (ch == 'p') {
			size = LONG;
#endif	NeXT			
		} else if (ch=='[')
			fmt = _getccl((unsigned char *)fmt);
		if (isupper(ch)) {
			ch = tolower(ch);
			size = LONG;
		}
		if (ch == '\0')
			return(-1);
		if (_innum(ptr, ch, len, size, iop, &fileended) && ptr)
			nmatch++;
		if (fileended)
			return(nmatch? nmatch: -1);
		break;

	case ' ':
	case '\n':
	case '\t': 
		while ((getcount++, ch1 = getc(iop))==' ' || ch1=='\t' || ch1=='\n')
			continue;
		if (ch1 != EOF) {
			getcount--;
			ungetc(ch1, iop);
		}
		break;

	default: 
	def:
		ch1 = getc(iop);
		getcount++;
		if (ch1 != ch) {
			if (ch1==EOF)
				return(-1);
			ungetc(ch1, iop);
			return(nmatch);
		}
	}
}

static
_innum(int **ptr, int type, int len, int size, FILE *iop, int *eofptr)
{
	extern double atof();
	register char *np;
	char numbuf[256];
	register c, base;
#if	NeXT
	int save;
#endif	NeXT	
	int expseen, scale, negflg, c1, ndigit;
	long lcval;

	if (type=='c' || type=='s' || type=='[')
		return(_instr(ptr? *(char **)ptr: (char *)NULL, type, len, iop, eofptr));
	lcval = 0;
	ndigit = 0;
	scale = INT;
	if (type=='e'||type=='f')
		scale = FLOAT;
	base = 10;
	if (type=='o')
		base = 8;
	else if (type=='x')
		base = 16;
#if	NeXT
	else if (type=='p')
		base = 16;
#endif	NeXT
	np = numbuf;
	expseen = 0;
	negflg = 0;
	while ((getcount++, c = getc(iop))==' ' || c=='\t' || c=='\n')
		continue;
	if (c=='-') {
		negflg++;
		*np++ = c;
		c = getc(iop);
		getcount++;
		len--;
	} else if (c=='+') {
		len--;
		c = getc(iop);
		getcount++;
	}
#if	NeXT
	/*
	 * Check for the leading 0x or 0X.
	 * Leave c pointing to the 'a' in 0xabcd1234.
	 */
	if (c == '0' && base == 16)  {
		save = c;
		c = getc(iop);
		if (c == 'x' || c == 'X')  {
			c = getc(iop);
		}
		else {
			ungetc(c, iop);
			c = save;
		}
	}
#endif	NeXT	
	for ( ; --len>=0; *np++ = c, c = getc(iop), getcount++) {
		if (isdigit(c)
		 || base==16 && ('a'<=c && c<='f' || 'A'<=c && c<='F')) {
			ndigit++;
			if (base==8)
				lcval <<=3;
			else if (base==10)
				lcval = ((lcval<<2) + lcval)<<1;
			else
				lcval <<= 4;
			c1 = c;
			if (isdigit(c))
				c -= '0';
			else if ('a'<=c && c<='f')
				c -= 'a'-10;
			else
				c -= 'A'-10;
			lcval += c;
			c = c1;
			continue;
		} else if (c=='.') {
			if (base!=10 || scale==INT)
				break;
			ndigit++;
			continue;
		} else if ((c=='e'||c=='E') && expseen==0) {
			if (base!=10 || scale==INT || ndigit==0)
				break;
			expseen++;
			*np++ = c;
			c = getc(iop);
			getcount++;
			if (c!='+'&&c!='-'&&('0'>c||c>'9'))
				break;
		} else
			break;
	}
	if (negflg)
		lcval = -lcval;
	if (c != EOF) {
		ungetc(c, iop);
		getcount--;
		*eofptr = 0;
	} else
		*eofptr = 1;
 	if (ptr==NULL || np==numbuf || (negflg && np==numbuf+1) )/* gene dykes*/
		return(0);
	*np++ = 0;
	switch((scale<<4) | size) {

	case (FLOAT<<4) | SHORT:
	case (FLOAT<<4) | REGULAR:
		**(float **)ptr = atof(numbuf);
		break;

	case (FLOAT<<4) | LONG:
		**(double **)ptr = atof(numbuf);
		break;

	case (INT<<4) | SHORT:
		**(short **)ptr = lcval;
		break;

	case (INT<<4) | REGULAR:
		**(int **)ptr = lcval;
		break;

	case (INT<<4) | LONG:
		**(long **)ptr = lcval;
		break;
	}
	return(1);
}

static
_instr(register char *ptr, int type, int len, register FILE *iop, int *eofptr)
{
	register ch;
	register char *optr;
	int ignstp;

	*eofptr = 0;
	optr = ptr;
	if (type=='c' && len==30000)
		len = 1;
	ignstp = 0;
	if (type=='s')
		ignstp = SPC;
	while ((getcount++, ch = getc(iop)) != EOF && _sctab[ch] & ignstp)
		;
	ignstp = SPC;
	if (type=='c')
		ignstp = 0;
	else if (type=='[')
		ignstp = STP;
	while (ch!=EOF && (_sctab[ch]&ignstp)==0) {
		if (ptr)
			*ptr++ = ch;
		if (--len <= 0)
			break;
		ch = getc(iop);
		getcount++;
	}
	if (ch != EOF) {
		if (len > 0) {
			ungetc(ch, iop);
			getcount--;
		}
		*eofptr = 0;
	} else
		*eofptr = 1;
	if (ptr && ptr!=optr) {
		if (type!='c')
			*ptr++ = '\0';
		return(1);
	}
	return(0);
}

static char *
_getccl(register unsigned char *s)
{
	register c, t;

	t = 0;
	if (*s == '^') {
		t++;
		s++;
	}
	for (c = 0; c < (sizeof _sctab / sizeof _sctab[0]); c++)
		if (t)
			_sctab[c] &= ~STP;
		else
			_sctab[c] |= STP;
	if ((c = *s) == ']' || c == '-') {	/* first char is special */
		if (t)
			_sctab[c] |= STP;
		else
			_sctab[c] &= ~STP;
		s++;
	}
	while ((c = *s++) != ']') {
		if (c==0)
			return((char *)--s);
		else if (c == '-' && *s != ']' && s[-2] < *s) {
			for (c = s[-2] + 1; c < *s; c++)
				if (t)
					_sctab[c] |= STP;
				else
					_sctab[c] &= ~STP;
		} else if (t)
			_sctab[c] |= STP;
		else
			_sctab[c] &= ~STP;
	}
	return((char *)s);
}



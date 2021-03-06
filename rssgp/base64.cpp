#include "stdafx.h"
#include "rssgp.h"
#include	<string.h>
#include	<malloc.h>

static unsigned char *base64 = (unsigned char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void enclode_char(unsigned long bb, int srclen, unsigned char *dest, int j)
{
	int					x, i, base;

	/* 最終位置の計算 */
	for ( i = srclen; i < 2; i++ ) 
		bb <<= 8;

	/* BASE64変換 */
	for ( base = 18, x = 0; x < srclen + 2; x++, base -= 6) {
		dest[j++] = base64[ (unsigned long)((bb>>base) & 0x3F) ];
	}

	/* 端数の判断 */
	for ( i = x; i < 4; i++ ) {
		dest[j++] = (unsigned char)'=';		/* 端数 */
	}
	
}

static void enclode(const unsigned char *src, unsigned char *dest)
{
	unsigned	char	*p = (unsigned char *)src;
	unsigned	long	bb = (unsigned long)0;
	int					i = 0, j = 0;

	while (*p)
	{
		bb <<= 8;
		bb |= (unsigned long)*p;

		/* 24bit単位に編集 */
		if (i == 2) {
			enclode_char(bb, i, dest, j);

			j = j + 4;
			i = 0;
			bb = 0;
		} else
			i++;

		p++;
	}

	/* 24bitに満たない場合 */
	if (i)
		enclode_char(bb, i - 1, dest, j);

}

typedef union {
	unsigned int	data;
	unsigned char	xyz[4];
} Union_data_t;

static long codetovalue(unsigned char c)
{
	if( (c >= (unsigned char)'A') && (c <= (unsigned char)'Z') ) {
		return (long)(c - (unsigned char)'A');
	}
	else if( (c >= (unsigned char)'a') && (c <= (unsigned char)'z') ) {
		return ((long)(c - (unsigned char)'a') +26);
	}
	else if( (c >= (unsigned char)'0') && (c <= (unsigned char)'9') ) {
		return ((long)(c - (unsigned char)'0') +52);
	}
	else if( (unsigned char)'+' == c ) {
		return (long)62;
	}
	else if( (unsigned char)'/' == c ) {
		return (long)63;
	}
	else if( (unsigned char)'=' == c ) {
		return (long)0;
	}
	else {
		return -1;
	}
}

int decode_str(int enc_ptr, const unsigned char *src, unsigned char *dest)
{
	int					i, j;
	unsigned	long	base64 = 0;
	unsigned	char	x;
	Union_data_t		bb;

	for (i = enc_ptr; i < enc_ptr + 4; i++) {
		x = (unsigned char)codetovalue(src[i]);
		base64 |= x;
		if ((i - enc_ptr) != 3) base64 <<= 6;
	}

	base64 <<= 8;

	bb.data = base64;

	for(j = 0, i = 3; i >= 1; i--) {
		dest[j++] = bb.xyz[i];
	}

	return 0;
}

int decode(const unsigned char *src, unsigned char *dest)
{
	int					x = 0, i = 0;
	int					srclen;
	char				tmp[4];

	/* NULL pointer */
	if (src == NULL)
		return -1;

	/**/
	srclen = (int)strlen((char*)src);

	if (~srclen % 4)
		return -2;

	printf("srclen = %d, %d\n", srclen, srclen % 4);
	while(srclen) {

		memset(tmp, 0, sizeof(tmp));
		decode_str(i, src, (unsigned char*)tmp);
		printf("i = %d, srclen = %d, tmp = %s\n", i, srclen, tmp);
		strcat((char*)dest, tmp);

		i += 4;
		srclen -= 4;
	}

	printf("¡ú dest = %s\n", dest);

	return 0;
}


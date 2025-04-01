#ifndef MY_STDEMB_H
#define MY_STDEMB_H


#include <stdarg.h>
#include <stddef.h>
#include <string.h>


#define PRINTF_BUFSZ    128
#define LOG_WARN 0
#define LOG_INFO 1
#define NOT_EXEC_LOG 2


// 一般的なマクロ定義
#define	SSIZE( m, s )	sizeof( ((m*)0)->s )		/*要素のバイト数に変換*/
#define	Bcnv( a )		( (a + 1) / 2 )				/*桁数をバイト数に変換*/
#define	_byte( a )		( (a + 1) / 2 )				/*桁数をバイト数に変換*/
#define	DIM( x )		( sizeof(x)/sizeof(x[0]) )	/*配列の要素数を求める*/

#define GET_1_OF_WORD(a)	(((unsigned short)a & 0xFF00) >> 8)		// unsigned short の上１バイトを取り出す
#define GET_2_OF_WORD(a)	((unsigned short)a & 0xFF)				// unsigned short の下１バイトを取り出す
#define GET_WORD(a)			((((unsigned char *)a)[0] << 8) + ((unsigned char *)a)[1])	// unsigned short を取り出す
#define SET_WORD(a,b)		{((unsigned char *)a)[0]=GET_1_OF_WORD((unsigned short)b); ((unsigned char *)a)[1]=GET_2_OF_WORD((unsigned short)b);}	// unsigned short を書き込む

#define GET_1_OF_DWORD(a)	(((unsigned int)a & 0xFF000000) >> 24)	// unsigned int(4バイト) の1バイト目を取り出す
#define GET_2_OF_DWORD(a)	(((unsigned int)a & 0xFF0000) >> 16)	// unsigned int(4バイト) の2バイト目を取り出す
#define GET_3_OF_DWORD(a)	(((unsigned int)a & 0xFF00) >> 8)		// unsigned int(4バイト) の3バイト目を取り出す
#define GET_4_OF_DWORD(a)	((unsigned int)a & 0xFF)				// unsigned int(4バイト) の4バイト目を取り出す

// unsigned intを取り出す
#define GET_DWORD(a)		((((unsigned char *)a)[0] << 24) + (((unsigned char *)a)[1] << 16) + (((unsigned char *)a)[2] << 8) + ((unsigned char *)a)[3])
// unsigned int を書き込む
#define SET_DWORD(a,b)		{((unsigned char *)a)[0]=GET_1_OF_DWORD((unsigned int)b); ((unsigned char *)a)[1]=GET_2_OF_DWORD((unsigned int)b); ((unsigned char *)a)[2]=GET_3_OF_DWORD((unsigned int)b); ((unsigned char *)a)[3]=GET_4_OF_DWORD((unsigned int)b);}


	/*==========================================================
							構造体宣言
	==========================================================*/
typedef struct {
	uint32_t upper;
	uint32_t lower;
} DLONG;

void *my_itoa( long value, void *asc, unsigned short keta );
void *my_ltoa( long value, void *asc, unsigned short keta );
int my_vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);
void syslog(char mode,const char *format, ...);
void dlong_div( DLONG *s, unsigned long dst, DLONG *a );
void dlong_mul( unsigned long src, unsigned long dst,  DLONG *a );

#endif /* MY_STDEMB_H */


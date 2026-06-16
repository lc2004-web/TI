/**
 * @file  oledfont.h
 * @brief OLED font data declarations (ASCII + GB2312 Chinese)
 *
 * Based on OLED-V2.0 by jiangxiekeji.com
 * Ported to MSPM0G3507
 */

#ifndef __OLEDFONT_H
#define __OLEDFONT_H

#include <stdint.h>

/* Charset selection */
//#define OLED_CHARSET_UTF8          /* UTF-8 mode */
#define OLED_CHARSET_GB2312          /* GB2312 mode */

/* Chinese character cell */
typedef struct
{

#ifdef OLED_CHARSET_UTF8
    char Index[5];                   /* UTF-8 code, max 4 bytes + null */
#endif

#ifdef OLED_CHARSET_GB2312
    char Index[3];                   /* GB2312 code, 2 bytes + null */
#endif

    uint8_t Data[32];                /* 16x16 font bitmap */
} ChineseCell_t;

/* ASCII font arrays */
extern const uint8_t OLED_F8x16[][16];
extern const uint8_t OLED_F6x8[][6];

/* Chinese font array */
extern const ChineseCell_t OLED_CF16x16[];

#endif /* __OLEDFONT_H */

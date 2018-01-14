/*
 * myHT1621.h
 *
 * Created: 11.01.2018 21:32:45
 *  Author: Reiner Knapp
 
 Basic driver for a HT1621 LCD Array like available here: http://www.ebay.de/itm/LCD-Module-6-Bit-8-Segment-3-wires-SPI-HT1621-Module-Character-LCD-for-Arduino-D/391868765851?hash=item5b3d32d69b:g:Ug4AAOSw7cxZmp86
 
 Features:
 - Includes 7Seg display for ACSII characters
 - Can buffer data and write whole display content
 - Direct writes supported
 
 Not implemented:
 - Multiple data writes (unnecessary, because of low data volume)
 - Read operation from LCD of uC (the cheap HT1621 LCD modules breakout are missing the RD Pin)
 
 */ 


#ifndef MYHT1621_H_
#define MYHT1621_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "myHT1621PinConfiguration.h"

// Some pre-compiler macros used

// Concat two symbols
#ifndef _concatx
#define _concatx(a,b)			a##b
#endif

#ifndef _concat
#define _concat(a,b)			_concatx(a,b)
#endif

#define pin_high(a,b)	(a|=(1<<b))
#define pin_low(a,b)	(a&=~(1<<b))

#define PORT_(port) PORT ## port
#define DDR_(port)  DDR  ## port
#define PIN_(port)  PIN  ## port

#define PORT(port) PORT_(port)
#define DDR(port)  DDR_(port)
#define PIN(port)  PIN_(port)

// Check, if value is between two values
#ifndef _inRange
#define _inRange(a,b,c)			((a>=b)?((a<=c)?true:false):false)
#endif

#define HT1621_MAX_ROW 16 // 16 Bytes = 32 Nibbles (see data sheet)
//#define HT1621_DATA_MASK 0xff

#define HT1621_DATA_PORT	PORT(HT1621_CONF_DATA_PORT)
#define HT1621_DATA_DDR		DDR(HT1621_CONF_DATA_PORT)
#define HT1621_WR_PORT		PORT(HT1621_CONF_WR_PORT)
#define HT1621_WR_DDR		DDR(HT1621_CONF_WR_PORT)
#define HT1621_CS_PORT		PORT(HT1621_CONF_CS_PORT)
#define HT1621_CS_DDR		DDR(HT1621_CONF_CS_PORT)

#define HT1621_CS_LOW		pin_low (HT1621_CS_PORT,HT1621_CONF_CS_PIN)
#define HT1621_CS_HIGH		pin_high (HT1621_CS_PORT,HT1621_CONF_CS_PIN)
#define HT1621_WR_LOW		pin_low (HT1621_WR_PORT,HT1621_CONF_WR_PIN)
#define HT1621_WR_HIGH		pin_high (HT1621_WR_PORT,HT1621_CONF_WR_PIN)
#define HT1621_DATA_LOW		pin_low (HT1621_DATA_PORT,HT1621_CONF_DATA_PIN)
#define HT1621_DATA_HIGH	pin_high (HT1621_DATA_PORT,HT1621_CONF_DATA_PIN)

#define HT1621_PIN_SETUP	{(HT1621_DATA_DDR|=_BV(HT1621_CONF_DATA_PIN));(HT1621_WR_DDR|=_BV(HT1621_CONF_WR_PIN));(HT1621_CS_DDR|=_BV(HT1621_CONF_CS_PIN));}
#define HT1621_PIN_DEFAULT	{HT1621_CS_HIGH;HT1621_WR_HIGH;HT1621_DATA_HIGH;}

#define HT1621_MODE_READ	0xC0
#define HT1621_MODE_WRITE	0xA0
#define HT1621_MODE_CMD		0x80

#define HT1621_CMD_SYS_DIS		0x00
#define HT1621_CMD_SYS_EN		0x01
#define HT1621_CMD_LCD_ON		0x03
#define HT1621_CMD_LCD_OFF		0x02
#define HT1621_CMD_TIMER_DIS	0x04
#define HT1621_CMD_WDT_DIS		0x05
#define HT1621_CMD_TIMER_EN		0x06
#define HT1621_CMD_WDT_EN		0x07
#define HT1621_CMD_TONE_OFF		0x08
#define HT1621_CMD_TONE_ON		0x09
#define HT1621_CMD_CLR_TIMER	0x0C
#define HT1621_CMD_CLR_WDT		0x0E
#define HT1621_CMD_XTAL_32K		0x14
#define HT1621_CMD_RC_256K		0x18
#define HT1621_CMD_EXT_256K		0x1C
#define HT1621_CMD_BIAS_H_2		0x20
#define HT1621_CMD_BIAS_H_3		0x24
#define HT1621_CMD_BIAS_H_4		0x28
#define HT1621_CMD_BIAS_T_2		HT1621_CMD_BIAS_H_2|0x01
#define HT1621_CMD_BIAS_T_3		HT1621_CMD_BIAS_H_3|0x01
#define HT1621_CMD_BIAS_T_4		HT1621_CMD_BIAS_H_4|0x01
#define HT1621_CMD_TONE_4K		0x40
#define HT1621_CMD_TONE_2K		0x60
#define HT1621_CMD_IRQ_DIS		0x80
#define HT1621_CMD_IRQ_EN		0x88
#define HT1621_CMD_F1			0xA0
#define HT1621_CMD_F2			HT1621_CMD_F1|0x01
#define HT1621_CMD_F4			HT1621_CMD_F1|0x02
#define HT1621_CMD_F8			HT1621_CMD_F1|0x03
#define HT1621_CMD_F16			HT1621_CMD_F1|0x04
#define HT1621_CMD_F32			HT1621_CMD_F1|0x05
#define HT1621_CMD_F64			HT1621_CMD_F1|0x06
#define HT1621_CMD_F128			HT1621_CMD_F1|0x07
#define HT1621_CMD_TEST			0xE0
#define HT1621_CMD_NORMAL		0xE3

// HT1621 Segments
#define HT1621_SEG_DP		0x80
#define HT1621_SEG_A		0x10
#define HT1621_SEG_B		0x20
#define HT1621_SEG_C		0x40
#define HT1621_SEG_D		0x08
#define HT1621_SEG_E		0x04
#define HT1621_SEG_F		0x01
#define HT1621_SEG_G		0x02
#define HT1621_SEG_BAT		0x80
#define HT1621_DIG_BAT_LOW	0x03
#define HT1621_DIG_BAT_MED	0x03
#define HT1621_DIG_BAT_HI	0x03

/*
 * Segments to be switched on for characters and digits on
 * 7-Segment Displays
 */
const static uint8_t HT1621_CHARMAP [] PROGMEM = {
	(HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_C|HT1621_SEG_D|HT1621_SEG_E|HT1621_SEG_F),			// 0 // First 16 Chars are HEX
	(HT1621_SEG_B|HT1621_SEG_C),																	// 1
	(HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_G|HT1621_SEG_E|HT1621_SEG_D),						// 2
	(HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_G|HT1621_SEG_C|HT1621_SEG_D),						// 3
	(HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_B|HT1621_SEG_C),										// 4
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_C|HT1621_SEG_D),						// 5
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_C|HT1621_SEG_D|HT1621_SEG_E),			// 6
	(HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_C),													// 7
	(0x7f),																							// 8
	(HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_C|HT1621_SEG_D),			// 9
	(HT1621_SEG_E|HT1621_SEG_G|HT1621_SEG_F|HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_C),			// A
	(HT1621_SEG_F|HT1621_SEG_E|HT1621_SEG_G|HT1621_SEG_C|HT1621_SEG_D),						// b
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_E|HT1621_SEG_D),										// C
	(HT1621_SEG_E|HT1621_SEG_G|HT1621_SEG_D|HT1621_SEG_C|HT1621_SEG_B),						// d
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_E|HT1621_SEG_D),						// E
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_E),										// F
	(0),(0),(0),(0),(0),(0),(0),(0),(0),(0),(0),(0),(0),(0),(0),(0),(0),(0),						// Chars 16-33 = Space
	(HT1621_SEG_F|HT1621_SEG_B),																	// "
	(0),(0),(0),(0),(HT1621_SEG_B),																// #$%&'
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_E|HT1621_SEG_D),(HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_C|HT1621_SEG_D), // ()
	(0),(0),(HT1621_SEG_DP),(HT1621_SEG_G),(HT1621_SEG_DP),(0),									// *+,-./
	(HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_C|HT1621_SEG_D|HT1621_SEG_E|HT1621_SEG_F),			// 0
	(HT1621_SEG_B|HT1621_SEG_C),																	// 1
	(HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_G|HT1621_SEG_E|HT1621_SEG_D),						// 2
	(HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_G|HT1621_SEG_C|HT1621_SEG_D),						// 3
	(HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_B|HT1621_SEG_C),										// 4
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_C|HT1621_SEG_D),						// 5
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_C|HT1621_SEG_D|HT1621_SEG_E),			// 6
	(HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_C),													// 7
	(0x7f),																							// 8
	(HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_C|HT1621_SEG_D),			// 9
	(0),(0),(0),(HT1621_SEG_G|HT1621_SEG_D),(0),(0),(0),											// :;<=>?@
	(HT1621_SEG_E|HT1621_SEG_G|HT1621_SEG_F|HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_C),			// A
	(HT1621_SEG_F|HT1621_SEG_E|HT1621_SEG_G|HT1621_SEG_C|HT1621_SEG_D),						// b
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_E|HT1621_SEG_D),										// C
	(HT1621_SEG_E|HT1621_SEG_G|HT1621_SEG_D|HT1621_SEG_C|HT1621_SEG_B),						// d
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_E|HT1621_SEG_D),						// E
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_E),										// F
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_C|HT1621_SEG_D|HT1621_SEG_E),			// G
	(HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_E|HT1621_SEG_C|HT1621_SEG_B),						// H
	(HT1621_SEG_C),																				// i
	(HT1621_SEG_B|HT1621_SEG_C|HT1621_SEG_D),													// j
	(HT1621_SEG_F|HT1621_SEG_E|HT1621_SEG_G|HT1621_SEG_C),										// k
	(HT1621_SEG_F|HT1621_SEG_E|HT1621_SEG_D),													// L
	(0),																							// M
	(0),																							// N
	(HT1621_SEG_G|HT1621_SEG_E|HT1621_SEG_D|HT1621_SEG_C),										// o
	(HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_E),						// P
	(HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_C|HT1621_SEG_D|HT1621_SEG_E|HT1621_SEG_F|HT1621_SEG_DP), // Q
	(0),																							// R
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_C|HT1621_SEG_D),						// S
	(HT1621_SEG_B|HT1621_SEG_G|HT1621_SEG_C|HT1621_SEG_D),										// T
	(HT1621_SEG_F|HT1621_SEG_E|HT1621_SEG_D|HT1621_SEG_C|HT1621_SEG_B),						// U
	(0),																							// V
	(0),																							// W
	(HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_E|HT1621_SEG_C|HT1621_SEG_B),						// X
	(0),																							// Y
	(0),																							// Z
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_E|HT1621_SEG_D),(0),(HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_C|HT1621_SEG_D),(0),// [\]^
	(HT1621_SEG_D),(HT1621_SEG_F),																// _`
	(HT1621_SEG_E|HT1621_SEG_G|HT1621_SEG_F|HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_C),			// A
	(HT1621_SEG_F|HT1621_SEG_E|HT1621_SEG_G|HT1621_SEG_C|HT1621_SEG_D),						// b
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_E|HT1621_SEG_D),										// C
	(HT1621_SEG_E|HT1621_SEG_G|HT1621_SEG_D|HT1621_SEG_C|HT1621_SEG_B),						// d
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_E|HT1621_SEG_D),						// E
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_E),										// F
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_C|HT1621_SEG_D|HT1621_SEG_E),			// G
	(HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_E|HT1621_SEG_C|HT1621_SEG_B),						// H
	(HT1621_SEG_C),																				// i
	(HT1621_SEG_B|HT1621_SEG_C|HT1621_SEG_D),													// j
	(HT1621_SEG_F|HT1621_SEG_E|HT1621_SEG_G|HT1621_SEG_C),										// k
	(HT1621_SEG_F|HT1621_SEG_E|HT1621_SEG_D),													// L
	(0),																							// M
	(HT1621_SEG_E|HT1621_SEG_G|HT1621_SEG_C),													// N
	(HT1621_SEG_G|HT1621_SEG_E|HT1621_SEG_D|HT1621_SEG_C),										// o
	(HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_E),						// P
	(HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_C|HT1621_SEG_D|HT1621_SEG_E|HT1621_SEG_F|HT1621_SEG_DP), // Q
	(HT1621_SEG_E|HT1621_SEG_G),																							// R
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_C|HT1621_SEG_D),						// S
	(HT1621_SEG_B|HT1621_SEG_G|HT1621_SEG_C|HT1621_SEG_D),										// T
	(HT1621_SEG_E|HT1621_SEG_D|HT1621_SEG_C),													// U
	(0),																							// V
	(0),																							// W
	(HT1621_SEG_F|HT1621_SEG_G|HT1621_SEG_E|HT1621_SEG_C|HT1621_SEG_B),						// X
	(0),																							// Y
	(0),																							// Z
	(HT1621_SEG_A|HT1621_SEG_F|HT1621_SEG_E|HT1621_SEG_D),(HT1621_SEG_F|HT1621_SEG_E),(HT1621_SEG_A|HT1621_SEG_B|HT1621_SEG_C|HT1621_SEG_D), // {|}
	(0),(0) // ~?
};


void HT1621Init ();
void HT1621Write (uint8_t adr, uint8_t data, uint8_t mode=HT1621_MODE_WRITE);
void HT1621Command (uint8_t cmd);
void HT1621SendDisplay ();
void HT1621SetRow (uint8_t row, uint8_t data);
void HT1621SetChar(uint8_t pos, char c, bool dp=false);
void HT1621SetString(uint8_t pos, char * buffer, bool numParser=false);
void HT1621LCDOn ();
void HT1621LCDOff ();

#endif /* MYHT1621_H_ */
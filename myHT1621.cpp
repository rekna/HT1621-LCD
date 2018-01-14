/*
 * myHT1621.cpp
 *
 * Created: 11.01.2018 21:32:56
 *  Author: Reiner
 */ 

#include "myHT1621.h"

uint8_t _HT1621DataArr [HT1621_MAX_ROW];

void HT1621Init () {
	// Setup Pins for Output and HIGH
	HT1621_PIN_SETUP
	HT1621_PIN_DEFAULT
	// Enable LCD
	HT1621Command(HT1621_CMD_SYS_EN);
	// Set clock to internal oscillator
	HT1621Command(HT1621_CMD_RC_256K);
	// Set connection BIAS for particular LCD Panel (may vary at other modules)
	HT1621Command(HT1621_CMD_BIAS_T_4);
	// Enable LCD Output
	HT1621Command(HT1621_CMD_LCD_ON);
}

void HT1621WriteBits (uint8_t data, uint8_t cnt) {
	// Bit bang n count of bits (MSB first)
	for (uint8_t i=0; i<cnt; i++,data<<=1) {
		// Write MSB bit and latch on raising edge of WR pin
		HT1621_WR_LOW;
		(data&0x80)!=0?HT1621_DATA_HIGH:HT1621_DATA_LOW;
		HT1621_WR_HIGH;
	}
}

void HT1621Write (uint8_t adr, uint8_t data, uint8_t mode) {
	// Activate Transmission
	HT1621_CS_LOW;
	
	// Send Data Mode
	HT1621WriteBits(mode,3);
	// Send Address Info
	HT1621WriteBits(adr<<2,6);
	// Send Data Byte
	HT1621WriteBits(data,8);
	
	// Stop Transmission
	HT1621_CS_HIGH;
}

void HT1621Command (uint8_t cmd) {
	// Aktivate Transmission
	HT1621_CS_LOW;

	// Send Command Mode Flag	
	HT1621WriteBits(HT1621_MODE_CMD,3);
	// Send Command Index
	HT1621WriteBits(cmd,8);
	// additional 9. Bit needs to be sent, but will be ignored
	HT1621WriteBits(0,1);
	
	// Stop Transmission
	HT1621_CS_HIGH;
}


void HT1621SendDisplay () {
	// Send complete buffer to display
	for (uint8_t i=0; i<HT1621_MAX_ROW; i++) {
		HT1621Write(i*2,_HT1621DataArr[i]);
	}
}

void HT1621SetRow (uint8_t row, uint8_t data) {
	// Set row data in buffer
	if (!_inRange(row,0,HT1621_MAX_ROW-1)) return;
	_HT1621DataArr [row]=data;
}

void HT1621SetChar(uint8_t pos, char c, bool dp) {
	// Translate char in row data info and set at designated position
	uint8_t index=c;
	// Catch out of bounds chars
	if (index>sizeof(HT1621_CHARMAP))
		index=32;
	// Retrieve row data info from Flash
	uint8_t rows=pgm_read_byte_near(HT1621_CHARMAP+index);
	// Dot-Segment needs to be set?
	if (dp) rows|=HT1621_SEG_DP;
	// Set row
	HT1621SetRow(pos,rows);
}

void HT1621SetString(uint8_t pos, char * buffer, bool numParser) {
	// Set string at segments
	uint8_t posCnd=0;
	bool dpFlag;
	uint8_t b;
	for (int l=0; posCnd<6; l++) {
		dpFlag=false;
		b=buffer[l];
		// Replace "." by decimal segment, if required
		if ((numParser)&&(b==(uint8_t)'.')) {
			// First char is "." ? => Output PreLeading 0 with decimal point
			if (posCnd==0) b=(uint8_t)'0';
			else {
				// If not first char, just set DP with previous char
				b=buffer[l-1];
				posCnd--;
			}
			dpFlag=true;
		}
		// Set char at correct buffer position (MSB first!)
		// If zero byte reached (= end of string), bail out
		if (b!=0x0)
			HT1621SetChar(5-posCnd,b,dpFlag);
		else
			break;
		posCnd++;
	}
}

void HT1621LCDOn () {
	// convenient function to turn LCD display ON
	HT1621Command(HT1621_CMD_LCD_ON);
}

void HT1621LCDOff () {
	// convenient function to turn LCD display OFF
	HT1621Command(HT1621_CMD_LCD_OFF);
}


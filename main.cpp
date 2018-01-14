/*
 * HT1621-Github.cpp
 *
 * Created: 14.01.2018 08:36:43
 * Author : Reiner Knapp
 
 Simple demonstation of HT1621 LCD driver
 
 */ 

#include <avr/io.h>
#include <avr/delay.h>
#include "myHT1621.h"

char buf[50];

bool toggle=true;

int main(void)
{
		
	HT1621Init();

	sprintf (buf,"Test");
	HT1621SetString(0,buf);
	HT1621SendDisplay();

	for (uint8_t ndx=0; ndx<40; ndx++) {
		toggle=!toggle;
		if (toggle)
			HT1621LCDOn();
		else
			HT1621LCDOff();
		_delay_ms(1000);
	}

	uint8_t bit=0;
	for (uint8_t rows=0; rows<HT1621_MAX_ROW; rows++) {
		for (uint8_t data=0; data<8; data++) {
			bit=(1<<data);
			HT1621SetRow(rows,bit);
			HT1621SendDisplay();
			_delay_ms(1000);
		}
	}
	
    while (1) 
    {
    }
}


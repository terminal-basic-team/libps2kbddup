#include "Arduino.h"

#include "ps2kbd.h"

#define DATA_PIN 5
#define CLK_PIN 2

PS2::Keyboard kbd;

void
setup()
{
	Serial.begin(115200);
	
	Serial.println("Test:");
	
	kbd.begin(DATA_PIN, CLK_PIN);
	
}

void
loop()
{
	if (kbd.available()) {
		char c = kbd.read();
		Serial.print(c);
		Serial.print("\t");
		if (c < 0x10)
			Serial.print('0');
		Serial.println(unsigned(c), HEX);
	}
}

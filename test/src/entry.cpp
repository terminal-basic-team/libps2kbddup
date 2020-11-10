#include "Arduino.h"

#include "ps2kbd.h"

#define DATA_PIN 5
#define CLK_PIN 2

PS2::Keyboard kbd;
PS2::KeyboardStream kstream(kbd);

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
	/*if (kbd.available()) {
		uint8_t c = kbd.read();
		Serial.print(uint8_t(c), HEX);
		Serial.print('\t');
		if (c > 127) {
			Serial.print("Released ");
			c -= 128;
		} else
			Serial.print("Pressed ");
		switch (c) {
		case PS2::KEY_F9:
			Serial.println("F9"); break;
		case PS2::KEY_F3:
			Serial.println("F3"); break;
		case PS2::KEY_F5:
			Serial.println("F5"); break;
		case PS2::KEY_F1:
			Serial.println("F1"); break;
		case PS2::KEY_F2:
			Serial.println("F2"); break;
		case PS2::KEY_F12:
			Serial.println("F12"); break;
		case PS2::KEY_F10:
			Serial.println("F10"); break;
		case PS2::KEY_F8:
			Serial.println("F8"); break;
		case PS2::KEY_F6:
			Serial.println("F6"); break;
		case PS2::KEY_F4:
			Serial.println("F4"); break;
		case PS2::KEY_TAB:
			Serial.println("TAB"); break;
		case PS2::KEY_TILDE:
			Serial.println("~"); break;
		case PS2::KEY_LALT:
			Serial.println("LEFT ALT"); break;
		case PS2::KEY_LSHIFT:
			Serial.println("LEFT SHIFT"); break;
		case PS2::KEY_LCTRL:
			Serial.println("LEFT CTRL"); break;
		case PS2::KEY_Q:
			Serial.println("Q"); break;
		default:
			Serial.println();
		}
	}*/
	
	while (kstream.available() > 0)
		Serial.write(kstream.read());
}

/*
  PS2Keyboard.cpp - PS2Keyboard library
  Copyright (c) 2007 Free Software Foundation.  All right reserved.
  Written by Christian Weichel <info@32leaves.net>

 ** Mostly rewritten Paul Stoffregen <paul@pjrc.com> 2010, 2011
 ** Modified for use beginning with Arduino 13 by L. Abraham Smith, <n3bah@microcompdesign.com> * 
 ** Modified for easy interrup pin assignement on method begin(datapin,irq_pin). Cuningan <cuninganreset@gmail.com> **
 ** Modified by Andrey V. Skvortsov 2019: Library main object reads only the scan
 *    codes, needs to use external scan-code to char-code parser library

  for more infoCopyright (c) 2007 Free Software Foundation.  All right reserved.rmation you can read the original wiki in arduino.cc
  at http://www.arduino.cc/playground/Main/PS2Keyboard
  or http://www.pjrc.com/teensy/td_libs_PS2Keyboard.html

  Version 2.4 (March 2013)
  - Support Teensy 3.0, Arduino Due, Arduino Leonardo & other boards
  - French keyboard layout, David Chochoi, tchoyyfr at yahoo dot fr

  Version 2.3 (October 2011)
  - Minor bugs fixed

  Version 2.2 (August 2011)
  - Support non-US keyboards - thanks to Rainer Bruch for a German keyboard :)

  Version 2.1 (May 2011)
  - timeout to recover from misaligned input
  - compatibility with Arduino "new-extension" branch
  - TODO: send function, proposed by Scott Penrose, scooterda at me dot com

  Version 2.0 (June 2010)
  - Buffering added, many scan codes can be captured without data loss
    if your sketch is busy doing other work
  - Shift keys supported, completely rewritten scan code to ascii
  - Slow linear search replaced with fast indexed table lookups
  - Support for Teensy, Arduino Mega, and Sanguino added

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
 * Version ps2kbd-1.0
 * Library object returns phisical key codes with the flag of press/depress
 */

#include <avr/pgmspace.h>
#include <string.h>

#include "ps2_keyboard.hpp"

namespace PS2
{

#define BUFFER_SIZE 16
static volatile uint8_t buffer[BUFFER_SIZE];
static volatile uint8_t head, tail;
static uint8_t DataPin;
static uint8_t CharBuffer = 0;

// The ISR for the external interrupt

void
Keyboard::ps2interrupt()
{
	static uint8_t bitcount = 0;
	static uint8_t incoming = 0;
	static uint32_t prev_ms = 0;
	uint32_t now_ms;
	uint8_t n, val;

	val = digitalRead(DataPin);
	now_ms = millis();
	if (now_ms - prev_ms > 250) {
		bitcount = 0;
		incoming = 0;
	}
	prev_ms = now_ms;
	n = bitcount - 1;
	if (n <= 7) {
		incoming |= (val << n);
	}
	bitcount++;
	if (bitcount == 11) {
		uint8_t i = head + 1;
		if (i >= BUFFER_SIZE)
			i = 0;
		if (i != tail) {
			buffer[i] = incoming;
			head = i;
		}
		bitcount = 0;
		incoming = 0;
	}
}

static uint8_t
get_scan_code()
{
	uint8_t c, i;

	i = tail;
	if (i == head)
		return 0;
	i++;
	if (i >= BUFFER_SIZE)
		i = 0;
	c = buffer[i];
	tail = i;
	return c;
}

bool
Keyboard::available()
{
	if (CharBuffer)
		return true;
	CharBuffer = get_scan_code();
	if (CharBuffer)
		return true;
	return false;
}

static const uint8_t scancodes1[] PROGMEM =
{
	KEY_NONE,   // 00
	KEY_F9,     // 01
	KEY_NONE,   // 02
	KEY_F5,     // 03
	KEY_F3,     // 04
	KEY_F1,     // 05
	KEY_F2,     // 06
	KEY_F12,    // 07
	KEY_NONE,   // 08
	KEY_F10,    // 09
	KEY_F8,     // 0A
	KEY_F6,     // 0B
	KEY_F4,     // 0C
	KEY_TAB,    // 0D
	KEY_TILDE,  // 0E
	KEY_NONE,   // 0F
	KEY_NONE,   // 10
	KEY_LALT,   // 11
	KEY_LSHIFT, // 12
	KEY_NONE,   // 13
	KEY_LCTRL,  // 14
	KEY_Q,      // 15
	KEY_1,      // 16
	KEY_NONE,   // 17
	KEY_NONE,   // 18
	KEY_NONE,   // 19
	KEY_Z,      // 1A
	KEY_S,      // 1B
	KEY_A,      // 1C
	KEY_W,      // 1D
	KEY_2,      // 1E
	KEY_NONE,   // 1F
	KEY_NONE,   // 20
	KEY_C,      // 21
	KEY_X,      // 22
	KEY_D,      // 23
	KEY_E,      // 24
	KEY_4,      // 25
	KEY_3,      // 26
	KEY_NONE,   // 27
	KEY_NONE,   // 28
	KEY_SPACE,  // 29
	KEY_V,      // 2A
	KEY_F,      // 2B
	KEY_T,      // 2C
	KEY_R,      // 2D
	KEY_5,      // 2E
	KEY_NONE,   // 2F
	KEY_NONE,   // 30
	KEY_N,      // 31
	KEY_B,      // 32
	KEY_H,      // 33
	KEY_G,      // 34
	KEY_Y,      // 35
	KEY_6,      // 36
	KEY_NONE,   // 37
	KEY_NONE,   // 38
	KEY_NONE,   // 39
	KEY_M,      // 3A
	KEY_J,      // 3B
	KEY_U,      // 3C
	KEY_7,      // 3D
	KEY_8,      // 3E
	KEY_NONE,   // 3F
	KEY_NONE,   // 40
	KEY_COMMA,  // 41
	KEY_K,      // 42
	KEY_I,      // 43
	KEY_O,      // 44
	KEY_0,      // 45
	KEY_9,      // 46
	KEY_NONE,   // 47
	KEY_NONE,   // 48
	KEY_PERIOD, // 49
	KEY_SLASH,  // 4A
	KEY_L,      // 4B
	KEY_SEMI,   // 4C
	KEY_P,      // 4D
	KEY_MINUS,  // 4E
	KEY_NONE,   // 4F
	KEY_NONE,   // 50
	KEY_NONE,   // 51
	KEY_SQT,    // 52
	KEY_NONE,   // 53
	KEY_LBRAC,  // 54
	KEY_EQUALS, // 55
	KEY_NONE,   // 56
	KEY_NONE,   // 57
	KEY_CAPSLOCK,// 58
	KEY_RSHIFT, // 59
	KEY_ENTER,  // 5A
	KEY_RBRAC,  // 5B
	KEY_NONE,   // 5C
	KEY_BACKSL, // 5D
	KEY_NONE,   // 5E
	KEY_NONE,   // 5F
	KEY_NONE,   // 60
	KEY_NONE,   // 61
	KEY_NONE,   // 62
	KEY_NONE,   // 63
	KEY_NONE,   // 64
	KEY_NONE,   // 65
	KEY_BACKSP, // 66
	KEY_NONE,   // 67
	KEY_NONE,   // 68
	KEY_NUM1,   // 69
	KEY_NONE,   // 6A
	KEY_NUM4,   // 6B
	KEY_NUM7,   // 6C
	KEY_NONE,   // 6D
	KEY_NONE,   // 6E
	KEY_NONE,   // 6F
	KEY_NUM0,   // 70
	KEY_NUMPER, // 71
	KEY_NUM2,   // 72
	KEY_NUM5,   // 73
	KEY_NUM6,   // 74
	KEY_NUM8,   // 75
	KEY_ESC,    // 76
	KEY_NUMLOCK,// 77
	KEY_F11,    // 78
	KEY_NUMPLUS,// 79
	KEY_NUM3,   // 7A
	KEY_NUMMINUS,// 7B
	KEY_NUMAST, // 7C
	KEY_NUM9,   // 7D
	KEY_SCROLLC,// 7E
	KEY_NONE,   // 7F
	KEY_NONE,   // 80
	KEY_NONE,   // 81
	KEY_NONE,   // 82
	KEY_F7,     // 83
	KEY_NONE,   // 84
};

static uint8_t
getNext()
{
	if (CharBuffer) {
		auto c = CharBuffer;
		CharBuffer = 0;
		return c;
	}
	return get_scan_code();
}

uint8_t
Keyboard::read()
{
	uint8_t result = KEY_NONE;
	uint8_t c = getNext();
	if (c == 0)
		return KEY_NONE;
	else if (c == 0xF0) {
		result += 128;
		uint8_t i = 5;
		do {
			delay(1);
			c = getNext();
			if (c)
				break;
		} while (i-- > 0);
	} else if (c == 0xE0) {
		uint8_t i = 5;
		do {
			delay(1);
			c = getNext();
			if (c)
				break;
		} while (i-- > 0);
		if (c == 0xF0) {
			result += 128;
			i = 5;
			do {
				delay(1);
				c = getNext();
				if (c)
					break;
			} while (i-- > 0);
		}
		/* E0 escaped codes */
		uint8_t cc = KEY_NONE;
		switch (c) {
		case 0x11:
			cc = KEY_RALT; break;
		case 0x14:
			cc = KEY_RCTRL; break;
		case 0x4A:
			cc = KEY_NUMSLASH; break;
		case 0x5A:
			cc = KEY_NUMENT; break;
		default:
			cc = KEY_NONE; break;
		}
		return result + cc;
	}
	result += pgm_read_byte(scancodes1+c);
	
	return result;
}

Keyboard::Keyboard() :
m_flags(FLAG_NONE)
{
	// nothing to do here, begin() does it all
}

void
Keyboard::begin(uint8_t data_pin, uint8_t irq_pin)
{
	uint8_t irq_num = 255;

	DataPin = data_pin;

	// initialize the pins
#ifdef INPUT_PULLUP
	pinMode(irq_pin, INPUT_PULLUP);
	pinMode(data_pin, INPUT_PULLUP);
#else
	pinMode(irq_pin, INPUT);
	digitalWrite(irq_pin, HIGH);
	pinMode(data_pin, INPUT);
	digitalWrite(data_pin, HIGH);
#endif

#ifdef CORE_INT_EVERY_PIN
	irq_num = irq_pin;

#else
	switch (irq_pin) {
#ifdef CORE_INT0_PIN
	case CORE_INT0_PIN:
		irq_num = 0;
		break;
#endif
#ifdef CORE_INT1_PIN
	case CORE_INT1_PIN:
		irq_num = 1;
		break;
#endif
#ifdef CORE_INT2_PIN
	case CORE_INT2_PIN:
		irq_num = 2;
		break;
#endif
#ifdef CORE_INT3_PIN
	case CORE_INT3_PIN:
		irq_num = 3;
		break;
#endif
#ifdef CORE_INT4_PIN
	case CORE_INT4_PIN:
		irq_num = 4;
		break;
#endif
#ifdef CORE_INT5_PIN
	case CORE_INT5_PIN:
		irq_num = 5;
		break;
#endif
#ifdef CORE_INT6_PIN
	case CORE_INT6_PIN:
		irq_num = 6;
		break;
#endif
#ifdef CORE_INT7_PIN
	case CORE_INT7_PIN:
		irq_num = 7;
		break;
#endif
#ifdef CORE_INT8_PIN
	case CORE_INT8_PIN:
		irq_num = 8;
		break;
#endif
#ifdef CORE_INT9_PIN
	case CORE_INT9_PIN:
		irq_num = 9;
		break;
#endif
#ifdef CORE_INT10_PIN
	case CORE_INT10_PIN:
		irq_num = 10;
		break;
#endif
#ifdef CORE_INT11_PIN
	case CORE_INT11_PIN:
		irq_num = 11;
		break;
#endif
#ifdef CORE_INT12_PIN
	case CORE_INT12_PIN:
		irq_num = 12;
		break;
#endif
#ifdef CORE_INT13_PIN
	case CORE_INT13_PIN:
		irq_num = 13;
		break;
#endif
#ifdef CORE_INT14_PIN
	case CORE_INT14_PIN:
		irq_num = 14;
		break;
#endif
#ifdef CORE_INT15_PIN
	case CORE_INT15_PIN:
		irq_num = 15;
		break;
#endif
#ifdef CORE_INT16_PIN
	case CORE_INT16_PIN:
		irq_num = 16;
		break;
#endif
#ifdef CORE_INT17_PIN
	case CORE_INT17_PIN:
		irq_num = 17;
		break;
#endif
#ifdef CORE_INT18_PIN
	case CORE_INT18_PIN:
		irq_num = 18;
		break;
#endif
#ifdef CORE_INT19_PIN
	case CORE_INT19_PIN:
		irq_num = 19;
		break;
#endif
#ifdef CORE_INT20_PIN
	case CORE_INT20_PIN:
		irq_num = 20;
		break;
#endif
#ifdef CORE_INT21_PIN
	case CORE_INT21_PIN:
		irq_num = 21;
		break;
#endif
#ifdef CORE_INT22_PIN
	case CORE_INT22_PIN:
		irq_num = 22;
		break;
#endif
#ifdef CORE_INT23_PIN
	case CORE_INT23_PIN:
		irq_num = 23;
		break;
#endif
	}
#endif

	head = 0;
	tail = 0;
	if (irq_num < 255) {
		attachInterrupt(irq_num, ps2interrupt, FALLING);
	}
}

} // namespace PS2

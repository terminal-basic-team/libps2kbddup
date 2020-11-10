/*
  PS2Keyboard.cpp - PS2Keyboard library
  Copyright (c) 2007 Free Software Foundation.  All right reserved.
  Written by Christian Weichel <info@32leaves.net>

 ** Mostly rewritten Paul Stoffregen <paul@pjrc.com> 2010, 2011
 ** Modified for use beginning with Arduino 13 by L. Abraham Smith, <n3bah@microcompdesign.com> * 
 ** Modified for easy interrup pin assignement on method begin(datapin,irq_pin). Cuningan <cuninganreset@gmail.com> **

  for more information you can read the original wiki in arduino.cc
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

#include <avr/pgmspace.h>
#include <string.h>

#include "ps2kbd.h"

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
	KEY_NONE,  // 00
	KEY_F9,    // 01
	KEY_NONE,  // 02
	KEY_F5,    // 03
	KEY_F3,    // 04
	KEY_F1,    // 05
	KEY_F2,    // 06
	KEY_F12,   // 07
	KEY_NONE,  // 08
	KEY_F10,   // 09
	KEY_F8,    // 0A
	KEY_F6,    // 0B
	KEY_F4,    // 0C
	KEY_TAB,   // 0D
	KEY_TILDE, // 0E
	KEY_NONE,  // 0F
	KEY_NONE,  // 10
	KEY_LALT,  // 11
	KEY_LSHIFT,// 12
	KEY_NONE,  // 13
	KEY_LCTRL, // 14
	KEY_Q,     // 15
	KEY_1,     // 16
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
	}
	result += pgm_read_byte(scancodes1+c);
	
	return result;
}

Keyboard::Keyboard()
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

KeyboardStream::KeyboardStream(Keyboard& k) :
m_boundary(k),
m_head(0),
m_tail(0),
m_lastChar(0)
{
	memset(m_charBuffer, 0, sizeof(m_charBuffer));
}

int
KeyboardStream::available()
{
	getScanCodes();
	if ((m_lastChar != 0) || ((m_lastChar = getChar()) != 0))
		return 1;
	return 0;
}

int
KeyboardStream::peek()
{
	getScanCodes();
	if ((m_lastChar != 0) || ((m_lastChar = getChar()) != 0))
		return m_lastChar;
	return 0;
}

int
KeyboardStream::read()
{
	getScanCodes();
	if ((m_lastChar != 0) || ((m_lastChar = getChar()) != 0)) {
		const uint8_t c = m_lastChar;
		m_lastChar = 0;
		return c;
	}
	return 0;
}

uint8_t
KeyboardStream::getChar()
{
	uint8_t c, i;

	i = m_tail;
	if (i == m_head)
		return 0;
	i++;
	if (i >= PS2_KSTREAM_BUFFER_SIZE)
		i = 0;
	c = m_charBuffer[i];
	m_tail = i;
	
	return c;
}

static const uint8_t lowCase_ascii[] PROGMEM
{
	0,    // KEY_NONE
	'\t', // KEY_TAB
	'`',  // KEY_TILDE
	'1',  // KEY_1
	'q',  // KEY_Q,
};

static const uint8_t lowCase_cp866[] PROGMEM
{
	0,    // KEY_NONE
	'\t', // KEY_TAB
	'ё',  // KEY_TILDE
	'1',  // KEY_1
	'й',  // KEY_Q,
};

static const uint8_t upCase_ascii[] PROGMEM
{
	0,    // KEY_NONE
	'\t', // KEY_TAB
	'~',  // KEY_TILDE
	'!',  // KEY_1
	'Q',  // KEY_Q,
};

void
KeyboardStream::getScanCodes()
{
	while (m_boundary.available()) {
		uint8_t c = m_boundary.read();
		if (c < 128) { // key pressed
			if (c < KEY_F1) {
				if (m_flags & FLAGS_SHIFT)
					c = pgm_read_byte(upCase_ascii+c);
				else
					c = pgm_read_byte(lowCase_ascii+c);
			} else {
				if (c == KEY_LSHIFT)
					m_flags |= FLAGS_SHIFT;
				continue;
			}
			    
			uint8_t i = m_head + 1;
			if (i >= PS2_KSTREAM_BUFFER_SIZE)
				i = 0;
			if (i != m_tail) {
				m_charBuffer[i] = c;
				m_head = i;
			}
		} else {
			c -= 128;
			if (c == KEY_LSHIFT)
				m_flags &= ~FLAGS_SHIFT;
		}
	}
}

size_t
KeyboardStream::write(uint8_t)
{
	return 0;
}

} // namespace PS2

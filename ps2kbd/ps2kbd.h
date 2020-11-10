/*
  PS2Keyboard.h - PS2Keyboard library
  Copyright (c) 2007 Free Software Foundation.  All right reserved.
  Written by Christian Weichel <info@32leaves.net>

 ** Mostly rewritten Paul Stoffregen <paul@pjrc.com>, June 2010
 ** Modified for use with Arduino 13 by L. Abraham Smith, <n3bah@microcompdesign.com> * 
 ** Modified for easy interrup pin assignement on method begin(datapin,irq_pin). Cuningan <cuninganreset@gmail.com> **

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

/**
 * Changed by Andrey V. Skvortsov starling13@mail.ru, 2019
 */

#ifndef PS2_HEYBOARD_HPP
#define PS2_HEYBOARD_HPP

#if defined(ARDUINO) && ARDUINO >= 100
#include <stdint.h>

#include "Arduino.h" // for attachInterrupt, FALLING
#else
#include "WProgram.h"
#endif

#include <Stream.h>
#include <stdio.h>

#include "utility/int_pins.h"

namespace PS2
{

enum Keys : uint8_t
{
	KEY_NONE = 0,
	/* Printable keys */
	KEY_TAB,
	KEY_TILDE,
	KEY_1,
	KEY_Q,
	/**/
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_ESC,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_LALT,
	KEY_RALT,
	KEY_LCTRL,
	KEY_RCTRL,
	KEY_CAPSLOCK,
	KEY_NUMLOCK
};

/**
 * Purpose: Provides an easy access to PS2 keyboards
 * Author:  Christian Weichel
 */
class Keyboard
{
public:
	/**
	 * This constructor does basically nothing. Please call the begin(int,int)
	 * method before using any other method of this class.
	 */
	Keyboard();

	/**
	 * Starts the keyboard "service" by registering the external interrupt.
	 * setting the pin modes correctly and driving those needed to high.
	 * The propably best place to call this method is in the setup routine.
	 * 
	 * @param dataPin
	 * @param irq_pin
	 */
	void begin(
	    uint8_t,
	    uint8_t);

	/**
	 * Returns true if there is a char to be read, false if not.
	 */
	bool available();

	/**
	 * Returns the char last read from the keyboard.
	 * If there is no char available, -1 is returned.
	 */
	uint8_t read();
	
private:
	
	static void ps2interrupt();
};

#define PS2_KSTREAM_BUFFER_SIZE 8

class KeyboardStream : public Stream
{
public:
	
	KeyboardStream(Keyboard&);
	
private:
	
	enum Flags_t : uint8_t
	{
		FLAGS_NONE = 0,
		FLAGS_SHIFT = 1 << 0,
		FLAGS_ALT = 1 << 1,
		FLAGS_CTRL = 1 << 2,
		FLAGS_CAPS = 1 << 3,
		FLAGS_NUM = 1 << 4,
		FLAGS_LOCALE = 1 << 5,
	};
	
	void getScanCodes();
	
	uint8_t getChar();
	
	Flags_t m_flags;
	
	Keyboard& m_boundary;
	
	uint8_t m_charBuffer[PS2_KSTREAM_BUFFER_SIZE];
	
	uint8_t m_head, m_tail;
	
	uint8_t m_lastChar;

// Stream cinterface
public:
	
	int available() override;
	
	int read() override;
	
	int peek() override;
// Print interface
public:
	
	size_t write(uint8_t);
};

} // namespace PS2

#endif // PS2_HEYBOARD_HPP

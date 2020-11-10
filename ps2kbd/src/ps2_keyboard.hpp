/*
  PS2Keyboard.h - PS2Keyboard library
  Copyright (c) 2007 Free Software Foundation.  All right reserved.
  Written by Christian Weichel <info@32leaves.net>

 ** Mostly rewritten Paul Stoffregen <paul@pjrc.com>, June 2010
 ** Modified for use with Arduino 13 by L. Abraham Smith, <n3bah@microcompdesign.com> * 
 ** Modified for easy interrup pin assignement on method begin(datapin,irq_pin). Cuningan <cuninganreset@gmail.com> **
 ** Modified by Andrey V. Skvortsov 2019: Library main object reads only the scan
 *    codes, needs to use external scan-code to char-code parser library

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

#ifndef PS2_HEYBOARD_HPP
#define PS2_HEYBOARD_HPP

#if defined(ARDUINO) && ARDUINO >= 100
#include <stdint.h>

#include "Arduino.h" // for attachInterrupt, FALLING
#else
#include "WProgram.h"
#endif

#include <stdio.h>

#include "utility/int_pins.h"

namespace PS2
{

enum Keys : uint8_t
{
	KEY_NONE = 0,
	/* Localeless and shiftless keys */
	KEY_ENTER,
	KEY_BACKSP,
	KEY_TAB,
	KEY_SPACE,
	KEY_NUM0,
	KEY_NUM1,
	KEY_NUM2,
	KEY_NUM3,
	KEY_NUM4,
	KEY_NUM5,
	KEY_NUM6,
	KEY_NUM7,
	KEY_NUM8,
	KEY_NUM9,
	KEY_NUMENT,
	KEY_NUMPER,
	KEY_NUMPLUS,
	KEY_NUMMINUS,
	KEY_NUMAST,
	KEY_NUMSLASH,
	/* Printable keys */
	KEY_TILDE,
	KEY_SLASH,
	KEY_BACKSL,
	KEY_PERIOD,
	KEY_SEMI,
	KEY_COMMA,
	KEY_SQT,
	KEY_MINUS,
	KEY_EQUALS,
	KEY_LBRAC,
	KEY_RBRAC,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	/* Non-printable */
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
	KEY_NUMLOCK,
	KEY_SCROLLC
};

/**
 * Purpose: Provides an easy access to PS2 keyboards
 * Author:  Christian Weichel
 */
class Keyboard
{
	friend class KeyboardStream;
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
	
	void setLeds(uint8_t);
	
private:
	
	static void ps2interrupt();
	
	void attachInterrupt();
	
	void setOutput();
	
	uint8_t getNext();
	
	void sendByte(uint8_t);
	
	bool waitPinState(uint8_t, int);
	
	enum Flags_t : uint8_t
	{
		FLAG_NONE = 0,
		FLAG_EXTRACODE = 1<<0,
		FLAG_COMMAND_RESPOND = 1<<7
	};
	
	Flags_t m_flags;
};

} // namespace PS2

#endif // PS2_HEYBOARD_HPP

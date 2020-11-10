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

#ifndef PS2_KEYBOARDSTREAM_HPP
#define PS2_KEYBOARDSTREAM_HPP

#include "ps2_keyboard.hpp"

#include <Stream.h>

namespace PS2
{

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
	
	void getScanCode();
	
	uint8_t getChar();
	
	void updateLeds();
	
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
	
	void flush() override;
// Print interface
public:
	
	size_t write(uint8_t);
};

} // namespace PS2

#endif /* PS2_KEYBOARDSTREAM_HPP */

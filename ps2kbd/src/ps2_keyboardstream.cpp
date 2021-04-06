/*
  PS2Keyboard.h - PS2Keyboard library
  Copyright (c) 2007 Free Software Foundation.  All right reserved.
  Written by Christian Weichel <info@32leaves.net>

 ** Mostly rewritten Paul Stoffregen <paul@pjrc.com>, June 2010
 ** Modified for use with Arduino 13 by L. Abraham Smith, <n3bah@microcompdesign.com> * 
 ** Modified for easy interrup pin assignement on method begin(datapin,irq_pin). Cuningan <cuninganreset@gmail.com> **
 ** Modified by Andrey V. Skvortsov 2019-2021: Library main object reads only the scan
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

#include "ps2_keyboardstream.hpp"

namespace PS2
{

KeyboardStream::KeyboardStream(Keyboard& k) :
m_flags(FLAGS_NONE),
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

void
KeyboardStream::updateLeds()
{
	uint8_t byt = 0;
	if (m_flags & FLAGS_CAPS)
		byt |= 4;
	if (m_flags & FLAGS_LOCALE)
		byt |= 1;
	if (m_flags & FLAGS_NUM)
		byt |= 2;
	m_boundary.setLeds(byt);
}

static const uint8_t asciiChars_upper[] PROGMEM
{
	'~',  // KEY_TILDE
	'?',  // KEY_SLASH
	'|',  // KEY_BACKSL
	'>',  // KEY_PERIOD
	':',  // KEY_SEMI
	'<',  // KEY_COMMA
	'"',  // KEY_SQT
	'_',  // KEY_MINUS
	'+',  // KEY_EQUALS
	'{',  // KEY_LBRAC
	'}',  // KEY_RBRAC
	')',  // KEY_0
	'!',  // KEY_1
	'@',  // KEY_2
	'#',  // KEY_3
	'$',  // KEY_4
	'%',  // KEY_5
	'^',  // KEY_6
	'&',  // KEY_7
	'*',  // KEY_8
	'(',  // KEY_9
	'A',  // KEY_A
	'B',  // KEY_B
	'C',  // KEY_C
	'D',  // KEY_D
	'E',  // KEY_E
	'F',  // KEY_F
	'G',  // KEY_G
	'H',  // KEY_H
	'I',  // KEY_I
	'J',  // KEY_J
	'K',  // KEY_K
	'L',  // KEY_L
	'M',  // KEY_M
	'N',  // KEY_N
	'O',  // KEY_O
	'P',  // KEY_P
	'Q',  // KEY_Q
	'R',  // KEY_R
	'S',  // KEY_S
	'T',  // KEY_T
	'U',  // KEY_U
	'V',  // KEY_V
	'W',  // KEY_W
	'X',  // KEY_X
	'Y',  // KEY_Y
	'Z',  // KEY_Z
};

static const uint8_t asciiChars_lower[] PROGMEM
{
	'`',  // KEY_TILDE
	'/',  // KEY_SLASH
	'\\', // KEY_BACKSL
	'.',  // KEY_PERIOD
	';',  // KEY_SEMI
	',',  // KEY_COMMA
	'\'', // KEY_SQT
	'-',  // KEY_MINUS
	'=',  // KEY_EQUALS
	'[',  // KEY_LBRAC
	']',  // KEY_RBRAC
	'0',  // KEY_0
	'1',  // KEY_1
	'2',  // KEY_2
	'3',  // KEY_3
	'4',  // KEY_4
	'5',  // KEY_5
	'6',  // KEY_6
	'7',  // KEY_7
	'8',  // KEY_8
	'9',  // KEY_9
	'a',  // KEY_A
	'b',  // KEY_B
	'c',  // KEY_C
	'd',  // KEY_D
	'e',  // KEY_E
	'f',  // KEY_F
	'g',  // KEY_G
	'h',  // KEY_H
	'i',  // KEY_I
	'j',  // KEY_J
	'k',  // KEY_K
	'l',  // KEY_L
	'm',  // KEY_M
	'n',  // KEY_N
	'o',  // KEY_O
	'p',  // KEY_P
	'q',  // KEY_Q
	'r',  // KEY_R
	's',  // KEY_S
	't',  // KEY_T
	'u',  // KEY_U
	'v',  // KEY_V
	'w',  // KEY_W
	'x',  // KEY_X
	'y',  // KEY_Y
	'z',  // KEY_Z
};

static const uint8_t upCase_cp866[] PROGMEM
{
	'ð',  // KEY_TILDE
	',',  // KEY_SLASH
	'/',  // KEY_BACKSL
	'ž',  // KEY_PERIOD
	'†',  // KEY_SEMI
	'',  // KEY_COMMA
	'',  // KEY_SQT
	'_',  // KEY_MINUS
	'+',  // KEY_EQUALS
	'•',  // KEY_LBRAC
	'š',  // KEY_RBRAC
	')',  // KEY_0
	'!',  // KEY_1
	'"',  // KEY_2
	'ü',  // KEY_3
	';',  // KEY_4
	'%',  // KEY_5
	':',  // KEY_6
	'?',  // KEY_7
	'*',  // KEY_8
	'(',  // KEY_9
	'”',  // KEY_A
	'ˆ',  // KEY_B
	'‘',  // KEY_C
	'‚',  // KEY_D
	'“',  // KEY_E
	'€',  // KEY_F
	'',  // KEY_G
	'',  // KEY_H
	'˜',  // KEY_I
	'Ž',  // KEY_J
	'‹',  // KEY_K
	'„',  // KEY_L
	'œ',  // KEY_M
	'’',  // KEY_N
	'™',  // KEY_O
	'‡',  // KEY_P
	'‰',  // KEY_Q
	'Š',  // KEY_R
	'›',  // KEY_S
	'…',  // KEY_T
	'ƒ',  // KEY_U
	'Œ',  // KEY_V
	'–',  // KEY_W
	'—',  // KEY_X
	'',  // KEY_Y
	'Ÿ',  // KEY_Z
};

static const uint8_t lowCase_cp866[] PROGMEM
{
	'ñ',  // KEY_TILDE
	'.',  // KEY_SLASH
	'\\', // KEY_BACKSL
	'î',  // KEY_PERIOD
	'¦',  // KEY_SEMI
	'¡',  // KEY_COMMA
	'í',  // KEY_SQT
	'-',  // KEY_MINUS
	'=',  // KEY_EQUALS
	'å',  // KEY_LBRAC
	'ê',  // KEY_RBRAC
	'0',  // KEY_0
	'1',  // KEY_1
	'2',  // KEY_2
	'3',  // KEY_3
	'4',  // KEY_4
	'5',  // KEY_5
	'6',  // KEY_6
	'7',  // KEY_7
	'8',  // KEY_8
	'9',  // KEY_9
	'ä',  // KEY_A
	'¨',  // KEY_B
	'á',  // KEY_C
	'¢',  // KEY_D
	'ã',  // KEY_E
	' ',  // KEY_F
	'¯',  // KEY_G
	'à',  // KEY_H
	'è',  // KEY_I
	'®',  // KEY_J
	'«',  // KEY_K
	'¤',  // KEY_L
	'ì',  // KEY_M
	'â',  // KEY_N
	'é',  // KEY_O
	'§',  // KEY_P
	'©',  // KEY_Q
	'ª',  // KEY_R
	'ë',  // KEY_S
	'¥',  // KEY_T
	'£',  // KEY_U
	'¬',  // KEY_V
	'æ',  // KEY_W
	'ç',  // KEY_X
	'­',  // KEY_Y
	'ï',  // KEY_Z
};

static const uint8_t asciicaps[] PROGMEM
{
	0b00000000,
	0b00000000,
	0b11100000,
	0b11111111,
	0b11111111,
	0b11111111
};

static const uint8_t cp866caps[] PROGMEM
{
	0b01111001,
	0b00000110,
	0b11100000,
	0b11111111,
	0b11111111,
	0b11111111
};

static const uint8_t controlChars[] PROGMEM
{
	0,   // KEY_NONE
	'\n',// KEY_ENTER
	'\b',// KEY_BACKSP
	'\t',// KEY_TAB
	' ', // KEY_SPACE
	'0', // KEY_NUM0
	'1', // KEY_NUM1
	'2', // KEY_NUM2
	'3', // KEY_NUM3
	'4', // KEY_NUM4
	'5', // KEY_NUM5
	'6', // KEY_NUM6
	'7', // KEY_NUM7
	'8', // KEY_NUM8
	'9', // KEY_NUM9
	'\n',// KEY_NUMENT
	'.', // KEY_NUMPER
	'+', // KEY_NUMPLUS
	'-', // KEY_NUMMINUS
	'*', // KEY_NUMAST
	'/', // KEY_NUMSLASH
};

void
KeyboardStream::getScanCode()
{
	uint8_t c = m_boundary.read();
	if (c < 128) { // key pressed
		if ((m_flags & FLAGS_CTRL) && (c >= KEY_A && c <= KEY_Z)) {
			c = c - KEY_A + 1;
		} else if (c < KEY_TILDE) { // Printable control characters
			c = pgm_read_byte(controlChars+c);
		} else if (c < KEY_F1) { // Printable character
			c -= KEY_TILDE;
			if (m_flags & FLAGS_LOCALE) { // Locale on
				uint8_t index = c / 8;
				index = pgm_read_byte(cp866caps+index);
				index = (index >> (c % 8)) & 1;
				bool upperCase =
				    ((m_flags & FLAGS_SHIFT) && !index) ||
				    ((m_flags & FLAGS_SHIFT) && !(m_flags & FLAGS_CAPS) && index) ||
				    (!(m_flags & FLAGS_SHIFT) && (m_flags & FLAGS_CAPS) && index);
				if (upperCase) {
					c = pgm_read_byte(upCase_cp866+c);
				} else
					c = pgm_read_byte(lowCase_cp866+c);
			} else { // Locale off
				uint8_t index = c / 8;
				index = pgm_read_byte(asciicaps+index);
				index = (index >> (c % 8)) & 1;
				bool upperCase =
				    ((m_flags & FLAGS_SHIFT) && !index) ||
				    ((m_flags & FLAGS_SHIFT) && !(m_flags & FLAGS_CAPS) && index) ||
				    (!(m_flags & FLAGS_SHIFT) && (m_flags & FLAGS_CAPS) && index);
				if (upperCase) {
					c = pgm_read_byte(asciiChars_upper+c);
				} else
					c = pgm_read_byte(asciiChars_lower+c);
			}
		} else {
			if (c == KEY_LSHIFT || c == KEY_RSHIFT)
				m_flags = Flags_t((uint8_t)m_flags | (uint8_t)FLAGS_SHIFT);
			else if (c == KEY_LCTRL)
				m_flags = Flags_t((uint8_t)m_flags | (uint8_t)FLAGS_CTRL);
			else if (c == KEY_RCTRL)
				m_flags = Flags_t((uint8_t)m_flags ^ (uint8_t)FLAGS_LOCALE);
			else if (c == KEY_CAPSLOCK)
				m_flags = Flags_t((uint8_t)m_flags ^ (uint8_t)FLAGS_CAPS);
			else if (c == KEY_NUMLOCK)
				m_flags = Flags_t((uint8_t)m_flags ^ (uint8_t)FLAGS_NUM);
			return;
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
		if (c == KEY_LSHIFT || c == KEY_RSHIFT)
			m_flags = Flags_t((uint8_t)m_flags & (uint8_t)~FLAGS_SHIFT);
		else if (c == KEY_LCTRL)
			m_flags = Flags_t((uint8_t)m_flags & (uint8_t)~FLAGS_CTRL);
		else if (c == KEY_RCTRL ||
			 c == KEY_CAPSLOCK ||
			 c == KEY_NUMLOCK)
			updateLeds();
	}
}

void
KeyboardStream::flush()
{
	getScanCodes();
}

void
KeyboardStream::getScanCodes()
{
	while (m_boundary.available())
		getScanCode();
}

size_t
KeyboardStream::write(uint8_t)
{
	return 0;
}

} // namespace PS2

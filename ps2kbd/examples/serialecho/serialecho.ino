#include "ps2_keyboardstream.hpp"

#define DATA_PIN 5
#define CLK_PIN 2

static PS2::Keyboard kbd;
static PS2::KeyboardStream kstream(kbd);

void
setup()
{
  Serial.begin(9600);
  kbd.begin(DATA_PIN, CLK_PIN);
  
}

void
loop()
{
  while (kstream.available() > 0)
    Serial.write(kstream.read());
}

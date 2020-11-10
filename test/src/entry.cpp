#include "ps2_keyboardstream.hpp"

#define DATA_PIN 5
#define CLK_PIN 2

PS2::Keyboard kbd;
PS2::KeyboardStream kstream(kbd);

void
setup()
{
  Serial.begin(115200);
  kbd.begin(DATA_PIN, CLK_PIN);
  
}

void
loop()
{
  while (kstream.available() > 0)
    Serial.write(kstream.read());
}

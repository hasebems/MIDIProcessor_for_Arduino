//
//  MIDI_Processor.ino  only for Arduino Mega
//
//  2018.10.08 by kigakudoh
//
#include "midiprocessor_class.h"

MidiProcessor mpr;
unsigned long ledTime = 0;

void setup() {
  // put your setup code here, to run once:
  mpr.init();

  Serial.begin(9600);
  Serial1.begin(31250);
  Serial2.begin(31250);
  Serial3.begin(31250);

  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  checkLeds();

  if (Serial1.available() > 0) {
    // read the incoming byte:
    mpr.input( Serial1.read(), 0 );
    blinkLedWhenMidiCome();
  }

  if (Serial2.available() > 0) {
    // read the incoming byte:
    mpr.input( Serial2.read(), 1 );
    blinkLedWhenMidiCome();
  }

  if (Serial3.available() > 0) {
    // read the incoming byte:
    mpr.input( Serial3.read(), 2 );
    blinkLedWhenMidiCome();
  }

  while ( mpr.hasOutputData(0) == true ){
    Serial1.write(mpr.output(0));
  }

  while ( mpr.hasOutputData(1) == true ){
    Serial2.write(mpr.output(1));
  }

  while ( mpr.hasOutputData(2) == true ){
    Serial3.write(mpr.output(2));
  }
}

void blinkLedWhenMidiCome( void )
{
  PORTB |= _BV(7);
  ledTime = millis() + 20;
}

void checkLeds( void )
{
  unsigned long crntTime = millis();
  if ( crntTime >= ledTime ){
    PORTB &= ~_BV(7);
  }
}

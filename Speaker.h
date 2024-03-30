#ifndef Speaker_h
#define Speaker_h
#include "Arduino.h"
#include "pitches.h"

class Speaker {
public:
  Speaker(int pin) : pin(pin) {} // Initialize the pin in the constructor's initializer list.

  void Play(int *melody, int melodySize, int tempo) {
    int notes = melodySize / 2; // Calculate number of notes.
    int wholenote = (60000 * 4) / tempo;
    int noteDuration = 0;

    for (int thisNote = 0; thisNote < notes * 2; thisNote += 2) {
      int divider = melody[thisNote + 1];
      if (divider > 0) {
        noteDuration = wholenote / divider;
      } else if (divider < 0) {
        noteDuration = wholenote / abs(divider);
        noteDuration *= 1.5; // Dotted notes.
      }

      tone(pin, melody[thisNote], noteDuration * 0.9);
      delay(noteDuration);
      noTone(pin);
    }
  }

private:
  int pin;
};

#endif

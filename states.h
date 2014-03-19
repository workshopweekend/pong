// LED Matrix Pong!
// by J.D. Zamfirescu
// Last update: January 2014
// states.h

enum states {
  START,
  GAME,
  SCORE,
  WIN
} state = START;

unsigned long lastStateChange = 0;

void setState(states s) {
  lastStateChange = millis();
  state = s;
}

// LED Matrix Pong!
// by J.D. Zamfirescu
// Last update: March 2014
// pong.ino


#include "HT1632.h" // Get this library here: http://github.com/workshopweekend/HT1632
#include "states.h"

#define DATA 2
#define WR   3
#define CS   4

#define WIDTH 24
#define HEIGHT 16
#define PLAYTO 10

const float XMIN = 2;
const float XMAX = (WIDTH-2);
const float PADDLELEN = 5;
const float PADDLEDIVISOR = (1024.0 / (HEIGHT - PADDLELEN + 1));

HT1632LEDMatrix matrix = HT1632LEDMatrix(DATA, WR, CS);

unsigned long lastUpdate;

void setup() {
  matrix.begin(HT1632_COMMON_16NMOS);
  matrix.clearScreen();  
  lastUpdate = millis();
}

// START state
unsigned long lastSecondsSinceStart = 1000;

void loopSTART(unsigned long now, float deltat) {
  unsigned long secondsSinceStart = (now-lastStateChange)/1000;
  if (secondsSinceStart != lastSecondsSinceStart) {
    matrix.setScreen(0);
    matrix.setTextSize(1);
    matrix.setTextColor(1);
    matrix.setCursor(9, 4);
    
    switch (secondsSinceStart) {
      case 0:
        matrix.print("3");
        break;
      case 1:
        matrix.print("2");
        break;
      case 2:
        matrix.print("1");
        break;
      default:
        setState(GAME);
    }
    lastSecondsSinceStart = secondsSinceStart;
    matrix.writeScreen();
  }
}


// GAME state
int points1 = 0;
int points2 = 0;

float x = 5, y = 12;
float vx = 15, vy = 6;
int paddle1 = 0, paddle2 = 0;

void updateBall(float deltat) {  
  x += deltat * vx;
  y += deltat * vy;
  if (x < XMIN) {
    if (y >= paddle1 && y <= paddle1+PADDLELEN) {
      x = 2*XMIN-x;
      vx = -vx;
    } else {
      points2++;
      setState(SCORE);
    }
  }
  if (x > XMAX) {
    if (y >= paddle2 && y <= paddle2+PADDLELEN) {
      x = 2*XMAX-x;
      vx = -vx;
    } else {
      points1++;
      setState(SCORE);
    }
  }
  if (y < 0) {
    y = -y;
    vy = -vy;
  }
  if (y > HEIGHT) {
    y = 2*HEIGHT-y;
    vy = -vy;
  }
  
}

void drawBall() {
  matrix.setPixel((int) x, (int) y);
}

void updatePaddles() {
  paddle1 = analogRead(A1) / PADDLEDIVISOR;
  paddle2 = analogRead(A0) / PADDLEDIVISOR;
}  

void drawPaddles() {
  for (int i = 0; i < PADDLELEN; ++i) {  
    matrix.setPixel(1, paddle1+i);
    matrix.setPixel(WIDTH-2, paddle2+i);
  }
}

void loopGAME(unsigned long now, float deltat) {
  matrix.setScreen(0);

  updateBall(deltat);
  drawBall();

  updatePaddles();
  drawPaddles();

  matrix.writeScreen();
}


// SCORE state
boolean scoreVisible = false;

void resetGame() {
  x = 5; y = random(0, HEIGHT-1);
  vx = 15, vy = 6;
}

void loopSCORE(unsigned long now, float deltat) {
  if (points1 >= PLAYTO-1 || points2 >= PLAYTO) {
    setState(WIN);
  } else { 
    if (! scoreVisible) {
      String score = String("") + points1 + ":" + points2;
      matrix.setScreen(0);
      matrix.setTextSize(1);
      matrix.setTextColor(1);
      matrix.setCursor(WIDTH/2-1 - (score.length()*5)/2, 4);
      matrix.print(score);
      matrix.writeScreen();
      scoreVisible = true;
    }
    if (now-lastStateChange > 2000) {
      matrix.clearScreen();
    }
    if (now-lastStateChange > 3000) {
      scoreVisible = false;
      matrix.clearScreen();
      resetGame();
      setState(GAME);
    }
  }
}


// WIN state
boolean winVisible = false;

void loopWIN(unsigned long now, float deltat) {
  if (! winVisible) {
    winVisible = true;
    matrix.setScreen(0);
    if (points1 > points2) {
      matrix.fillRect(0, 0, 2, HEIGHT, 1);
    } else {
      matrix.fillRect(WIDTH-2, 0, 2, HEIGHT, 1);
    }
    matrix.setTextSize(1);
    matrix.setTextColor(1);
    matrix.setCursor(4, 4);
    matrix.print("WIN");
    matrix.writeScreen();
  }  
}


void loop() {
  unsigned long now = millis();
  float deltat = (now - lastUpdate) / 1000.0;
  lastUpdate = now;

  if (state == START) {
    loopSTART(now, deltat);
  } else if (state == GAME) {
    loopGAME(now, deltat);
  } else if (state == SCORE) {
    loopSCORE(now, deltat);
  } else if (state == WIN) {
    loopWIN(now, deltat);
  }
}

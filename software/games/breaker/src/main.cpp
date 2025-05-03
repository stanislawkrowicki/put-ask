#include <Arduino.h>
#include <WiFi.h>
#include <TFT_eSPI.h>

#include <OTAServer.h>
#include "wifi_secrets.h"

#define BTN_LEFT 18
#define BTN_RIGHT 19

#define COLOR_1 TFT_BLUE    
#define COLOR_2 TFT_YELLOW   
#define COLOR_3 TFT_GREEN 

OTAServer otaServer;
TFT_eSPI tft = TFT_eSPI();




//breaker constants

const int SCREEN_WIDTH = 240;
const int SCREEN_HEIGHT = 320;

const int rows = 6;
const int cols = 8;
const int blockWidth = 26;
const int blockHeight = 10;

bool blocks[rows][cols];
int blockPosX[cols];
int blockPosY[rows];

int blockPoints[rows][cols] = {
  {10, 20, 30, 20, 10, 20, 30, 20},  
  {20, 10, 20, 30, 20, 10, 20, 30},  
  {30, 20, 10, 20, 30, 20, 10, 20},  
  {20, 30, 20, 10, 20, 30, 20, 10},  
  {10, 20, 30, 20, 10, 20, 30, 20},  
  {20, 10, 20, 30, 20, 10, 20, 30}  
};


uint16_t blockColors[rows][cols] = {
  {COLOR_1, COLOR_2, COLOR_3, COLOR_1, COLOR_2, COLOR_3, COLOR_1, COLOR_2},
  {COLOR_2, COLOR_3, COLOR_1, COLOR_2, COLOR_3, COLOR_1, COLOR_2, COLOR_3},
  {COLOR_3, COLOR_1, COLOR_2, COLOR_3, COLOR_1, COLOR_2, COLOR_3, COLOR_1},
  {COLOR_1, COLOR_2, COLOR_3, COLOR_1, COLOR_2, COLOR_3, COLOR_1, COLOR_2},
  {COLOR_2, COLOR_3, COLOR_1, COLOR_2, COLOR_3, COLOR_1, COLOR_2, COLOR_3},
  {COLOR_3, COLOR_1, COLOR_2, COLOR_3, COLOR_1, COLOR_2, COLOR_3, COLOR_1}
};

int platform_level = 270;
int platformX = 100;
float ballX = 120.0;
float ballY = 200.0;
float ballDx = 1.0;
float ballDy = -1.0;
int ballSize = 4;
int yJoyStick = 0;
float ballSpeed = 5.0;
bool game_over = false;
bool win = false;
float maxSpeed = 40.0;
int score=0;

void setup_blocks() {
  for (int i = 0; i < rows; i++) {
    blockPosY[i] = 20 + i * (blockHeight + 4);
    for (int j = 0; j < cols; j++) {
      if (i == 0) blockPosX[j] = 5 + j * (blockWidth + 4);
      blocks[i][j] = true;
    }
  }
}

void setup()
{
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  otaServer.enable();

  pinMode(21, OUTPUT);
  digitalWrite(21, HIGH);

  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);

  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 100);
  tft.println("BREAKER");
  delay(2000);
  tft.fillScreen(TFT_BLACK);

  setup_blocks();
}

void check_win() {
  win = true;
  for(int i=0; i<rows; i++) {
    for(int j=0; j<cols; j++) {
      if(blocks[i][j]) {
        win = false;
        return;
      }
    }
  }
}

void ball_movement() {
  ballX += ballDx;
  ballY += ballDy;

  if(ballX - ballSize <= 0 || ballX + ballSize >= SCREEN_WIDTH) ballDx = -ballDx;
  if(ballY - ballSize <= 0) ballDy = -ballDy;

  if(ballY + ballSize >= platform_level && ballY + ballSize <= platform_level + 4 &&
     ballX >= platformX && ballX <= platformX + 40) {
    ballDy = -abs(ballDy);
    ballY = platform_level - ballSize;
    if (ballSpeed < maxSpeed) {
      ballSpeed += 0.2;
    }
  }

  if(ballY + ballSize > SCREEN_HEIGHT) game_over = true;

  // Speed limit
  if(ballDx > maxSpeed) ballDx = maxSpeed;
  if(ballDx < -maxSpeed) ballDx = -maxSpeed;
  if(ballDy > maxSpeed) ballDy = maxSpeed;
  if(ballDy < -maxSpeed) ballDy = -maxSpeed;
  float angle = atan2(ballDy, ballDx);
  ballDx = cos(angle) * ballSpeed;
  ballDy = sin(angle) * ballSpeed;
}

void check_block_collision() {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (blocks[i][j]) {
        int bx = blockPosX[j];
        int by = blockPosY[i];

        if (ballX + ballSize >= bx && ballX - ballSize <= bx + blockWidth &&
            ballY + ballSize >= by && ballY - ballSize <= by + blockHeight) {
          blocks[i][j] = false;
          ballDy = -ballDy;
          score += blockPoints[i][j];
          if (ballSpeed < maxSpeed) {
            ballSpeed += 0.1;
          }
          return;
        }
      }
    }
  }
}

void draw_blocks() {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (blocks[i][j]) {
        tft.fillRect(blockPosX[j], blockPosY[i], blockWidth, blockHeight, blockColors[i][j]);
      } else {
        tft.fillRect(blockPosX[j], blockPosY[i], blockWidth, blockHeight, TFT_BLACK);
      }
    }
  }
}

void draw_scene() {
  tft.fillRect(platformX, platform_level, 40, 4, TFT_WHITE);
  tft.fillCircle((int)ballX, (int)ballY, ballSize, TFT_RED);
}

void draw_score() {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, SCREEN_HEIGHT - 20);
  tft.print("Score: ");
  tft.print(score);
}

void button_reaction() {
  if (digitalRead(BTN_RIGHT) == LOW && platformX + 40 < SCREEN_WIDTH) {
    platformX += 5;
  }
  if (digitalRead(BTN_LEFT) == LOW && platformX > 0) {
    platformX -= 5;
  }
}
void check_restart() {
  if (digitalRead(BTN_LEFT) == LOW && digitalRead(BTN_RIGHT) == LOW) {
    game_over = false;
    score=0;
    win = false;
    ballX = 120.0;
    ballY = 200.0;
    ballDx = 1.0;
    ballDy = -1.0;
    platformX = 100;
    ballSpeed=5.0;
    setup_blocks();
    tft.fillScreen(TFT_BLACK);
  }
}
void erase_previous(float prevBallX, float prevBallY, int prevPlatformX) {
  tft.fillRect(prevPlatformX, platform_level, 40, 4, TFT_BLACK);       
  tft.fillCircle((int)prevBallX, (int)prevBallY, ballSize, TFT_BLACK);
  tft.fillRect(10, SCREEN_HEIGHT - 20, 120, 16, TFT_BLACK);
}

void draw_game_over() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_RED);
  tft.setTextSize(3);
  tft.setCursor(50, 120);
  tft.print("GAME OVER");

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(60, 170);
  tft.print("Score: ");
  tft.print(score);
}

void draw_win() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(3);
  tft.setCursor(60, 140);
  tft.print("YOU WIN!");
}

void loop() {
  static float prevBallX = ballX;
  static float prevBallY = ballY;
  static int prevPlatformX = platformX;

  if (!game_over && !win) {
    erase_previous(prevBallX, prevBallY, prevPlatformX);
    button_reaction();
    ball_movement();
    check_block_collision();
    check_win();
    draw_blocks();
    draw_scene();
    draw_score();

    prevBallX = ballX;
    prevBallY = ballY;
    prevPlatformX = platformX;

  } else if (game_over) {
    draw_game_over();
    check_restart();
    delay(100);
  } else if (win) {
    draw_win();
    check_restart();
    delay(100);
  }

  delay(20);
  otaServer.handleHttpClient();
  otaServer.handleDiscoveries();
}
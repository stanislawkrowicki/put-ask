#include <Arduino.h>
#include <WiFi.h>
#include <TFT_eSPI.h>

#include <OTAServer.h>
#include "wifi_secrets.h"

#define BTN_UP 16
#define BTN_DOWN 17
#define BTN_LEFT 18
#define BTN_RIGHT 19

OTAServer otaServer;
TFT_eSPI tft = TFT_eSPI();


//snake globals

int direction = 4; // 0=up, 1=down, 2=right, 3=left
bool in_boarder = true;
bool colision = false;
bool is_apple = false;

#define SNAKE_MAX 60
int snakeX[SNAKE_MAX] = {40};
int snakeY[SNAKE_MAX] = {30};
int prevSnakeX[SNAKE_MAX];
int prevSnakeY[SNAKE_MAX];
int snakeLength = 1;
int appleX, appleY;
int rockX,rockY;
bool rock_active = false;
unsigned long rock_spawn_time = 0;
int rock_time = 5000;
int rock_interval = 7000;
unsigned long last_rock_spawn = 0;

int score = 0;
int game_speed = 150;
int last_score_for_speed = -2;

bool game_over_flag = false;

void setup()
{
  Serial.begin(115200);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // while (WiFi.status() != WL_CONNECTED)
  // {
  //   delay(500);
  //   Serial.print(".");
  // }
  Serial.println("WiFi connected");

  otaServer.enable();

  pinMode(21, OUTPUT);
  digitalWrite(21, HIGH);
  
  //tft.init();
  //tft.fillScreen(TFT_RED);
  //tft.setCursor(0,0,4);
  //tft.setTextColor(TFT_BLACK);
  //tft.println ("Hello World!");

  tft.init();
  tft.fillScreen(TFT_DARKCYAN);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 100);
  tft.println("SNAKE GAME");
  delay(2000);
  tft.fillScreen(TFT_DARKCYAN);



  
}

void draw_score() {
  // tft.fillRect(0, 0, 80, 16, TFT_DARKCYAN);

  tft.setCursor(0, 0);
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(2);
  tft.print("Score: ");
  tft.print(score);
}

void handle_buttons() {
  if (digitalRead(BTN_UP) == LOW && direction!=1) direction = 0;
  if (digitalRead(BTN_DOWN) == LOW && direction!=0) direction = 1;
  if (digitalRead(BTN_RIGHT) == LOW && direction!=3) direction = 2;
  if (digitalRead(BTN_LEFT) == LOW && direction!=2) direction = 3;
}

void move_snake() {

  for (int i = 0; i < snakeLength; i++) {
    prevSnakeX[i] = snakeX[i];
    prevSnakeY[i] = snakeY[i];
  }

  for (int i = snakeLength - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  switch (direction) {
    case 0: snakeY[0] -= 8; break; // up
    case 1: snakeY[0] += 8; break; // down
    case 2: snakeX[0] += 8; break; // right
    case 3: snakeX[0] -= 8; break; // left
  }
}

void draw_snake() {
  for (int i = 0; i < snakeLength; i++) {
    tft.fillRect(prevSnakeX[i], prevSnakeY[i], 8, 8, TFT_DARKCYAN);
    tft.fillRect(snakeX[i], snakeY[i], 8, 8, TFT_GREEN);
    
  }
}

void draw_apple() {
  if (!is_apple) {
    bool valid = false;

    while (!valid) {
      appleX = random(2, 29) * 8;
      appleY = random(2, 28) * 8;
      valid = true;

      for (int i = 0; i < snakeLength; i++) {
        if (snakeX[i] == appleX && snakeY[i] == appleY) {
          valid = false;
          break;
        }
      }
    }

    is_apple = true;
  }

  tft.fillRect(appleX, appleY, 8, 8, TFT_RED);
}

void check_apple() {
  if (abs(snakeX[0] - appleX) < 8 && abs(snakeY[0] - appleY) < 8) {
    if (snakeLength < SNAKE_MAX) snakeLength++;
    is_apple = false;
    tft.fillRect(appleX, appleY, 8, 8, TFT_DARKCYAN);
    score++;
    tft.fillRect(50, 0,60 , 16, TFT_DARKCYAN);
    draw_score();
  }

  if (score % 2 == 0 && score != last_score_for_speed && game_speed > 50) {
    game_speed -= 10;
    last_score_for_speed = score;
  }

}

void check_collision() {
  if (snakeX[0] < 0 || snakeX[0] > 232 || snakeY[0] < 0 || snakeY[0] > 312) {
    in_boarder = false;
  }

  for (int i = 1; i < snakeLength; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      colision = true;
    }
  }
}

void game_over() {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(40, 80);
  tft.setTextSize(2);
  tft.setTextColor(TFT_RED);
  tft.println("GAME OVER");
  
  tft.setCursor(40, 110);
  tft.setTextColor(TFT_WHITE);
  tft.print("Score: ");
  tft.println(score);

  tft.setCursor(20, 140);
  tft.println("Press any key");


  game_over_flag = true;
}
void draw_rock() {
  bool valid = false;
  while(!valid){
    rockX = random(2, 29) * 8;
    rockY = random(2, 28) * 8;
    valid = true;
    for(int i=0;i<snakeLength;i++){
      if((snakeX[i]==rockX && snakeY[i]==rockY)||(appleX==rockX && appleY==rockY)){
        valid = false;
        break;
      }
    }
  }
  tft.fillRect(rockX,rockY,12,12,TFT_BLACK);
  rock_active = true;
  rock_spawn_time = millis();
}

void handle_rock(){
  unsigned long current_time = millis();
  if(!rock_active && current_time - last_rock_spawn >= rock_interval){
    draw_rock();
    last_rock_spawn = current_time;
  }

  if(rock_active && current_time - last_rock_spawn >= rock_time){
    tft.fillRect(rockX,rockY,12,12,TFT_DARKCYAN);
    rock_active = false;
  }

  if (abs(snakeX[0] - rockX) < 12 && abs(snakeY[0] - rockY) < 12) {
    game_over();
  }
}
void reset_game() {
  direction = 4;
  in_boarder = true;
  colision = false;
  is_apple = false;
  score = 0;
  game_speed = 200;
  snakeLength = 1;
  snakeX[0] = 40;
  snakeY[0] = 30;
  game_over_flag = false;

  tft.fillScreen(TFT_DARKCYAN);
}

void loop()
{

  if (game_over_flag) {
    if (digitalRead(BTN_UP) == LOW || digitalRead(BTN_DOWN) == LOW || 
        digitalRead(BTN_LEFT) == LOW || digitalRead(BTN_RIGHT) == LOW) {
      reset_game();
    }
    delay(100);
    return;
  }

  if (!in_boarder || colision) {
    game_over();
    return;
  }

  // tft.fillScreen(TFT_DARKCYAN);
  handle_buttons();
  move_snake();
  draw_score();
  draw_apple();
  draw_snake();
  check_apple();
  handle_rock();
  check_collision();


  delay(game_speed);

  //if (digitalRead(BTN_UP) == LOW) Serial.println("Button 1 pressed");
  //if (digitalRead(BTN_DOWN) == LOW) Serial.println("Button 2 pressed");
  //if (digitalRead(BTN_LEFT) == LOW) Serial.println("Button 3 pressed");
  //if (digitalRead(BTN_RIGHT) == LOW) Serial.println("Button 4 pressed");
  //delay(100);
  otaServer.handleHttpClient();
  otaServer.handleDiscoveries();
}
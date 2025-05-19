#include <Arduino.h>
#include <WiFi.h>
#include <TFT_eSPI.h>

#include <OTAServer.h>
#include "wifi_secrets.h"

#define BTN_LEFT_1 27
#define BTN_RIGHT_1 35
#define BTN_LEFT_2 32
#define BTN_RIGHT_2 14

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

#define PADDLE_WIDTH 60
#define PADDLE_HEIGHT 6
#define BALL_SIZE 6

#define P1_Y 40
#define P2_Y (SCREEN_HEIGHT - 40)

OTAServer otaServer;
TFT_eSPI tft = TFT_eSPI();

// pong globals

int p1X = (SCREEN_WIDTH - PADDLE_WIDTH) / 2;
int p2X = (SCREEN_WIDTH - PADDLE_WIDTH) / 2;

float ballX = SCREEN_WIDTH / 2;
float ballY = SCREEN_HEIGHT / 2;
float ballDx = random(0, 2) == 0 ? 2.2 : -2.2;
float ballDy = random(0, 2) == 0 ? 2.6 : -2.6;

int p1Score = 0;
int p2Score = 0;

int prevP1X = p1X;
int prevP2X = p2X;
float prevBallX = ballX;
float prevBallY = ballY;

bool gameOver = false;
int winner = 0;

void setup()
{
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  pinMode(21, OUTPUT);
  digitalWrite(21, HIGH);

  pinMode(BTN_LEFT_1, INPUT_PULLUP);
  pinMode(BTN_RIGHT_1, INPUT_PULLUP);
  pinMode(BTN_LEFT_2, INPUT_PULLUP);
  pinMode(BTN_RIGHT_2, INPUT_PULLUP);

  tft.init();
  tft.setRotation(0); // vertical
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 100);
  tft.println("PONG");
  delay(2000);
  tft.fillScreen(TFT_BLACK);
}

void update_paddles()
{
  if (digitalRead(BTN_LEFT_1) == LOW && p1X > 0)
    p1X -= 5;
  if (digitalRead(BTN_RIGHT_1) == LOW && p1X + PADDLE_WIDTH < SCREEN_WIDTH)
    p1X += 5;
  if (digitalRead(BTN_LEFT_2) == LOW && p2X > 0)
    p2X -= 5;
  if (digitalRead(BTN_RIGHT_2) == LOW && p2X + PADDLE_WIDTH < SCREEN_WIDTH)
    p2X += 5;
}
void reset_ball()
{
  ballX = SCREEN_WIDTH / 2;
  ballY = SCREEN_HEIGHT / 2;
  ballDx = random(0, 2) == 0 ? 2.2 : -2.2;
  ballDy = random(0, 2) == 0 ? 2.6 : -2.6;
}

void move_ball()
{
  ballX += ballDx;
  ballY += ballDy;

  // Bounce off side walls
  if (ballX <= 0 || ballX >= SCREEN_WIDTH)
  {
    ballDx = -ballDx;
  }

  // Player 1 paddle collision (moving upward)
  if (ballDy < 0 && // Only bounce if moving upward
      ballY - BALL_SIZE <= P1_Y + PADDLE_HEIGHT &&
      ballY - BALL_SIZE >= P1_Y &&
      ballX >= p1X && ballX <= p1X + PADDLE_WIDTH)
  {
    ballDy = abs(ballDy); // Bounce down
  }

  // Player 2 paddle collision (moving downward)
  if (ballDy > 0 && // Only bounce if moving downward
      ballY + BALL_SIZE >= P2_Y &&
      ballY + BALL_SIZE <= P2_Y + PADDLE_HEIGHT &&
      ballX >= p2X && ballX <= p2X + PADDLE_WIDTH)
  {
    ballDy = -abs(ballDy); // Bounce up
  }

  // Player 1 missed
  if (ballY < 0)
  {
    p2Score++;
    if (p2Score == 3)
    {
      gameOver = true;
      winner = 2;
    }
    reset_ball();
    return;
  }

  // Player 2 missed
  if (ballY > SCREEN_HEIGHT)
  {
    p1Score++;
    if (p1Score == 3)
    {
      gameOver = true;
      winner = 1;
    }
    reset_ball();
    return;
  }
}
void resetGame()
{
  p1Score = 0;
  p2Score = 0;
  winner = 0;
  gameOver = false;
  ballX = SCREEN_WIDTH / 2;
  ballY = SCREEN_HEIGHT / 2;
  ballDx = 2;
  ballDy = -2;
  tft.fillScreen(TFT_BLACK);
}

void showWinner()
{
  while (1)
  {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(3);
    tft.setCursor(30, SCREEN_HEIGHT / 2 - 20);
    if (winner == 1)
    {
      tft.print("P1 WINS!");
    }
    else
    {
      tft.print("P2 WINS!");
    }

    tft.setTextSize(1);
    tft.setCursor(30, SCREEN_HEIGHT / 2 + 30);
    tft.print("Press both buttons to restart");

    if (digitalRead(BTN_LEFT_1) == LOW && digitalRead(BTN_RIGHT_1) == LOW)
    {
      delay(500);
      return;
    }
    delay(200);
  }
}

void draw_game()
{

  tft.fillRect(prevP1X, P1_Y, PADDLE_WIDTH, PADDLE_HEIGHT, TFT_BLACK);
  tft.fillRect(prevP2X, P2_Y, PADDLE_WIDTH, PADDLE_HEIGHT, TFT_BLACK);
  tft.fillCircle((int)prevBallX, (int)prevBallY, BALL_SIZE, TFT_BLACK);

  tft.fillRect(p1X, P1_Y, PADDLE_WIDTH, PADDLE_HEIGHT, TFT_BLUE);
  tft.fillRect(p2X, P2_Y, PADDLE_WIDTH, PADDLE_HEIGHT, TFT_GREEN);

  tft.fillCircle((int)ballX, (int)ballY, BALL_SIZE, TFT_WHITE);

  tft.fillRect(0, 0, 80, 30, TFT_BLACK);
  tft.fillRect(0, SCREEN_HEIGHT - 30, 80, 30, TFT_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 10);
  tft.printf("P1: %d", p1Score);

  tft.setCursor(10, SCREEN_HEIGHT - 30);
  tft.printf("P2: %d", p2Score);

  prevP1X = p1X;
  prevP2X = p2X;
  prevBallX = ballX;
  prevBallY = ballY;
}

void loop()
{
  update_paddles();
  move_ball();
  draw_game();
  if (!gameOver)
  {
    move_ball();
    draw_game();
  }
  else
  {
    showWinner();
    resetGame();
  }
  delay(20);
  otaServer.connectionLoop();
  otaServer.handleHttpClient();
  otaServer.handleDiscoveries();
}
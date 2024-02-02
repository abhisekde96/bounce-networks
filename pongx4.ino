#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define BALL_SIZE 4 // Diameter of the ball
#define BOUNCE_FACTOR -1 // Factor of velocity lost after bouncing
#define BALL_SPEED_X 1 // Horizontal speed of the ball
#define BALL_SPEED_Y 1 // Vertical speed of the ball
#define NUM_BALLS 4 // Number of balls
#define COLLISION_DISTANCE 0.1 // Distance threshold for collision detection
#define MIN_JOIN_DISTANCE 17.5 // Minimum distance threshold for joining balls
#define SPEED_INCREASE_FACTOR 1.005 // Factor to increase ball speed when a line is drawn

// Define container dimensions
#define CONTAINER_WIDTH SCREEN_WIDTH
#define CONTAINER_HEIGHT 48 // Adjusted to leave space for scoreboard

// Define push button pins for player movement
#define LEFT_BUTTON_PIN 3
#define RIGHT_BUTTON_PIN 4

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Structure to represent a ball
struct Ball {
  float x;
  float y;
  float velocityX;
  float velocityY;
  bool visible; // Track if ball is still visible
};

Ball balls[NUM_BALLS]; // Array to store ball instances

bool simulationPaused = false; // Flag to track simulation state
int score = 0; // Player's score
bool gameOver = false; // Flag to track game over state

// Player attributes
#define PLAYER_WIDTH 40
#define PLAYER_HEIGHT 2
#define PLAYER_SPEED 4
struct Player {
  float x;
  float y;
};

Player player;

void setup() {
  // Initialize display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  // Clear the buffer
  display.clearDisplay();

  // Initialize ball positions and velocities
  for (int i = 0; i < NUM_BALLS; i++) {
    balls[i].x = random(SCREEN_WIDTH - BALL_SIZE);
    balls[i].y = random(CONTAINER_HEIGHT - BALL_SIZE);
    balls[i].velocityX = BALL_SPEED_X;
    balls[i].velocityY = BALL_SPEED_Y;
    balls[i].visible = true; // Set all balls to be initially visible
  }

  // Initialize player position
  player.x = SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2;
  player.y = CONTAINER_HEIGHT - 10; // Position the player near the bottom of the screen within the boundary

  // Set button pins as input
  pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  // Check if the left button is pressed and move player left
  if (digitalRead(LEFT_BUTTON_PIN) == LOW) {
    player.x -= PLAYER_SPEED;
    // Ensure player stays within the boundary
    if (player.x < 0) {
      player.x = 0;
    }
  }

  // Check if the right button is pressed and move player right
  if (digitalRead(RIGHT_BUTTON_PIN) == LOW) {
    player.x += PLAYER_SPEED;
    // Ensure player stays within the boundary
    if (player.x + PLAYER_WIDTH > SCREEN_WIDTH) {
      player.x = SCREEN_WIDTH - PLAYER_WIDTH;
    }
  }

  if (!simulationPaused && !gameOver) {
    // Clear the buffer
    display.clearDisplay();

    // Draw the player
    display.fillRect(player.x, player.y, PLAYER_WIDTH, PLAYER_HEIGHT, WHITE);

    // Update ball positions
    bool anyBallVisible = false; // Track if any ball is still visible
    for (int i = 0; i < NUM_BALLS; i++) {
      if (balls[i].visible) {
        balls[i].x += balls[i].velocityX;
        balls[i].y += balls[i].velocityY;

        // Check for collisions with player
        if (balls[i].x + BALL_SIZE >= player.x && balls[i].x <= player.x + PLAYER_WIDTH &&
            balls[i].y + BALL_SIZE >= player.y && balls[i].y <= player.y + PLAYER_HEIGHT) {
          // Collision with player, bounce the ball and increase score
          balls[i].velocityY *= BOUNCE_FACTOR;
          score++;
        }

        // Check for collisions with boundaries
        if (balls[i].x >= CONTAINER_WIDTH - BALL_SIZE || balls[i].x <= 0) {
          balls[i].velocityX *= BOUNCE_FACTOR; // Reverse velocity to simulate reflection
        }
        if (balls[i].y <= 0) {
          balls[i].velocityY *= BOUNCE_FACTOR; // Reverse velocity to simulate reflection
        }

        // Draw the ball
        display.fillCircle(balls[i].x, balls[i].y, BALL_SIZE / 2, WHITE);

        // Check if ball is still visible on screen
        if (balls[i].y < CONTAINER_HEIGHT) {
          anyBallVisible = true;
        } else {
          balls[i].visible = false; // Mark the ball as not visible when it goes off-screen
        }

        // Connect balls within minimum join distance with a line
        for (int j = i + 1; j < NUM_BALLS; j++) {
          float dx = balls[j].x - balls[i].x;
          float dy = balls[j].y - balls[i].y;
          float distance = sqrt(dx * dx + dy * dy);
          if (distance <= MIN_JOIN_DISTANCE && balls[j].visible && balls[i].visible) {
            // Increase ball speed
            balls[i].velocityX *= SPEED_INCREASE_FACTOR;
            balls[i].velocityY *= SPEED_INCREASE_FACTOR;
            balls[j].velocityX *= SPEED_INCREASE_FACTOR;
            balls[j].velocityY *= SPEED_INCREASE_FACTOR;
            // Draw the line connecting the balls
            display.drawLine(balls[i].x, balls[i].y, balls[j].x, balls[j].y, WHITE);
          }
        }
      }
    }

    // Display the score outside the boundary
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, CONTAINER_HEIGHT);
    display.print("Score: ");
    display.println(score);

    // Check game over condition
    if (!anyBallVisible) {
      gameOver = true;
    }
    if (gameOver) {
      display.setTextSize(2);
      display.setCursor(SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 - 10);
      display.print("Rekt!");
    }

    // Display the buffer
    display.display();
  }

  // Delay to control animation speed
  delay(10);
}

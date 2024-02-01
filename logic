#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define BALL_SIZE 3 // Diameter of the ball
#define BOUNCE_FACTOR -1 // Factor of velocity lost after bouncing
#define BALL_SPEED_X 1 // Horizontal speed of the ball
#define BALL_SPEED_Y 1 // Vertical speed of the ball
#define NUM_BALLS 13 // Number of balls
#define COLLISION_DISTANCE 0.1 // Distance threshold for collision detection
#define MIN_JOIN_DISTANCE 15 // Minimum distance threshold for joining balls
#define SPEED_INCREASE_FACTOR 1.0005 // Factor to increase ball speed when a line is drawn

// Define container dimensions
#define CONTAINER_WIDTH SCREEN_WIDTH
#define CONTAINER_HEIGHT SCREEN_HEIGHT

// Define push button pins
#define PAUSE_RESUME_BUTTON_PIN 3
#define RESET_BUTTON_PIN 4

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Structure to represent a ball
struct Ball {
  float x;
  float y;
  float velocityX;
  float velocityY;
};

Ball balls[NUM_BALLS]; // Array to store ball instances

bool simulationPaused = false; // Flag to track simulation state

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
    balls[i].x = random(SCREEN_WIDTH);
    balls[i].y = random(SCREEN_HEIGHT);
    balls[i].velocityX = BALL_SPEED_X;
    balls[i].velocityY = BALL_SPEED_Y;
  }

  // Set button pins as input
  pinMode(PAUSE_RESUME_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  // Check if the pause/resume button is pressed
  if (digitalRead(PAUSE_RESUME_BUTTON_PIN) == LOW) {
    simulationPaused = !simulationPaused; // Toggle simulation state
    delay(200); // Debounce delay
  }

  // Check if the reset button is pressed
  if (digitalRead(RESET_BUTTON_PIN) == LOW) {
    // Reset ball positions
    for (int i = 0; i < NUM_BALLS; i++) {
      balls[i].x = random(SCREEN_WIDTH);
      balls[i].y = random(SCREEN_HEIGHT);
    }
    delay(200); // Debounce delay
  }

  if (!simulationPaused) {
    // Clear the buffer
    display.clearDisplay();

    // Update ball positions
    for (int i = 0; i < NUM_BALLS; i++) {
      // Update ball positions
      balls[i].x += balls[i].velocityX;
      balls[i].y += balls[i].velocityY;

      // Check boundary conditions and prevent balls from getting stuck
      if (balls[i].x >= CONTAINER_WIDTH - BALL_SIZE) {
        balls[i].x = CONTAINER_WIDTH - BALL_SIZE - 1; // Move the ball inside the container
        balls[i].velocityX *= BOUNCE_FACTOR; // Reverse velocity to simulate reflection
      } else if (balls[i].x <= BALL_SIZE) {
        balls[i].x = BALL_SIZE + 1; // Move the ball inside the container
        balls[i].velocityX *= BOUNCE_FACTOR; // Reverse velocity to simulate reflection
      }

      if (balls[i].y >= CONTAINER_HEIGHT - BALL_SIZE) {
        balls[i].y = CONTAINER_HEIGHT - BALL_SIZE - 1; // Move the ball inside the container
        balls[i].velocityY *= BOUNCE_FACTOR; // Reverse velocity to simulate reflection
      } else if (balls[i].y <= BALL_SIZE) {
        balls[i].y = BALL_SIZE + 1; // Move the ball inside the container
        balls[i].velocityY *= BOUNCE_FACTOR; // Reverse velocity to simulate reflection
      }

      // Check for collisions with other balls
      for (int j = i + 1; j < NUM_BALLS; j++) {
        float dx = balls[j].x - balls[i].x;
        float dy = balls[j].y - balls[i].y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance <= COLLISION_DISTANCE) {
          // Swap velocities to simulate bouncing off each other
          float tempVx = balls[i].velocityX;
          float tempVy = balls[i].velocityY;
          balls[i].velocityX = balls[j].velocityX;
          balls[i].velocityY = balls[j].velocityY;
          balls[j].velocityX = tempVx;
          balls[j].velocityY = tempVy;
        }
      }

      // Draw the ball
      display.fillCircle(balls[i].x, balls[i].y, BALL_SIZE / 2, WHITE);
    }

    // Join balls if they are close enough
    for (int i = 0; i < NUM_BALLS; i++) {
      for (int j = i + 1; j < NUM_BALLS; j++) {
        float dx = balls[j].x - balls[i].x;
        float dy = balls[j].y - balls[i].y;
        float distance = sqrt(dx * dx + dy * dy);

        // Dynamic join distance based on distance between balls
        float joinDistance = MIN_JOIN_DISTANCE + (distance / 5);
        if (distance <= joinDistance) {
          // Increase speed by 0.05%
          balls[i].velocityX *= SPEED_INCREASE_FACTOR;
          balls[i].velocityY *= SPEED_INCREASE_FACTOR;
          balls[j].velocityX *= SPEED_INCREASE_FACTOR;
          balls[j].velocityY *= SPEED_INCREASE_FACTOR;

          // Draw the line
          display.drawLine(balls[i].x, balls[i].y, balls[j].x, balls[j].y, WHITE);
        }
      }
    }

    // Display the buffer
    display.display();
  }

  // Delay to control animation speed
  delay(10);
}

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET -1  // Share Reset pin with arduino
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

#define BUTTON_A 9
#define BUTTON_B 8
#define BUTTON_C 7
#define SOUND_PIN 6

int tamagotchiPosX = 64;        // Start at the middle horizontally
int tamagotchiPosY = 32;        // Start below the menu line vertically
int moveDirectionX = 1;         // Movement direction: 1 for right, -1 for left
int moveDirectionY = 1;         // Movement direction: 1 for down, -1 for up
const int moveSpeed = 1.10;     // Adjust for faster or slower movement
const int menuLineHeight = 16;  // Height of the menu line area

// Tamagotchi stats
int hunger = 50;
int happy = 50;
int health = 100;
int sleep = 100;
int discipline = 100;
int poops = 0;

bool menuOpened = false;
bool displayingStats = false;
int menuSel = 0;

void playButtonSound() {
  tone(SOUND_PIN, 1000, 100);
  delay(150);
  noTone(SOUND_PIN);
}

void capValue(int &value, int increment, int min = 0, int max = 100) {
  value += increment;
  if (value > max) {
    value = max;
  } else if (value < min) {
    value = min;
  }
}

void displayMenuLine(int menuSel) {
  display.setCursor(0, 0);
  display.clearDisplay();
  // Highlight the selected menu item
  for (int i = 0; i < 6; i++) {
    if (i == menuSel) {
      display.print("|");
    }
    switch (i) {
      case 0: display.print("F"); break;
      case 1: display.print("H"); break;
      case 2: display.print("Hp"); break;
      case 3: display.print("S"); break;
      case 4: display.print("D"); break;
      case 5: display.print("Z"); break;
    }
    if (i == menuSel) {
      display.print("| ");
    } else {
      display.print("  ");
    }
  }
  display.println();
}

void displayTamagotchi() {
  // Update horizontal position
  tamagotchiPosX += moveSpeed * moveDirectionX;
  // Reverse direction if hitting the horizontal display boundaries
  if (tamagotchiPosX <= 0 || tamagotchiPosX >= display.width() - 64) {
    moveDirectionX *= -1;
  }

  // Update vertical position
  tamagotchiPosY += moveSpeed * moveDirectionY;
  // Reverse direction if hitting the vertical display boundaries, taking into account the menu line height
  if (tamagotchiPosY <= menuLineHeight || tamagotchiPosY >= display.height() - 32) {
    moveDirectionY *= -1;
  }

  // Ensure the position stays within the display boundaries, accounting for the menu line height
  tamagotchiPosX = max(0, min(tamagotchiPosX, display.width() - 64));
  tamagotchiPosY = max(menuLineHeight, min(tamagotchiPosY, display.height() - 32));

  // Set cursor based on current position and draw the Tamagotchi
  display.setCursor(tamagotchiPosX, tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 10);
  display.println(" ( o.o ) ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 20);
  display.println("  > ^ <  ");
}

void displayStats() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Stats:");
  display.println("");
  display.print("Hunger: ");
  display.println(hunger);
  display.print("Happy: ");
  display.println(happy);
  display.print("Health: ");
  display.println(health);
  display.print("Sleep: ");
  display.println(sleep);
  display.print("Discipline: ");
  display.println(discipline);
}

void handleMenuSelection(int menuSel) {
  switch (menuSel) {
    case 0:
      capValue(hunger, -10);
      break;
    case 1:
      capValue(happy, 10);
      break;
    case 2:
      capValue(health, 10);
      break;
    case 3:
      capValue(sleep, 10);
      break;
    case 4:
      capValue(discipline, 10);
      break;
    case 5:
      displayingStats = !displayingStats;
      break;
  }
}

void setup() {
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  pinMode(SOUND_PIN, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  display.clearDisplay();
  // Read button states
  if (!digitalRead(BUTTON_A)) {
    playButtonSound();
    if (menuOpened) {
      menuSel = (menuSel + 1) % 6;  // Cycle through menu options if the menu is opened
    }
    while (!digitalRead(BUTTON_A))
      ;  // Wait for button release
  }

  if (!digitalRead(BUTTON_B)) {
    playButtonSound();
    handleMenuSelection(menuSel);
    while (!digitalRead(BUTTON_B))
      ;  // Wait for button release
  }

  if (!digitalRead(BUTTON_C)) {
    playButtonSound();
    if (displayingStats) {
      displayingStats = false;
    } else {
      menuOpened = !menuOpened;
    }
    while (!digitalRead(BUTTON_C))
      ;  // Wait for button release
  }

  if (!displayingStats) {


    if (menuOpened) {
      displayMenuLine(menuSel);  // Show the menu line
    }
    displayTamagotchi();
  }

  else {
    displayStats();
  }
  display.display();  // Actually draw everything on the screen
  delay(100);         // Small delay to reduce flickering
}
